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

#include "ISerializer.h"

#include <array>
#include <cstring>
#include <list>
#include <map>
#include <string>
#include <set>
#include <type_traits>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

namespace Orbis {

template<typename T>
typename std::enable_if<std::is_pod<T>::value>::type
serializeAsBinary(std::vector<T>& value, Common::StringView name, Orbis::ISerializer& serializer) {
  std::string blob;
  if (serializer.type() == ISerializer::INPUT) {
    serializer.binary(blob, name);
    value.resize(blob.size() / sizeof(T));
    if (blob.size()) {
      memcpy(&value[0], blob.data(), blob.size());
    }
  } else {
    if (!value.empty()) {
      blob.assign(reinterpret_cast<const char*>(&value[0]), value.size() * sizeof(T));
    }
    serializer.binary(blob, name);
  }
}

template<typename T>
typename std::enable_if<std::is_pod<T>::value>::type
serializeAsBinary(std::list<T>& value, Common::StringView name, Orbis::ISerializer& serializer) {
  std::string blob;
  if (serializer.type() == ISerializer::INPUT) {
    serializer.binary(blob, name);

    size_t count = blob.size() / sizeof(T);
    const T* ptr = reinterpret_cast<const T*>(blob.data());

    while (count--) {
      value.push_back(*ptr++);
    }
  } else {
    if (!value.empty()) {
      blob.resize(value.size() * sizeof(T));
      T* ptr = reinterpret_cast<T*>(&blob[0]);

      for (const auto& item : value) {
        *ptr++ = item;
      }
    }
    serializer.binary(blob, name);
  }
}

template <typename Cont>
bool serializeContainer(Cont& value, Common::StringView name, Orbis::ISerializer& serializer) {
  size_t size = value.size();
  if (!serializer.beginArray(size, name)) {
    value.clear();
    return false;
  }

  value.resize(size);

  for (auto& item : value) {
    serializer(const_cast<typename Cont::value_type&>(item), "");
  }

  serializer.endArray();
  return true;
}

template <typename E>
bool serializeEnumClass(E& value, Common::StringView name, Orbis::ISerializer& serializer) {
  static_assert(std::is_enum<E>::value, "E must be an enum class");

  typedef typename std::underlying_type<E>::type EType;

  if (serializer.type() == Orbis::ISerializer::INPUT) {
    EType numericValue;
    serializer(numericValue, name);
    value = static_cast<E>(numericValue);
  } else {
    auto numericValue = static_cast<EType>(value);
    serializer(numericValue, name);
  }

  return true;
}

template<typename T>
bool serialize(std::vector<T>& value, Common::StringView name, Orbis::ISerializer& serializer) {
  return serializeContainer(value, name, serializer);
}

template<typename T>
bool serialize(std::list<T>& value, Common::StringView name, Orbis::ISerializer& serializer) {
  return serializeContainer(value, name, serializer);
}

template<typename MapT, typename ReserveOp>
bool serializeMap(MapT& value, Common::StringView name, Orbis::ISerializer& serializer, ReserveOp reserve) {
  size_t size = value.size();

  if (!serializer.beginArray(size, name)) {
    value.clear();
    return false;
  }

  if (serializer.type() == Orbis::ISerializer::INPUT) {
    reserve(size);

    for (size_t i = 0; i < size; ++i) {
      typename MapT::key_type key;
      typename MapT::mapped_type v;

      serializer.beginObject("");
      serializer(key, "key");
      serializer(v, "value");
      serializer.endObject();

      value.insert(std::make_pair(std::move(key), std::move(v)));
    }
  } else {
    for (auto& kv : value) {
      serializer.beginObject("");
      serializer(const_cast<typename MapT::key_type&>(kv.first), "key");
      serializer(kv.second, "value");
      serializer.endObject();
    }
  }

  serializer.endArray();
  return true;
}

template<typename SetT>
bool serializeSet(SetT& value, Common::StringView name, Orbis::ISerializer& serializer) {
  size_t size = value.size();

  if (!serializer.beginArray(size, name)) {
    value.clear();
    return false;
  }

  if (serializer.type() == Orbis::ISerializer::INPUT) {
    for (size_t i = 0; i < size; ++i) {
      typename SetT::value_type key;
      serializer(key, "");
      value.insert(std::move(key));
    }
  } else {
    for (auto& key : value) {
      serializer(const_cast<typename SetT::value_type&>(key), "");
    }
  }

  serializer.endArray();
  return true;
}

template<typename K, typename Hash>
bool serialize(std::unordered_set<K, Hash>& value, Common::StringView name, Orbis::ISerializer& serializer) {
  return serializeSet(value, name, serializer);
}

template<typename K, typename Cmp>
bool serialize(std::set<K, Cmp>& value, Common::StringView name, Orbis::ISerializer& serializer) {
  return serializeSet(value, name, serializer);
}

template<typename K, typename V, typename Hash>
bool serialize(std::unordered_map<K, V, Hash>& value, Common::StringView name, Orbis::ISerializer& serializer) {
  return serializeMap(value, name, serializer, [&value](size_t size) { value.reserve(size); });
}

template<typename K, typename V, typename Hash>
bool serialize(std::unordered_multimap<K, V, Hash>& value, Common::StringView name, Orbis::ISerializer& serializer) {
  return serializeMap(value, name, serializer, [&value](size_t size) { value.reserve(size); });
}

template<typename K, typename V, typename Hash>
bool serialize(std::map<K, V, Hash>& value, Common::StringView name, Orbis::ISerializer& serializer) {
  return serializeMap(value, name, serializer, [](size_t size) {});
}

template<typename K, typename V, typename Hash>
bool serialize(std::multimap<K, V, Hash>& value, Common::StringView name, Orbis::ISerializer& serializer) {
  return serializeMap(value, name, serializer, [](size_t size) {});
}

template<size_t size>
bool serialize(std::array<uint8_t, size>& value, Common::StringView name, Orbis::ISerializer& s) {
  return s.binary(value.data(), value.size(), name);
}

template <typename T1, typename T2> 
void serialize(std::pair<T1, T2>& value, ISerializer& s) {
  s(value.first, "first");
  s(value.second, "second");
}

template <typename Element, typename Iterator>
void writeSequence(Iterator begin, Iterator end, Common::StringView name, ISerializer& s) {
  size_t size = std::distance(begin, end);
  s.beginArray(size, name);
  for (Iterator i = begin; i != end; ++i) {
    s(const_cast<Element&>(*i), "");
  }
  s.endArray();
}

template <typename Element, typename Iterator>
void readSequence(Iterator outputIterator, Common::StringView name, ISerializer& s) {
  size_t size = 0;
  s.beginArray(size, name);

  while (size--) {
    Element e;
    s(e, "");
    *outputIterator++ = std::move(e);
  }

  s.endArray();
}

//convinience function since we change block height type
void serializeBlockHeight(ISerializer& s, uint32_t& blockHeight, Common::StringView name);

//convinience function since we change global output index type
void serializeGlobalOutputIndex(ISerializer& s, uint32_t& globalOutputIndex, Common::StringView name);

}
