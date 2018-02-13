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
#include "KVBinaryOutputStreamSerializer.h"
#include "KVBinaryCommon.h"

#include <cassert>
#include <stdexcept>
#include <Common/StreamTools.h>

using namespace Common;
using namespace Orbis;

namespace {

template <typename T>
void writePod(IOutputStream& s, const T& value) {
  write(s, &value, sizeof(T));
}

template<class T>
size_t packVarint(IOutputStream& s, uint8_t type_or, size_t pv) {
  T v = static_cast<T>(pv << 2);
  v |= type_or;
  write(s, &v, sizeof(T));
  return sizeof(T);
}

void writeElementName(IOutputStream& s, Common::StringView name) {
  if (name.getSize() > std::numeric_limits<uint8_t>::max()) {
    throw std::runtime_error("Element name is too long");
  }

  uint8_t len = static_cast<uint8_t>(name.getSize());
  write(s, &len, sizeof(len));
  write(s, name.getData(), len);
}

size_t writeArraySize(IOutputStream& s, size_t val) {
  if (val <= 63) {
    return packVarint<uint8_t>(s, PORTABLE_RAW_SIZE_MARK_BYTE, val);
  } else if (val <= 16383) {
    return packVarint<uint16_t>(s, PORTABLE_RAW_SIZE_MARK_WORD, val);
  } else if (val <= 1073741823) {
    return packVarint<uint32_t>(s, PORTABLE_RAW_SIZE_MARK_DWORD, val);
  } else {
    if (val > 4611686018427387903) {
      throw std::runtime_error("failed to pack varint - too big amount");
    }
    return packVarint<uint64_t>(s, PORTABLE_RAW_SIZE_MARK_INT64, val);
  }
}

}

