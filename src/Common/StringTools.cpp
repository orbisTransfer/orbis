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
#include "StringTools.h"
#include <fstream>

namespace Common {

namespace {

const uint8_t characterValues[256] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

}

std::string asString(const void* data, size_t size) {
  return std::string(static_cast<const char*>(data), size);
}

std::string asString(const std::vector<uint8_t>& data) {
  return std::string(reinterpret_cast<const char*>(data.data()), data.size());
}

std::vector<uint8_t> asBinaryArray(const std::string& data) {
  auto dataPtr = reinterpret_cast<const uint8_t*>(data.data());
  return std::vector<uint8_t>(dataPtr, dataPtr + data.size());
}

uint8_t fromHex(char character) {
  uint8_t value = characterValues[static_cast<unsigned char>(character)];
  if (value > 0x0f) {
    throw std::runtime_error("fromHex: invalid character");
  }

  return value;
}

bool fromHex(char character, uint8_t& value) {
  if (characterValues[static_cast<unsigned char>(character)] > 0x0f) {
    return false;
  }

  value = characterValues[static_cast<unsigned char>(character)];
  return true;
}

size_t fromHex(const std::string& text, void* data, size_t bufferSize) {
  if ((text.size() & 1) != 0) {
    throw std::runtime_error("fromHex: invalid string size");
  }

  if (text.size() >> 1 > bufferSize) {
    throw std::runtime_error("fromHex: invalid buffer size");
  }

  for (size_t i = 0; i < text.size() >> 1; ++i) {
    static_cast<uint8_t*>(data)[i] = fromHex(text[i << 1]) << 4 | fromHex(text[(i << 1) + 1]);
  }

  return text.size() >> 1;
}

bool fromHex(const std::string& text, void* data, size_t bufferSize, size_t& size) {
  if ((text.size() & 1) != 0) {
    return false;
  }

  if (text.size() >> 1 > bufferSize) {
    return false;
  }

  for (size_t i = 0; i < text.size() >> 1; ++i) {
    uint8_t value1;
    if (!fromHex(text[i << 1], value1)) {
      return false;
    }

    uint8_t value2;
    if (!fromHex(text[(i << 1) + 1], value2)) {
      return false;
    }

    static_cast<uint8_t*>(data)[i] = value1 << 4 | value2;
  }

  size = text.size() >> 1;
  return true;
}

std::vector<uint8_t> fromHex(const std::string& text) {
  if ((text.size() & 1) != 0) {
    throw std::runtime_error("fromHex: invalid string size");
  }

  std::vector<uint8_t> data(text.size() >> 1);
  for (size_t i = 0; i < data.size(); ++i) {
    data[i] = fromHex(text[i << 1]) << 4 | fromHex(text[(i << 1) + 1]);
  }

  return data;
}

bool fromHex(const std::string& text, std::vector<uint8_t>& data) {
  if ((text.size() & 1) != 0) {
    return false;
  }

  for (size_t i = 0; i < text.size() >> 1; ++i) {
    uint8_t value1;
    if (!fromHex(text[i << 1], value1)) {
      return false;
    }

    uint8_t value2;
    if (!fromHex(text[(i << 1) + 1], value2)) {
      return false;
    }

    data.push_back(value1 << 4 | value2);
  }

  return true;
}

std::string toHex(const void* data, size_t size) {
  std::string text;
  for (size_t i = 0; i < size; ++i) {
    text += "0123456789abcdef"[static_cast<const uint8_t*>(data)[i] >> 4];
    text += "0123456789abcdef"[static_cast<const uint8_t*>(data)[i] & 15];
  }

  return text;
}

void toHex(const void* data, size_t size, std::string& text) {
  for (size_t i = 0; i < size; ++i) {
    text += "0123456789abcdef"[static_cast<const uint8_t*>(data)[i] >> 4];
    text += "0123456789abcdef"[static_cast<const uint8_t*>(data)[i] & 15];
  }
}

std::string toHex(const std::vector<uint8_t>& data) {
  std::string text;
  for (size_t i = 0; i < data.size(); ++i) {
    text += "0123456789abcdef"[data[i] >> 4];
    text += "0123456789abcdef"[data[i] & 15];
  }

  return text;
}

void toHex(const std::vector<uint8_t>& data, std::string& text) {
  for (size_t i = 0; i < data.size(); ++i) {
    text += "0123456789abcdef"[data[i] >> 4];
    text += "0123456789abcdef"[data[i] & 15];
  }
}

std::string extract(std::string& text, char delimiter) {
  size_t delimiterPosition = text.find(delimiter);
  std::string subText;
  if (delimiterPosition != std::string::npos) {
    subText = text.substr(0, delimiterPosition);
    text = text.substr(delimiterPosition + 1);
  } else {
    subText.swap(text);
  }

  return subText;
}

std::string extract(const std::string& text, char delimiter, size_t& offset) {
  size_t delimiterPosition = text.find(delimiter, offset);
  if (delimiterPosition != std::string::npos) {
    offset = delimiterPosition + 1;
    return text.substr(offset, delimiterPosition);
  } else {
    offset = text.size();
    return text.substr(offset);
  }
}

namespace {

static const std::string base64chars =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz"
  "0123456789+/";

bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

}

std::string base64Decode(std::string const& encoded_string) {
  size_t in_len = encoded_string.size();
  size_t i = 0;
  size_t j = 0;
  size_t in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i == 4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = (unsigned char)base64chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = (unsigned char)base64chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}


bool loadFileToString(const std::string& filepath, std::string& buf) {
  try {
    std::ifstream fstream;
    fstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fstream.open(filepath, std::ios_base::binary | std::ios_base::in | std::ios::ate);

    size_t fileSize = static_cast<size_t>(fstream.tellg());
    buf.resize(fileSize);

    if (fileSize > 0)  {
      fstream.seekg(0, std::ios::beg);
      fstream.read(&buf[0], buf.size());
    }
  } catch (const std::exception&) {
    return false;
  }

  return true;
}

bool saveStringToFile(const std::string& filepath, const std::string& buf) {
  try {
    std::ofstream fstream;
    fstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fstream.open(filepath, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
    fstream << buf;
  } catch (const std::exception&)  {
    return false;
  }

  return true;
}


std::string ipAddressToString(uint32_t ip) {
  uint8_t bytes[4];
  bytes[0] = ip & 0xFF;
  bytes[1] = (ip >> 8) & 0xFF;
  bytes[2] = (ip >> 16) & 0xFF;
  bytes[3] = (ip >> 24) & 0xFF;

  char buf[16];
  sprintf(buf, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);

  return std::string(buf);
}

bool parseIpAddressAndPort(uint32_t& ip, uint32_t& port, const std::string& addr) {
  uint32_t v[4];
  uint32_t localPort;

  if (sscanf(addr.c_str(), "%d.%d.%d.%d:%d", &v[0], &v[1], &v[2], &v[3], &localPort) != 5) {
    return false;
  }

  for (int i = 0; i < 4; ++i) {
    if (v[i] > 0xff) {
      return false;
    }
  }

  ip = (v[3] << 24) | (v[2] << 16) | (v[1] << 8) | v[0];
  port = localPort;
  return true;
}

std::string timeIntervalToString(uint64_t intervalInSeconds) {
  auto tail = intervalInSeconds;

  auto days = tail / (60 * 60 * 24);
  tail = tail % (60 * 60 * 24);
  auto hours = tail / (60 * 60);
  tail = tail % (60 * 60);
  auto  minutes = tail / (60);
  tail = tail % (60);
  auto seconds = tail;

  return 
    "d" + std::to_string(days) + 
    ".h" + std::to_string(hours) + 
    ".m" + std::to_string(minutes) +
    ".s" + std::to_string(seconds);
}


}
