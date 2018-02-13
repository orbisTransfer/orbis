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
#include "RPCTestNode.h"

#include <future>
#include <vector>
#include <thread>

#include "Common/StringTools.h"
#include "OrbisCore/OrbisTools.h"
#include "NodeRpcProxy/NodeRpcProxy.h"
#include "Rpc/CoreRpcServerCommandsDefinitions.h"
#include "Rpc/HttpClient.h"
#include "Rpc/JsonRpc.h"

#include "Logger.h"
#include "NodeCallback.h"

using namespace Orbis;
using namespace System;

namespace Tests {

RPCTestNode::RPCTestNode(uint16_t port, System::Dispatcher& d) : 
  m_rpcPort(port), m_dispatcher(d), m_httpClient(d, "127.0.0.1", port) {
}

bool RPCTestNode::startMining(size_t threadsCount, const std::string& address) { 
  LOG_DEBUG("startMining()");

  try {
    COMMAND_RPC_START_MINING::request req;
    COMMAND_RPC_START_MINING::response resp;
    req.miner_address = address;
    req.threads_count = threadsCount;

    invokeJsonCommand(m_httpClient, "/start_mining", req, resp);
    if (resp.status != CORE_RPC_STATUS_OK) {
      throw std::runtime_error(resp.status);
    }
  } catch (std::exception& e) {
    std::cout << "startMining() RPC call fail: " << e.what();
    return false;
  }

  return true;
}

bool RPCTestNode::getBlockTemplate(const std::string& minerAddress, Orbis::Block& blockTemplate, uint64_t& difficulty) {
  LOG_DEBUG("getBlockTemplate()");

  try {
    COMMAND_RPC_GETBLOCKTEMPLATE::request req;
    COMMAND_RPC_GETBLOCKTEMPLATE::response rsp;
    req.wallet_address = minerAddress;
    req.reserve_size = 0;

    JsonRpc::invokeJsonRpcCommand(m_httpClient, "getblocktemplate", req, rsp);
    if (rsp.status != CORE_RPC_STATUS_OK) {
      throw std::runtime_error(rsp.status);
    }

    difficulty = rsp.difficulty;

    BinaryArray blockBlob = (::Common::fromHex(rsp.blocktemplate_blob));
    return fromBinaryArray(blockTemplate, blockBlob);
  } catch (std::exception& e) {
    LOG_ERROR("JSON-RPC call startMining() failed: " + std::string(e.what()));
    return false;
  }

  return true;
}

bool RPCTestNode::submitBlock(const std::string& block) {
  LOG_DEBUG("submitBlock()");

  try {
    COMMAND_RPC_SUBMITBLOCK::request req;
    COMMAND_RPC_SUBMITBLOCK::response res;
    req.push_back(block);
    JsonRpc::invokeJsonRpcCommand(m_httpClient, "submitblock", req, res);
    if (res.status != CORE_RPC_STATUS_OK) {
      throw std::runtime_error(res.status);
    }
  } catch (std::exception& e) {
    LOG_ERROR("RPC call of submit_block returned error: " + std::string(e.what()));
    return false;
  }

  return true;
}

bool RPCTestNode::stopMining() { 
  LOG_DEBUG("stopMining()");

  try {
    COMMAND_RPC_STOP_MINING::request req;
    COMMAND_RPC_STOP_MINING::response resp;
    invokeJsonCommand(m_httpClient, "/stop_mining", req, resp);
    if (resp.status != CORE_RPC_STATUS_OK) {
      throw std::runtime_error(resp.status);
    }
  } catch (std::exception& e) {
    std::cout << "stopMining() RPC call fail: " << e.what();
    return false;
  }

  return true;
}

bool RPCTestNode::getTailBlockId(Crypto::Hash& tailBlockId) {
  LOG_DEBUG("getTailBlockId()");

  try {
    COMMAND_RPC_GET_LAST_BLOCK_HEADER::request req;
    COMMAND_RPC_GET_LAST_BLOCK_HEADER::response rsp;
    JsonRpc::invokeJsonRpcCommand(m_httpClient, "getlastblockheader", req, rsp);
    if (rsp.status != CORE_RPC_STATUS_OK) {
      throw std::runtime_error(rsp.status);
    }

    return ::Common::podFromHex(rsp.block_header.hash, tailBlockId);
  } catch (std::exception& e) {
    LOG_ERROR("JSON-RPC call getTailBlockId() failed: " + std::string(e.what()));
    return false;
  }

  return true;
}

bool RPCTestNode::makeINode(std::unique_ptr<Orbis::INode>& node) {
  std::unique_ptr<Orbis::INode> newNode(new Orbis::NodeRpcProxy("127.0.0.1", m_rpcPort));
  NodeCallback cb;
  newNode->init(cb.callback());
  auto ec = cb.get();

  if (ec) {
    LOG_ERROR("init error: " + ec.message() + ':' + TO_STRING(ec.value()));
    return false;
  }

  LOG_DEBUG("NodeRPCProxy on port " + TO_STRING(m_rpcPort) + " initialized");
  node = std::move(newNode);
  return true;
}

bool RPCTestNode::stopDaemon() {
  try {
    LOG_DEBUG("stopDaemon()");
    COMMAND_RPC_STOP_DAEMON::request req;
    COMMAND_RPC_STOP_DAEMON::response resp;
    invokeJsonCommand(m_httpClient, "/stop_daemon", req, resp);
    if (resp.status != CORE_RPC_STATUS_OK) {
      throw std::runtime_error(resp.status);
    }
  } catch (std::exception& e) {
    std::cout << "stopDaemon() RPC call fail: " << e.what();
    return false;
  }

  return true;
}

uint64_t RPCTestNode::getLocalHeight() {
  try {
    Orbis::COMMAND_RPC_GET_INFO::request req;
    Orbis::COMMAND_RPC_GET_INFO::response rsp;
    invokeJsonCommand(m_httpClient, "/getinfo", req, rsp);
    if (rsp.status == CORE_RPC_STATUS_OK) {
      return rsp.height;
    }
  } catch (std::exception&) {
  }

  return 0;
}

}
