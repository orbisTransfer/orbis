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
#include "P2pNodeConfig.h"
#include "P2pNetworks.h"

#include <OrbisConfig.h>

namespace Orbis {

namespace {

const std::chrono::nanoseconds P2P_DEFAULT_CONNECT_INTERVAL = std::chrono::seconds(2);
const size_t P2P_DEFAULT_CONNECT_RANGE = 20;
const size_t P2P_DEFAULT_PEERLIST_GET_TRY_COUNT = 10;

}

P2pNodeConfig::P2pNodeConfig() :
  timedSyncInterval(std::chrono::seconds(P2P_DEFAULT_HANDSHAKE_INTERVAL)),
  handshakeTimeout(std::chrono::milliseconds(P2P_DEFAULT_HANDSHAKE_INVOKE_TIMEOUT)),
  connectInterval(P2P_DEFAULT_CONNECT_INTERVAL),
  connectTimeout(std::chrono::milliseconds(P2P_DEFAULT_CONNECTION_TIMEOUT)),
  networkId(ORBIS_NETWORK),
  expectedOutgoingConnectionsCount(P2P_DEFAULT_CONNECTIONS_COUNT),
  whiteListConnectionsPercent(P2P_DEFAULT_WHITELIST_CONNECTIONS_PERCENT),
  peerListConnectRange(P2P_DEFAULT_CONNECT_RANGE),
  peerListGetTryCount(P2P_DEFAULT_PEERLIST_GET_TRY_COUNT) {
}

// getters

std::chrono::nanoseconds P2pNodeConfig::getTimedSyncInterval() const {
  return timedSyncInterval;
}

std::chrono::nanoseconds P2pNodeConfig::getHandshakeTimeout() const {
  return handshakeTimeout;
}

std::chrono::nanoseconds P2pNodeConfig::getConnectInterval() const {
  return connectInterval;
}

std::chrono::nanoseconds P2pNodeConfig::getConnectTimeout() const {
  return connectTimeout;
}

size_t P2pNodeConfig::getExpectedOutgoingConnectionsCount() const {
  return expectedOutgoingConnectionsCount;
}

size_t P2pNodeConfig::getWhiteListConnectionsPercent() const {
  return whiteListConnectionsPercent;
}

boost::uuids::uuid P2pNodeConfig::getNetworkId() const {
  if (getTestnet()) {
    boost::uuids::uuid copy = networkId;
    copy.data[0] += 1;
    return copy;
  }
  return networkId;
}

size_t P2pNodeConfig::getPeerListConnectRange() const {
  return peerListConnectRange;
}

size_t P2pNodeConfig::getPeerListGetTryCount() const {
  return peerListGetTryCount;
}

// setters

void P2pNodeConfig::setTimedSyncInterval(std::chrono::nanoseconds interval) {
  timedSyncInterval = interval;
}

void P2pNodeConfig::setHandshakeTimeout(std::chrono::nanoseconds timeout) {
  handshakeTimeout = timeout;
}

void P2pNodeConfig::setConnectInterval(std::chrono::nanoseconds interval) {
  connectInterval = interval;
}

void P2pNodeConfig::setConnectTimeout(std::chrono::nanoseconds timeout) {
  connectTimeout = timeout;
}

void P2pNodeConfig::setExpectedOutgoingConnectionsCount(size_t count) {
  expectedOutgoingConnectionsCount = count;
}

void P2pNodeConfig::setWhiteListConnectionsPercent(size_t percent) {
  if (percent > 100) {
    throw std::invalid_argument("whiteListConnectionsPercent cannot be greater than 100");
  }

  whiteListConnectionsPercent = percent;
}

void P2pNodeConfig::setNetworkId(const boost::uuids::uuid& id) {
  networkId = id;
}

void P2pNodeConfig::setPeerListConnectRange(size_t range) {
  peerListConnectRange = range;
}

void P2pNodeConfig::setPeerListGetTryCount(size_t count) {
  peerListGetTryCount = count;
}

}
