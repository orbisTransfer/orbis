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

#include <set>
#include <unordered_map>
#include <unordered_set>

#include <boost/utility.hpp>

// multi index
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include "Common/Util.h"
#include "Common/int-util.h"
#include "Common/ObserverManager.h"
#include "orbis/hash.h"

#include "OrbisCore/OrbisBasic.h"
#include "OrbisCore/OrbisBasicImpl.h"
#include "OrbisCore/Currency.h"
#include "OrbisCore/ITimeProvider.h"
#include "OrbisCore/ITransactionValidator.h"
#include "OrbisCore/ITxPoolObserver.h"
#include "OrbisCore/VerificationContext.h"
#include "OrbisCore/BlockchainIndices.h"

#include <Logging/LoggerRef.h>

namespace Orbis {

  class ISerializer;

  class OnceInTimeInterval {
  public:
    OnceInTimeInterval(unsigned interval, Orbis::ITimeProvider& timeProvider)
      : m_interval(interval), m_timeProvider(timeProvider) {
      m_lastWorkedTime = 0;
    }

    template<class functor_t>
    bool call(functor_t functr) {
      time_t now = m_timeProvider.now();

      if (now - m_lastWorkedTime > m_interval) {
        bool res = functr();
        m_lastWorkedTime = m_timeProvider.now();
        return res;
      }

      return true;
    }

  private:
    time_t m_lastWorkedTime;
    unsigned m_interval;
    Orbis::ITimeProvider& m_timeProvider;
  };

  using Orbis::BlockInfo;
  using namespace boost::multi_index;

  /************************************************************************/
  /*                                                                      */
  /************************************************************************/
  class tx_memory_pool: boost::noncopyable {
  public:
    tx_memory_pool(
      const Orbis::Currency& currency, 
      Orbis::ITransactionValidator& validator,
      Orbis::ITimeProvider& timeProvider,
      Logging::ILogger& log);

    bool addObserver(ITxPoolObserver* observer);
    bool removeObserver(ITxPoolObserver* observer);

    // load/store operations
    bool init(const std::string& config_folder);
    bool deinit();

    bool have_tx(const Crypto::Hash &id) const;
    bool add_tx(const Transaction &tx, const Crypto::Hash &id, size_t blobSize, tx_verification_context& tvc, bool keeped_by_block);
    bool add_tx(const Transaction &tx, tx_verification_context& tvc, bool keeped_by_block);
    //gets tx and remove it from pool
    bool take_tx(const Crypto::Hash &id, Transaction &tx, size_t& blobSize, uint64_t& fee);

    bool on_blockchain_inc(uint64_t new_block_height, const Crypto::Hash& top_block_id);
    bool on_blockchain_dec(uint64_t new_block_height, const Crypto::Hash& top_block_id);

    void lock() const;
    void unlock() const;
    std::unique_lock<std::recursive_mutex> obtainGuard() const;

    bool fill_block_template(Block &bl, size_t median_size, size_t maxCumulativeSize, uint64_t already_generated_coins, size_t &total_size, uint64_t &fee);

    void get_transactions(std::list<Transaction>& txs) const;
    void get_difference(const std::vector<Crypto::Hash>& known_tx_ids, std::vector<Crypto::Hash>& new_tx_ids, std::vector<Crypto::Hash>& deleted_tx_ids) const;
    size_t get_transactions_count() const;
    std::string print_pool(bool short_format) const;
    void on_idle();

    bool getTransactionIdsByPaymentId(const Crypto::Hash& paymentId, std::vector<Crypto::Hash>& transactionIds);
    bool getTransactionIdsByTimestamp(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t transactionsNumberLimit, std::vector<Crypto::Hash>& hashes, uint64_t& transactionsNumberWithinTimestamps);

