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

#include "StringView.h"
#include <limits>
#include <string.h>

namespace Common {

// 'StringBuffer' is a string of fixed maximum size.
template<size_t MAXIMUM_SIZE_VALUE> class StringBuffer {
public:
  typedef char Object;
  typedef size_t Size;

  const static Size MAXIMUM_SIZE = MAXIMUM_SIZE_VALUE;
  const static Size INVALID;

  static_assert(MAXIMUM_SIZE != 0, "StringBuffer's size must not be zero");

  // Default constructor.
  // After construction, 'StringBuffer' is empty, that is 'size' == 0
  StringBuffer() : size(0) {
  }

  // Direct constructor.
  // Copies string from 'stringData' to 'StringBuffer'.
  // The behavior is undefined unless ('stringData' != 'nullptr' || 'stringSize' == 0) && 'stringSize' <= 'MAXIMUM_SIZE'.
  StringBuffer(const Object* stringData, Size stringSize) : size(stringSize) {
    assert(stringData != nullptr || size == 0);
    assert(size <= MAXIMUM_SIZE);
    memcpy(data, stringData, size);
  }

  // Constructor from C array.
  // Copies string from 'stringData' to 'StringBuffer'.
  // The behavior is undefined unless ('stringData' != 'nullptr' || 'stringSize' == 0) && 'stringSize' <= 'MAXIMUM_SIZE'. Input state can be malformed using poiner conversions.
  template<Size stringSize> explicit StringBuffer(const Object(&stringData)[stringSize]) : size(stringSize - 1) {
    assert(stringData != nullptr || size == 0);
    assert(size <= MAXIMUM_SIZE);
    memcpy(data, stringData, size);
  }

  // Constructor from StringView
  // Copies string from 'stringView' to 'StringBuffer'.
  // The behavior is undefined unless 'stringView.size()' <= 'MAXIMUM_SIZE'.
  explicit StringBuffer(StringView stringView) : size(stringView.getSize()) {
    assert(size <= MAXIMUM_SIZE);
    memcpy(data, stringView.getData(), size);
  }

  // Copy constructor.
  // Copies string from 'other' to 'StringBuffer'.
  StringBuffer(const StringBuffer& other) : size(other.size) {
    memcpy(data, other.data, size);
  }

  // Destructor.
  // No special action is performed.
  ~StringBuffer() {
  }

  // Copy assignment operator.
  StringBuffer& operator=(const StringBuffer& other) {
    size = other.size;
    memcpy(data, other.data, size);
    return *this;
  }

  // StringView assignment operator.
  // Copies string from 'stringView' to 'StringBuffer'.
  // The behavior is undefined unless 'stringView.size()' <= 'MAXIMUM_SIZE'.
  StringBuffer& operator=(StringView stringView) {
    assert(stringView.getSize() <= MAXIMUM_SIZE);
    memcpy(data, stringView.getData(), stringView.getSize());
    size = stringView.getSize();
    return *this;
  }

  operator StringView() const {
    return StringView(data, size);
  }

  explicit operator std::string() const {
    return std::string(data, size);
  }

  Object* getData() {
    return data;
  }

  const Object* getData() const {
    return data;
  }

  Size getSize() const {
    return size;
  }

  // Return false if 'StringView' is not EMPTY.
  bool isEmpty() const {
    return size == 0;
  }

  // Get 'StringBuffer' element by index.
  // The behavior is undefined unless 'index' < 'size'.
  Object& operator[](Size index) {
    assert(index < size);
    return *(data + index);
  }

  // Get 'StringBuffer' element by index.
  // The behavior is undefined unless 'index' < 'size'.
  const Object& operator[](Size index) const {
    assert(index < size);
    return *(data + index);
  }

  // Get first element.
  // The behavior is undefined unless 'size' > 0
  Object& first() {
    assert(size > 0);
    return *data;
  }

  // Get first element.
  // The behavior is undefined unless 'size' > 0
  const Object& first() const {
    assert(size > 0);
    return *data;
  }

  // Get last element.
  // The behavior is undefined unless 'size' > 0
  Object& last() {
    assert(size > 0);
    return *(data + (size - 1));
  }

  // Get last element.
  // The behavior is undefined unless 'size' > 0
  const Object& last() const {
    assert(size > 0);
    return *(data + (size - 1));
  }

  // Return a pointer to the first element.
  Object* begin() {
    return data;
  }

  // Return a pointer to the first element.
  const Object* begin() const {
    return data;
  }

  // Return a pointer after the last element.
  Object* end() {
    return data + size;
  }

  // Return a pointer after the last element.
  const Object* end() const {
    return data + size;
  }

  // Compare elements of two strings, return false if there is a difference.
  bool operator==(StringView other) const {
    if (size == other.getSize()) {
      for (Size i = 0;; ++i) {
        if (i == size) {
          return true;
        }

        if (!(*(data + i) == *(other.getData() + i))) {
          break;
        }
      }
    }

    return false;
  }

  // Compare elements two strings, return false if there is no difference.
  bool operator!=(StringView other) const {
    return !(*this == other);
  }

  // Compare two strings character-wise.
  bool operator<(StringView other) const {
    Size count = other.getSize() < size ? other.getSize() : size;
    for (Size i = 0; i < count; ++i) {
      Object char1 = *(data + i);
      Object char2 = *(other.getData() + i);
      if (char1 < char2) {
        return true;
      }

      if (char2 < char1) {
        return false;
      }
    }

    return size < other.getSize();
  }

  // Compare two strings character-wise.
  bool operator<=(StringView other) const {
    return !(other < *this);
  }

  // Compare two strings character-wise.
  bool operator>(StringView other) const {
    return other < *this;
  }

  // Compare two strings character-wise.
  bool operator>=(StringView other) const {
    return !(*this < other);
  }

  // Return false if 'StringView' does not contain 'object' at the beginning.
  bool beginsWith(const Object& object) const {
    if (size == 0) {
      return false;
    }

    return *data == object;
  }

  // Return false if 'StringView' does not contain 'other' at the beginning.
  bool beginsWith(StringView other) const {
    if (size >= other.getSize()) {
      for (Size i = 0;; ++i) {
        if (i == other.getSize()) {
          return true;
        }

        if (!(*(data + i) == *(other.getData() + i))) {
          break;
        }
      }
    }

    return false;
  }

  // Return false if 'StringView' does not contain 'object'.
  bool contains(const Object& object) const {
    for (Size i = 0; i < size; ++i) {
      if (*(data + i) == object) {
        return true;
      }
    }

    return false;
  }

  // Return false if 'StringView' does not contain 'other'.
  bool contains(StringView other) const {
    if (size >= other.getSize()) {
      Size i = size - other.getSize();
      for (Size j = 0; !(i < j); ++j) {
        for (Size k = 0;; ++k) {
          if (k == other.getSize()) {
            return true;
          }

          if (!(*(data + j + k) == *(other.getData() + k))) {
            break;
          }
        }
      }
    }

    return false;
  }

  // Return false if 'StringView' does not contain 'object' at the end.
  bool endsWith(const Object& object) const {
    if (size == 0) {
      return false;
    }

    return *(data + (size - 1)) == object;
  }

  // Return false if 'StringView' does not contain 'other' at the end.
  bool endsWith(StringView other) const {
    if (size >= other.getSize()) {
      Size i = size - other.getSize();
      for (Size j = 0;; ++j) {
        if (j == other.getSize()) {
          return true;
        }

        if (!(*(data + i + j) == *(other.getData() + j))) {
          break;
        }
      }
    }

    return false;
  }

  // Looks for the first occurence of 'object' in 'StringView',
  // returns index or INVALID if there are no occurences.
  Size find(const Object& object) const {
    for (Size i = 0; i < size; ++i) {
      if (*(data + i) == object) {
        return i;
      }
    }

    return INVALID;
  }

  // Looks for the first occurence of 'other' in 'StringView',
  // returns index or INVALID if there are no occurences.
  Size find(StringView other) const {
    if (size >= other.getSize()) {
      Size i = size - other.getSize();
      for (Size j = 0; !(i < j); ++j) {
        for (Size k = 0;; ++k) {
          if (k == other.getSize()) {
            return j;
          }

          if (!(*(data + j + k) == *(other.getData() + k))) {
            break;
          }
        }
      }
    }

    return INVALID;
  }

  // Looks for the last occurence of 'object' in 'StringView',
  // returns index or INVALID if there are no occurences.
  Size findLast(const Object& object) const {
    for (Size i = 0; i < size; ++i) {
      if (*(data + (size - 1 - i)) == object) {
        return size - 1 - i;
      }
    }

    return INVALID;
  }

  // Looks for the first occurence of 'other' in 'StringView',
  // returns index or INVALID if there are no occurences.
  Size findLast(StringView other) const {
    if (size >= other.getSize()) {
      Size i = size - other.getSize();
      for (Size j = 0; !(i < j); ++j) {
        for (Size k = 0;; ++k) {
          if (k == other.getSize()) {
            return i - j;
          }

          if (!(*(data + (i - j + k)) == *(other.getData() + k))) {
            break;
          }
        }
      }
    }

    return INVALID;
  }

  // Returns substring of 'headSize' first elements.
  // The behavior is undefined unless 'headSize' <= 'size'.
  StringView head(Size headSize) const {
    assert(headSize <= size);
    return StringView(data, headSize);
  }

  // Returns substring of 'tailSize' last elements.
  // The behavior is undefined unless 'tailSize' <= 'size'.
  StringView tail(Size tailSize) const {
    assert(tailSize <= size);
    return StringView(data + (size - tailSize), tailSize);
  }

  // Returns 'StringView' without 'headSize' first elements.
  // The behavior is undefined unless 'headSize' <= 'size'.
  StringView unhead(Size headSize) const {
    assert(headSize <= size);
    return StringView(data + headSize, size - headSize);
  }

  // Returns 'StringView' without 'tailSize' last elements.
  // The behavior is undefined unless 'tailSize' <= 'size'.
  StringView untail(Size tailSize) const {
    assert(tailSize <= size);
    return StringView(data, size - tailSize);
  }

  // Returns substring starting at 'startIndex' and contaning 'endIndex' - 'startIndex' elements.
  // The behavior is undefined unless 'startIndex' <= 'endIndex' and 'endIndex' <= 'size'.
  StringView range(Size startIndex, Size endIndex) const {
    assert(startIndex <= endIndex && endIndex <= size);
    return StringView(data + startIndex, endIndex - startIndex);
  }

  // Returns substring starting at 'startIndex' and contaning 'sliceSize' elements.
  // The behavior is undefined unless 'startIndex' <= 'size' and 'sliceSize' <= 'size' - 'startIndex'.
  StringView slice(Size startIndex, Size sliceSize) const {
    assert(startIndex <= size && sliceSize <= size - startIndex);
    return StringView(data + startIndex, sliceSize);
  }

  // Appends 'object' to 'StringBuffer'.
  // The behavior is undefined unless 1 <= 'MAXIMUM_SIZE' - 'size'.
  StringBuffer& append(Object object) {
    assert(1 <= MAXIMUM_SIZE - size);
    data[size] = object;
    ++size;
    return *this;
  }

  // Appends 'stringView' to 'StringBuffer'.
  // The behavior is undefined unless 'stringView.size()' <= 'MAXIMUM_SIZE' - 'size'.
  StringBuffer& append(StringView stringView) {
    assert(stringView.getSize() <= MAXIMUM_SIZE - size);
    if (stringView.getSize() != 0) {
      memcpy(data + size, stringView.getData(), stringView.getSize());
      size += stringView.getSize();
    }

    return *this;
  }

  // Sets 'StringBuffer' to empty state, that is 'size' == 0
  StringBuffer& clear() {
    size = 0;
    return *this;
  }

  // Removes substring starting at 'startIndex' and contaning 'cutSize' elements.
  // The behavior is undefined unless 'startIndex' <= 'size' and 'cutSize' <= 'size' - 'startIndex'.
  StringBuffer& cut(Size startIndex, Size cutSize) {
    assert(startIndex <= size && cutSize <= size - startIndex);
    if (cutSize != 0) {
      memcpy(data + startIndex, data + startIndex + cutSize, size - startIndex - cutSize);
      size -= cutSize;
    }

    return *this;
  }

  // Copy 'object' to each element of 'StringBuffer'.
  StringBuffer& fill(Object object) {
    if (size > 0) {
      memset(data, object, size);
    }

    return *this;
  }

  // Inserts 'object' into 'StringBuffer' at 'index'.
  // The behavior is undefined unless 'index' <= 'size' and 1 <= 'MAXIMUM_SIZE' - 'size'.
  StringBuffer& insert(Size index, Object object) {
    assert(index <= size);
    assert(1 <= MAXIMUM_SIZE - size);
    memmove(data + index + 1, data + index, size - index);
    data[index] = object;
    ++size;
    return *this;
  }

  // Inserts 'stringView' into 'StringBuffer' at 'index'.
  // The behavior is undefined unless 'index' <= 'size' and 'stringView.size()' <= 'MAXIMUM_SIZE' - 'size'.
  StringBuffer& insert(Size index, StringView stringView) {
    assert(index <= size);
    assert(stringView.getSize() <= MAXIMUM_SIZE - size);
    if (stringView.getSize() != 0) {
      memmove(data + index + stringView.getSize(), data + index, size - index);
      memcpy(data + index, stringView.getData(), stringView.getSize());
      size += stringView.getSize();
    }

    return *this;
  }

  // Overwrites 'StringBuffer' starting at 'index' with 'stringView', possibly expanding 'StringBuffer'.
  // The behavior is undefined unless 'index' <= 'size' and 'stringView.size()' <= 'MAXIMUM_SIZE' - 'index'.
  StringBuffer& overwrite(Size index, StringView stringView) {
    assert(index <= size);
    assert(stringView.getSize() <= MAXIMUM_SIZE - index);
    memcpy(data + index, stringView.getData(), stringView.getSize());
    if (size < index + stringView.getSize()) {
      size = index + stringView.getSize();
    }

    return *this;
  }

  // Sets 'size' to 'bufferSize', assigning value '\0' to newly inserted elements.
  // The behavior is undefined unless 'bufferSize' <= 'MAXIMUM_SIZE'.
  StringBuffer& resize(Size bufferSize) {
    assert(bufferSize <= MAXIMUM_SIZE);
    if (bufferSize > size) {
      memset(data + size, 0, bufferSize - size);
    }

    size = bufferSize;
    return *this;
  }

  // Reverse 'StringBuffer' elements.
  StringBuffer& reverse() {
    for (Size i = 0; i < size / 2; ++i) {
      Object object = *(data + i);
      *(data + i) = *(data + (size - 1 - i));
      *(data + (size - 1 - i)) = object;
    }

    return *this;
  }

  // Sets 'size' to 'bufferSize'.
  // The behavior is undefined unless 'bufferSize' <= 'size'.
  StringBuffer& shrink(Size bufferSize) {
    assert(bufferSize <= size);
    size = bufferSize;
    return *this;
  }

protected:
  Object data[MAXIMUM_SIZE];
  Size size;
};

template<size_t MAXIMUM_SIZE> const typename StringBuffer<MAXIMUM_SIZE>::Size StringBuffer<MAXIMUM_SIZE>::INVALID = std::numeric_limits<typename StringBuffer<MAXIMUM_SIZE>::Size>::max();

}
