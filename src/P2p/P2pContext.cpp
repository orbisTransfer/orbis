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
#include "P2pContext.h"

#include <System/EventLock.h>
#include <System/InterruptedException.h>
#include <System/Ipv4Address.h>
#include <System/OperationTimeout.h>

#include "LevinProtocol.h"

using namespace System;

namespace Orbis {

P2pContext::Message::Message(P2pMessage&& msg, Type messageType, uint32_t returnCode) :
  messageType(messageType), returnCode(returnCode) {
  type = msg.type;
  data = std::move(msg.data);
}

size_t P2pContext::Message::size() const {
  return data.size();
}

P2pContext::P2pContext(
  Dispatcher& dispatcher,
  TcpConnection&& conn,
  bool isIncoming,
  const NetworkAddress& remoteAddress,
  std::chrono::nanoseconds timedSyncInterval,
  const CORE_SYNC_DATA& timedSyncData)
  :
  incoming(isIncoming),
  remoteAddress(remoteAddress),
  dispatcher(dispatcher),
  contextGroup(dispatcher),
  timeStarted(Clock::now()),
  timedSyncInterval(timedSyncInterval),
  timedSyncData(timedSyncData),
  timedSyncTimer(dispatcher),
  timedSyncFinished(dispatcher),
  connection(std::move(conn)),
  writeEvent(dispatcher),
  readEvent(dispatcher) {
  writeEvent.set();
  readEvent.set();
  lastReadTime = timeStarted; // use current time
  contextGroup.spawn(std::bind(&P2pContext::timedSyncLoop, this));
}

P2pContext::~P2pContext() {
  stop();
  // wait for timedSyncLoop finish
  timedSyncFinished.wait();
  // ensure that all read/write operations completed
  readEvent.wait();
  writeEvent.wait();
}

PeerIdType P2pContext::getPeerId() const {
  return peerId;
}

uint16_t P2pContext::getPeerPort() const {
  return peerPort;
}

const NetworkAddress& P2pContext::getRemoteAddress() const {
  return remoteAddress;
}

bool P2pContext::isIncoming() const {
  return incoming;
}

void P2pContext::setPeerInfo(uint8_t protocolVersion, PeerIdType id, uint16_t port) {
  version = protocolVersion;
  peerId = id;
  if (isIncoming()) {
    peerPort = port;
  }
}

bool P2pContext::readCommand(LevinProtocol::Command& cmd) {
  if (stopped) {
    throw InterruptedException();
  }

  EventLock lk(readEvent);
  bool result = LevinProtocol(connection).readCommand(cmd);
  lastReadTime = Clock::now();
  return result;
}

void P2pContext::writeMessage(const Message& msg) {
  if (stopped) {
    throw InterruptedException();
  }

  EventLock lk(writeEvent);
  LevinProtocol proto(connection);

  switch (msg.messageType) {
  case P2pContext::Message::NOTIFY:
    proto.sendMessage(msg.type, msg.data, false);
    break;
  case P2pContext::Message::REQUEST:
    proto.sendMessage(msg.type, msg.data, true);
    break;
  case P2pContext::Message::REPLY:
    proto.sendReply(msg.type, msg.data, msg.returnCode);
    break;
  }
}

void P2pContext::start() {
  // stub for OperationTimeout class
} 

void P2pContext::stop() {
  if (!stopped) {
    stopped = true;
    contextGroup.interrupt();
  }
}

void P2pContext::timedSyncLoop() {
  // construct message
  P2pContext::Message timedSyncMessage{ 
    P2pMessage{ 
      COMMAND_TIMED_SYNC::ID, 
      LevinProtocol::encode(COMMAND_TIMED_SYNC::request{ timedSyncData })
    }, 
    P2pContext::Message::REQUEST 
  };

  while (!stopped) {
    try {
      timedSyncTimer.sleep(timedSyncInterval);

      OperationTimeout<P2pContext> timeout(dispatcher, *this, timedSyncInterval);
      writeMessage(timedSyncMessage);

      // check if we had read operation in given time interval
      if ((lastReadTime + timedSyncInterval * 2) < Clock::now()) {
        stop();
        break;
      }
    } catch (InterruptedException&) {
      // someone stopped us
    } catch (std::exception&) {
      stop(); // stop connection on write error
      break;
    }
  }

  timedSyncFinished.set();
}

P2pContext::Message makeReply(uint32_t command, const BinaryArray& data, uint32_t returnCode) {
  return P2pContext::Message(
    P2pMessage{ command, data },
    P2pContext::Message::REPLY,
    returnCode);
}

P2pContext::Message makeRequest(uint32_t command, const BinaryArray& data) {
  return P2pContext::Message(
    P2pMessage{ command, data },
    P2pContext::Message::REQUEST);
}

std::ostream& operator <<(std::ostream& s, const P2pContext& conn) {
  return s << "[" << conn.getRemoteAddress() << "]";
}

}
