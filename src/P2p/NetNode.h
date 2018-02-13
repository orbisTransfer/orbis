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
#include <unordered_map>

#include <boost/functional/hash.hpp>

#include <System/Context.h>
#include <System/ContextGroup.h>
#include <System/Dispatcher.h>
#include <System/Event.h>
#include <System/Timer.h>
#include <System/TcpConnection.h>
#include <System/TcpListener.h>

#include "OrbisCore/OnceInInterval.h"
#include "OrbisProtocol/OrbisProtocolHandler.h"
#include "Common/CommandLine.h"
#include "Logging/LoggerRef.h"

#include "ConnectionContext.h"
#include "LevinProtocol.h"
#include "NetNodeCommon.h"
#include "NetNodeConfig.h"
#include "P2pProtocolDefinitions.h"
#include "P2pNetworks.h"
#include "PeerListManager.h"

namespace System {
class TcpConnection;
}

namespace Orbis
{
  class LevinProtocol;
  class ISerializer;

  struct P2pMessage {
    enum Type {
      COMMAND,
      REPLY,
      NOTIFY
    };

    P2pMessage(Type type, uint32_t command, const BinaryArray& buffer, int32_t returnCode = 0) :
      type(type), command(command), buffer(buffer), returnCode(returnCode) {
    }

    P2pMessage(P2pMessage&& msg) :
      type(msg.type), command(msg.command), buffer(std::move(msg.buffer)), returnCode(msg.returnCode) {
    }

    size_t size() {
      return buffer.size();
    }

    Type type;
    uint32_t command;
    const BinaryArray buffer;
    int32_t returnCode;
  };

  struct P2pConnectionContext : public OrbisConnectionContext {
  public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    System::Context<void>* context;
    PeerIdType peerId;
    System::TcpConnection connection;

    P2pConnectionContext(System::Dispatcher& dispatcher, Logging::ILogger& log, System::TcpConnection&& conn) :
      context(nullptr),
      peerId(0),
      connection(std::move(conn)),
      logger(log, "node_server"),
      queueEvent(dispatcher),
      stopped(false) {
    }

    P2pConnectionContext(P2pConnectionContext&& ctx) : 
      OrbisConnectionContext(std::move(ctx)),
      context(ctx.context),
      peerId(ctx.peerId),
      connection(std::move(ctx.connection)),
      logger(ctx.logger.getLogger(), "node_server"),
      queueEvent(std::move(ctx.queueEvent)),
      stopped(std::move(ctx.stopped)) {
    }

    bool pushMessage(P2pMessage&& msg);
    std::vector<P2pMessage> popBuffer();
    void interrupt();

    uint64_t writeDuration(TimePoint now) const;

  private:
    Logging::LoggerRef logger;
    TimePoint writeOperationStartTime;
    System::Event queueEvent;
    std::vector<P2pMessage> writeQueue;
    size_t writeQueueSize = 0;
    bool stopped;
  };

  class NodeServer :  public IP2pEndpoint
  {
  public:

    static void init_options(boost::program_options::options_description& desc);

    NodeServer(System::Dispatcher& dispatcher, Orbis::OrbisProtocolHandler& payload_handler, Logging::ILogger& log);

    bool run();
    bool init(const NetNodeConfig& config);
    bool deinit();
    bool sendStopSignal();
    uint32_t get_this_peer_port(){return m_listeningPort;}
    Orbis::OrbisProtocolHandler& get_payload_object();

    void serialize(ISerializer& s);

    // debug functions
    bool log_peerlist();
    bool log_connections();
    virtual uint64_t get_connections_count() override;
    size_t get_outgoing_connections_count();

    Orbis::PeerlistManager& getPeerlistManager() { return m_peerlist; }

  private:

    int handleCommand(const LevinProtocol::Command& cmd, BinaryArray& buff_out, P2pConnectionContext& context, bool& handled);

    //----------------- commands handlers ----------------------------------------------
    int handle_handshake(int command, COMMAND_HANDSHAKE::request& arg, COMMAND_HANDSHAKE::response& rsp, P2pConnectionContext& context);
    int handle_timed_sync(int command, COMMAND_TIMED_SYNC::request& arg, COMMAND_TIMED_SYNC::response& rsp, P2pConnectionContext& context);
    int handle_ping(int command, COMMAND_PING::request& arg, COMMAND_PING::response& rsp, P2pConnectionContext& context);
#ifdef ALLOW_DEBUG_COMMANDS
    int handle_get_stat_info(int command, COMMAND_REQUEST_STAT_INFO::request& arg, COMMAND_REQUEST_STAT_INFO::response& rsp, P2pConnectionContext& context);
    int handle_get_network_state(int command, COMMAND_REQUEST_NETWORK_STATE::request& arg, COMMAND_REQUEST_NETWORK_STATE::response& rsp, P2pConnectionContext& context);
    int handle_get_peer_id(int command, COMMAND_REQUEST_PEER_ID::request& arg, COMMAND_REQUEST_PEER_ID::response& rsp, P2pConnectionContext& context);
#endif

