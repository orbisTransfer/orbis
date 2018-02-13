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
#include "TestNetwork.h"

#include <fstream>
#include <boost/filesystem.hpp>

#include "OrbisConfig.h"
#include "InProcTestNode.h"
#include "RPCTestNode.h"

#ifdef _WIN32
const std::string daemonExec = std::string(Orbis::ORBIS_NAME) + "d.exe";
#else
const std::string daemonExec = std::string(Orbis::ORBIS_NAME) + "d";
#endif

namespace {

using namespace Tests;

void writeConfiguration(const std::string& confFile, const TestNodeConfiguration& cfg) {
  std::ofstream config(confFile, std::ios_base::trunc | std::ios_base::out);

  config
    << "rpc-bind-port=" << cfg.rpcPort << std::endl
    << "p2p-bind-port=" << cfg.p2pPort << std::endl
    << "log-level=4" << std::endl
    << "log-file=" << cfg.logFile << std::endl;

  for (const auto& ex : cfg.exclusiveNodes) {
    config << "add-exclusive-node=" << ex << std::endl;
  }
}

bool waitDaemonReady(TestNode& node) {
  for (size_t i = 0;; ++i) {
    if (node.getLocalHeight() > 0) {
      break;
    } else if (i < 2 * 60) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    } else {
      return false;
    }
  }
  return true;
}

void copyBlockchainFiles(bool testnet, const std::string& from, const std::string& to) {
  boost::filesystem::path fromPath(from);
  boost::filesystem::path toPath(to);

  auto files = {
    std::make_pair("blockindexes.dat", true),
    std::make_pair("blocks.dat", true),
    std::make_pair("blockscache.dat", false),
    std::make_pair("blockchainindices.dat", false)
  };

  for (const auto& item : files) {
    try {
      boost::filesystem::path filePath = std::string(testnet ? "testnet_" : "") + item.first;
      boost::filesystem::copy(fromPath / filePath, toPath / filePath);
    } catch (...) {
      if (item.second) { 
        // if file is required, the rethrow error
        throw;
      }
    }
  }
}

}


namespace Tests {


TestNetworkBuilder::TestNetworkBuilder(size_t nodeCount, Topology topology, uint16_t rpcBasePort, uint16_t p2pBasePort) :
  nodeCount(nodeCount), 
  topology(topology), 
  rpcBasePort(rpcBasePort), 
  p2pBasePort(p2pBasePort),
  baseDataDir("."),
  testnet(true)
{}

std::vector<TestNodeConfiguration> TestNetworkBuilder::build() {
  std::vector<TestNodeConfiguration> cfg;

  for (size_t i = 0; i < nodeCount; ++i) {
    cfg.push_back(buildNodeConfiguration(i));
  }

  return cfg;
}

TestNetworkBuilder& TestNetworkBuilder::setDataDirectory(const std::string& dataDir) {
  this->baseDataDir = dataDir;
  return *this;
}

TestNetworkBuilder& TestNetworkBuilder::setBlockchain(const std::string& blockchainDir) {
  this->blockchainLocation = blockchainDir;
  return *this;
}

TestNetworkBuilder& TestNetworkBuilder::setTestnet(bool isTestnet) {
  this->testnet = isTestnet;
  return *this;
}

TestNodeConfiguration TestNetworkBuilder::buildNodeConfiguration(size_t index) {
  TestNodeConfiguration cfg;

  if (!baseDataDir.empty()) {
    cfg.dataDir = baseDataDir + "/node" + std::to_string(index);
  }

  if (!blockchainLocation.empty()) {
    cfg.blockchainLocation = blockchainLocation;
  }

  cfg.daemonPath = daemonExec; // default
  cfg.testnet = testnet;
  cfg.logFile = std::string("test_") + Orbis::ORBIS_NAME + "d" + std::to_string(index) + ".log";

  uint16_t rpcPort = static_cast<uint16_t>(rpcBasePort + index);
  uint16_t p2pPort = static_cast<uint16_t>(p2pBasePort + index);

  cfg.p2pPort = p2pPort;
  cfg.rpcPort = rpcPort;

  switch (topology) {
  case Topology::Line:
    if (index != 0) {
      cfg.exclusiveNodes.push_back("127.0.0.1:" + std::to_string(p2pPort - 1));
    }
    break;

  case Topology::Ring: {
    uint16_t p2pExternalPort = static_cast<uint16_t>(p2pBasePort + (index + 1) % nodeCount);
    cfg.exclusiveNodes.push_back("127.0.0.1:" + std::to_string(p2pExternalPort));
    break;
  }

  case Topology::Star:
    if (index == 0) {
      for (size_t node = 1; node < nodeCount; ++node) {
        cfg.exclusiveNodes.push_back("127.0.0.1:" + std::to_string(p2pBasePort + node));
      }
    }
    break;
  }

  return cfg;

}

TestNetwork::TestNetwork(System::Dispatcher& dispatcher, const Orbis::Currency& currency) : 
  m_dispatcher(dispatcher),
  m_currency(currency) {
}

void TestNetwork::addNodes(const std::vector<TestNodeConfiguration>& nodes) {
  for (const auto& n : nodes) {
    addNode(n);
  }
}

void TestNetwork::addNode(const TestNodeConfiguration& cfg) {
  std::unique_ptr<TestNode> node;

  boost::system::error_code ec;
  boost::filesystem::remove_all(cfg.dataDir, ec);
  boost::filesystem::create_directory(cfg.dataDir);

  if (!cfg.blockchainLocation.empty()) {
    copyBlockchainFiles(cfg.testnet, cfg.blockchainLocation, cfg.dataDir);
  }

  switch (cfg.nodeType) {
  case NodeType::InProcess:
    node.reset(new InProcTestNode(cfg, m_currency));
    break;
  case NodeType::RPC:
    node = startDaemon(cfg);
    break;
  }

  nodes.push_back(std::make_pair(std::move(node), cfg));
}

void TestNetwork::waitNodesReady() {
  for (auto& node : nodes) {
    if (!waitDaemonReady(*node.first)) {
      throw std::runtime_error("Daemon startup failure (timeout)");
    }
  }
}

TestNode& TestNetwork::getNode(size_t index) {
  if (index >= nodes.size()) {
    throw std::runtime_error("Invalid node index");
  }

  return *nodes[index].first;
}

void TestNetwork::shutdown() {
  for (auto& node : nodes) {
    node.first->stopDaemon();
  }

  for (auto& daemon : m_daemons) {
    daemon.wait();
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));

  for (auto& node : nodes) {
    if (node.second.cleanupDataDir) {
      boost::filesystem::remove_all(node.second.dataDir);
    }
  }

}


std::unique_ptr<TestNode> TestNetwork::startDaemon(const TestNodeConfiguration& cfg) {
  if (!boost::filesystem::exists(cfg.daemonPath)) {
    throw std::runtime_error("daemon binary wasn't found");
  }

  writeConfiguration(cfg.dataDir + "/daemon.conf", cfg);

  Process process;
  std::vector<std::string> daemonArgs = { "--data-dir=" + cfg.dataDir, "--config-file=daemon.conf" };

  if (cfg.testnet) {
    daemonArgs.emplace_back("--testnet");
  }

  process.startChild(cfg.daemonPath, daemonArgs);

  std::unique_ptr<TestNode> node(new RPCTestNode(cfg.rpcPort, m_dispatcher));
  m_daemons.push_back(process);

  return node;
}





}
