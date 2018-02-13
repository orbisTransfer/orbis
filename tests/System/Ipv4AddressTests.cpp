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
#include <System/Ipv4Address.h>
#include <gtest/gtest.h>

using namespace System;

TEST(Ipv4AddressTest, value) {
  Ipv4Address address1(0x00000000);
  ASSERT_EQ(0x00000000, address1.getValue());
  Ipv4Address address2(0xfefdfcfb);
  ASSERT_EQ(0xfefdfcfb, address2.getValue());
  Ipv4Address address3 = address1;
  ASSERT_EQ(0x00000000, address3.getValue());
  Ipv4Address address4 = address2;
  ASSERT_EQ(0xfefdfcfb, address4.getValue());
  address3 = address2;
  ASSERT_EQ(0xfefdfcfb, address3.getValue());
  address4 = address1;
  ASSERT_EQ(0x00000000, address4.getValue());
}

TEST(Ipv4AddressTest, dottedDecimal) {
  ASSERT_EQ(0x00000000, Ipv4Address("0.0.0.0").getValue());
  ASSERT_EQ(0x01020304, Ipv4Address("1.2.3.4").getValue());
  ASSERT_EQ(0x7f000001, Ipv4Address("127.0.0.1").getValue());
  ASSERT_EQ(0xfefdfcfb, Ipv4Address("254.253.252.251").getValue());
  ASSERT_EQ(0xffffffff, Ipv4Address("255.255.255.255").getValue());
  ASSERT_EQ("0.0.0.0", Ipv4Address(0x00000000).toDottedDecimal());
  ASSERT_EQ("1.2.3.4", Ipv4Address(0x01020304).toDottedDecimal());
  ASSERT_EQ("127.0.0.1", Ipv4Address(0x7f000001).toDottedDecimal());
  ASSERT_EQ("254.253.252.251", Ipv4Address(0xfefdfcfb).toDottedDecimal());
  ASSERT_EQ("255.255.255.255", Ipv4Address(0xffffffff).toDottedDecimal());
  ASSERT_THROW(Ipv4Address(".0.0.0.0"), std::runtime_error);
  ASSERT_THROW(Ipv4Address("0..0.0.0"), std::runtime_error);
  ASSERT_THROW(Ipv4Address("0.0.0"), std::runtime_error);
  ASSERT_THROW(Ipv4Address("0.0.0."), std::runtime_error);
  ASSERT_THROW(Ipv4Address("0.0.0.0."), std::runtime_error);
  ASSERT_THROW(Ipv4Address("0.0.0.0.0"), std::runtime_error);
  ASSERT_THROW(Ipv4Address("0.0.0.00"), std::runtime_error);
  ASSERT_THROW(Ipv4Address("0.0.0.01"), std::runtime_error);
  ASSERT_THROW(Ipv4Address("0.0.0.256"), std::runtime_error);
  ASSERT_THROW(Ipv4Address("00.0.0.0"), std::runtime_error);
  ASSERT_THROW(Ipv4Address("01.0.0.0"), std::runtime_error);
  ASSERT_THROW(Ipv4Address("256.0.0.0"), std::runtime_error);
}

TEST(Ipv4AddressTest, isLoopback) {
  // 127.0.0.0/8
  ASSERT_TRUE(Ipv4Address("127.0.0.1").isLoopback());
  ASSERT_TRUE(Ipv4Address("127.1.1.1").isLoopback());
  ASSERT_TRUE(Ipv4Address("127.1.0.0").isLoopback());
  ASSERT_TRUE(Ipv4Address("127.255.255.255").isLoopback());

  ASSERT_FALSE(Ipv4Address("255.0.0.0").isLoopback());
  ASSERT_FALSE(Ipv4Address("255.255.255.255").isLoopback());
  ASSERT_FALSE(Ipv4Address("128.1.0.0").isLoopback());
  ASSERT_FALSE(Ipv4Address("192.168.1.1").isLoopback());
  ASSERT_FALSE(Ipv4Address("10.0.0.1").isLoopback());
}

TEST(Ipv4AddressTest, isPrivate) {
  // 10.0.0.0/8
  ASSERT_TRUE(Ipv4Address("10.0.0.0").isPrivate());
  ASSERT_TRUE(Ipv4Address("10.0.0.1").isPrivate());
  ASSERT_TRUE(Ipv4Address("10.0.0.255").isPrivate());
  ASSERT_TRUE(Ipv4Address("10.255.255.255").isPrivate());

  ASSERT_FALSE(Ipv4Address("11.0.0.255").isPrivate());
  ASSERT_FALSE(Ipv4Address("9.0.0.0").isPrivate());
  ASSERT_FALSE(Ipv4Address("138.0.0.1").isPrivate());
  ASSERT_FALSE(Ipv4Address("255.255.255.255").isPrivate());

  // 172.16.0.0/12 
  ASSERT_TRUE(Ipv4Address("172.16.0.255").isPrivate());
  ASSERT_TRUE(Ipv4Address("172.17.0.0").isPrivate());
  ASSERT_TRUE(Ipv4Address("172.19.1.1").isPrivate());
  ASSERT_TRUE(Ipv4Address("172.31.255.255").isPrivate());

  ASSERT_FALSE(Ipv4Address("172.32.0.0").isPrivate());
  ASSERT_FALSE(Ipv4Address("172.32.0.1").isPrivate());
  ASSERT_FALSE(Ipv4Address("172.15.0.0").isPrivate());
  ASSERT_FALSE(Ipv4Address("172.15.255.255").isPrivate());

  // 192.168.0.0/16
  ASSERT_TRUE(Ipv4Address("192.168.0.0").isPrivate());
  ASSERT_TRUE(Ipv4Address("192.168.1.1").isPrivate());
  ASSERT_TRUE(Ipv4Address("192.168.100.100").isPrivate());
  ASSERT_TRUE(Ipv4Address("192.168.255.255").isPrivate());

  ASSERT_FALSE(Ipv4Address("192.167.255.255").isPrivate());
  ASSERT_FALSE(Ipv4Address("191.168.255.255").isPrivate());
  ASSERT_FALSE(Ipv4Address("192.169.255.255").isPrivate());
  ASSERT_FALSE(Ipv4Address("192.169.0.0").isPrivate());

  ASSERT_FALSE(Ipv4Address("255.255.255.255").isPrivate());

}
