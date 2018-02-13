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
#include <System/ContextGroup.h>
#include <System/Dispatcher.h>
#include <System/Event.h>
#include <System/InterruptedException.h>
#include <System/Ipv4Address.h>
#include <System/Timer.h>
#include <System/TcpConnection.h>
#include <System/TcpConnector.h>
#include <System/TcpListener.h>
#include <gtest/gtest.h>

#include <thread>

using namespace System;

TEST(ContextGroupTests, testHangingUp) {
  Dispatcher dispatcher;
  Event e(dispatcher);
  Context<> context(dispatcher, [&] {
    Timer(dispatcher).sleep(std::chrono::milliseconds(100));
  });

  Context<> contextSecond(dispatcher, [&] {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    e.set();
    dispatcher.yield();
  });

  e.wait();
}

TEST(ContextGroupTests, ContextGroupWaitIsWaiting) {
  Dispatcher dispatcher;

  bool contextFinished = false;
  ContextGroup cg1(dispatcher);
  cg1.spawn([&] {
    dispatcher.yield();
    contextFinished = true;
  });

  cg1.wait();
  ASSERT_TRUE(contextFinished);
}

TEST(ContextGroupTests, ContextGroupInterruptIsInterrupting) {
  Dispatcher dispatcher;

  bool interrupted = false;
  ContextGroup cg1(dispatcher);
  cg1.spawn([&] {
    interrupted = dispatcher.interrupted();
  });

  cg1.interrupt();
  cg1.wait();

  ASSERT_TRUE(interrupted);
} 

TEST(ContextGroupTests, ContextGroupDestructorIsInterrupt_Waitable) {
  Dispatcher dispatcher;

  bool interrupted = false;
  bool contextFinished = false;
  {
    ContextGroup cg1(dispatcher);
    cg1.spawn([&] {
      interrupted = dispatcher.interrupted();
      Timer(dispatcher).sleep(std::chrono::milliseconds(100));
      contextFinished = true;
    });
  }

  ASSERT_TRUE(interrupted);
  ASSERT_TRUE(contextFinished);
}

TEST(ContextGroupTests, TimerIsContextIntrerruptible) {
  Dispatcher dispatcher;
  
  bool interrupted = false;
  {
    ContextGroup cg1(dispatcher);
    cg1.spawn([&] {
      try {
        Timer(dispatcher).sleep(std::chrono::milliseconds(1000));
      } catch (InterruptedException&) {
        interrupted = true;
      }
    });

    dispatcher.yield();
  }

  ASSERT_TRUE(interrupted);
}

TEST(ContextGroupTests, ListenerAcceptIsContextIntrerruptible) {
  Dispatcher dispatcher;

  bool interrupted = false;
  {
    ContextGroup cg1(dispatcher);
    cg1.spawn([&] {
      try {
        TcpListener(dispatcher, Ipv4Address("0.0.0.0"), 12345).accept();
      } catch (InterruptedException&) {
        interrupted = true;
      }
    });

    dispatcher.yield();
  }

  ASSERT_TRUE(interrupted);
}

TEST(ContextGroupTests, ConnectorConnectIsContextIntrerruptible) {
  Dispatcher dispatcher;

  bool interrupted = false;
  {
    ContextGroup cg1(dispatcher);
    cg1.spawn([&] {
      try {
        TcpConnector(dispatcher).connect(Ipv4Address("127.0.0.1"), 12345);
      } catch (InterruptedException&) {
        interrupted = true;
      }
    });

    dispatcher.yield();
  }
  ASSERT_TRUE(interrupted);
}

TEST(ContextGroupTests, ConnectionReadIsContextIntrerruptible) {
  Dispatcher dispatcher;

  bool interrupted = false;
  {
    Event connected(dispatcher);
    ContextGroup cg1(dispatcher);
    cg1.spawn([&] {
      try {
        auto conn = TcpListener(dispatcher, Ipv4Address("0.0.0.0"), 12345).accept();
        Timer(dispatcher).sleep(std::chrono::milliseconds(1000));
        conn.write(nullptr, 0);
      } catch (InterruptedException&) {
      }
    });

    cg1.spawn([&] {
      try {
        auto conn = TcpConnector(dispatcher).connect(Ipv4Address("127.0.0.1"), 12345);
        connected.set();
        uint8_t a[10];
        conn.read(a, 10);
        conn.write(nullptr, 0);
      } catch (InterruptedException&) {
        interrupted = true;
      }
    });

    connected.wait();
    dispatcher.yield();
  }

  ASSERT_TRUE(interrupted);
}

TEST(ContextGroupTests, TimerSleepIsThrowingWhenCurrentContextIsInterrupted) {
  Dispatcher dispatcher;

  bool interrupted = false;
  {
    ContextGroup cg1(dispatcher);
    cg1.spawn([&] {
      try {
        Timer(dispatcher).sleep(std::chrono::milliseconds(1000));
      } catch (InterruptedException&) {
        interrupted = true;
      }
    });
  }

  ASSERT_TRUE(interrupted);
}

