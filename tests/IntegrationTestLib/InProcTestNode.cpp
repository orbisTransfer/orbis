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
#include "InProcTestNode.h"

#include <future>

#include <Common/StringTools.h>
#include <Logging/ConsoleLogger.h>

#include "OrbisCore/Core.h"
#include "OrbisCore/CoreConfig.h"
#include "OrbisCore/Miner.h"
#include "OrbisProtocol/OrbisProtocolHandler.h"
#include "P2p/NetNode.h"
#include "InProcessNode/InProcessNode.h"

using namespace Orbis;

#undef ERROR

namespace Tests {

namespace {
bool parse_peer_from_string(NetworkAddress &pe, const std::string &node_addr) {
  return ::Common::parseIpAddressAndPort(pe.ip, pe.port, node_addr);
}
}


InProcTestNode::InProcTestNode(const TestNodeConfiguration& cfg, const Orbis::Currency& currency) : 
  m_cfg(cfg), m_currency(currency) {

  std::promise<std::string> initPromise;
  std::future<std::string> initFuture = initPromise.get_future();

  m_thread = std::thread(std::bind(&InProcTestNode::workerThread, this, std::ref(initPromise)));
  auto initError = initFuture.get();

  if (!initError.empty()) {
    m_thread.join();
    throw std::runtime_error(initError);
  }
}

InProcTestNode::~InProcTestNode() {
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

void InProcTestNode::workerThread(std::promise<std::string>& initPromise) {

  System::Dispatcher dispatcher;

  Logging::ConsoleLogger log;

  Logging::LoggerRef logger(log, "InProcTestNode");

  try {

    core.reset(new Orbis::core(m_currency, NULL, log));
    protocol.reset(new Orbis::OrbisProtocolHandler(m_currency, dispatcher, *core, NULL, log));
    p2pNode.reset(new Orbis::NodeServer(dispatcher, *protocol, log));
    protocol->set_p2p_endpoint(p2pNode.get());
    core->set_orbis_protocol(protocol.get());

    Orbis::NetNodeConfig p2pConfig;

    p2pConfig.setBindIp("127.0.0.1");
    p2pConfig.setBindPort(m_cfg.p2pPort);
    p2pConfig.setExternalPort(0);
    p2pConfig.setAllowLocalIp(false);
    p2pConfig.setHideMyPort(false);
    p2pConfig.setConfigFolder(m_cfg.dataDir);

    std::vector<NetworkAddress> exclusiveNodes;
    for (const auto& en : m_cfg.exclusiveNodes) {
      NetworkAddress na;
      parse_peer_from_string(na, en);
      exclusiveNodes.push_back(na);
    }

    p2pConfig.setExclusiveNodes(exclusiveNodes);

    if (!p2pNode->init(p2pConfig)) {
      throw std::runtime_error("Failed to init p2pNode");
    }

    Orbis::MinerConfig emptyMiner;
    Orbis::CoreConfig coreConfig;

    coreConfig.configFolder = m_cfg.dataDir;
    
    if (!core->init(coreConfig, emptyMiner, true)) {
      throw std::runtime_error("Core failed to initialize");
    }

    initPromise.set_value(std::string());

  } catch (std::exception& e) {
    logger(Logging::ERROR) << "Failed to initialize: " << e.what();
    initPromise.set_value(e.what());
    return;
  }

  try {
    p2pNode->run();
  } catch (std::exception& e) {
    logger(Logging::ERROR) << "exception in p2p::run: " << e.what();
  }

  core->deinit();
  p2pNode->deinit();
  core->set_orbis_protocol(NULL);
  protocol->set_p2p_endpoint(NULL);

  p2pNode.reset();
  protocol.reset();
  core.reset();
}

bool InProcTestNode::startMining(size_t threadsCount, const std::string &address) {
  assert(core.get());
  AccountPublicAddress addr;
  m_currency.parseAccountAddressString(address, addr);
  return core->get_miner().start(addr, threadsCount);
}

bool InProcTestNode::stopMining() {
  assert(core.get());
  return core->get_miner().stop();
}

bool InProcTestNode::stopDaemon() {
  if (!p2pNode.get()) {
    return false;
  }

  p2pNode->sendStopSignal();
  m_thread.join();
  return true;
}

bool InProcTestNode::getBlockTemplate(const std::string &minerAddress, Orbis::Block &blockTemplate, uint64_t &difficulty) {
  AccountPublicAddress addr;
  m_currency.parseAccountAddressString(minerAddress, addr);
  uint32_t height = 0;
  return core->get_block_template(blockTemplate, addr, difficulty, height, BinaryArray());
}

bool InProcTestNode::submitBlock(const std::string& block) {
  block_verification_context bvc = boost::value_initialized<block_verification_context>();
  core->handle_incoming_block_blob(Common::fromHex(block), bvc, true, true);
  return bvc.m_added_to_main_chain;
}

bool InProcTestNode::getTailBlockId(Crypto::Hash &tailBlockId) {
  tailBlockId = core->get_tail_id();
  return true;
}

bool InProcTestNode::makeINode(std::unique_ptr<Orbis::INode> &node) {

  std::unique_ptr<InProcessNode> inprocNode(new Orbis::InProcessNode(*core, *protocol));

  std::promise<std::error_code> p;
  auto future = p.get_future();

  inprocNode->init([&p](std::error_code ec) {
    std::promise<std::error_code> localPromise(std::move(p));
    localPromise.set_value(ec);
  });

  auto ec = future.get();

  if (!ec) {
    node = std::move(inprocNode);
    return true;
  }

  return false;
}

uint64_t InProcTestNode::getLocalHeight() {
  return core->get_current_blockchain_height();
}

}
