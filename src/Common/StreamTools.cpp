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
#include "StreamTools.h"
#include <stdexcept>
#include "IInputStream.h"
#include "IOutputStream.h"

namespace Common {

void read(IInputStream& in, void* data, size_t size) {
  while (size > 0) {
    size_t readSize = in.readSome(data, size);
    if (readSize == 0) {
      throw std::runtime_error("Failed to read from IInputStream");
    }

    data = static_cast<uint8_t*>(data) + readSize;
    size -= readSize;
  }
}

void read(IInputStream& in, int8_t& value) {
  read(in, &value, sizeof(value));
}

void read(IInputStream& in, int16_t& value) {
  // TODO: Convert from little endian on big endian platforms
  read(in, &value, sizeof(value));
}

void read(IInputStream& in, int32_t& value) {
  // TODO: Convert from little endian on big endian platforms
  read(in, &value, sizeof(value));
}

void read(IInputStream& in, int64_t& value) {
  // TODO: Convert from little endian on big endian platforms
  read(in, &value, sizeof(value));
}

void read(IInputStream& in, uint8_t& value) {
  read(in, &value, sizeof(value));
}

void read(IInputStream& in, uint16_t& value) {
  // TODO: Convert from little endian on big endian platforms
  read(in, &value, sizeof(value));
}

void read(IInputStream& in, uint32_t& value) {
  // TODO: Convert from little endian on big endian platforms
  read(in, &value, sizeof(value));
}

void read(IInputStream& in, uint64_t& value) {
  // TODO: Convert from little endian on big endian platforms
  read(in, &value, sizeof(value));
}

void read(IInputStream& in, std::vector<uint8_t>& data, size_t size) {
  data.resize(size);
  read(in, data.data(), size);
}

void read(IInputStream& in, std::string& data, size_t size) {
  std::vector<char> temp(size);
  read(in, temp.data(), size);
  data.assign(temp.data(), size);
}

void readVarint(IInputStream& in, uint8_t& value) {
  uint8_t temp = 0;
  for (uint8_t shift = 0;; shift += 7) {
    uint8_t piece;
    read(in, piece);
    if (shift >= sizeof(temp) * 8 - 7 && piece >= 1 << (sizeof(temp) * 8 - shift)) {
      throw std::runtime_error("readVarint, value overflow");
    }

    temp |= static_cast<size_t>(piece & 0x7f) << shift;
    if ((piece & 0x80) == 0) {
      if (piece == 0 && shift != 0) {
        throw std::runtime_error("readVarint, invalid value representation");
      }

      break;
    }
  }

  value = temp;
}

void readVarint(IInputStream& in, uint16_t& value) {
  uint16_t temp = 0;
  for (uint8_t shift = 0;; shift += 7) {
    uint8_t piece;
    read(in, piece);
    if (shift >= sizeof(temp) * 8 - 7 && piece >= 1 << (sizeof(temp) * 8 - shift)) {
      throw std::runtime_error("readVarint, value overflow");
    }

    temp |= static_cast<size_t>(piece & 0x7f) << shift;
    if ((piece & 0x80) == 0) {
      if (piece == 0 && shift != 0) {
        throw std::runtime_error("readVarint, invalid value representation");
      }

      break;
    }
  }

  value = temp;
}

void readVarint(IInputStream& in, uint32_t& value) {
  uint32_t temp = 0;
  for (uint8_t shift = 0;; shift += 7) {
    uint8_t piece;
    read(in, piece);
    if (shift >= sizeof(temp) * 8 - 7 && piece >= 1 << (sizeof(temp) * 8 - shift)) {
      throw std::runtime_error("readVarint, value overflow");
    }

    temp |= static_cast<size_t>(piece & 0x7f) << shift;
    if ((piece & 0x80) == 0) {
      if (piece == 0 && shift != 0) {
        throw std::runtime_error("readVarint, invalid value representation");
      }

      break;
    }
  }

  value = temp;
}

void readVarint(IInputStream& in, uint64_t& value) {
  uint64_t temp = 0;
  for (uint8_t shift = 0;; shift += 7) {
    uint8_t piece;
    read(in, piece);
    if (shift >= sizeof(temp) * 8 - 7 && piece >= 1 << (sizeof(temp) * 8 - shift)) {
      throw std::runtime_error("readVarint, value overflow");
    }

    temp |= static_cast<uint64_t>(piece & 0x7f) << shift;
    if ((piece & 0x80) == 0) {
      if (piece == 0 && shift != 0) {
        throw std::runtime_error("readVarint, invalid value representation");
      }

      break;
    }
  }

  value = temp;
}

void write(IOutputStream& out, const void* data, size_t size) {
  while (size > 0) {
    size_t writtenSize = out.writeSome(data, size);
    if (writtenSize == 0) {
      throw std::runtime_error("Failed to write to IOutputStream");
    }

    data = static_cast<const uint8_t*>(data) + writtenSize;
    size -= writtenSize;
  }
}

void write(IOutputStream& out, int8_t value) {
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, int16_t value) {
  // TODO: Convert to little endian on big endian platforms
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, int32_t value) {
  // TODO: Convert to little endian on big endian platforms
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, int64_t value) {
  // TODO: Convert to little endian on big endian platforms
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, uint8_t value) {
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, uint16_t value) {
  // TODO: Convert to little endian on big endian platforms
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, uint32_t value) {
  // TODO: Convert to little endian on big endian platforms
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, uint64_t value) {
  // TODO: Convert to little endian on big endian platforms
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, const std::vector<uint8_t>& data) {
  write(out, data.data(), data.size());
}

void write(IOutputStream& out, const std::string& data) {
  write(out, data.data(), data.size());
}

void writeVarint(IOutputStream& out, uint32_t value) {
  while (value >= 0x80) {
    write(out, static_cast<uint8_t>(value | 0x80));
    value >>= 7;
  }

  write(out, static_cast<uint8_t>(value));
}

void writeVarint(IOutputStream& out, uint64_t value) {
  while (value >= 0x80) {
    write(out, static_cast<uint8_t>(value | 0x80));
    value >>= 7;
  }

  write(out, static_cast<uint8_t>(value));
}

}
