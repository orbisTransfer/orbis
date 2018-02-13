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
#include <numeric>

#include <System/Timer.h>
#include <Common/StringTools.h>
#include <Logging/ConsoleLogger.h>

#include "PaymentGate/WalletService.h"
#include "PaymentGate/WalletFactory.h"

// test helpers
#include "INodeStubs.h"
#include "TestBlockchainGenerator.h"

using namespace PaymentService;
using namespace Orbis;

class PaymentGateTest : public testing::Test {
public:

  PaymentGateTest() : 
    currency(Orbis::CurrencyBuilder(logger).currency()), 
    generator(currency),
    nodeStub(generator) 
  {}

  WalletConfiguration createWalletConfiguration(const std::string& walletFile = "pgwalleg.bin") const {
    return WalletConfiguration{ walletFile, "pass" };
  }

  std::unique_ptr<WalletService> createWalletService(const WalletConfiguration& cfg) {
    wallet.reset(WalletFactory::createWallet(currency, nodeStub, dispatcher));
    std::unique_ptr<WalletService> service(new WalletService(currency, dispatcher, nodeStub, *wallet, cfg, logger));
    service->init();
    return service;
  }

  void generateWallet(const WalletConfiguration& conf) {
    unlink(conf.walletFile.c_str());
    generateNewWallet(currency, conf, logger, dispatcher);
  }

protected:  
  Logging::ConsoleLogger logger;
  Orbis::Currency currency;
  TestBlockchainGenerator generator;
  INodeTrivialRefreshStub nodeStub;
  System::Dispatcher dispatcher;

  std::unique_ptr<Orbis::IWallet> wallet;
};


TEST_F(PaymentGateTest, createWallet) {
  auto cfg = createWalletConfiguration();
  generateWallet(cfg);
  auto service = createWalletService(cfg);
}

TEST_F(PaymentGateTest, addTransaction) {
  auto cfg = createWalletConfiguration();
  generateWallet(cfg);
  auto service = createWalletService(cfg);

  std::string addressStr;
  ASSERT_TRUE(!service->createAddress(addressStr));

  AccountPublicAddress address;
  ASSERT_TRUE(currency.parseAccountAddressString(addressStr, address));

  generator.getBlockRewardForAddress(address);
  generator.getBlockRewardForAddress(address);
  generator.generateEmptyBlocks(11);
  generator.getBlockRewardForAddress(address);

  nodeStub.updateObservers();

  System::Timer(dispatcher).sleep(std::chrono::seconds(2));

  uint64_t pending = 0, actual = 0;

  service->getBalance(actual, pending);

  ASSERT_NE(0, pending);
  ASSERT_NE(0, actual);

  ASSERT_EQ(pending * 2, actual);
}

