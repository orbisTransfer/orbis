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
#pragma once

#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace Common {

std::string asString(const void* data, size_t size); // Does not throw
std::string asString(const std::vector<uint8_t>& data); // Does not throw
std::vector<uint8_t> asBinaryArray(const std::string& data);

uint8_t fromHex(char character); // Returns value of hex 'character', throws on error
bool fromHex(char character, uint8_t& value); // Assigns value of hex 'character' to 'value', returns false on error, does not throw
size_t fromHex(const std::string& text, void* data, size_t bufferSize); // Assigns values of hex 'text' to buffer 'data' up to 'bufferSize', returns actual data size, throws on error
bool fromHex(const std::string& text, void* data, size_t bufferSize, size_t& size); // Assigns values of hex 'text' to buffer 'data' up to 'bufferSize', assigns actual data size to 'size', returns false on error, does not throw
std::vector<uint8_t> fromHex(const std::string& text); // Returns values of hex 'text', throws on error
bool fromHex(const std::string& text, std::vector<uint8_t>& data); // Appends values of hex 'text' to 'data', returns false on error, does not throw

template <typename T>
bool podFromHex(const std::string& text, T& val) {
  size_t outSize;
  return fromHex(text, &val, sizeof(val), outSize) && outSize == sizeof(val);
}

std::string toHex(const void* data, size_t size); // Returns hex representation of ('data', 'size'), does not throw
void toHex(const void* data, size_t size, std::string& text); // Appends hex representation of ('data', 'size') to 'text', does not throw
std::string toHex(const std::vector<uint8_t>& data); // Returns hex representation of 'data', does not throw
void toHex(const std::vector<uint8_t>& data, std::string& text); // Appends hex representation of 'data' to 'text', does not throw

template<class T>
std::string podToHex(const T& s) {
  return toHex(&s, sizeof(s));
}

std::string extract(std::string& text, char delimiter); // Does not throw
std::string extract(const std::string& text, char delimiter, size_t& offset); // Does not throw

template<typename T> T fromString(const std::string& text) { // Throws on error
  T value;
  std::istringstream stream(text);
  stream >> value;
  if (stream.fail()) {
    throw std::runtime_error("fromString: unable to parse value");
  }

  return value;
}

template<typename T> bool fromString(const std::string& text, T& value) { // Does not throw
  std::istringstream stream(text);
  stream >> value;
  return !stream.fail();
}

template<typename T> std::vector<T> fromDelimitedString(const std::string& source, char delimiter) { // Throws on error
  std::vector<T> data;
  for (size_t offset = 0; offset != source.size();) {
    data.emplace_back(fromString<T>(extract(source, delimiter, offset)));
  }

  return data;
}

template<typename T> bool fromDelimitedString(const std::string& source, char delimiter, std::vector<T>& data) { // Does not throw
  for (size_t offset = 0; offset != source.size();) {
    T value;
    if (!fromString<T>(extract(source, delimiter, offset), value)) {
      return false;
    }

    data.emplace_back(value);
  }

  return true;
}

template<typename T> std::string toString(const T& value) { // Does not throw
  std::ostringstream stream;
  stream << value;
  return stream.str();
}

template<typename T> void toString(const T& value, std::string& text) { // Does not throw
  std::ostringstream stream;
  stream << value;
  text += stream.str();
}

bool loadFileToString(const std::string& filepath, std::string& buf);
bool saveStringToFile(const std::string& filepath, const std::string& buf);


std::string base64Decode(std::string const& encoded_string);

std::string ipAddressToString(uint32_t ip);
bool parseIpAddressAndPort(uint32_t& ip, uint32_t& port, const std::string& addr);

std::string timeIntervalToString(uint64_t intervalInSeconds);

}
