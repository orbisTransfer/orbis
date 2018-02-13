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

#include "INode.h"
#include <mutex>
#include <condition_variable>

namespace Orbis {


template <typename T>
class ObservableValue {
public:
  ObservableValue(const T defaultValue = 0) : 
    m_prev(defaultValue), m_value(defaultValue) {
  }

  void init(T value) {
    std::lock_guard<std::mutex> lk(m_mutex);
    m_value = m_prev = value;
  }

  void set(T value) {
    std::lock_guard<std::mutex> lk(m_mutex);
    m_value = value;
    m_cv.notify_all();
  }

  T get() {
    std::lock_guard<std::mutex> lk(m_mutex);
    return m_value;
  }

  bool waitFor(std::chrono::milliseconds ms, T& value) {
    std::unique_lock<std::mutex> lk(m_mutex);

    if (m_cv.wait_for(lk, ms, [this] { return m_prev != m_value; })) {
      value = m_prev = m_value;
      return true;
    }

    return false;
  }

  T wait() {
    std::unique_lock<std::mutex> lk(m_mutex);

    m_cv.wait(lk, [this] { return m_prev != m_value; });
    m_prev = m_value;
    return m_value;
  }

private:

  std::mutex m_mutex;
  std::condition_variable m_cv;

  T m_prev;
  T m_value;
};

class NodeObserver: public INodeObserver {
public:

  NodeObserver(INode& node) : m_node(node) {
    m_knownHeight.init(node.getLastKnownBlockHeight());
    node.addObserver(this);
  }

  ~NodeObserver() {
    m_node.removeObserver(this);
  }

  virtual void lastKnownBlockHeightUpdated(uint32_t height) override {
    m_knownHeight.set(height);
  }

  virtual void localBlockchainUpdated(uint32_t height) override {
    m_localHeight.set(height);
  }

  virtual void peerCountUpdated(size_t count) override {
    m_peerCount.set(count);
  }

  bool waitLastKnownBlockHeightUpdated(std::chrono::milliseconds ms, uint32_t& value) {
    return m_knownHeight.waitFor(ms, value);
  }

  bool waitLocalBlockchainUpdated(std::chrono::milliseconds ms, uint32_t& value) {
    return m_localHeight.waitFor(ms, value);
  }

  uint32_t waitLastKnownBlockHeightUpdated() {
    return m_knownHeight.wait();
  }

  ObservableValue<uint32_t> m_knownHeight;
  ObservableValue<uint32_t> m_localHeight;
  ObservableValue<size_t> m_peerCount;

private:

  INode& m_node;
};


}