    bool init_config();
    bool make_default_config();
    bool store_config();
    bool check_trust(const proof_of_trust& tr);
    void initUpnp();

    bool handshake(Orbis::LevinProtocol& proto, P2pConnectionContext& context, bool just_take_peerlist = false);
    bool timedSync();
    bool handleTimedSyncResponse(const BinaryArray& in, P2pConnectionContext& context);
    void forEachConnection(std::function<void(P2pConnectionContext&)> action);

    void on_connection_new(P2pConnectionContext& context);
    void on_connection_close(P2pConnectionContext& context);

    //----------------- i_p2p_endpoint -------------------------------------------------------------
    virtual void relay_notify_to_all(int command, const BinaryArray& data_buff, const net_connection_id* excludeConnection) override;
    virtual bool invoke_notify_to_peer(int command, const BinaryArray& req_buff, const OrbisConnectionContext& context) override;
    virtual void for_each_connection(std::function<void(Orbis::OrbisConnectionContext&, PeerIdType)> f) override;
    virtual void externalRelayNotifyToAll(int command, const BinaryArray& data_buff) override;

    //-----------------------------------------------------------------------------------------------
    bool handle_command_line(const boost::program_options::variables_map& vm);
    bool handleConfig(const NetNodeConfig& config);
    bool append_net_address(std::vector<NetworkAddress>& nodes, const std::string& addr);
    bool idle_worker();
    bool handle_remote_peerlist(const std::list<PeerlistEntry>& peerlist, time_t local_time, const OrbisConnectionContext& context);
    bool get_local_node_data(basic_node_data& node_data);

    bool merge_peerlist_with_local(const std::list<PeerlistEntry>& bs);
    bool fix_time_delta(std::list<PeerlistEntry>& local_peerlist, time_t local_time, int64_t& delta);

    bool connections_maker();
    bool make_new_connection_from_peerlist(bool use_white_list);
    bool try_to_connect_and_handshake_with_new_peer(const NetworkAddress& na, bool just_take_peerlist = false, uint64_t last_seen_stamp = 0, bool white = true);
    bool is_peer_used(const PeerlistEntry& peer);
    bool is_addr_connected(const NetworkAddress& peer);  
    bool try_ping(basic_node_data& node_data, P2pConnectionContext& context);
    bool make_expected_connections_count(bool white_list, size_t expected_connections);
    bool is_priority_node(const NetworkAddress& na);

    bool connect_to_peerlist(const std::vector<NetworkAddress>& peers);

    bool parse_peers_and_add_to_container(const boost::program_options::variables_map& vm, 
      const command_line::arg_descriptor<std::vector<std::string> > & arg, std::vector<NetworkAddress>& container);

    //debug functions
    std::string print_connections_container();

    typedef std::unordered_map<boost::uuids::uuid, P2pConnectionContext, boost::hash<boost::uuids::uuid>> ConnectionContainer;
    typedef ConnectionContainer::iterator ConnectionIterator;
    ConnectionContainer m_connections;

    void acceptLoop();
    void connectionHandler(const boost::uuids::uuid& connectionId, P2pConnectionContext& connection);
    void writeHandler(P2pConnectionContext& ctx);
    void onIdle();
    void timedSyncLoop();
    void timeoutLoop();

    struct config
    {
      network_config m_net_config;
      uint64_t m_peer_id;

      void serialize(ISerializer& s) {
        KV_MEMBER(m_net_config)
        KV_MEMBER(m_peer_id)
      }
    };

    config m_config;
    std::string m_config_folder;

    bool m_have_address;
    bool m_first_connection_maker_call;
    uint32_t m_listeningPort;
    uint32_t m_external_port;
    uint32_t m_ip_address;
    bool m_allow_local_ip;
    bool m_hide_my_port;
    std::string m_p2p_state_filename;

    System::Dispatcher& m_dispatcher;
    System::ContextGroup m_workingContextGroup;
    System::Event m_stopEvent;
    System::Timer m_idleTimer;
    System::Timer m_timeoutTimer;
    System::TcpListener m_listener;
    Logging::LoggerRef logger;
    std::atomic<bool> m_stop;

    OrbisProtocolHandler& m_payload_handler;
    PeerlistManager m_peerlist;

    // OnceInInterval m_peer_handshake_idle_maker_interval;
    OnceInInterval m_connections_maker_interval;
    OnceInInterval m_peerlist_store_interval;
    System::Timer m_timedSyncTimer;

    std::string m_bind_ip;
    std::string m_port;
#ifdef ALLOW_DEBUG_COMMANDS
    uint64_t m_last_stat_request_time;
#endif
    std::vector<NetworkAddress> m_priority_peers;
    std::vector<NetworkAddress> m_exclusive_peers;
    std::vector<NetworkAddress> m_seed_nodes;
    std::list<PeerlistEntry> m_command_line_peers;
    uint64_t m_peer_livetime;
    boost::uuids::uuid m_network_id;
  };
}