namespace Orbis {

KVBinaryOutputStreamSerializer::KVBinaryOutputStreamSerializer() {
  beginObject(std::string());
}

void KVBinaryOutputStreamSerializer::dump(IOutputStream& target) {
  assert(m_objectsStack.size() == 1);
  assert(m_stack.size() == 1);

  KVBinaryStorageBlockHeader hdr;
  hdr.m_signature_a = PORTABLE_STORAGE_SIGNATUREA;
  hdr.m_signature_b = PORTABLE_STORAGE_SIGNATUREB;
  hdr.m_ver = PORTABLE_STORAGE_FORMAT_VER;

  Common::write(target, &hdr, sizeof(hdr));
  writeArraySize(target, m_stack.front().count);
  write(target, stream().data(), stream().size());
}

ISerializer::SerializerType KVBinaryOutputStreamSerializer::type() const {
  return ISerializer::OUTPUT;
}

bool KVBinaryOutputStreamSerializer::beginObject(Common::StringView name) {
  checkArrayPreamble(BIN_KV_SERIALIZE_TYPE_OBJECT);
 
  m_stack.push_back(Level(name));
  m_objectsStack.push_back(MemoryStream());

  return true;
}

void KVBinaryOutputStreamSerializer::endObject() {
  assert(m_objectsStack.size());

  auto level = std::move(m_stack.back());
  m_stack.pop_back();

  auto objStream = std::move(m_objectsStack.back());
  m_objectsStack.pop_back();

  auto& out = stream();

  writeElementPrefix(BIN_KV_SERIALIZE_TYPE_OBJECT, level.name);

  writeArraySize(out, level.count);
  write(out, objStream.data(), objStream.size());
}

bool KVBinaryOutputStreamSerializer::beginArray(size_t& size, Common::StringView name) {
  m_stack.push_back(Level(name, size));
  return true;
}

void KVBinaryOutputStreamSerializer::endArray() {
  bool validArray = m_stack.back().state == State::Array;
  m_stack.pop_back();

  if (m_stack.back().state == State::Object && validArray) {
    ++m_stack.back().count;
  }
}

bool KVBinaryOutputStreamSerializer::operator()(uint8_t& value, Common::StringView name) {
  writeElementPrefix(BIN_KV_SERIALIZE_TYPE_UINT8, name);
  writePod(stream(), value);
  return true;
}

bool KVBinaryOutputStreamSerializer::operator()(uint16_t& value, Common::StringView name) {
  writeElementPrefix(BIN_KV_SERIALIZE_TYPE_UINT16, name);
  writePod(stream(), value);
  return true;
}

bool KVBinaryOutputStreamSerializer::operator()(int16_t& value, Common::StringView name) {
  writeElementPrefix(BIN_KV_SERIALIZE_TYPE_INT16, name);
  writePod(stream(), value);
  return true;
}

bool KVBinaryOutputStreamSerializer::operator()(uint32_t& value, Common::StringView name) {
  writeElementPrefix(BIN_KV_SERIALIZE_TYPE_UINT32, name);
  writePod(stream(), value);
  return true;
}

bool KVBinaryOutputStreamSerializer::operator()(int32_t& value, Common::StringView name) {
  writeElementPrefix(BIN_KV_SERIALIZE_TYPE_INT32, name);
  writePod(stream(), value);
  return true;
}

bool KVBinaryOutputStreamSerializer::operator()(int64_t& value, Common::StringView name) {
  writeElementPrefix(BIN_KV_SERIALIZE_TYPE_INT64, name);
  writePod(stream(), value);
  return true;
}

bool KVBinaryOutputStreamSerializer::operator()(uint64_t& value, Common::StringView name) {
  writeElementPrefix(BIN_KV_SERIALIZE_TYPE_UINT64, name);
  writePod(stream(), value);
  return true;
}

bool KVBinaryOutputStreamSerializer::operator()(bool& value, Common::StringView name) {
  writeElementPrefix(BIN_KV_SERIALIZE_TYPE_BOOL, name);
  writePod(stream(), value);
  return true;
}

bool KVBinaryOutputStreamSerializer::operator()(double& value, Common::StringView name) {
  writeElementPrefix(BIN_KV_SERIALIZE_TYPE_DOUBLE, name);
  writePod(stream(), value);
  return true;
}

bool KVBinaryOutputStreamSerializer::operator()(std::string& value, Common::StringView name) {
  writeElementPrefix(BIN_KV_SERIALIZE_TYPE_STRING, name);

  auto& out = stream();
  writeArraySize(out, value.size());
  write(out, value.data(), value.size());
  return true;
}

bool KVBinaryOutputStreamSerializer::binary(void* value, size_t size, Common::StringView name) {
  if (size > 0) {
    writeElementPrefix(BIN_KV_SERIALIZE_TYPE_STRING, name);
    auto& out = stream();
    writeArraySize(out, size);
    write(out, value, size);
  }
  return true;
}

bool KVBinaryOutputStreamSerializer::binary(std::string& value, Common::StringView name) {
  return binary(const_cast<char*>(value.data()), value.size(), name);
}

void KVBinaryOutputStreamSerializer::writeElementPrefix(uint8_t type, Common::StringView name) {  
  assert(m_stack.size());

  checkArrayPreamble(type);
  Level& level = m_stack.back();
  
  if (level.state != State::Array) {
    if (!name.isEmpty()) {
      auto& s = stream();
      writeElementName(s, name);
      write(s, &type, 1);
    }
    ++level.count;
  }
}

void KVBinaryOutputStreamSerializer::checkArrayPreamble(uint8_t type) {
  if (m_stack.empty()) {
    return;
  }

  Level& level = m_stack.back();

  if (level.state == State::ArrayPrefix) {
    auto& s = stream();
    writeElementName(s, level.name);
    char c = BIN_KV_SERIALIZE_FLAG_ARRAY | type;
    write(s, &c, 1);
    writeArraySize(s, level.count);
    level.state = State::Array;
  }
}


MemoryStream& KVBinaryOutputStreamSerializer::stream() {
  assert(m_objectsStack.size());
  return m_objectsStack.back();
}

}
