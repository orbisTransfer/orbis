// Copyright (c) 2018 The Orbis developers
// ======================================================================
//              ===== Spell of Creation =====
// script inspired by the CNC code, on February 11, 2018
// 
// ======================================================================
/*                ,     
			     d 
                 :8;        oo 
                ,888     od88 Y 
                " b     "`88888  ,. 
                  :    88888888o. 
                   Y.   `8888888bo 
                    "bo  d888888888oooooo,. 
                      "Yo8:8888888888888Yd8. 
                        `8b;Y88888888888888' 
                        ,888d"Y888888888888.  , 
                        P88Y8b.888888888888b.         `b 
                        :;  "8888888888888888P8b       d; 
                         .   Y88888888888888bo   ",o ,d8 
                              `"8888888888888888888888",oo. 
                               :888888888888888888888P""   ` 
                               `88888888888888888888oooooo. : 
                            ;  ,888888Y888888888888888888888" 
                            `'`P""8.`8;`888888888888888888888o. 
                              `    ` :;  `888888888888888"""8P"o. 
                                 ,  '`8    Y88888888888888;  :b Yb 
                                  8.  :.    `Y8888888888888; dP  `8 
                                ,8d8    "     `888888888888d      Y; 
                                :888d8;        88888888888Pb       ; 
                                :888' `   o   d888888888888;      :' 
                               oo888bo  ,."8888888888888888;    ' ' 
                               8888888888888888888888888888; 
                       ,.`88booo`Y888888888888888888888888P' 
                           :888888888888888888888888888888' 
                   ,ooooood888888888888888888888888888888' 
                  ""888888888888888888888888888888888888; 
             ,oooooo888888888888888888888888888888888888' 
               "88888888888888888888888888888888888888P 
       ,oo bo ooo88888888888888888888888888888888888Y8 
     ."8P88d888888888888888888888888888888888888888"`"o. 
      oo888888888888888888888888888888888888888888"    8 
     d88Y8888888888888888888888888888888888888888' ooo8bYooooo. 
    ,""o888888888888888888888888888888888P":888888888888888888b 
    `   ,d88888888888888888888888888888"'  :888888888888888bod8 
      ,88888888888888888888888888888"      `d8888888888888o`88"b 
    ,88888888888888888888888888""            ,88888' 88  Y8b 
    " ,8888888888888888888""        ,;       88' ;   `8'  P 
     d8888888888888888888boo8888888P"         :.     ` 
    d888888888888888888888888888888boooo 
   :"Y888888888888P':88888"""8P"88888P' 
   ` :88888888888'   88""  ,dP' :888888. 
    ,88888888888'          '`  ,88,8b d" 
    d88888888888               dP"`888' 
    Y :888888888;                   8' 
      :8888888888.                 ` 
      :888888888888oo                        ,ooooo 
      :8888888888888o              ,o   oo d88888oooo. 
       ' :888888888888888booooood888888888888888888888bo.  -hrr- 
          Y88888888888888888888888888888888888"""888888o. 
           "`"Y8888888888888888888888888""""'     `"88888b. 
               "888"Y888888888888888"                Y888.' 
                `"'  `""' `"""""'  "          ,       8888 
                                              :.      8888 
                                           d888d8o    88;` 
                                           Y888888;  d88; 
                                         o88888888,o88P:' 
                                    "ood888888888888"' 
                                ,oo88888""888888888. 
                              ,o8P"b8YP  '`"888888;"b. 
                           ' d8"`o8",P    """""""8P 
                                    `;          d8; 
                                                 8;
// =============================================================
//              ===== it's not an illusion =====
//                  ===== it's real =====
//
// =============================================================
*/                          
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#pragma once

#include "IWalletLegacy.h"
#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <unordered_map>

#include "../IntegrationTestLib/ObservableValue.h"

namespace Orbis {

class WalletLegacyObserver: public IWalletLegacyObserver {
public:

  WalletLegacyObserver() :
    m_actualBalance(0),
    m_actualBalancePrev(0),
    m_pendingBalance(0),
    m_pendingBalancePrev(0),
    m_syncResult(m_mutex, m_cv) {}

  virtual void actualBalanceUpdated(uint64_t actualBalance) override {
    std::unique_lock<std::mutex> lk(m_mutex);
    m_actualBalance = actualBalance;
    lk.unlock();
    m_cv.notify_all();
  }

