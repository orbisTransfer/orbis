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

#include <list>
#include <vector>
#include <Common/MemoryInputStream.h>
#include <Common/StringOutputStream.h>
#include "JsonInputStreamSerializer.h"
#include "JsonOutputStreamSerializer.h"
#include "KVBinaryInputStreamSerializer.h"
#include "KVBinaryOutputStreamSerializer.h"

namespace Common {

template <typename T>
T getValueAs(const JsonValue& js) {
  return js;
  //cdstatic_assert(false, "undefined conversion");
}

template <>
inline std::string getValueAs<std::string>(const JsonValue& js) { return js.getString(); }

template <>
inline uint64_t getValueAs<uint64_t>(const JsonValue& js) { return static_cast<uint64_t>(js.getInteger()); }

}

namespace Orbis {

template <typename T>
Common::JsonValue storeToJsonValue(const T& v) {
  JsonOutputStreamSerializer s;
  serialize(const_cast<T&>(v), s);
  return s.getValue();
}

template <typename T>
Common::JsonValue storeContainerToJsonValue(const T& cont) {
  Common::JsonValue js(Common::JsonValue::ARRAY);
  for (const auto& item : cont) {
    js.pushBack(item);
  }
  return js;
}

template <typename T>
Common::JsonValue storeToJsonValue(const std::vector<T>& v) { return storeContainerToJsonValue(v); }

template <typename T>
Common::JsonValue storeToJsonValue(const std::list<T>& v) { return storeContainerToJsonValue(v); }

template <>
inline Common::JsonValue storeToJsonValue(const std::string& v) { return Common::JsonValue(v); }

template <typename T>
void loadFromJsonValue(T& v, const Common::JsonValue& js) {
  JsonInputValueSerializer s(js);
  serialize(v, s);
}

template <typename T>
void loadFromJsonValue(std::vector<T>& v, const Common::JsonValue& js) {
  for (size_t i = 0; i < js.size(); ++i) {
    v.push_back(Common::getValueAs<T>(js[i]));
  }
}

template <typename T>
void loadFromJsonValue(std::list<T>& v, const Common::JsonValue& js) {
  for (size_t i = 0; i < js.size(); ++i) {
    v.push_back(Common::getValueAs<T>(js[i]));
  }
}

template <typename T>
std::string storeToJson(const T& v) {
  return storeToJsonValue(v).toString();
}

template <typename T>
bool loadFromJson(T& v, const std::string& buf) {
  try {
    if (buf.empty()) {
      return true;
    }
    auto js = Common::JsonValue::fromString(buf);
    loadFromJsonValue(v, js);
  } catch (std::exception&) {
    return false;
  }
  return true;
}

template <typename T>
std::string storeToBinaryKeyValue(const T& v) {
  KVBinaryOutputStreamSerializer s;
  serialize(const_cast<T&>(v), s);
  
  std::string result;
  Common::StringOutputStream stream(result);
  s.dump(stream);
  return result;
}

template <typename T>
bool loadFromBinaryKeyValue(T& v, const std::string& buf) {
  try {
    Common::MemoryInputStream stream(buf.data(), buf.size());
    KVBinaryInputStreamSerializer s(stream);
    serialize(v, s);
    return true;
  } catch (std::exception&) {
    return false;
  }
}

}
