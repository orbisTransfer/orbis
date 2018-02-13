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

#include <condition_variable>
#include <mutex>
#include <thread>

namespace System {

namespace Detail {

namespace { 

enum class State : unsigned {
  STARTED,
  COMPLETED,
  CONSUMED
};

}

// Simplest possible future implementation. The reason why this class even exist is because currenty std future has a
// memory corrupting bug on OSX. Spawn a new thread, execute procedure in it, get result, and wait thread to shut down.
// Actualy, this is what libstdc++'s std::future is doing.
template<class T> class Future {
public:
  // Create a new thread, and run `operation` in it.
  explicit Future(std::function<T()>&& operation) : procedure(std::move(operation)), state(State::STARTED), worker{[this] { asyncOp(); }} {
  }

  // Wait for async op to complete, then if thread wasn't detached, join it.
  ~Future() {
    wait();
    if (worker.joinable()) {
      worker.join();
    }
  }

  // Get result of async operation. UB if called more than once.
  T get() const {
    assert(state != State::CONSUMED);
    wait();
    state = State::CONSUMED;
    if (currentException != nullptr) {
      std::rethrow_exception(currentException);
    }

    return std::move(result);
  }

  // Wait for async operation to complete, if op is already completed, return immediately.
  void wait() const {
    std::unique_lock<std::mutex> guard(operationMutex);
    while (state == State::STARTED) {
      operationCondition.wait(guard);
    }
  }

  bool valid() const {
    std::unique_lock<std::mutex> guard(operationMutex);
    return state != State::CONSUMED;
  }

private:
  // This function is executed in a separate thread.
  void asyncOp() {
    try {
      assert(procedure != nullptr);
      result = procedure();
    } catch (...) {
      currentException = std::current_exception();
    }

    std::unique_lock<std::mutex> guard(operationMutex);
    state = State::COMPLETED;
    operationCondition.notify_one();
  }

  mutable T result;
  std::function<T()> procedure;
  std::exception_ptr currentException;
  mutable std::mutex operationMutex;
  mutable std::condition_variable operationCondition;
  mutable State state;
  std::thread worker;
};

template<> class Future<void> {
public:
  // Create a new thread, and run `operation` in it.
  explicit Future(std::function<void()>&& operation) : procedure(std::move(operation)), state(State::STARTED), worker{[this] { asyncOp(); }} {
  }

  // Wait for async op to complete, then if thread wasn't detached, join it.
  ~Future() {
    wait();
    if (worker.joinable()) {
      worker.join();
    }
  }

  // Get result of async operation. UB if called more than once.
  void get() const {
    assert(state != State::CONSUMED);
    wait();
    state = State::CONSUMED;
    if (currentException != nullptr) {
      std::rethrow_exception(currentException);
    }
  }

  // Wait for async operation to complete, if op is already completed, return immediately.
  void wait() const {
    std::unique_lock<std::mutex> guard(operationMutex);
    while (state == State::STARTED) {
      operationCondition.wait(guard);
    }
  }

  bool valid() const {
    std::unique_lock<std::mutex> guard(operationMutex);
    return state != State::CONSUMED;
  }

private:
  // This function is executed in a separate thread.
  void asyncOp() {
    try {
      assert(procedure != nullptr);
      procedure();
    } catch (...) {
      currentException = std::current_exception();
    }

    std::unique_lock<std::mutex> guard(operationMutex);
    state = State::COMPLETED;
    operationCondition.notify_one();
  }

  std::function<void()> procedure;
  std::exception_ptr currentException;
  mutable std::mutex operationMutex;
  mutable std::condition_variable operationCondition;
  mutable State state;
  std::thread worker;
};

template<class T> std::function<T()> async(std::function<T()>&& operation) {
  return operation;
}

}

}
