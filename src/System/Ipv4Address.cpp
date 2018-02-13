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
#include "Ipv4Address.h"
#include <stdexcept>

namespace System {

namespace {

uint8_t readUint8(const std::string& source, size_t& offset) {
  if (offset == source.size() || source[offset] < '0' || source[offset] > '9') {
    throw std::runtime_error("Unable to read value from string");
  }

  uint8_t value = source[offset] - '0';
  if (offset + 1 == source.size() || source[offset + 1] < '0' || source[offset + 1] > '9') {
    offset = offset + 1;
    return value;
  }

  if (value == 0) {
    throw std::runtime_error("Unable to read value from string");
  }

  value = value * 10 + (source[offset + 1] - '0');
  if (offset + 2 == source.size() || source[offset + 2] < '0' || source[offset + 2] > '9') {
    offset = offset + 2;
    return value;
  }

  if ((value == 25 && source[offset + 2] > '5') || value > 25) {
    throw std::runtime_error("Unable to read value from string");
  }

  value = value * 10 + (source[offset + 2] - '0');
  offset = offset + 3;
  return value;
}

}

Ipv4Address::Ipv4Address(uint32_t value) : value(value) {
}

Ipv4Address::Ipv4Address(const std::string& dottedDecimal) {
  size_t offset = 0;
  value = readUint8(dottedDecimal, offset);
  if (offset == dottedDecimal.size() || dottedDecimal[offset] != '.') {
    throw std::runtime_error("Invalid Ipv4 address string");
  }

  ++offset;
  value = value << 8 | readUint8(dottedDecimal, offset);
  if (offset == dottedDecimal.size() || dottedDecimal[offset] != '.') {
    throw std::runtime_error("Invalid Ipv4 address string");
  }

  ++offset;
  value = value << 8 | readUint8(dottedDecimal, offset);
  if (offset == dottedDecimal.size() || dottedDecimal[offset] != '.') {
    throw std::runtime_error("Invalid Ipv4 address string");
  }

  ++offset;
  value = value << 8 | readUint8(dottedDecimal, offset);
  if (offset < dottedDecimal.size()) {
    throw std::runtime_error("Invalid Ipv4 address string");
  }
}

bool Ipv4Address::operator!=(const Ipv4Address& other) const {
  return value != other.value;
}

bool Ipv4Address::operator==(const Ipv4Address& other) const {
  return value == other.value;
}

uint32_t Ipv4Address::getValue() const {
  return value;
}

std::string Ipv4Address::toDottedDecimal() const {
  std::string result;
  result += std::to_string(value >> 24);
  result += '.';
  result += std::to_string(value >> 16 & 255);
  result += '.';
  result += std::to_string(value >> 8 & 255);
  result += '.';
  result += std::to_string(value & 255);
  return result;
}

bool Ipv4Address::isLoopback() const {
  // 127.0.0.0/8
  return (value & 0xff000000) == (127 << 24);
}

bool Ipv4Address::isPrivate() const {
  return
    // 10.0.0.0/8
    (value & 0xff000000) == (10 << 24) ||
    // 172.16.0.0/12
    (value & 0xfff00000) == ((172 << 24) | (16 << 16)) ||
    // 192.168.0.0/16
    (value & 0xffff0000) == ((192 << 24) | (168 << 16));
}

}
