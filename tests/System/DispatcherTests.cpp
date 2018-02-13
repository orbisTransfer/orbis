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
#include <future>
#include <System/Context.h>
#include <System/Dispatcher.h>
#include <System/Event.h>
#include <System/Timer.h>
#include <gtest/gtest.h>

using namespace System;

class DispatcherTests : public testing::Test {
public:
  Dispatcher dispatcher;
};

TEST_F(DispatcherTests, clearRemainsDispatcherWorkable) {
  dispatcher.clear();
  bool spawnDone = false;
  Context<> context(dispatcher, [&]() {
    spawnDone = true;
  });

  dispatcher.yield();
  ASSERT_TRUE(spawnDone);
}

TEST_F(DispatcherTests, clearRemainsDispatcherWorkableAfterAsyncOperation) {
  bool spawn1Done = false;
  bool spawn2Done = false;
  Context<> context(dispatcher, [&]() {
    spawn1Done = true;
  });
  
  dispatcher.yield();
  ASSERT_TRUE(spawn1Done);
  dispatcher.clear();
  Context<> contextSecond(dispatcher, [&]() {
    spawn2Done = true;
  });

  dispatcher.yield();
  ASSERT_TRUE(spawn2Done);
}

TEST_F(DispatcherTests, clearCalledFromSpawnRemainsDispatcherWorkable) {
  bool spawn1Done = false;
  bool spawn2Done = false;
  Context<> context(dispatcher, [&]() {
    dispatcher.clear();
    spawn1Done = true;
  });

  dispatcher.yield();
  ASSERT_TRUE(spawn1Done);
  Context<> contextSecond(dispatcher, [&]() {
    spawn2Done = true;
  });

  dispatcher.yield();
  ASSERT_TRUE(spawn2Done);
}

TEST_F(DispatcherTests, timerIsHandledOnlyAfterAllSpawnedTasksAreHandled) {
  Event event1(dispatcher);
  Event event2(dispatcher);
  Context<> context(dispatcher, [&]() {
    event1.set();
    Timer(dispatcher).sleep(std::chrono::milliseconds(1));
    event2.set();
  });

  dispatcher.yield();
  ASSERT_TRUE(event1.get());
  ASSERT_FALSE(event2.get());
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  dispatcher.pushContext(dispatcher.getCurrentContext());
  dispatcher.dispatch();
  ASSERT_FALSE(event2.get());
  dispatcher.yield();
  ASSERT_TRUE(event2.get());
}

TEST_F(DispatcherTests, dispatchKeepsSpawnOrder) {
  std::deque<size_t> executionOrder;
  std::deque<size_t> expectedOrder = { 1, 2 };
  Context<> context(dispatcher, [&]() {
    executionOrder.push_back(1);
  });

  Context<> contextSecond(dispatcher, [&]() {
    executionOrder.push_back(2);
  });

  dispatcher.pushContext(dispatcher.getCurrentContext());
  dispatcher.dispatch();
  ASSERT_EQ(executionOrder, expectedOrder);
}

TEST_F(DispatcherTests, dispatchKeepsSpawnOrderWithNesting) {
  std::deque<size_t> executionOrder;
  std::deque<size_t> expectedOrder = { 1, 2, 3, 4 };
  auto mainContext = dispatcher.getCurrentContext();
  Context<> context(dispatcher, [&]() {
    executionOrder.push_back(1);
    Context<> context(dispatcher, [&]() {
      executionOrder.push_back(3);
    });
  });

  Context<> contextSecond(dispatcher, [&]() {
    executionOrder.push_back(2);
    Context<> context(dispatcher, [&]() {
      executionOrder.push_back(4);
      dispatcher.pushContext(mainContext);
    });
  });

  dispatcher.dispatch();
  ASSERT_EQ(executionOrder, expectedOrder);
}

