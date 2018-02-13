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
#include "Timer.h"
#include <cassert>
#include <stdexcept>

#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "Dispatcher.h"
#include <System/ErrorMessage.h>
#include <System/InterruptedException.h>

namespace System {

Timer::Timer() : dispatcher(nullptr) {
}

Timer::Timer(Dispatcher& dispatcher) : dispatcher(&dispatcher), context(nullptr), timer(-1) {
}

Timer::Timer(Timer&& other) : dispatcher(other.dispatcher) {
  if (other.dispatcher != nullptr) {
    assert(other.context == nullptr);
    timer = other.timer;
    context = nullptr;
    other.dispatcher = nullptr;
  }
}

Timer::~Timer() {
  assert(dispatcher == nullptr || context == nullptr);
}

Timer& Timer::operator=(Timer&& other) {
  assert(dispatcher == nullptr || context == nullptr);
  dispatcher = other.dispatcher;
  if (other.dispatcher != nullptr) {
    assert(other.context == nullptr);
    timer = other.timer;
    context = nullptr;
    other.dispatcher = nullptr;
    other.timer = -1;
  }

  return *this;
}

void Timer::sleep(std::chrono::nanoseconds duration) {
  assert(dispatcher != nullptr);
  assert(context == nullptr);
  if (dispatcher->interrupted()) {
    throw InterruptedException();
  }

  if(duration.count() == 0 ) {
    dispatcher->yield();
  } else {
    timer = dispatcher->getTimer();

    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    itimerspec expires;
    expires.it_interval.tv_nsec = expires.it_interval.tv_sec = 0;
    expires.it_value.tv_sec = seconds.count();
    expires.it_value.tv_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds).count();
    timerfd_settime(timer, 0, &expires, NULL);

    ContextPair contextPair;
    OperationContext timerContext;
    timerContext.interrupted = false;
    timerContext.context = dispatcher->getCurrentContext();
    contextPair.writeContext = nullptr;
    contextPair.readContext = &timerContext;

    epoll_event timerEvent;
    timerEvent.events = EPOLLIN | EPOLLONESHOT;
    timerEvent.data.ptr = &contextPair;

    if (epoll_ctl(dispatcher->getEpoll(), EPOLL_CTL_MOD, timer, &timerEvent) == -1) {
      throw std::runtime_error("Timer::sleep, epoll_ctl failed, " + lastErrorMessage());
    }
    dispatcher->getCurrentContext()->interruptProcedure = [&]() {
        assert(dispatcher != nullptr);
        assert(context != nullptr);
        OperationContext* timerContext = static_cast<OperationContext*>(context);
        if (!timerContext->interrupted) {
          uint64_t value = 0;
          if(::read(timer, &value, sizeof value) == -1 ){
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
              timerContext->interrupted = true;
              dispatcher->pushContext(timerContext->context);
            } else {
              throw std::runtime_error("Timer::interrupt, read failed, "  + lastErrorMessage());
            }
          } else {
            assert(value>0);
            dispatcher->pushContext(timerContext->context);
          }

          epoll_event timerEvent;
          timerEvent.events = 0;
          timerEvent.data.ptr = nullptr;

          if (epoll_ctl(dispatcher->getEpoll(), EPOLL_CTL_MOD, timer, &timerEvent) == -1) {
            throw std::runtime_error("Timer::interrupt, epoll_ctl failed, " + lastErrorMessage());
          }
        }
    };

    context = &timerContext;
    dispatcher->dispatch();
    dispatcher->getCurrentContext()->interruptProcedure = nullptr;
    assert(dispatcher != nullptr);
    assert(timerContext.context == dispatcher->getCurrentContext());
    assert(contextPair.writeContext == nullptr);
    assert(context == &timerContext);
    context = nullptr;
    timerContext.context = nullptr;
    dispatcher->pushTimer(timer);
    if (timerContext.interrupted) {
      throw InterruptedException();
    }
  }
}

}
