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

#include <functional>
#include <list>

#include <Logging/LoggerRef.h>
#include <System/ContextGroup.h>
#include <System/Dispatcher.h>
#include <System/Event.h>
#include <System/TcpListener.h>
#include <System/Timer.h>

#include "IP2pNodeInternal.h"
#include "IStreamSerializable.h"
#include "NetNodeConfig.h"
#include "P2pInterfaces.h"
#include "P2pNodeConfig.h"
#include "P2pProtocolDefinitions.h"
#include "PeerListManager.h"

namespace Orbis {

class P2pContext;
class P2pConnectionProxy;

class P2pNode : 
  public IP2pNode, 
  public IStreamSerializable,
  IP2pNodeInternal {

public:

  P2pNode(
    const P2pNodeConfig& cfg,
    System::Dispatcher& dispatcher, 
    Logging::ILogger& log, 
    const Crypto::Hash& genesisHash, 
    PeerIdType peerId);

  ~P2pNode();
  
  // IP2pNode
  virtual std::unique_ptr<IP2pConnection> receiveConnection() override;
  virtual void stop() override;

  // IStreamSerializable
  virtual void save(std::ostream& os) override;
  virtual void load(std::istream& in) override;

  // P2pNode
  void start();
  void serialize(ISerializer& s);

private:
  typedef std::unique_ptr<P2pContext> ContextPtr;
  typedef std::list<ContextPtr> ContextList;

  Logging::LoggerRef logger;
  bool m_stopRequested;
  const P2pNodeConfig m_cfg;
  const PeerIdType m_myPeerId;
  const Crypto::Hash m_genesisHash;
  const CORE_SYNC_DATA m_genesisPayload;

  System::Dispatcher& m_dispatcher;
  System::ContextGroup workingContextGroup;
  System::TcpListener m_listener;
  System::Timer m_connectorTimer;
  PeerlistManager m_peerlist;
  ContextList m_contexts;
  System::Event m_queueEvent;
  std::deque<std::unique_ptr<IP2pConnection>> m_connectionQueue;

  // IP2pNodeInternal
  virtual const CORE_SYNC_DATA& getGenesisPayload() const override;
  virtual std::list<PeerlistEntry> getLocalPeerList() const override;
  virtual basic_node_data getNodeData() const override;
  virtual PeerIdType getPeerId() const override;

  virtual void handleNodeData(const basic_node_data& node, P2pContext& ctx) override;
  virtual bool handleRemotePeerList(const std::list<PeerlistEntry>& peerlist, time_t local_time) override;
  virtual void tryPing(P2pContext& ctx) override;

  // spawns
  void acceptLoop();
  void connectorLoop();

  // connection related
  void connectPeers();
  void connectPeerList(const std::vector<NetworkAddress>& peers);
  bool isPeerConnected(const NetworkAddress& address);
  bool isPeerUsed(const PeerlistEntry& peer);
  ContextPtr tryToConnectPeer(const NetworkAddress& address);
  bool fetchPeerList(ContextPtr connection);

  // making and processing connections
  size_t getOutgoingConnectionsCount() const;
  void makeExpectedConnectionsCount(const PeerlistManager::Peerlist& peerlist, size_t connectionsCount);
  bool makeNewConnectionFromPeerlist(const PeerlistManager::Peerlist& peerlist);
  void preprocessIncomingConnection(ContextPtr ctx);
  void enqueueConnection(std::unique_ptr<P2pConnectionProxy> proxy);
  std::unique_ptr<P2pConnectionProxy> createProxy(ContextPtr ctx);
};

}
