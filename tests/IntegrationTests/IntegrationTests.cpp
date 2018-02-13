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
#include <Logging/LoggerRef.h>

#include "../IntegrationTestLib/BaseFunctionalTests.h"
#include "../IntegrationTestLib/NodeObserver.h"

#include "WalletLegacy/WalletLegacy.h"
#include "WalletLegacyObserver.h"

using namespace Orbis;
using namespace Logging;

extern Tests::Common::BaseFunctionalTestsConfig baseCfg;
// extern System::Dispatcher globalDispatcher;

struct TotalWalletBalance {

  TotalWalletBalance(uint64_t actual_ = 0, uint64_t pending_ = 0) 
    : actual(actual_), pending(pending_) {}

  TotalWalletBalance(IWalletLegacy& wallet) 
    : TotalWalletBalance(wallet.actualBalance(), wallet.pendingBalance()) {}

  uint64_t actual = 0;
  uint64_t pending = 0;

  uint64_t total() const {
    return actual + pending;
  }
};

class IntegrationTest : public Tests::Common::BaseFunctionalTests, public ::testing::Test {
public:

  IntegrationTest() : 
    currency(Orbis::CurrencyBuilder(log).testnet(true).currency()), 
    BaseFunctionalTests(currency, dispatcher, baseCfg),
    logger(log, "IntegrationTest") {
  }

  ~IntegrationTest() {
    wallets.clear();
    inodes.clear();

    stopTestnet();
  }

  void makeINodes() {
    for (auto& n : nodeDaemons) {
      std::unique_ptr<INode> node;
      n->makeINode(node);
      inodes.push_back(std::move(node));
    }
  }

  void makeWallets() {
    for (auto& n: inodes) {
      std::unique_ptr<Orbis::IWalletLegacy> wallet(new Orbis::WalletLegacy(m_currency, *n));
      std::unique_ptr<WalletLegacyObserver> observer(new WalletLegacyObserver());

      wallet->initAndGenerate(walletPassword);
      wallet->addObserver(observer.get());

      wallets.push_back(std::move(wallet));
      walletObservers.push_back(std::move(observer));
    }
  }

  void mineBlocksFor(size_t node, const std::string& address, size_t blockCount) {
    auto prevHeight = nodeDaemons[node]->getLocalHeight();
    nodeDaemons[node]->startMining(1, address);

    do {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    } while (prevHeight + blockCount < nodeDaemons[node]->getLocalHeight());

    nodeDaemons[node]->stopMining();
  }

  void printWalletBalances() {
    for (auto& w: wallets) {
      logger(INFO) << "Wallet " << w->getAddress().substr(0, 6);
      logger(INFO) << "  " << currency.formatAmount(w->actualBalance()) << " actual / " << currency.formatAmount(w->pendingBalance()) << " pending";
    }
  }

  void mineEmptyBlocks(size_t nodeNum, size_t blocksCount) {
  }

  void mineMoneyForWallet(size_t nodeNum, size_t walletNum) {
    auto& wallet = *wallets[walletNum];
    auto& node = *nodeDaemons[nodeNum];

    node.startMining(1, wallet.getAddress());
    walletObservers[walletNum]->waitActualBalanceChange();
    node.stopMining();

    while (node.getLocalHeight() > walletObservers[walletNum]->getCurrentHeight()) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

  std::error_code transferMoney(size_t srcWallet, size_t dstWallet, uint64_t amount, uint64_t fee) {
    logger(INFO) 
      << "Transferring from " << wallets[srcWallet]->getAddress().substr(0, 6) 
      << " to " << wallets[dstWallet]->getAddress().substr(0, 6) << " " << currency.formatAmount(amount);

    Orbis::WalletLegacyTransfer tr;
    tr.address = wallets[dstWallet]->getAddress();
    tr.amount = amount;
    std::error_code result;

    auto txId = wallets[srcWallet]->sendTransaction(tr, fee);

    logger(DEBUGGING) << "Transaction id = " << txId;

    return walletObservers[srcWallet]->waitSendResult(txId);
  }

  void checkIncomingTransfer(size_t dstWallet, uint64_t amount) {
    startMining(1);

    auto txId = walletObservers[dstWallet]->waitExternalTransaction();

    stopMining();

    WalletLegacyTransaction txInfo;

    ASSERT_TRUE(wallets[dstWallet]->getTransaction(txId, txInfo));
    ASSERT_EQ(txInfo.totalAmount, amount);
  }

  System::Dispatcher dispatcher;
  std::string walletPassword = "pass";
  Orbis::Currency currency;
  Logging::ConsoleLogger log;
  Logging::LoggerRef logger;

  std::vector<std::unique_ptr<INode>> inodes;
  std::vector<std::unique_ptr<IWalletLegacy>> wallets;
  std::vector<std::unique_ptr<WalletLegacyObserver>> walletObservers;
};



TEST_F(IntegrationTest, Wallet2Wallet) {
  const uint64_t FEE = 1000000;

  launchTestnet(2);

  logger(INFO) << "Testnet launched";

  makeINodes();
  makeWallets();

  logger(INFO) << "Created wallets";

  mineMoneyForWallet(0, 0);

  logger(INFO) << "Mined money";

  printWalletBalances();

  TotalWalletBalance w0pre(*wallets[0]);
  TotalWalletBalance w1pre(*wallets[1]);

  auto sendAmount = w0pre.actual / 2;

  ASSERT_TRUE(!transferMoney(0, 1, sendAmount, currency.minimumFee()));
  ASSERT_NO_FATAL_FAILURE(checkIncomingTransfer(1, sendAmount));

  printWalletBalances();

  TotalWalletBalance w0after(*wallets[0]);
  TotalWalletBalance w1after(*wallets[1]);

  // check total 
  ASSERT_EQ(w0pre.total() + w1pre.total() - currency.minimumFee(), w0after.total() + w1after.total());

  // check diff
  ASSERT_EQ(sendAmount, w1after.total() - w1pre.total());
}

TEST_F(IntegrationTest, BlockPropagationSpeed) {

  launchTestnet(3, Line);
  makeINodes();

  {
    std::unique_ptr<Orbis::INode>& localNode = inodes.front();
    std::unique_ptr<Orbis::INode>& remoteNode = inodes.back();

    std::unique_ptr<Orbis::IWalletLegacy> wallet;
    makeWallet(wallet, localNode);

    NodeObserver localObserver(*localNode);
    NodeObserver remoteObserver(*remoteNode);

    const size_t BLOCKS_COUNT = 10;

    nodeDaemons.front()->startMining(1, wallet->getAddress());

    for (size_t blockNumber = 0; blockNumber < BLOCKS_COUNT; ++blockNumber) {
      uint32_t localHeight = localObserver.waitLastKnownBlockHeightUpdated();
      uint32_t remoteHeight = 0;

      while (remoteHeight != localHeight) {
        ASSERT_TRUE(remoteObserver.waitLastKnownBlockHeightUpdated(std::chrono::milliseconds(5000), remoteHeight));
      }

      logger(INFO) << "Iteration " << blockNumber + 1 << ": " << "height = " << localHeight;
    }

    nodeDaemons.front()->stopMining();
  }
}
