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
#include <thread>
#include <System/Context.h>
#include <System/Dispatcher.h>
#include <System/ContextGroup.h>
#include <System/Event.h>
#include <System/InterruptedException.h>
#include <System/Timer.h>
#include <gtest/gtest.h>

using namespace System;

class TimerTests : public testing::Test {
public:
  TimerTests() : contextGroup(dispatcher) {
  }

  Dispatcher dispatcher;
  ContextGroup contextGroup;
};

TEST_F(TimerTests, timerIsWorking) {
  bool done = false;
  contextGroup.spawn([&]() {
    done = true;
  });

  ASSERT_FALSE(done);
  Timer(dispatcher).sleep(std::chrono::milliseconds(10));
  ASSERT_TRUE(done);
}

TEST_F(TimerTests, movedTimerIsWorking) {
  Timer t{Timer{dispatcher}};
  bool done = false;
  contextGroup.spawn([&]() {
    done = true;
  });

  ASSERT_FALSE(done);
  t.sleep(std::chrono::milliseconds(10));
  ASSERT_TRUE(done);
}

TEST_F(TimerTests, movedAndStoopedTimerIsWorking) {
  contextGroup.spawn([&] {
    Timer src(dispatcher);
    contextGroup.interrupt();
    Timer t(std::move(src));

    ASSERT_ANY_THROW(t.sleep(std::chrono::milliseconds(1)));
  });
}

TEST_F(TimerTests, doubleTimerTest) {
  auto begin = std::chrono::high_resolution_clock::now();
  Event first(dispatcher);
  Event second(dispatcher);
  Context<> context(dispatcher, [&] {
    Timer(dispatcher).sleep(std::chrono::milliseconds(100));
    first.set();
  });

  Context<> contextSecond(dispatcher, [&] {
    Timer(dispatcher).sleep(std::chrono::milliseconds(200));
    second.set();
  });

  first.wait();
  second.wait();
  ASSERT_GE(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin).count(), 150);
  ASSERT_TRUE((std::chrono::high_resolution_clock::now() - begin) < std::chrono::milliseconds(275));
}

TEST_F(TimerTests, doubleTimerTestGroup) {
  auto begin = std::chrono::high_resolution_clock::now();
  Event first(dispatcher);
  Event second(dispatcher);
  contextGroup.spawn([&] {
    Timer(dispatcher).sleep(std::chrono::milliseconds(100));
    first.set();
  });

  contextGroup.spawn([&] {
    Timer(dispatcher).sleep(std::chrono::milliseconds(200));
    second.set();
  });

  first.wait();
  second.wait();
  ASSERT_GE(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin).count(), 150);
  ASSERT_TRUE((std::chrono::high_resolution_clock::now() - begin) < std::chrono::milliseconds(250));
}

TEST_F(TimerTests, doubleTimerTestGroupWait) {
  auto begin = std::chrono::high_resolution_clock::now();
  contextGroup.spawn([&] {
    Timer(dispatcher).sleep(std::chrono::milliseconds(100));
  });

  contextGroup.spawn([&] {
    Timer(dispatcher).sleep(std::chrono::milliseconds(200));
  });

  contextGroup.wait();
  ASSERT_GE(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin).count(), 150);
  ASSERT_TRUE((std::chrono::high_resolution_clock::now() - begin) < std::chrono::milliseconds(250));
}

TEST_F(TimerTests, doubleTimerTestTwoGroupsWait) {
  auto begin = std::chrono::high_resolution_clock::now();
  ContextGroup cg(dispatcher);
  cg.spawn([&] {
    Timer(dispatcher).sleep(std::chrono::milliseconds(100));
  });

  contextGroup.spawn([&] {
    Timer(dispatcher).sleep(std::chrono::milliseconds(200));
  });

  contextGroup.wait();
  ASSERT_GE(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin).count(), 150);
  ASSERT_TRUE((std::chrono::high_resolution_clock::now() - begin) < std::chrono::milliseconds(275));
}

TEST_F(TimerTests, movedTimerIsWorking2) {
  bool done = false;
  contextGroup.spawn([&] {
    Timer t(dispatcher);
    t = Timer{dispatcher};
    //contextGroup.spawn([&]() { done = true; });

    ASSERT_FALSE(done);
    t.sleep(std::chrono::milliseconds(10));
    ASSERT_TRUE(done);
  });

  contextGroup.spawn([&]() { 
    done = true; 
  });

  contextGroup.wait();
}

TEST_F(TimerTests, movedAndStoopedTimerIsWorking2) {
  contextGroup.spawn([&] {
    Timer src(dispatcher);
    contextGroup.interrupt();
    Timer t(dispatcher);
    t = std::move(src);

    ASSERT_ANY_THROW(t.sleep(std::chrono::milliseconds(1)));
  });
}

