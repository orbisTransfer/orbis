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
#include "StringView.h"
#include <limits>

namespace Common {

const StringView::Size StringView::INVALID = std::numeric_limits<StringView::Size>::max();
const StringView StringView::EMPTY(reinterpret_cast<Object*>(1), 0);
const StringView StringView::NIL(nullptr, 0);

StringView::StringView()
#ifndef NDEBUG
  : data(nullptr), size(INVALID) // In debug mode, fill in object with invalid state (undefined).
#endif
{
}

StringView::StringView(const Object* stringData, Size stringSize) : data(stringData), size(stringSize) {
  assert(data != nullptr || size == 0);
}

StringView::StringView(const std::string& string) : data(string.data()), size(string.size()) {
}

StringView::StringView(const StringView& other) : data(other.data), size(other.size) {
  assert(data != nullptr || size == 0);
}

StringView::~StringView() {
}

StringView& StringView::operator=(const StringView& other) {
  assert(other.data != nullptr || other.size == 0);
  data = other.data;
  size = other.size;
  return *this;
}

StringView::operator std::string() const {
  return std::string(data, size);
}

const StringView::Object* StringView::getData() const {
  assert(data != nullptr || size == 0);
  return data;
}

StringView::Size StringView::getSize() const {
  assert(data != nullptr || size == 0);
  return size;
}

bool StringView::isEmpty() const {
  assert(data != nullptr || size == 0);
  return size == 0;
}

bool StringView::isNil() const {
  assert(data != nullptr || size == 0);
  return data == nullptr;
}

const StringView::Object& StringView::operator[](Size index) const {
  assert(data != nullptr || size == 0);
  assert(index < size);
  return *(data + index);
}

const StringView::Object& StringView::first() const {
  assert(data != nullptr || size == 0);
  assert(size > 0);
  return *data;
}

const StringView::Object& StringView::last() const {
  assert(data != nullptr || size == 0);
  assert(size > 0);
  return *(data + (size - 1));
}

const StringView::Object* StringView::begin() const {
  assert(data != nullptr || size == 0);
  return data;
}

const StringView::Object* StringView::end() const {
  assert(data != nullptr || size == 0);
  return data + size;
}

bool StringView::operator==(StringView other) const {
  assert(data != nullptr || size == 0);
  assert(other.data != nullptr || other.size == 0);
  if (size == other.size) {
    for (Size i = 0;; ++i) {
      if (i == size) {
        return true;
      }

      if (!(*(data + i) == *(other.data + i))) {
        break;
      }
    }
  }

  return false;
}

bool StringView::operator!=(StringView other) const {
  return !(*this == other);
}

bool StringView::operator<(StringView other) const {
  assert(data != nullptr || size == 0);
  assert(other.data != nullptr || other.size == 0);
  Size count = other.size < size ? other.size : size;
  for (Size i = 0; i < count; ++i) {
    Object char1 = *(data + i);
    Object char2 = *(other.data + i);
    if (char1 < char2) {
      return true;
    }

    if (char2 < char1) {
      return false;
    }
  }

  return size < other.size;
}

bool StringView::operator<=(StringView other) const {
  return !(other < *this);
}

bool StringView::operator>(StringView other) const {
  return other < *this;
}

bool StringView::operator>=(StringView other) const {
  return !(*this < other);
}

bool StringView::beginsWith(const Object& object) const {
  assert(data != nullptr || size == 0);
  if (size == 0) {
    return false;
  }

  return *data == object;
}

bool StringView::beginsWith(StringView other) const {
  assert(data != nullptr || size == 0);
  assert(other.data != nullptr || other.size == 0);
  if (size >= other.size) {
    for (Size i = 0;; ++i) {
      if (i == other.size) {
        return true;
      }

      if (!(*(data + i) == *(other.data + i))) {
        break;
      }
    }
  }

  return false;
}

bool StringView::contains(const Object& object) const {
  assert(data != nullptr || size == 0);
  for (Size i = 0; i < size; ++i) {
    if (*(data + i) == object) {
      return true;
    }
  }

  return false;
}

bool StringView::contains(StringView other) const {
  assert(data != nullptr || size == 0);
  assert(other.data != nullptr || other.size == 0);
  if (size >= other.size) {
    Size i = size - other.size;
    for (Size j = 0; !(i < j); ++j) {
      for (Size k = 0;; ++k) {
        if (k == other.size) {
          return true;
        }

        if (!(*(data + j + k) == *(other.data + k))) {
          break;
        }
      }
    }
  }

  return false;
}

bool StringView::endsWith(const Object& object) const {
  assert(data != nullptr || size == 0);
  if (size == 0) {
    return false;
  }

  return *(data + (size - 1)) == object;
}

bool StringView::endsWith(StringView other) const {
  assert(data != nullptr || size == 0);
  assert(other.data != nullptr || other.size == 0);
  if (size >= other.size) {
    Size i = size - other.size;
    for (Size j = 0;; ++j) {
      if (j == other.size) {
        return true;
      }

      if (!(*(data + i + j) == *(other.data + j))) {
        break;
      }
    }
  }

  return false;
}

StringView::Size StringView::find(const Object& object) const {
  assert(data != nullptr || size == 0);
  for (Size i = 0; i < size; ++i) {
    if (*(data + i) == object) {
      return i;
    }
  }

  return INVALID;
}

StringView::Size StringView::find(StringView other) const {
  assert(data != nullptr || size == 0);
  assert(other.data != nullptr || other.size == 0);
  if (size >= other.size) {
    Size i = size - other.size;
    for (Size j = 0; !(i < j); ++j) {
      for (Size k = 0;; ++k) {
        if (k == other.size) {
          return j;
        }

        if (!(*(data + j + k) == *(other.data + k))) {
          break;
        }
      }
    }
  }

  return INVALID;
}

StringView::Size StringView::findLast(const Object& object) const {
  assert(data != nullptr || size == 0);
  for (Size i = 0; i < size; ++i) {
    if (*(data + (size - 1 - i)) == object) {
      return size - 1 - i;
    }
  }

  return INVALID;
}

StringView::Size StringView::findLast(StringView other) const {
  assert(data != nullptr || size == 0);
  assert(other.data != nullptr || other.size == 0);
  if (size >= other.size) {
    Size i = size - other.size;
    for (Size j = 0; !(i < j); ++j) {
      for (Size k = 0;; ++k) {
        if (k == other.size) {
          return i - j;
        }

        if (!(*(data + (i - j + k)) == *(other.data + k))) {
          break;
        }
      }
    }
  }

  return INVALID;
}

StringView StringView::head(Size headSize) const {
  assert(data != nullptr || size == 0);
  assert(headSize <= size);
  return StringView(data, headSize);
}

StringView StringView::tail(Size tailSize) const {
  assert(data != nullptr || size == 0);
  assert(tailSize <= size);
  return StringView(data + (size - tailSize), tailSize);
}

StringView StringView::unhead(Size headSize) const {
  assert(data != nullptr || size == 0);
  assert(headSize <= size);
  return StringView(data + headSize, size - headSize);
}

StringView StringView::untail(Size tailSize) const {
  assert(data != nullptr || size == 0);
  assert(tailSize <= size);
  return StringView(data, size - tailSize);
}

StringView StringView::range(Size startIndex, Size endIndex) const {
  assert(data != nullptr || size == 0);
  assert(startIndex <= endIndex && endIndex <= size);
  return StringView(data + startIndex, endIndex - startIndex);
}

StringView StringView::slice(Size startIndex, Size sliceSize) const {
  assert(data != nullptr || size == 0);
  assert(startIndex <= size && startIndex + sliceSize <= size);
  return StringView(data + startIndex, sliceSize);
}

}
