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
#include <chrono>
#include <thread>

#include <Logging/LoggerRef.h>
#include <Logging/ConsoleLogger.h>

#include "NodeRpcProxy/NodeRpcProxy.h"

using namespace Orbis;
using namespace Logging;

#undef ERROR

class NodeObserver : public INodeObserver {
public:
  NodeObserver(const std::string& name, NodeRpcProxy& nodeProxy, ILogger& log)
    : m_name(name)
    , m_nodeProxy(nodeProxy)
    , logger(log, "NodeObserver:" + name) {
  }

  virtual ~NodeObserver() {
  }

  virtual void peerCountUpdated(size_t count) override {
    logger(INFO) << '[' << m_name << "] peerCountUpdated " << count << " = " << m_nodeProxy.getPeerCount();
  }

  virtual void localBlockchainUpdated(uint32_t height) override {
    logger(INFO) << '[' << m_name << "] localBlockchainUpdated " << height << " = " << m_nodeProxy.getLastLocalBlockHeight();

    std::vector<uint64_t> amounts;
    amounts.push_back(100000000);
    auto outs = std::make_shared<std::vector<COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>>();
    m_nodeProxy.getRandomOutsByAmounts(std::move(amounts), 10, *outs.get(), [outs, this](std::error_code ec) {
      if (!ec) {
        if (1 == outs->size() && 10 == (*outs)[0].outs.size()) {
          logger(INFO) << "getRandomOutsByAmounts called successfully";
        } else {
          logger(ERROR) << "getRandomOutsByAmounts returned invalid result";
        }
      } else {
        logger(ERROR) << "failed to call getRandomOutsByAmounts: " << ec.message() << ':' << ec.value();
      }
    });
  }

  virtual void lastKnownBlockHeightUpdated(uint32_t height) override {
    logger(INFO) << '[' << m_name << "] lastKnownBlockHeightUpdated " << height << " = " << m_nodeProxy.getLastKnownBlockHeight();
  }

private:
  LoggerRef logger;
  std::string m_name;
  NodeRpcProxy& m_nodeProxy;
};

int main(int argc, const char** argv) {

  Logging::ConsoleLogger log;
  Logging::LoggerRef logger(log, "main");
  NodeRpcProxy nodeProxy("127.0.0.1", 18081);

  NodeObserver observer1("obs1", nodeProxy, log);
  NodeObserver observer2("obs2", nodeProxy, log);

  nodeProxy.addObserver(&observer1);
  nodeProxy.addObserver(&observer2);

  nodeProxy.init([&](std::error_code ec) {
    if (ec) {
      logger(ERROR) << "init error: " << ec.message() << ':' << ec.value();
    } else {
      logger(INFO, BRIGHT_GREEN) << "initialized";
    }
  });

  //nodeProxy.init([](std::error_code ec) {
  //  if (ec) {
  //    logger(ERROR) << "init error: " << ec.message() << ':' << ec.value();
  //  } else {
  //    LOG_PRINT_GREEN("initialized", LOG_LEVEL_0);
  //  }
  //});

  std::this_thread::sleep_for(std::chrono::seconds(5));
  if (nodeProxy.shutdown()) {
    logger(INFO, BRIGHT_GREEN) << "shutdown";
  } else {
    logger(ERROR) << "shutdown error";
  }

  nodeProxy.init([&](std::error_code ec) {
    if (ec) {
      logger(ERROR) << "init error: " << ec.message() << ':' << ec.value();
    } else {
      logger(INFO, BRIGHT_GREEN) << "initialized";
    }
  });

  std::this_thread::sleep_for(std::chrono::seconds(5));
  if (nodeProxy.shutdown()) {
    logger(INFO, BRIGHT_GREEN) << "shutdown";
  } else {
    logger(ERROR) << "shutdown error";
  }

  Orbis::Transaction tx;
  nodeProxy.relayTransaction(tx, [&](std::error_code ec) {
    if (!ec) {
      logger(INFO) << "relayTransaction called successfully";
    } else {
      logger(ERROR) << "failed to call relayTransaction: " << ec.message() << ':' << ec.value();
    }
  });

  nodeProxy.init([&](std::error_code ec) {
    if (ec) {
      logger(ERROR) << "init error: " << ec.message() << ':' << ec.value();
    } else {
      logger(INFO, BRIGHT_GREEN) << "initialized";
    }
  });

  std::this_thread::sleep_for(std::chrono::seconds(5));
  nodeProxy.relayTransaction(tx, [&](std::error_code ec) {
    if (!ec) {
      logger(INFO) << "relayTransaction called successfully";
    } else {
      logger(ERROR) << "failed to call relayTransaction: " << ec.message() << ':' << ec.value();
    }
  });

  std::this_thread::sleep_for(std::chrono::seconds(60));
}