TEST(ContextGroupTests, ListenerAcceptIsThrowingWhenCurrentContextIsInterrupted) {
  Dispatcher dispatcher;

  bool interrupted = false;
  {
    ContextGroup cg1(dispatcher);
    cg1.spawn([&] {
      try {
        TcpListener(dispatcher, Ipv4Address("0.0.0.0"), 12345).accept();
      } catch (InterruptedException&) {
        interrupted = true;
      }
    });
  }

  ASSERT_TRUE(interrupted);
}

TEST(ContextGroupTests, ConnectorConnectIsThrowingWhenCurrentContextIsInterrupted) {
  Dispatcher dispatcher;

  bool interrupted = false;
  {
    ContextGroup cg1(dispatcher);
    cg1.spawn([&] {
      try {
        TcpConnector(dispatcher).connect(Ipv4Address("127.0.0.1"), 12345);
      } catch (InterruptedException&) {
        interrupted = true;
      }
    });
  }

  ASSERT_TRUE(interrupted);
}

TEST(ContextGroupTests, ConnectionReadIsThrowingWhenCurrentContextIsInterrupted) {
  Dispatcher dispatcher;

  bool interrupted = false;
  {
    Event connected(dispatcher);
    ContextGroup cg1(dispatcher);
    cg1.spawn([&] {
      try {
        auto conn = TcpListener(dispatcher, Ipv4Address("0.0.0.0"), 12345).accept();
        conn.write(nullptr, 0);
      } catch (InterruptedException&) {
      }
    });

    cg1.spawn([&] {
      try {
        auto conn = TcpConnector(dispatcher).connect(Ipv4Address("127.0.0.1"), 12345);
        connected.set();
        dispatcher.yield();
        uint8_t a[10];
        conn.read(a, 10);
        conn.write(nullptr, 0);
      } catch (InterruptedException&) {
        interrupted = true;
      }
    });

    connected.wait();
  }

  ASSERT_TRUE(interrupted);
}

TEST(ContextGroupTests, ConnectionWriteIsThrowingWhenCurrentContextIsInterrupted) {
  Dispatcher dispatcher;

  bool interrupted = false;
  {
    Event connected(dispatcher);
    ContextGroup cg1(dispatcher);
    cg1.spawn([&] {
      try {
        auto conn = TcpListener(dispatcher, Ipv4Address("0.0.0.0"), 12345).accept();
        conn.write(nullptr, 0);
      } catch (InterruptedException&) {
      }
    });

    cg1.spawn([&] {
      try {
        auto conn = TcpConnector(dispatcher).connect(Ipv4Address("127.0.0.1"), 12345);
        connected.set();
        dispatcher.yield();
        conn.write(nullptr, 0);
      } catch (InterruptedException&) {
        interrupted = true;
      }
    });

    connected.wait();
  }

  ASSERT_TRUE(interrupted);
}

TEST(ContextGroupTests, DispatcherInterruptIsInterrupting) {
  bool interrupted = false;
  {
    Dispatcher dispatcher;
    Context<> context(dispatcher, [&] {
      try {
        Timer(dispatcher).sleep(std::chrono::milliseconds(1000));
      } catch (InterruptedException&) {
        interrupted = true;
      }
    });

    dispatcher.yield();
  }

  ASSERT_TRUE(interrupted);
}

TEST(ContextGroupTests, DispatcherInterruptSetsFlag) {
  Dispatcher dispatcher;
  Context<> context(dispatcher, [&] {
    try {
      Timer(dispatcher).sleep(std::chrono::milliseconds(10));
    } catch (InterruptedException&) {
    }
  });

  dispatcher.interrupt();
  dispatcher.yield();
  ASSERT_TRUE(dispatcher.interrupted());
  ASSERT_FALSE(dispatcher.interrupted());
}

TEST(ContextGroupTests, ContextGroupIsWaitingIncludigNestedSpawns) {
  Dispatcher dispatcher;

  bool contextFinished = false;
  bool nestedContextFinished = false;

  ContextGroup cg1(dispatcher);
  cg1.spawn([&] {
    try {
      cg1.spawn([&] {
        try {
          Timer(dispatcher).sleep(std::chrono::milliseconds(100));
          nestedContextFinished = true;
        } catch (InterruptedException&) {
        }
      });

      Timer(dispatcher).sleep(std::chrono::milliseconds(100));
      contextFinished = true;
    } catch (InterruptedException&) {
    }
  });

  cg1.wait();

  ASSERT_TRUE(contextFinished);
  ASSERT_TRUE(nestedContextFinished);
}

TEST(ContextGroupTests, ContextGroupIsWaitingNestedSpawnsEvenThoughItWasInterrupted) {
  Dispatcher dispatcher;

  bool contextFinished = false;
  bool nestedContextFinished = false;

  {
    ContextGroup cg1(dispatcher);
    cg1.spawn([&] {
      try {
        Timer(dispatcher).sleep(std::chrono::milliseconds(100));
        contextFinished = true;
      } catch (InterruptedException&) {
        cg1.spawn([&] {
          try {
            Timer(dispatcher).sleep(std::chrono::milliseconds(100));
            nestedContextFinished = true;
          } catch (InterruptedException&) {
          }
        });
      }
    });

    dispatcher.yield();
  }

  ASSERT_FALSE(contextFinished);
  ASSERT_TRUE(nestedContextFinished);
}
