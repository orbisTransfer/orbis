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
#include <System/Dispatcher.h>
#include <System/ContextGroup.h>
#include <System/InterruptedException.h>
#include <System/Ipv4Address.h>
#include <System/Ipv4Resolver.h>
#include <gtest/gtest.h>

using namespace System;

class Ipv4ResolverTests : public testing::Test {
public:
  Ipv4ResolverTests() : contextGroup(dispatcher), resolver(dispatcher) {
  }

  Dispatcher dispatcher;
  ContextGroup contextGroup;
  Ipv4Resolver resolver;
};

TEST_F(Ipv4ResolverTests, start) {
  contextGroup.spawn([&] { 
    ASSERT_NO_THROW(Ipv4Resolver(dispatcher).resolve("localhost")); 
  });
  contextGroup.wait();
}

TEST_F(Ipv4ResolverTests, stop) {
  contextGroup.spawn([&] {
    contextGroup.interrupt();
    ASSERT_THROW(resolver.resolve("localhost"), InterruptedException);
  });
  contextGroup.wait();
}

TEST_F(Ipv4ResolverTests, interruptWhileResolving) {
  contextGroup.spawn([&] {
    ASSERT_THROW(resolver.resolve("localhost"), InterruptedException);
  });
  contextGroup.interrupt();
  contextGroup.wait();
}

TEST_F(Ipv4ResolverTests, reuseAfterInterrupt) {
  contextGroup.spawn([&] {
    ASSERT_THROW(resolver.resolve("localhost"), InterruptedException);
  });
  contextGroup.interrupt();
  contextGroup.wait();
  contextGroup.spawn([&] {
    ASSERT_NO_THROW(resolver.resolve("localhost"));
  });
  contextGroup.wait();
}

TEST_F(Ipv4ResolverTests, resolve) {
  ASSERT_EQ(Ipv4Address("0.0.0.0"), resolver.resolve("0.0.0.0"));
  ASSERT_EQ(Ipv4Address("1.2.3.4"), resolver.resolve("1.2.3.4"));
  ASSERT_EQ(Ipv4Address("127.0.0.1"), resolver.resolve("127.0.0.1"));
  ASSERT_EQ(Ipv4Address("254.253.252.251"), resolver.resolve("254.253.252.251"));
  ASSERT_EQ(Ipv4Address("255.255.255.255"), resolver.resolve("255.255.255.255"));
  ASSERT_EQ(Ipv4Address("127.0.0.1"), resolver.resolve("localhost"));
//ASSERT_EQ(Ipv4Address("93.184.216.34"), resolver.resolve("example.com"));
  ASSERT_THROW(resolver.resolve(".0.0.0.0"), std::runtime_error);
  ASSERT_THROW(resolver.resolve("0..0.0.0"), std::runtime_error);
//ASSERT_THROW(resolver.resolve("0.0.0"), std::runtime_error);
  ASSERT_THROW(resolver.resolve("0.0.0."), std::runtime_error);
//ASSERT_THROW(resolver.resolve("0.0.0.0."), std::runtime_error);
  ASSERT_THROW(resolver.resolve("0.0.0.0.0"), std::runtime_error);
//ASSERT_THROW(resolver.resolve("0.0.0.00"), std::runtime_error);
//ASSERT_THROW(resolver.resolve("0.0.0.01"), std::runtime_error);
  ASSERT_THROW(resolver.resolve("0.0.0.256"), std::runtime_error);
//ASSERT_THROW(resolver.resolve("00.0.0.0"), std::runtime_error);
//ASSERT_THROW(resolver.resolve("01.0.0.0"), std::runtime_error);
  ASSERT_THROW(resolver.resolve("256.0.0.0"), std::runtime_error);
  ASSERT_THROW(resolver.resolve("invalid"), std::runtime_error);
}