TEST_F(DispatcherTests, dispatchKeepsSpawnResumingOrder) {
  std::deque<size_t> executionOrder;
  std::deque<size_t> expectedOrder = { 1, 2, 3, 4 };
  std::vector<NativeContext*> contexts;
  Context<> context(dispatcher, [&]() {
    executionOrder.push_back(1);
    contexts.push_back(dispatcher.getCurrentContext());
    dispatcher.dispatch();
    executionOrder.push_back(3);
  });

  Context<> contextSecond(dispatcher, [&]() {
    executionOrder.push_back(2);
    contexts.push_back(dispatcher.getCurrentContext());
    dispatcher.dispatch();
    executionOrder.push_back(4);
  });

  dispatcher.pushContext(dispatcher.getCurrentContext());
  dispatcher.dispatch();
  for (auto& ctx : contexts) {
    dispatcher.pushContext(ctx);
  }

  dispatcher.pushContext(dispatcher.getCurrentContext());
  dispatcher.dispatch();
  ASSERT_EQ(executionOrder, expectedOrder);
}

TEST_F(DispatcherTests, getCurrentContextDiffersForParallelSpawn) {
  void* ctx1 = nullptr;
  void* ctx2 = nullptr;
  Context<> context(dispatcher, [&]() {
    ctx1 = dispatcher.getCurrentContext();
  });

  Context<> contextSecond(dispatcher, [&]() {
    ctx2 = dispatcher.getCurrentContext();
  });

  dispatcher.yield();
  ASSERT_NE(ctx1, nullptr);
  ASSERT_NE(ctx2, nullptr);
  ASSERT_NE(ctx1, ctx2);
}

TEST_F(DispatcherTests, getCurrentContextSameForSequentialSpawn) {
  void* ctx1 = nullptr;
  void* ctx2 = nullptr;
  Context<> context(dispatcher, [&]() {
    ctx1 = dispatcher.getCurrentContext();
    dispatcher.yield();
    ctx2 = dispatcher.getCurrentContext();
  });

  dispatcher.yield();
  dispatcher.yield();
  ASSERT_NE(ctx1, nullptr);
  ASSERT_EQ(ctx1, ctx2);
}

TEST_F(DispatcherTests, pushedContextMustGoOn) {
  bool spawnDone = false;
  Context<> context(dispatcher, [&]() {
    spawnDone = true;
  });

  dispatcher.pushContext(dispatcher.getCurrentContext());
  dispatcher.dispatch();
  ASSERT_TRUE(spawnDone);
}

TEST_F(DispatcherTests, pushedContextMustGoOnFromNestedSpawns) {
  bool spawnDone = false;
  auto mainContext = dispatcher.getCurrentContext();
  Context<> context(dispatcher, [&]() {
    spawnDone = true;
    dispatcher.pushContext(mainContext);
  });

  dispatcher.dispatch();
  ASSERT_TRUE(spawnDone);
}

TEST_F(DispatcherTests, remoteSpawnActuallySpawns) {
  Event remoteSpawnDone(dispatcher);
  auto remoteSpawnThread = std::thread([&] {
    dispatcher.remoteSpawn([&]() {
      remoteSpawnDone.set();
    });
  });

  if (remoteSpawnThread.joinable()) {
    remoteSpawnThread.join();
  }

  dispatcher.yield();
  ASSERT_TRUE(remoteSpawnDone.get());
}

TEST_F(DispatcherTests, remoteSpawnActuallySpawns2) {
  Event remoteSpawnDone(dispatcher);
  auto remoteSpawnThread = std::thread([&] { 
    dispatcher.remoteSpawn([&]() { 
      remoteSpawnDone.set(); 
    }); 
  });

  if (remoteSpawnThread.joinable()) {
    remoteSpawnThread.join();
  }

  Timer(dispatcher).sleep(std::chrono::milliseconds(3));
  ASSERT_TRUE(remoteSpawnDone.get());
}

