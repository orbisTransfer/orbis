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

#include "Logging/LoggerManager.h"
#include "System/Dispatcher.h"
#include "System/InterruptedException.h"

#include "../IntegrationTestLib/BaseFunctionalTests.h"
#include "../IntegrationTestLib/TestWalletLegacy.h"


using namespace Orbis;
using namespace Crypto;
using namespace Tests::Common;

extern System::Dispatcher globalSystem;
extern Tests::Common::BaseFunctionalTestsConfig config;

namespace {
  class NodeRpcProxyTest : public Tests::Common::BaseFunctionalTests, public ::testing::Test {
  public:
    NodeRpcProxyTest() :
      BaseFunctionalTests(m_currency, globalSystem, config),
      m_currency(CurrencyBuilder(m_logManager).testnet(true).currency()) {
    }

  protected:
    Logging::LoggerManager m_logManager;
    Orbis::Currency m_currency;
  };

  class PoolChangedObserver : public INodeObserver {
  public:
    virtual void poolChanged() override {
      std::unique_lock<std::mutex> lk(mutex);
      ready = true;
      cv.notify_all();
    }

    bool waitPoolChanged(size_t seconds) {
      std::unique_lock<std::mutex> lk(mutex);
      bool r = cv.wait_for(lk, std::chrono::seconds(seconds), [this]() { return ready; });
      ready = false;
      return r;
    }

  private:
    std::mutex mutex;
    std::condition_variable cv;
    bool ready = false;
  };

  TEST_F(NodeRpcProxyTest, PoolChangedCalledWhenTxCame) {
    const size_t NODE_0 = 0;
    const size_t NODE_1 = 1;

    launchTestnet(2, Tests::Common::BaseFunctionalTests::Line);

    std::unique_ptr<Orbis::INode> node0;
    std::unique_ptr<Orbis::INode> node1;

    nodeDaemons[NODE_0]->makeINode(node0);
    nodeDaemons[NODE_1]->makeINode(node1);

    TestWalletLegacy wallet1(m_dispatcher, m_currency, *node0);
    TestWalletLegacy wallet2(m_dispatcher, m_currency, *node1);

    ASSERT_FALSE(static_cast<bool>(wallet1.init()));
    ASSERT_FALSE(static_cast<bool>(wallet2.init()));

    ASSERT_TRUE(mineBlocks(*nodeDaemons[NODE_0], wallet1.address(), 1));
    ASSERT_TRUE(mineBlocks(*nodeDaemons[NODE_0], wallet1.address(), m_currency.minedMoneyUnlockWindow()));

    wallet1.waitForSynchronizationToHeight(static_cast<uint32_t>(m_currency.minedMoneyUnlockWindow()) + 1);
    wallet2.waitForSynchronizationToHeight(static_cast<uint32_t>(m_currency.minedMoneyUnlockWindow()) + 1);

    PoolChangedObserver observer;
    node0->addObserver(&observer);

    Hash dontCare;
    ASSERT_FALSE(static_cast<bool>(wallet1.sendTransaction(m_currency.accountAddressAsString(wallet2.address()), m_currency.coin(), dontCare)));
    ASSERT_TRUE(observer.waitPoolChanged(10));
  }
}