    template<class t_ids_container, class t_tx_container, class t_missed_container>
    void getTransactions(const t_ids_container& txsIds, t_tx_container& txs, t_missed_container& missedTxs) {
      std::lock_guard<std::recursive_mutex> lock(m_transactions_lock);

      for (const auto& id : txsIds) {
        auto it = m_transactions.find(id);
        if (it == m_transactions.end()) {
          missedTxs.push_back(id);
        } else {
          txs.push_back(it->tx);
        }
      }
    }

    void serialize(ISerializer& s);

    struct TransactionCheckInfo {
      BlockInfo maxUsedBlock;
      BlockInfo lastFailedBlock;
    };

    struct TransactionDetails : public TransactionCheckInfo {
      Crypto::Hash id;
      Transaction tx;
      size_t blobSize;
      uint64_t fee;
      bool keptByBlock;
      time_t receiveTime;
    };

  private:

    struct TransactionPriorityComparator {
      // lhs > hrs
      bool operator()(const TransactionDetails& lhs, const TransactionDetails& rhs) const {
        // price(lhs) = lhs.fee / lhs.blobSize
        // price(lhs) > price(rhs) -->
        // lhs.fee / lhs.blobSize > rhs.fee / rhs.blobSize -->
        // lhs.fee * rhs.blobSize > rhs.fee * lhs.blobSize
        uint64_t lhs_hi, lhs_lo = mul128(lhs.fee, rhs.blobSize, &lhs_hi);
        uint64_t rhs_hi, rhs_lo = mul128(rhs.fee, lhs.blobSize, &rhs_hi);

        return
          // prefer more profitable transactions
          (lhs_hi >  rhs_hi) ||
          (lhs_hi == rhs_hi && lhs_lo >  rhs_lo) ||
          // prefer smaller
          (lhs_hi == rhs_hi && lhs_lo == rhs_lo && lhs.blobSize <  rhs.blobSize) ||
          // prefer older
          (lhs_hi == rhs_hi && lhs_lo == rhs_lo && lhs.blobSize == rhs.blobSize && lhs.receiveTime < rhs.receiveTime);
      }
    };

    typedef hashed_unique<BOOST_MULTI_INDEX_MEMBER(TransactionDetails, Crypto::Hash, id)> main_index_t;
    typedef ordered_non_unique<identity<TransactionDetails>, TransactionPriorityComparator> fee_index_t;

    typedef multi_index_container<TransactionDetails,
      indexed_by<main_index_t, fee_index_t>
    > tx_container_t;

    typedef std::pair<uint64_t, uint64_t> GlobalOutput;
    typedef std::set<GlobalOutput> GlobalOutputsContainer;
    typedef std::unordered_map<Crypto::KeyImage, std::unordered_set<Crypto::Hash> > key_images_container;


    // double spending checking
    bool addTransactionInputs(const Crypto::Hash& id, const Transaction& tx, bool keptByBlock);
    bool haveSpentInputs(const Transaction& tx) const;
    bool removeTransactionInputs(const Crypto::Hash& id, const Transaction& tx, bool keptByBlock);

    tx_container_t::iterator removeTransaction(tx_container_t::iterator i);
    bool removeExpiredTransactions();
    bool is_transaction_ready_to_go(const Transaction& tx, TransactionCheckInfo& txd) const;

    void buildIndices();

    Tools::ObserverManager<ITxPoolObserver> m_observerManager;
    const Orbis::Currency& m_currency;
    OnceInTimeInterval m_txCheckInterval;
    mutable std::recursive_mutex m_transactions_lock;
    key_images_container m_spent_key_images;
    GlobalOutputsContainer m_spentOutputs;

    std::string m_config_folder;
    Orbis::ITransactionValidator& m_validator;
    Orbis::ITimeProvider& m_timeProvider;

    tx_container_t m_transactions;  
    tx_container_t::nth_index<1>::type& m_fee_index;
    std::unordered_map<Crypto::Hash, uint64_t> m_recentlyDeletedTransactions;

    Logging::LoggerRef logger;

    PaymentIdIndex m_paymentIdIndex;
    TimestampTransactionsIndex m_timestampIndex;
  };
}


