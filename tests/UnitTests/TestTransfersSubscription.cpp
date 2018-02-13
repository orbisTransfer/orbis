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
#include "gtest/gtest.h"
#include <tuple>

#include "OrbisCore/TransactionApi.h"
#include "Logging/ConsoleLogger.h"
#include "Transfers/TransfersSubscription.h"
#include "Transfers/TypeHelpers.h"
#include "ITransfersContainer.h"

#include "TransactionApiHelpers.h"
#include "TransfersObserver.h"

using namespace Orbis;

namespace {

const uint32_t UNCONFIRMED_TRANSACTION_HEIGHT = std::numeric_limits<uint32_t>::max();
const uint32_t UNCONFIRMED = std::numeric_limits<uint32_t>::max();

std::error_code createError() {
  return std::make_error_code(std::errc::invalid_argument);
}


class TransfersSubscriptionTest : public ::testing::Test {
public:

  TransfersSubscriptionTest() :
    currency(CurrencyBuilder(m_logger).currency()),
    account(generateAccountKeys()),
    syncStart(SynchronizationStart{ 0, 0 }),
    sub(currency, AccountSubscription{ account, syncStart, 10 }) {
    sub.addObserver(&observer);
  }

  std::shared_ptr<ITransactionReader> addTransaction(uint64_t amount, uint32_t height, uint32_t outputIndex) {
    TestTransactionBuilder b1;
    auto unknownSender = generateAccountKeys();
    b1.addTestInput(amount, unknownSender);
    auto outInfo = b1.addTestKeyOutput(amount, outputIndex, account);
    auto tx = std::shared_ptr<ITransactionReader>(b1.build().release());

    std::vector<TransactionOutputInformationIn> outputs = { outInfo };
    sub.addTransaction(TransactionBlockInfo{ height, 100000 }, *tx, outputs);
    return tx;
  }

  Logging::ConsoleLogger m_logger;
  Currency currency;
  AccountKeys account;
  SynchronizationStart syncStart;
  TransfersSubscription sub;
  TransfersObserver observer;
};
}



TEST_F(TransfersSubscriptionTest, getInitParameters) {
  ASSERT_EQ(syncStart.height, sub.getSyncStart().height);
  ASSERT_EQ(syncStart.timestamp, sub.getSyncStart().timestamp);
  ASSERT_EQ(account.address, sub.getAddress());
  ASSERT_EQ(account, sub.getKeys());
}

TEST_F(TransfersSubscriptionTest, addTransaction) {
  auto tx1 = addTransaction(10000, 1, 0);
  auto tx2 = addTransaction(10000, 2, 1);

  // this transaction should not be added, so no notification
  auto tx = createTransaction();
  addTestInput(*tx, 20000);
  sub.addTransaction(TransactionBlockInfo{ 2, 100000 }, *tx, {});

  ASSERT_EQ(2, sub.getContainer().transactionsCount());
  ASSERT_EQ(2, observer.updated.size());
  ASSERT_EQ(tx1->getTransactionHash(), observer.updated[0]);
  ASSERT_EQ(tx2->getTransactionHash(), observer.updated[1]);
}

TEST_F(TransfersSubscriptionTest, onBlockchainDetach) {
  addTransaction(10000, 10, 0);
  auto txHash = addTransaction(10000, 11, 1)->getTransactionHash();
  ASSERT_EQ(2, sub.getContainer().transactionsCount());
  
  sub.onBlockchainDetach(11);

  ASSERT_EQ(1, sub.getContainer().transactionsCount());
  ASSERT_EQ(1, observer.deleted.size());
  ASSERT_EQ(txHash, observer.deleted[0]);
}

TEST_F(TransfersSubscriptionTest, onError) {

  auto err = createError();

  addTransaction(10000, 10, 0);
  addTransaction(10000, 11, 1);

  ASSERT_EQ(2, sub.getContainer().transactionsCount());

  sub.onError(err, 12);

  ASSERT_EQ(2, sub.getContainer().transactionsCount());
  ASSERT_EQ(1, observer.errors.size());
  ASSERT_EQ(std::make_tuple(12, err), observer.errors[0]);

  sub.onError(err, 11);

  ASSERT_EQ(1, sub.getContainer().transactionsCount()); // one transaction should be detached
  ASSERT_EQ(2, observer.errors.size());

  ASSERT_EQ(std::make_tuple(12, err), observer.errors[0]);
  ASSERT_EQ(std::make_tuple(11, err), observer.errors[1]);
}

TEST_F(TransfersSubscriptionTest, advanceHeight) {
  ASSERT_TRUE(sub.advanceHeight(10));
  ASSERT_FALSE(sub.advanceHeight(9)); // can't go backwards
}


TEST_F(TransfersSubscriptionTest, markTransactionConfirmed) {
  auto txHash = addTransaction(10000, UNCONFIRMED_TRANSACTION_HEIGHT, UNCONFIRMED)->getTransactionHash();
  ASSERT_EQ(1, sub.getContainer().transactionsCount());
  ASSERT_EQ(1, observer.updated.size()); // added

  sub.markTransactionConfirmed(TransactionBlockInfo{ 10, 100000 }, txHash, { 1 });

  ASSERT_EQ(2, observer.updated.size()); // added + updated
  ASSERT_EQ(txHash, observer.updated[0]);
}

TEST_F(TransfersSubscriptionTest, deleteUnconfirmedTransaction) {
  auto txHash = addTransaction(10000, UNCONFIRMED_TRANSACTION_HEIGHT, UNCONFIRMED)->getTransactionHash();
  ASSERT_EQ(1, sub.getContainer().transactionsCount());

  sub.deleteUnconfirmedTransaction(txHash);

  ASSERT_EQ(0, sub.getContainer().transactionsCount());
  ASSERT_EQ(1, observer.deleted.size());
  ASSERT_EQ(txHash, observer.deleted[0]);
}
