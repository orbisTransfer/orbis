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

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

#include "P2pProtocolTypes.h"
#include "OrbisConfig.h"

namespace Orbis {

class ISerializer;
/************************************************************************/
/*                                                                      */
/************************************************************************/
class PeerlistManager {
  struct by_time{};
  struct by_id{};
  struct by_addr{};

  typedef boost::multi_index_container<
    PeerlistEntry,
    boost::multi_index::indexed_by<
    // access by peerlist_entry::net_adress
    boost::multi_index::ordered_unique<boost::multi_index::tag<by_addr>, boost::multi_index::member<PeerlistEntry, NetworkAddress, &PeerlistEntry::adr> >,
    // sort by peerlist_entry::last_seen<
    boost::multi_index::ordered_non_unique<boost::multi_index::tag<by_time>, boost::multi_index::member<PeerlistEntry, uint64_t, &PeerlistEntry::last_seen> >
    >
  > peers_indexed;

public:

  class Peerlist {
  public:
    Peerlist(peers_indexed& peers, size_t maxSize);
    size_t count() const;
    bool get(PeerlistEntry& entry, size_t index) const;
    void trim();

  private:
    peers_indexed& m_peers;
    const size_t m_maxSize;
  };

  PeerlistManager();

  bool init(bool allow_local_ip);
  size_t get_white_peers_count() const { return m_peers_white.size(); }
  size_t get_gray_peers_count() const { return m_peers_gray.size(); }
  bool merge_peerlist(const std::list<PeerlistEntry>& outer_bs);
  bool get_peerlist_head(std::list<PeerlistEntry>& bs_head, uint32_t depth = Orbis::P2P_DEFAULT_PEERS_IN_HANDSHAKE) const;
  bool get_peerlist_full(std::list<PeerlistEntry>& pl_gray, std::list<PeerlistEntry>& pl_white) const;
  bool get_white_peer_by_index(PeerlistEntry& p, size_t i) const;
  bool get_gray_peer_by_index(PeerlistEntry& p, size_t i) const;
  bool append_with_peer_white(const PeerlistEntry& pr);
  bool append_with_peer_gray(const PeerlistEntry& pr);
  bool set_peer_just_seen(PeerIdType peer, uint32_t ip, uint32_t port);
  bool set_peer_just_seen(PeerIdType peer, const NetworkAddress& addr);
  bool set_peer_unreachable(const PeerlistEntry& pr);
  bool is_ip_allowed(uint32_t ip) const;
  void trim_white_peerlist();
  void trim_gray_peerlist();

  void serialize(ISerializer& s);

  Peerlist& getWhite();
  Peerlist& getGray();

private:
  std::string m_config_folder;
  bool m_allow_local_ip;
  peers_indexed m_peers_gray;
  peers_indexed m_peers_white;
  Peerlist m_whitePeerlist;
  Peerlist m_grayPeerlist;
};

}
