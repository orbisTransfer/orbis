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
#include "Event.h"
#include <cassert>
#include <System/Dispatcher.h>
#include <System/InterruptedException.h>

namespace System {

namespace {

struct EventWaiter {
  bool interrupted;
  EventWaiter* prev;
  EventWaiter* next;
  NativeContext* context;
};

}

Event::Event() : dispatcher(nullptr) {
}

Event::Event(Dispatcher& dispatcher) : dispatcher(&dispatcher), state(false), first(nullptr) {
}

Event::Event(Event&& other) : dispatcher(other.dispatcher) {
  if (dispatcher != nullptr) {
    state = other.state;
    if (!state) {
      assert(other.first == nullptr);
      first = nullptr;
    }

    other.dispatcher = nullptr;
  }
}

Event::~Event() {
  assert(dispatcher == nullptr || state || first == nullptr);
}

Event& Event::operator=(Event&& other) {
  assert(dispatcher == nullptr || state || first == nullptr);
  dispatcher = other.dispatcher;
  if (dispatcher != nullptr) {
    state = other.state;
    if (!state) {
      assert(other.first == nullptr);
      first = nullptr;
    }

    other.dispatcher = nullptr;
  }

  return *this;
}

bool Event::get() const {
  assert(dispatcher != nullptr);
  return state;
}

void Event::clear() {
  assert(dispatcher != nullptr);
  if (state) {
    state = false;
    first = nullptr;
  }
}

void Event::set() {
  assert(dispatcher != nullptr);
  if (!state) {
    state = true;
    for (EventWaiter* waiter = static_cast<EventWaiter*>(first); waiter != nullptr; waiter = waiter->next) {
      waiter->context->interruptProcedure = nullptr;
      dispatcher->pushContext(waiter->context);
    }
  }
}

void Event::wait() {
  assert(dispatcher != nullptr);
  if (dispatcher->interrupted()) {
    throw InterruptedException();
  }

  if (!state) {
    EventWaiter waiter = { false, nullptr, nullptr, dispatcher->getCurrentContext() };
    waiter.context->interruptProcedure = [&] {
      if (waiter.next != nullptr) {
        assert(waiter.next->prev == &waiter);
        waiter.next->prev = waiter.prev;
      } else {
        assert(last == &waiter);
        last = waiter.prev;
      }

      if (waiter.prev != nullptr) { 
        assert(waiter.prev->next == &waiter);
        waiter.prev->next = waiter.next;
      } else {
        assert(first == &waiter);
        first = waiter.next;
      }

      assert(!waiter.interrupted);
      waiter.interrupted = true;
      dispatcher->pushContext(waiter.context);
    };

    if (first != nullptr) {
      static_cast<EventWaiter*>(last)->next = &waiter;
      waiter.prev = static_cast<EventWaiter*>(last);
    } else {
      first = &waiter;
    }

    last = &waiter;
    dispatcher->dispatch();
    assert(waiter.context == dispatcher->getCurrentContext());
    assert( waiter.context->interruptProcedure == nullptr);
    assert(dispatcher != nullptr);
    if (waiter.interrupted) {
      throw InterruptedException();
    } 
  }
}

}