TEST_F(DispatcherTests, remoteSpawnActuallySpawns3) {
  Event remoteSpawnDone(dispatcher);
  auto mainCtx = dispatcher.getCurrentContext();
  auto remoteSpawnThread = std::thread([&, this] {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    dispatcher.remoteSpawn([&, this]() {
      remoteSpawnDone.set();
      dispatcher.pushContext(mainCtx);
    });
  });

  dispatcher.dispatch();
  ASSERT_TRUE(remoteSpawnDone.get());
  if (remoteSpawnThread.joinable()) {
    remoteSpawnThread.join();
  }
}

TEST_F(DispatcherTests, remoteSpawnSpawnsProcedureInDispatcherThread) {
  Event remoteSpawnDone(dispatcher);
  auto mainSpawnThrId = std::this_thread::get_id();
  decltype(mainSpawnThrId) remoteSpawnThrId;
  auto remoteSpawnThread = std::thread([&] {
    dispatcher.remoteSpawn([&]() {
      remoteSpawnThrId = std::this_thread::get_id();
      remoteSpawnDone.set();
    });
  });

  remoteSpawnDone.wait();
  if (remoteSpawnThread.joinable()) {
    remoteSpawnThread.join();
  }

  ASSERT_EQ(mainSpawnThrId, remoteSpawnThrId);
}

TEST_F(DispatcherTests, remoteSpawnSpawnsProcedureAndKeepsOrder) {
  Event remoteSpawnDone(dispatcher);
  std::deque<size_t> executionOrder;
  std::deque<size_t> expectedOrder = { 1, 2 };
  auto remoteSpawnThread = std::thread([&] {
    dispatcher.remoteSpawn([&]() {
      executionOrder.push_back(1);
    });

    dispatcher.remoteSpawn([&]() {
      executionOrder.push_back(2);
      remoteSpawnDone.set();
    });
  });

  if (remoteSpawnThread.joinable()) {
    remoteSpawnThread.join();
  }

  remoteSpawnDone.wait();
  ASSERT_EQ(executionOrder, expectedOrder);
}

TEST_F(DispatcherTests, remoteSpawnActuallyWorksParallel) {
  Event remoteSpawnDone(dispatcher);
  auto remoteSpawnThread = std::thread([&] {
    dispatcher.remoteSpawn([&]() {
      remoteSpawnDone.set();
    });
  });

  Timer(dispatcher).sleep(std::chrono::milliseconds(100));
  ASSERT_TRUE(remoteSpawnDone.get());

  if (remoteSpawnThread.joinable()) {
    remoteSpawnThread.join();
  }
}

TEST_F(DispatcherTests, spawnActuallySpawns) {
  bool spawnDone = false;
  Context<> context(dispatcher, [&]() {
    spawnDone = true;
  });

  dispatcher.yield();
  ASSERT_TRUE(spawnDone);
}

TEST_F(DispatcherTests, spawnJustSpawns) {
  bool spawnDone = false;
  Context<> context(dispatcher, [&]() {
    spawnDone = true;
  });

  ASSERT_FALSE(spawnDone);
  dispatcher.yield();
  ASSERT_TRUE(spawnDone);
}

TEST_F(DispatcherTests, yieldReturnsIfNothingToSpawn) {
  dispatcher.yield();
}

TEST_F(DispatcherTests, yieldReturnsAfterExecutionOfSpawnedProcedures) {
  bool spawnDone = false;
  Context<> context(dispatcher, [&]() {
    spawnDone = true;
  });

  dispatcher.yield();
  ASSERT_TRUE(spawnDone);
}

TEST_F(DispatcherTests, yieldReturnsAfterExecutionOfIO) {
  Context<> context(dispatcher, [&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    dispatcher.yield();
  });

  Timer(dispatcher).sleep(std::chrono::milliseconds(1));
  dispatcher.yield();
  SUCCEED();
}

TEST_F(DispatcherTests, yieldExecutesIoOnItsFront) {
  bool spawnDone = false;
  Context<> context(dispatcher, [&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    dispatcher.yield();
    spawnDone = true;
  });

  Timer(dispatcher).sleep(std::chrono::milliseconds(1));
  ASSERT_FALSE(spawnDone);
  dispatcher.yield();
  ASSERT_TRUE(spawnDone);
}
