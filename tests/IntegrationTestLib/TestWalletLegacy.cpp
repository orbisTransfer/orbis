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
#include "TestWalletLegacy.h"

namespace Tests {
namespace Common {

using namespace Orbis;
using namespace Crypto;

const std::string TEST_PASSWORD = "password";

TestWalletLegacy::TestWalletLegacy(System::Dispatcher& dispatcher, const Currency& currency, INode& node) :
    m_dispatcher(dispatcher),
    m_synchronizationCompleted(dispatcher),
    m_someTransactionUpdated(dispatcher),
    m_currency(currency),
    m_node(node),
    m_wallet(new Orbis::WalletLegacy(currency, node)),
    m_currentHeight(0) {
  m_wallet->addObserver(this);
}

TestWalletLegacy::~TestWalletLegacy() {
  m_wallet->removeObserver(this);
  // Make sure all remote spawns are executed
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  m_dispatcher.yield();
}

std::error_code TestWalletLegacy::init() {
  Orbis::AccountBase walletAccount;
  walletAccount.generate();

  m_wallet->initWithKeys(walletAccount.getAccountKeys(), TEST_PASSWORD);
  m_synchronizationCompleted.wait();
  return m_lastSynchronizationResult;
}

namespace {
  struct TransactionSendingWaiter : public IWalletLegacyObserver {
    System::Dispatcher& m_dispatcher;
    System::Event m_event;
    bool m_waiting = false;
    TransactionId m_expectedTxId;
    std::error_code m_result;

    TransactionSendingWaiter(System::Dispatcher& dispatcher) : m_dispatcher(dispatcher), m_event(dispatcher) {
    }

    void wait(TransactionId expectedTxId) {
      m_waiting = true;
      m_expectedTxId = expectedTxId;
      m_event.wait();
      m_waiting = false;
    }

    virtual void sendTransactionCompleted(TransactionId transactionId, std::error_code result) override {
      m_dispatcher.remoteSpawn([this, transactionId, result]() {
        if (m_waiting &&  m_expectedTxId == transactionId) {
          m_result = result;
          m_event.set();
        }
      });
    }
  };
}

std::error_code TestWalletLegacy::sendTransaction(const std::string& address, uint64_t amount, Hash& txHash) {
  TransactionSendingWaiter transactionSendingWaiter(m_dispatcher);
  m_wallet->addObserver(&transactionSendingWaiter);

  WalletLegacyTransfer transfer{ address, static_cast<int64_t>(amount) };
  auto txId = m_wallet->sendTransaction(transfer, m_currency.minimumFee());
  transactionSendingWaiter.wait(txId);
  m_wallet->removeObserver(&transactionSendingWaiter);
  // TODO workaround: make sure ObserverManager doesn't have local pointers to transactionSendingWaiter, so it can be destroyed
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // Run all spawned handlers from TransactionSendingWaiter::sendTransactionCompleted
  m_dispatcher.yield();

  WalletLegacyTransaction txInfo;
  if (!m_wallet->getTransaction(txId, txInfo)) {
    return std::make_error_code(std::errc::identifier_removed);
  }

  txHash = txInfo.hash;
  return transactionSendingWaiter.m_result;
}

void TestWalletLegacy::waitForSynchronizationToHeight(uint32_t height) {
  while (m_synchronizedHeight < height) {
    m_synchronizationCompleted.wait();
  }
}

IWalletLegacy* TestWalletLegacy::wallet() {
  return m_wallet.get();
}

AccountPublicAddress TestWalletLegacy::address() const {
  std::string addressString = m_wallet->getAddress();
  AccountPublicAddress address;
  bool ok = m_currency.parseAccountAddressString(addressString, address);
  assert(ok);
  return address;
}

void TestWalletLegacy::synchronizationCompleted(std::error_code result) {
  m_dispatcher.remoteSpawn([this, result]() {
    m_lastSynchronizationResult = result;
    m_synchronizedHeight = m_currentHeight;
    m_synchronizationCompleted.set();
    m_synchronizationCompleted.clear();
  });
}

void TestWalletLegacy::synchronizationProgressUpdated(uint32_t current, uint32_t total) {
  m_dispatcher.remoteSpawn([this, current]() {
    m_currentHeight = current;
  });
}

}
}
