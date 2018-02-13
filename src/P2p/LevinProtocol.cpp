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
#include "LevinProtocol.h"
#include <System/TcpConnection.h>

using namespace Orbis;

namespace {

const uint64_t LEVIN_SIGNATURE = 0x0101010101012101LL;  //Bender's nightmare
const uint32_t LEVIN_PACKET_REQUEST = 0x00000001;
const uint32_t LEVIN_PACKET_RESPONSE = 0x00000002;
const uint32_t LEVIN_DEFAULT_MAX_PACKET_SIZE = 100000000;      //100MB by default
const uint32_t LEVIN_PROTOCOL_VER_1 = 1;

#pragma pack(push)
#pragma pack(1)
struct bucket_head2
{
  uint64_t m_signature;
  uint64_t m_cb;
  bool     m_have_to_return_data;
  uint32_t m_command;
  int32_t  m_return_code;
  uint32_t m_flags;
  uint32_t m_protocol_version;
};
#pragma pack(pop)

}

bool LevinProtocol::Command::needReply() const {
  return !(isNotify || isResponse);
}

LevinProtocol::LevinProtocol(System::TcpConnection& connection) 
  : m_conn(connection) {}

void LevinProtocol::sendMessage(uint32_t command, const BinaryArray& out, bool needResponse) {
  bucket_head2 head = { 0 };
  head.m_signature = LEVIN_SIGNATURE;
  head.m_cb = out.size();
  head.m_have_to_return_data = needResponse;
  head.m_command = command;
  head.m_protocol_version = LEVIN_PROTOCOL_VER_1;
  head.m_flags = LEVIN_PACKET_REQUEST;

  // write header and body in one operation
  BinaryArray writeBuffer;
  writeBuffer.reserve(sizeof(head) + out.size());

  Common::VectorOutputStream stream(writeBuffer);
  stream.writeSome(&head, sizeof(head));
  stream.writeSome(out.data(), out.size());

  writeStrict(writeBuffer.data(), writeBuffer.size());
}

bool LevinProtocol::readCommand(Command& cmd) {
  bucket_head2 head = { 0 };

  if (!readStrict(reinterpret_cast<uint8_t*>(&head), sizeof(head))) {
    return false;
  }

  if (head.m_signature != LEVIN_SIGNATURE) {
    throw std::runtime_error("Levin signature mismatch");
  }

  if (head.m_cb > LEVIN_DEFAULT_MAX_PACKET_SIZE) {
    throw std::runtime_error("Levin packet size is too big");
  }

  BinaryArray buf;

  if (head.m_cb != 0) {
    buf.resize(head.m_cb);
    if (!readStrict(&buf[0], head.m_cb)) {
      return false;
    }
  }

  cmd.command = head.m_command;
  cmd.buf = std::move(buf);
  cmd.isNotify = !head.m_have_to_return_data;
  cmd.isResponse = (head.m_flags & LEVIN_PACKET_RESPONSE) == LEVIN_PACKET_RESPONSE;

  return true;
}

void LevinProtocol::sendReply(uint32_t command, const BinaryArray& out, int32_t returnCode) {
  bucket_head2 head = { 0 };
  head.m_signature = LEVIN_SIGNATURE;
  head.m_cb = out.size();
  head.m_have_to_return_data = false;
  head.m_command = command;
  head.m_protocol_version = LEVIN_PROTOCOL_VER_1;
  head.m_flags = LEVIN_PACKET_RESPONSE;
  head.m_return_code = returnCode;

  BinaryArray writeBuffer;
  writeBuffer.reserve(sizeof(head) + out.size());

  Common::VectorOutputStream stream(writeBuffer);
  stream.writeSome(&head, sizeof(head));
  stream.writeSome(out.data(), out.size());

  writeStrict(writeBuffer.data(), writeBuffer.size());
}

void LevinProtocol::writeStrict(const uint8_t* ptr, size_t size) {
  size_t offset = 0;
  while (offset < size) {
    offset += m_conn.write(ptr + offset, size - offset);
  }
}

bool LevinProtocol::readStrict(uint8_t* ptr, size_t size) {
  size_t offset = 0;
  while (offset < size) {
    size_t read = m_conn.read(ptr + offset, size - offset);
    if (read == 0) {
      return false;
    }

    offset += read;
  }

  return true;
}