  virtual void pendingBalanceUpdated(uint64_t pendingBalance) override {
    std::unique_lock<std::mutex> lk(m_mutex);
    m_pendingBalance = pendingBalance;
    lk.unlock();
    m_cv.notify_all();
  }

  virtual void sendTransactionCompleted(Orbis::TransactionId transactionId, std::error_code result) override {
    std::unique_lock<std::mutex> lk(m_mutex);
    m_sendResults[transactionId] = result;
    m_cv.notify_all();
  }

  virtual void synchronizationCompleted(std::error_code result) override {
    m_syncResult.set(result);
  }

  virtual void synchronizationProgressUpdated(uint32_t current, uint32_t total) override {
    std::unique_lock<std::mutex> lk(m_mutex);
    m_syncProgress.emplace_back(current, total);
    m_currentHeight = current;
    m_cv.notify_all();
  }

  virtual void externalTransactionCreated(TransactionId transactionId) override {
    std::unique_lock<std::mutex> lk(m_mutex);
    m_externalTransactions.push_back(transactionId);
    m_cv.notify_all();
  }
  
  uint64_t getCurrentHeight() {
    std::unique_lock<std::mutex> lk(m_mutex);
    return m_currentHeight;
  }

  uint64_t waitPendingBalanceChange() {
    std::unique_lock<std::mutex> lk(m_mutex);
    while (m_pendingBalance == m_pendingBalancePrev) {
      m_cv.wait(lk);
    }
    m_pendingBalancePrev = m_pendingBalance;
    return m_pendingBalance;
  }

  uint64_t waitTotalBalanceChange() {
    std::unique_lock<std::mutex> lk(m_mutex);
    while (m_pendingBalance == m_pendingBalancePrev && m_actualBalance == m_actualBalancePrev) {
      m_cv.wait(lk);
    }

    m_actualBalancePrev = m_actualBalance;
    m_pendingBalancePrev = m_pendingBalance;

    return m_actualBalance + m_pendingBalance;
  }

  Orbis::TransactionId waitExternalTransaction() {
    std::unique_lock<std::mutex> lk(m_mutex);

    while (m_externalTransactions.empty()) {
      m_cv.wait(lk);
    }

    Orbis::TransactionId txId = m_externalTransactions.front();
    m_externalTransactions.pop_front();
    return txId;
  }

  template<class Rep, class Period>
  std::pair<bool, uint64_t> waitPendingBalanceChangeFor(const std::chrono::duration<Rep, Period>& timePeriod) {
    std::unique_lock<std::mutex> lk(m_mutex);
    bool result = m_cv.wait_for(lk, timePeriod, [&] { return m_pendingBalance != m_pendingBalancePrev; });
    m_pendingBalancePrev = m_pendingBalance;
    return std::make_pair(result, m_pendingBalance);
  }

  uint64_t waitActualBalanceChange() {
    std::unique_lock<std::mutex> lk(m_mutex);
    while (m_actualBalance == m_actualBalancePrev) {
      m_cv.wait(lk);
    }
    m_actualBalancePrev = m_actualBalance;
    return m_actualBalance;
  }

  std::error_code waitSendResult(Orbis::TransactionId txid) {
    std::unique_lock<std::mutex> lk(m_mutex);

    std::unordered_map<Orbis::TransactionId, std::error_code>::iterator it;

    while ((it = m_sendResults.find(txid)) == m_sendResults.end()) {
      m_cv.wait(lk);
    }

    return it->second;
  }

  uint64_t totalBalance() {
    std::unique_lock<std::mutex> lk(m_mutex);
    m_pendingBalancePrev = m_pendingBalance;
    m_actualBalancePrev = m_actualBalance;
    return m_pendingBalance + m_actualBalance;
  }

  std::vector<std::pair<uint32_t, uint32_t>> getSyncProgress() {
    std::unique_lock<std::mutex> lk(m_mutex);
    return m_syncProgress;
  }

  ObservableValueBase<std::error_code> m_syncResult;

private:
    
  std::mutex m_mutex;
  std::condition_variable m_cv;

  uint64_t m_actualBalance;
  uint64_t m_actualBalancePrev;
  uint64_t m_pendingBalance;
  uint64_t m_pendingBalancePrev;

  uint32_t m_currentHeight;

  std::vector<std::pair<uint32_t, uint32_t>> m_syncProgress;

  std::unordered_map<Orbis::TransactionId, std::error_code> m_sendResults;
  std::deque<Orbis::TransactionId> m_externalTransactions;
};

}