/*
TEST_F(PaymentGateTest, DISABLED_sendTransaction) {

  auto cfg = createWalletConfiguration();
  generateWallet(cfg);
  auto service = createWalletService(cfg);

  std::string addressStr;
  ASSERT_TRUE(!service->createAddress(addressStr));

  AccountPublicAddress address;
  ASSERT_TRUE(currency.parseAccountAddressString(addressStr, address));

  generator.getBlockRewardForAddress(address);
  generator.generateEmptyBlocks(11);

  nodeStub.updateObservers();

  System::Timer(dispatcher).sleep(std::chrono::seconds(5));

  auto cfg2 = createWalletConfiguration("pgwallet2.bin");
  generateWallet(cfg2);
  auto serviceRecv = createWalletService(cfg2);

  std::string recvAddress;
  serviceRecv->createAddress(recvAddress);

  uint64_t TEST_AMOUNT = 0;
  currency.parseAmount("100000.0", TEST_AMOUNT);

  Crypto::Hash paymentId;
  std::iota(reinterpret_cast<char*>(&paymentId), reinterpret_cast<char*>(&paymentId) + sizeof(paymentId), 0);
  std::string paymentIdStr = Common::podToHex(paymentId);

  uint64_t txId = 0;

  {
    SendTransaction::Request req;
    SendTransaction::Response res;

    req.transfers.push_back(WalletRpcOrder{ TEST_AMOUNT, recvAddress });
    req.fee = currency.minimumFee();
    req.anonymity = 1;
    req.unlockTime = 0;
    req.paymentId = paymentIdStr;

    ASSERT_TRUE(!service->sendTransaction(req, res.transactionHash));

    txId = res.transactionId;
  }

  generator.generateEmptyBlocks(11);

  nodeStub.updateObservers();

  System::Timer(dispatcher).sleep(std::chrono::seconds(5));

  TransactionRpcInfo txInfo;
  bool found = false;

  ASSERT_TRUE(!service->getTransaction(txId, found, txInfo));
  ASSERT_TRUE(found);

  uint64_t recvTxCount = 0;
  ASSERT_TRUE(!serviceRecv->getTransactionsCount(recvTxCount));
  ASSERT_EQ(1, recvTxCount);

  uint64_t sendTxCount = 0;
  ASSERT_TRUE(!service->getTransactionsCount(sendTxCount));
  ASSERT_EQ(2, sendTxCount); // 1 from mining, 1 transfer

  TransactionRpcInfo recvTxInfo;
  ASSERT_TRUE(!serviceRecv->getTransaction(0, found, recvTxInfo));
  ASSERT_TRUE(found);

  ASSERT_EQ(txInfo.hash, recvTxInfo.hash);
  ASSERT_EQ(txInfo.extra, recvTxInfo.extra);
  ASSERT_EQ(-txInfo.totalAmount - currency.minimumFee(), recvTxInfo.totalAmount);
  ASSERT_EQ(txInfo.blockHeight, recvTxInfo.blockHeight);

  {
    // check payments
    WalletService::IncomingPayments payments;
    ASSERT_TRUE(!serviceRecv->getIncomingPayments({ paymentIdStr }, payments));

    ASSERT_EQ(1, payments.size());

    ASSERT_EQ(paymentIdStr, payments.begin()->first);

    const auto& recvPayment = payments.begin()->second;

    ASSERT_EQ(1, recvPayment.size());

    ASSERT_EQ(txInfo.hash, recvPayment[0].txHash);
    ASSERT_EQ(TEST_AMOUNT, recvPayment[0].amount);
    ASSERT_EQ(txInfo.blockHeight, recvPayment[0].blockHeight);
  }

  // reload services

  service->saveWallet();
  serviceRecv->saveWallet();

  service.reset();
  serviceRecv.reset();

  service = createWalletService(cfg);
  serviceRecv = createWalletService(cfg2);

  recvTxInfo = boost::value_initialized<TransactionRpcInfo>();
  ASSERT_TRUE(!serviceRecv->getTransaction(0, found, recvTxInfo));
  ASSERT_TRUE(found);

  ASSERT_EQ(txInfo.hash, recvTxInfo.hash);
  ASSERT_EQ(txInfo.extra, recvTxInfo.extra);
  ASSERT_EQ(-txInfo.totalAmount - currency.minimumFee(), recvTxInfo.totalAmount);
  ASSERT_EQ(txInfo.blockHeight, recvTxInfo.blockHeight);

  // send some money back
  std::reverse(paymentIdStr.begin(), paymentIdStr.end());

  {
    std::string recvAddress;
    service->createAddress(recvAddress);

    SendTransactionRequest req;
    SendTransactionResponse res;

    req.destinations.push_back(TransferDestination{ TEST_AMOUNT/2, recvAddress });
    req.fee = currency.minimumFee();
    req.mixin = 1;
    req.unlockTime = 0;
    req.paymentId = paymentIdStr;

    ASSERT_TRUE(!serviceRecv->sendTransaction(req, res));

    txId = res.transactionId;
  }

  generator.generateEmptyBlocks(11);
  nodeStub.updateObservers();

  System::Timer(dispatcher).sleep(std::chrono::seconds(5));

  ASSERT_TRUE(!service->getTransactionsCount(recvTxCount));
  ASSERT_EQ(3, recvTxCount);

  {
    WalletService::IncomingPayments payments;
    ASSERT_TRUE(!service->getIncomingPayments({ paymentIdStr }, payments));
    ASSERT_EQ(1, payments.size());
    ASSERT_EQ(paymentIdStr, payments.begin()->first);

    const auto& recvPayment = payments.begin()->second;

    ASSERT_EQ(1, recvPayment.size());
    ASSERT_EQ(TEST_AMOUNT / 2, recvPayment[0].amount);
  }
} */
