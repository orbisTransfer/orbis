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

#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

template < typename T, typename Container = std::deque<T> >
class BlockingQueue {
public:

  typedef BlockingQueue<T, Container> ThisType;

  BlockingQueue(size_t maxSize = 1) : 
    m_maxSize(maxSize), m_closed(false) {}

  template <typename TT>
  bool push(TT&& v) {
    std::unique_lock<std::mutex> lk(m_mutex);

    while (!m_closed && m_queue.size() >= m_maxSize) {
      m_haveSpace.wait(lk);
    }

    if (m_closed) {
      return false;
    }

    m_queue.push_back(std::forward<TT>(v));
    m_haveData.notify_one();
    return true;
  }

  bool pop(T& v) {
    std::unique_lock<std::mutex> lk(m_mutex);

    while (m_queue.empty()) {
      if (m_closed) {
        // all data has been processed, queue is closed
        return false;
      }
      m_haveData.wait(lk);
    }
   
    v = std::move(m_queue.front());
    m_queue.pop_front();

    // we can have several waiting threads to unblock
    if (m_closed && m_queue.empty())
      m_haveSpace.notify_all();
    else
      m_haveSpace.notify_one();

    return true;
  }

  void close(bool wait = false) {
    std::unique_lock<std::mutex> lk(m_mutex);
    m_closed = true;
    m_haveData.notify_all(); // wake up threads in pop()
    m_haveSpace.notify_all();

    if (wait) {
      while (!m_queue.empty()) {
        m_haveSpace.wait(lk);
      }
    }
  }

  size_t size() {
    std::unique_lock<std::mutex> lk(m_mutex);
    return m_queue.size();
  }

  size_t capacity() const {
    return m_maxSize;
  }

private:

  const size_t m_maxSize;
  Container m_queue;
  bool m_closed;
  
  std::mutex m_mutex;
  std::condition_variable m_haveData;
  std::condition_variable m_haveSpace;
};

template <typename QueueT>
class GroupClose {
public:

  GroupClose(QueueT& queue, size_t groupSize)
    : m_queue(queue), m_count(groupSize) {}

  void close() {
    if (m_count == 0)
      return;
    if (m_count.fetch_sub(1) == 1)
      m_queue.close();
  }

private:

  std::atomic<size_t> m_count;
  QueueT& m_queue;

};
