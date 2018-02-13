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
#include <System/Context.h>
#include <System/Dispatcher.h>
#include <System/Event.h>
#include <System/InterruptedException.h>
#include <gtest/gtest.h>

using namespace System;

TEST(EventTests, newEventIsNotSet) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  ASSERT_FALSE(event.get());
}

TEST(EventTests, eventIsWorking) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  Context<> context(dispatcher, [&]() {
    event.set();
  });

  event.wait();
  ASSERT_TRUE(event.get());
}

TEST(EventTests, movedEventIsWorking) {
  Dispatcher dispatcher;
  Event event{Event(dispatcher)};
  Context<> context(dispatcher, [&]() {
    event.set();
  });

  event.wait();
  ASSERT_TRUE(event.get());
}

TEST(EventTests, movedEventKeepsState) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  Context<> context(dispatcher, [&]() {
    event.set();
  });

  event.wait();
  Event event2(std::move(event));
  ASSERT_TRUE(event2.get());
}

TEST(EventTests, movedEventIsWorking2) {
  Dispatcher dispatcher;
  Event srcEvent(dispatcher);
  Event event;
  event = std::move(srcEvent);
  Context<> context(dispatcher, [&]() {
    event.set();
  });

  event.wait();
  ASSERT_TRUE(event.get());
}

TEST(EventTests, movedEventKeepsState2) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  Context<> context(dispatcher, [&]() {
    event.set();
  });

  event.wait();
  Event dstEvent;
  dstEvent = std::move(event);
  ASSERT_TRUE(dstEvent.get());
}

TEST(EventTests, moveClearsEventState) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  Context<> context(dispatcher, [&]() {
    event.set();
  });

  event.wait();
  ASSERT_TRUE(event.get());
  Event srcEvent(dispatcher);
  event = std::move(srcEvent);
  ASSERT_FALSE(event.get());
}

TEST(EventTests, movedEventIsTheSame) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  auto eventPtr1 = &event;
  Event srcEvent(dispatcher);
  event = std::move(srcEvent);
  auto eventPtr2 = &event;
  ASSERT_EQ(eventPtr1, eventPtr2);
}

TEST(EventTests, eventIsWorkingAfterClear) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  event.clear();
  Context<> context(dispatcher, [&]() {
    event.set();
  });

  event.wait();
  ASSERT_TRUE(event.get());
}

TEST(EventTests, eventIsWorkingAfterClearOnWaiting) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  Context<> context(dispatcher, [&]() {
    event.clear();
    event.set();
  });

  event.wait();
  ASSERT_TRUE(event.get());
}

TEST(EventTests, eventIsReusableAfterClear) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  Context<> context(dispatcher, [&]() {
    event.set();
    dispatcher.yield();
    event.set();
  });

  event.wait();
  event.clear();
  event.wait();
  SUCCEED();
}

TEST(EventTests, eventSetIsWorkingOnNewEvent) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  event.set();
  ASSERT_TRUE(event.get());
}

TEST(EventTests, setActuallySets) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  Context<> context(dispatcher, [&]() {
    event.set();
  });

  event.wait();
  SUCCEED();
}

TEST(EventTests, setJustSets) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  bool done = false;
  Context<> context(dispatcher, [&]() {
    event.wait();
    done = true;
  });

  dispatcher.yield();
  ASSERT_FALSE(done);
  event.set();
  ASSERT_FALSE(done);
  dispatcher.yield();
  ASSERT_TRUE(done);
}

TEST(EventTests, setSetsOnlyOnce) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  auto i = 0;
  Context<> context(dispatcher, [&]() {
    event.set();
    event.set();
    event.set();
    dispatcher.yield();
    i++;
  });

  event.wait();
  i++;
  event.wait();
  ASSERT_EQ(i, 1);
  dispatcher.yield();
  ASSERT_EQ(i, 2);
}

TEST(EventTests, waitIsWaiting) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  bool done = false;
  Context<> context(dispatcher, [&]() {
    event.wait();
    done = true;
  });

  dispatcher.yield();
  ASSERT_FALSE(done);
  event.set();
  dispatcher.yield();
  ASSERT_TRUE(done);
}

TEST(EventTests, setEventIsNotWaiting) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  auto i = 0;
  Context<> context(dispatcher, [&]() {
    event.set();
    dispatcher.yield();
    i++;
  });

  event.wait();
  i++;
  ASSERT_EQ(i, 1);
  event.wait();
  ASSERT_EQ(i, 1);
  dispatcher.yield();
  ASSERT_EQ(i, 2);
}

TEST(EventTests, waitIsParallel) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  auto i = 0;
  Context<> context(dispatcher, [&]() {
    i++;
    event.set();
  });

  ASSERT_EQ(i, 0);
  event.wait();
  ASSERT_EQ(i, 1);
}

TEST(EventTests, waitIsMultispawn) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  auto i = 0;
  Context<> context(dispatcher, [&]() {
    event.wait();
    i++;
  });

  Context<> contextSecond(dispatcher, [&]() {
    event.wait();
    i++;
  });

  ASSERT_EQ(i, 0);
  dispatcher.yield();
  ASSERT_EQ(i, 0);
  event.set();
  dispatcher.yield();
  ASSERT_EQ(i, 2);
}

TEST(EventTests, setEventInPastUnblocksWaitersEvenAfterClear) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  auto i = 0;
  Context<> context(dispatcher, [&]() {
    event.wait();
    i++;
  });

  Context<> contextSecond(dispatcher, [&]() {
    event.wait();
    i++;
  });

  dispatcher.yield();
  ASSERT_EQ(i, 0);
  event.set();
  event.clear();
  dispatcher.yield();
  ASSERT_EQ(i, 2);
}

TEST(EventTests, waitIsInterruptibleOnFront) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  bool interrupted = false;
  Context<>(dispatcher, [&] {
    try {
      event.wait();
    } catch (InterruptedException&) {
      interrupted = true;
    }
  });
  
  ASSERT_TRUE(interrupted);  
}

TEST(EventTests, waitIsInterruptibleOnBody) {
  Dispatcher dispatcher;
  Event event(dispatcher);
  Event event2(dispatcher);
  bool interrupted = false;
  Context<> context(dispatcher, [&] {
    try {
      event2.set();
      event.wait();
    } catch (InterruptedException&) {
      interrupted = true;
    }
  });

  event2.wait();
  context.interrupt();
  context.get();
  ASSERT_TRUE(interrupted);
}
