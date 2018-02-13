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
#include "P2pConnectionProxy.h"

#include "LevinProtocol.h"
#include "P2pContext.h"
#include "P2pNode.h"

#include <System/InterruptedException.h>

using namespace System;

namespace Orbis {

P2pConnectionProxy::P2pConnectionProxy(P2pContextOwner&& ctx, IP2pNodeInternal& node)
  : m_contextOwner(std::move(ctx)), m_context(m_contextOwner.get()), m_node(node) {}

P2pConnectionProxy::~P2pConnectionProxy() {
  m_context.stop();
}

bool P2pConnectionProxy::processIncomingHandshake() {
  LevinProtocol::Command cmd;
  if (!m_context.readCommand(cmd)) {
    throw std::runtime_error("Connection unexpectedly closed");
  }

  if (cmd.command == COMMAND_HANDSHAKE::ID) {
    handleHandshakeRequest(cmd);
    return true;
  } 
  
  if (cmd.command == COMMAND_PING::ID) {
    COMMAND_PING::response resp{ PING_OK_RESPONSE_STATUS_TEXT, m_node.getPeerId() };
    m_context.writeMessage(makeReply(COMMAND_PING::ID, LevinProtocol::encode(resp), LEVIN_PROTOCOL_RETCODE_SUCCESS));
    return false;
  } 

  throw std::runtime_error("Unexpected command: " + std::to_string(cmd.command));
}

void P2pConnectionProxy::read(P2pMessage& message) {
  if (!m_readQueue.empty()) {
    message = std::move(m_readQueue.front());
    m_readQueue.pop();
    return;
  }

  for (;;) {
    LevinProtocol::Command cmd;
    if (!m_context.readCommand(cmd)) {
      throw InterruptedException();
    }

    message.type = cmd.command;

    if (cmd.command == COMMAND_HANDSHAKE::ID) {
      handleHandshakeResponse(cmd, message);
      break;
    } else if (cmd.command == COMMAND_TIMED_SYNC::ID) {
      handleTimedSync(cmd);
    } else {
      message.data = std::move(cmd.buf);
      break;
    }
  }
}

void P2pConnectionProxy::write(const P2pMessage &message) {
  if (message.type == COMMAND_HANDSHAKE::ID) {
    writeHandshake(message);
  } else {
    m_context.writeMessage(P2pContext::Message(P2pMessage(message), P2pContext::Message::NOTIFY));
  }
}

void P2pConnectionProxy::ban() {
  // not implemented
}

void P2pConnectionProxy::stop() {
  m_context.stop();
}

void P2pConnectionProxy::writeHandshake(const P2pMessage &message) {
  CORE_SYNC_DATA coreSync;
  LevinProtocol::decode(message.data, coreSync);

  if (m_context.isIncoming()) {
    // response
    COMMAND_HANDSHAKE::response res;
    res.node_data = m_node.getNodeData();
    res.payload_data = coreSync;
    res.local_peerlist = m_node.getLocalPeerList();
    m_context.writeMessage(makeReply(COMMAND_HANDSHAKE::ID, LevinProtocol::encode(res), LEVIN_PROTOCOL_RETCODE_SUCCESS));
    m_node.tryPing(m_context);
  } else {
    // request
    COMMAND_HANDSHAKE::request req;
    req.node_data = m_node.getNodeData();
    req.payload_data = coreSync;
    m_context.writeMessage(makeRequest(COMMAND_HANDSHAKE::ID, LevinProtocol::encode(req)));
  }
}

void P2pConnectionProxy::handleHandshakeRequest(const LevinProtocol::Command& cmd) {
  COMMAND_HANDSHAKE::request req;
  if (!LevinProtocol::decode<COMMAND_HANDSHAKE::request>(cmd.buf, req)) {
    throw std::runtime_error("Failed to decode COMMAND_HANDSHAKE request");
  }

  m_node.handleNodeData(req.node_data, m_context);
  m_readQueue.push(P2pMessage{ 
    cmd.command, LevinProtocol::encode(req.payload_data) }); // enqueue payload info
}

void P2pConnectionProxy::handleHandshakeResponse(const LevinProtocol::Command& cmd, P2pMessage& message) {
  if (m_context.isIncoming()) {
    // handshake should be already received by P2pNode
    throw std::runtime_error("Unexpected COMMAND_HANDSHAKE from incoming connection");
  }

  COMMAND_HANDSHAKE::response res;
  if (!LevinProtocol::decode(cmd.buf, res)) {
    throw std::runtime_error("Invalid handshake message format");
  }

  m_node.handleNodeData(res.node_data, m_context);
  m_node.handleRemotePeerList(res.local_peerlist, res.node_data.local_time);

  message.data = LevinProtocol::encode(res.payload_data);
}

void P2pConnectionProxy::handleTimedSync(const LevinProtocol::Command& cmd) {
  if (cmd.isResponse) {
    COMMAND_TIMED_SYNC::response res;
    LevinProtocol::decode(cmd.buf, res);
    m_node.handleRemotePeerList(res.local_peerlist, res.local_time);
  } else {
    // we ignore information from the request
    // COMMAND_TIMED_SYNC::request req;
    // LevinProtocol::decode(cmd.buf, req);
    COMMAND_TIMED_SYNC::response res;

    res.local_time = time(nullptr);
    res.local_peerlist = m_node.getLocalPeerList();
    res.payload_data = m_node.getGenesisPayload();

    m_context.writeMessage(makeReply(COMMAND_TIMED_SYNC::ID, LevinProtocol::encode(res), LEVIN_PROTOCOL_RETCODE_SUCCESS));
  }
}

}
