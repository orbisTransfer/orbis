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

namespace Orbis {

const uint32_t PORTABLE_STORAGE_SIGNATUREA = 0x01011101;
const uint32_t PORTABLE_STORAGE_SIGNATUREB = 0x01020101; // bender's nightmare 
const uint8_t PORTABLE_STORAGE_FORMAT_VER = 1;

const uint8_t PORTABLE_RAW_SIZE_MARK_MASK = 0x03;
const uint8_t PORTABLE_RAW_SIZE_MARK_BYTE = 0;
const uint8_t PORTABLE_RAW_SIZE_MARK_WORD = 1;
const uint8_t PORTABLE_RAW_SIZE_MARK_DWORD = 2;
const uint8_t PORTABLE_RAW_SIZE_MARK_INT64 = 3;

#ifndef MAX_STRING_LEN_POSSIBLE       
#define MAX_STRING_LEN_POSSIBLE       2000000000 //do not let string be so big
#endif

//data types 

const uint8_t BIN_KV_SERIALIZE_TYPE_INT64 = 1;
const uint8_t BIN_KV_SERIALIZE_TYPE_INT32 = 2;
const uint8_t BIN_KV_SERIALIZE_TYPE_INT16 = 3;
const uint8_t BIN_KV_SERIALIZE_TYPE_INT8 = 4;
const uint8_t BIN_KV_SERIALIZE_TYPE_UINT64 = 5;
const uint8_t BIN_KV_SERIALIZE_TYPE_UINT32 = 6;
const uint8_t BIN_KV_SERIALIZE_TYPE_UINT16 = 7;
const uint8_t BIN_KV_SERIALIZE_TYPE_UINT8 = 8;
const uint8_t BIN_KV_SERIALIZE_TYPE_DOUBLE = 9;
const uint8_t BIN_KV_SERIALIZE_TYPE_STRING = 10;
const uint8_t BIN_KV_SERIALIZE_TYPE_BOOL = 11;
const uint8_t BIN_KV_SERIALIZE_TYPE_OBJECT = 12;
const uint8_t BIN_KV_SERIALIZE_TYPE_ARRAY = 13;
const uint8_t BIN_KV_SERIALIZE_FLAG_ARRAY = 0x80;

#pragma pack(push)
#pragma pack(1)
struct KVBinaryStorageBlockHeader
{
  uint32_t m_signature_a;
  uint32_t m_signature_b;
  uint8_t  m_ver;
};
#pragma pack(pop)


}
