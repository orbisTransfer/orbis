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
#include "ContextGroup.h"
#include <cassert>

namespace System {

ContextGroup::ContextGroup(Dispatcher& dispatcher) : dispatcher(&dispatcher) {
  contextGroup.firstContext = nullptr;
}

ContextGroup::ContextGroup(ContextGroup&& other) : dispatcher(other.dispatcher) {
  if (dispatcher != nullptr) {
    assert(other.contextGroup.firstContext == nullptr);
    contextGroup.firstContext = nullptr;
    other.dispatcher = nullptr;
  }
}

ContextGroup::~ContextGroup() {
  if (dispatcher != nullptr) {
    interrupt();
    wait();
  }
}

ContextGroup& ContextGroup::operator=(ContextGroup&& other) {
  assert(dispatcher == nullptr || contextGroup.firstContext == nullptr);
  dispatcher = other.dispatcher;
  if (dispatcher != nullptr) {
    assert(other.contextGroup.firstContext == nullptr);
    contextGroup.firstContext = nullptr;
    other.dispatcher = nullptr;
  }

  return *this;
}

void ContextGroup::interrupt() {
  assert(dispatcher != nullptr);
  for (NativeContext* context = contextGroup.firstContext; context != nullptr; context = context->groupNext) {
    dispatcher->interrupt(context);
  }
}

void ContextGroup::spawn(std::function<void()>&& procedure) {
  assert(dispatcher != nullptr);
  NativeContext& context = dispatcher->getReusableContext();
  if (contextGroup.firstContext != nullptr) {
    context.groupPrev = contextGroup.lastContext;
    assert(contextGroup.lastContext->groupNext == nullptr);
    contextGroup.lastContext->groupNext = &context;
  } else {
    context.groupPrev = nullptr;
    contextGroup.firstContext = &context;
    contextGroup.firstWaiter = nullptr;
  }

  context.interrupted = false;
  context.group = &contextGroup;
  context.groupNext = nullptr;
  context.procedure = std::move(procedure);
  contextGroup.lastContext = &context;
  dispatcher->pushContext(&context);
}

void ContextGroup::wait() {
  if (contextGroup.firstContext != nullptr) {
    NativeContext* context = dispatcher->getCurrentContext();
    context->next = nullptr;
    if (contextGroup.firstWaiter != nullptr) {
      assert(contextGroup.lastWaiter->next == nullptr);
      contextGroup.lastWaiter->next = context;
    } else {
      contextGroup.firstWaiter = context;
    }

    contextGroup.lastWaiter = context;
    dispatcher->dispatch();
    assert(context == dispatcher->getCurrentContext());
  }
}

}