TEST_F(TimerTests, movedTimerIsTheSame) {
  contextGroup.spawn([&] {
    Timer timer(dispatcher);
    auto timerPtr1 = &timer;
    Timer srcEvent(dispatcher);
    timer = std::move(srcEvent);
    auto timerPtr2 = &timer;
    ASSERT_EQ(timerPtr1, timerPtr2);
  });
}

TEST_F(TimerTests, timerStartIsWorking) {
  contextGroup.spawn([&] {
    Timer t(dispatcher);
    contextGroup.interrupt();
    ASSERT_ANY_THROW(t.sleep(std::chrono::milliseconds(1)));
    ASSERT_NO_THROW(t.sleep(std::chrono::milliseconds(1)));
  });
}

TEST_F(TimerTests, timerStopBeforeSleep) {
  contextGroup.spawn([&] {
    Timer t(dispatcher);
    contextGroup.interrupt();
    ASSERT_THROW(t.sleep(std::chrono::milliseconds(1)), InterruptedException);
    contextGroup.interrupt();
    ASSERT_THROW(t.sleep(std::chrono::milliseconds(1)), InterruptedException);
  });
}

TEST_F(TimerTests, timerIsCancelable) {
  contextGroup.spawn([&] {
    Timer t(dispatcher);
    ASSERT_THROW(t.sleep(std::chrono::milliseconds(100)), InterruptedException);
  });

  contextGroup.spawn([&]() { 
    contextGroup.interrupt(); 
  });
}

// Disabled, because on OS X it is currently impossible to distinguish timer timeout and interrupt
TEST_F(TimerTests, DISABLED_sleepThrowsOnlyIfTimerIsStoppedBeforeTime1) {
  contextGroup.spawn([&] {
    Timer t(dispatcher);
    ASSERT_NO_THROW(t.sleep(std::chrono::milliseconds(1)));
    ASSERT_THROW(t.sleep(std::chrono::milliseconds(1)), InterruptedException);
  });

  contextGroup.spawn([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    contextGroup.interrupt();
  });
}

TEST_F(TimerTests, sleepIsSleepingAtLeastTakenTime) {
  auto timepoint1 = std::chrono::high_resolution_clock::now();
  contextGroup.spawn([&] {
    Timer(dispatcher).sleep(std::chrono::milliseconds(100));
  });

  contextGroup.wait();
  auto timepoint2 = std::chrono::high_resolution_clock::now();  
  ASSERT_LE(95, std::chrono::duration_cast<std::chrono::milliseconds>(timepoint2 - timepoint1).count());
}

TEST_F(TimerTests, timerIsReusable) {
  Timer t(dispatcher);
  auto timepoint1 = std::chrono::high_resolution_clock::now();
  contextGroup.spawn([&] {
    ASSERT_NO_THROW(t.sleep(std::chrono::seconds(1))); 
  });

  contextGroup.wait();
  auto timepoint2 = std::chrono::high_resolution_clock::now();
  contextGroup.spawn([&] {
    ASSERT_NO_THROW(t.sleep(std::chrono::seconds(1))); 
  });

  contextGroup.wait();
  auto timepoint3 = std::chrono::high_resolution_clock::now();
  ASSERT_LE(950, std::chrono::duration_cast<std::chrono::milliseconds>(timepoint2 - timepoint1).count());
  ASSERT_LE(950, std::chrono::duration_cast<std::chrono::milliseconds>(timepoint3 - timepoint2).count());
}

TEST_F(TimerTests, timerIsReusableAfterInterrupt) {
  contextGroup.spawn([&] {
    Timer t(dispatcher);
    contextGroup.interrupt();
    auto timepoint1 = std::chrono::high_resolution_clock::now();
    ASSERT_THROW(t.sleep(std::chrono::seconds(1)), InterruptedException);
    auto timepoint2 = std::chrono::high_resolution_clock::now();
    ASSERT_NO_THROW(t.sleep(std::chrono::seconds(1)));
    auto timepoint3 = std::chrono::high_resolution_clock::now();
    ASSERT_LE(0, std::chrono::duration_cast<std::chrono::milliseconds>(timepoint2 - timepoint1).count());
    ASSERT_LE(950, std::chrono::duration_cast<std::chrono::milliseconds>(timepoint3 - timepoint2).count());
  });
}

TEST_F(TimerTests, timerWithZeroTimeIsYielding) {
  bool done = false;
  contextGroup.spawn([&]() {
    done = true;
  });

  ASSERT_FALSE(done);
  Timer(dispatcher).sleep(std::chrono::milliseconds(0));
  ASSERT_TRUE(done);
}
