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

#include <list>
#include <vector>
#include <memory>
#include <mutex>              
#include <condition_variable> 
#include <queue>

#include <boost/noncopyable.hpp>
#include <boost/program_options.hpp>

#include <System/Dispatcher.h>
#include <Logging/ConsoleLogger.h>

#include "OrbisCore/Currency.h"
#include "IWalletLegacy.h"
#include "INode.h"
#include "TestNode.h"
#include "NetworkConfiguration.h"

namespace Tests {
  namespace Common {

  namespace po = boost::program_options;
    class Semaphore{
    private:
      std::mutex mtx;
      std::condition_variable cv;
      bool available;

    public:
      Semaphore() : available(false) { }

      void notify() {
        std::unique_lock<std::mutex> lck(mtx);
        available = true;
        cv.notify_one();
      }

      void wait() {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck, [this](){ return available; });
        available = false;
      }

      bool wait_for(const std::chrono::milliseconds& rel_time) {
        std::unique_lock<std::mutex> lck(mtx);
        auto result = cv.wait_for(lck, rel_time, [this](){ return available; });
        available = false;
        return result;
      }
    };

    const uint16_t P2P_FIRST_PORT = 9000;
    const uint16_t RPC_FIRST_PORT = 9200;


    class BaseFunctionalTestsConfig {
    public:
      BaseFunctionalTestsConfig() {}

      void init(po::options_description& desc) {
        desc.add_options()
          ("daemon-dir,d", po::value<std::string>()->default_value("."), "path to daemon")
          ("data-dir,n", po::value<std::string>()->default_value("."), "path to daemon's data directory")
          ("add-daemons,a", po::value<std::vector<std::string>>()->multitoken(), "add daemon to topology");
      }

      bool handleCommandLine(const po::variables_map& vm) {
        if (vm.count("daemon-dir")) {
          daemonDir = vm["daemon-dir"].as<std::string>();
        }

        if (vm.count("data-dir")) {
          dataDir = vm["data-dir"].as<std::string>();
        }

        if (vm.count("add-daemons")) {
          daemons = vm["add-daemons"].as<std::vector<std::string>>();
        }

        return true;
      }

      std::string daemonDir;
      std::string dataDir;
      std::vector<std::string> daemons;
    };



    class BaseFunctionalTests : boost::noncopyable {
    public:
      BaseFunctionalTests(const Orbis::Currency& currency, System::Dispatcher& d, const BaseFunctionalTestsConfig& config) :
          m_dispatcher(d),
          m_currency(currency),
          m_nextTimestamp(time(nullptr) - 365 * 24 * 60 * 60),
          m_config(config),
          m_dataDir(config.dataDir),
          m_daemonDir(config.daemonDir),
          m_testnetSize(1) {
        if (m_dataDir.empty()) {
          m_dataDir = ".";
        }
        if (m_daemonDir.empty()) {
          m_daemonDir = ".";
        }
      };

      ~BaseFunctionalTests();

      enum Topology {
        Ring,
        Line,
        Star
      };

    protected:

      TestNodeConfiguration createNodeConfiguration(size_t i);

      std::vector< std::unique_ptr<TestNode> > nodeDaemons;
      System::Dispatcher& m_dispatcher;
      const Orbis::Currency& m_currency;

      void launchTestnet(size_t count, Topology t = Line);
      void launchTestnetWithInprocNode(size_t count, Topology t = Line);
      void launchInprocTestnet(size_t count, Topology t = Line);
      void stopTestnet();

      void startNode(size_t index);
      void stopNode(size_t index);

      bool makeWallet(std::unique_ptr<Orbis::IWalletLegacy> & wallet, std::unique_ptr<Orbis::INode>& node, const std::string& password = "pass");
      bool mineBlocks(TestNode& node, const Orbis::AccountPublicAddress& address, size_t blockCount);
      bool mineBlock(std::unique_ptr<Orbis::IWalletLegacy>& wallet);
      bool mineBlock();
      bool startMining(size_t threads);
      bool stopMining();

      bool getNodeTransactionPool(size_t nodeIndex, Orbis::INode& node, std::vector<std::unique_ptr<Orbis::ITransactionReader>>& txPool);

      bool waitDaemonsReady();
      bool waitDaemonReady(size_t nodeIndex);
      bool waitForPeerCount(Orbis::INode& node, size_t expectedPeerCount);
      bool waitForPoolSize(size_t nodeIndex, Orbis::INode& node, size_t expectedPoolSize,
        std::vector<std::unique_ptr<Orbis::ITransactionReader>>& txPool);

      bool prepareAndSubmitBlock(TestNode& node, Orbis::Block&& blockTemplate);

    private:
#ifdef __linux__
      std::vector<__pid_t> pids;
#endif


      Logging::ConsoleLogger logger;
      std::unique_ptr<Orbis::INode> mainNode;
      std::unique_ptr<Orbis::IWalletLegacy> workingWallet;
      uint64_t m_nextTimestamp;
      Topology m_topology;
      size_t m_testnetSize;

      BaseFunctionalTestsConfig m_config;
      std::string m_dataDir;
      std::string m_daemonDir;
      uint16_t m_mainDaemonRPCPort;
    };
  }
}
