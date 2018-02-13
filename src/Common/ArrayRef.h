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

#include "ArrayView.h"
#include <limits>

namespace Common {

// 'ArrayRef' is a pair of pointer to object of parametrized type and size.
// It is recommended to pass 'ArrayRef' to procedures by value.
// 'ArrayRef' supports 'EMPTY' and 'NIL' representations as follows:
//   'data' == 'nullptr' && 'size' == 0 - EMPTY NIL
//   'data' != 'nullptr' && 'size' == 0 - EMPTY NOTNIL
//   'data' == 'nullptr' && 'size' > 0 - Undefined
//   'data' != 'nullptr' && 'size' > 0 - NOTEMPTY NOTNIL
// For signed integer 'Size', 'ArrayRef' with 'size' < 0 is undefined.
template<class ObjectType = uint8_t, class SizeType = size_t> class ArrayRef {
public:
  typedef ObjectType Object;
  typedef SizeType Size;

  const static Size INVALID;
  const static ArrayRef EMPTY;
  const static ArrayRef NIL;

  // Default constructor.
  // Leaves object uninitialized. Any usage before initializing it is undefined.
  ArrayRef()
#ifndef NDEBUG
    : data(nullptr), size(INVALID) // In debug mode, fill in object with invalid state (undefined).
#endif
  {
  }

  // Direct constructor.
  // The behavior is undefined unless 'arrayData' != 'nullptr' || 'arraySize' == 0
  ArrayRef(Object* arrayData, Size arraySize) : data(arrayData), size(arraySize) {
    assert(data != nullptr || size == 0);
  }

  // Constructor from C array.
  // The behavior is undefined unless 'arrayData' != 'nullptr' || 'arraySize' == 0. Input state can be malformed using poiner conversions.
  template<Size arraySize> ArrayRef(Object(&arrayData)[arraySize]) : data(arrayData), size(arraySize) {
    assert(data != nullptr || size == 0);
  }

  // Copy constructor.
  // Performs default action - bitwise copying of source object.
  // The behavior is undefined unless 'other' 'ArrayRef' is in defined state, that is 'data' != 'nullptr' || 'size' == 0
  ArrayRef(const ArrayRef& other) : data(other.data), size(other.size) {
    assert(data != nullptr || size == 0);
  }

  // Destructor.
  // No special action is performed.
  ~ArrayRef() {
  }

  // Copy assignment operator.
  // The behavior is undefined unless 'other' 'ArrayRef' is in defined state, that is 'data' != 'nullptr' || 'size' == 0
  ArrayRef& operator=(const ArrayRef& other) {
    assert(other.data != nullptr || other.size == 0);
    data = other.data;
    size = other.size;
    return *this;
  }

  operator ArrayView<Object, Size>() const {
    return ArrayView<Object, Size>(data, size);
  }

  Object* getData() const {
    assert(data != nullptr || size == 0);
    return data;
  }

  Size getSize() const {
    assert(data != nullptr || size == 0);
    return size;
  }

  // Return false if 'ArrayRef' is not EMPTY.
  // The behavior is undefined unless 'ArrayRef' was initialized.
  bool isEmpty() const {
    assert(data != nullptr || size == 0);
    return size == 0;
  }

  // Return false if 'ArrayRef' is not NIL.
  // The behavior is undefined unless 'ArrayRef' was initialized.
  bool isNil() const {
    assert(data != nullptr || size == 0);
    return data == nullptr;
  }

  // Get 'ArrayRef' element by index.
  // The behavior is undefined unless 'ArrayRef' was initialized and 'index' < 'size'.
  Object& operator[](Size index) const {
    assert(data != nullptr || size == 0);
    assert(index < size);
    return *(data + index);
  }

  // Get first element.
  // The behavior is undefined unless 'ArrayRef' was initialized and 'size' > 0
  Object& first() const {
    assert(data != nullptr || size == 0);
    assert(size > 0);
    return *data;
  }

  // Get last element.
  // The behavior is undefined unless 'ArrayRef' was initialized and 'size' > 0
  Object& last() const {
    assert(data != nullptr || size == 0);
    assert(size > 0);
    return *(data + (size - 1));
  }

  // Return a pointer to the first element.
  // The behavior is undefined unless 'ArrayRef' was initialized.
  Object* begin() const {
    assert(data != nullptr || size == 0);
    return data;
  }

  // Return a pointer after the last element.
  // The behavior is undefined unless 'ArrayRef' was initialized.
  Object* end() const {
    assert(data != nullptr || size == 0);
    return data + size;
  }

  // Compare elements of two arrays, return false if there is a difference.
  // EMPTY and NIL arrays are considered equal.
  // The behavior is undefined unless both arrays were initialized.
  bool operator==(ArrayView<Object, Size> other) const {
    assert(data != nullptr || size == 0);
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

  // Compare elements two arrays, return false if there is no difference.
  // EMPTY and NIL arrays are considered equal.
  // The behavior is undefined unless both arrays were initialized.
  bool operator!=(ArrayView<Object, Size> other) const {
    assert(data != nullptr || size == 0);
    if (size == other.getSize()) {
      for (Size i = 0;; ++i) {
        if (i == size) {
          return false;
        }

        if (*(data + i) != *(other.getData() + i)) {
          break;
        }
      }
    }

    return true;
  }

  // Return false if 'ArrayRef' does not contain 'object' at the beginning.
  // The behavior is undefined unless 'ArrayRef' was initialized.
  bool beginsWith(const Object& object) const {
    assert(data != nullptr || size == 0);
    if (size == 0) {
      return false;
    }

    return *data == object;
  }

  // Return false if 'ArrayRef' does not contain 'other' at the beginning.
  // The behavior is undefined unless both arrays were initialized.
  bool beginsWith(ArrayView<Object, Size> other) const {
    assert(data != nullptr || size == 0);
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

  // Return false if 'ArrayRef' does not contain 'object'.
  // The behavior is undefined unless 'ArrayRef' was initialized.
  bool contains(const Object& object) const {
    assert(data != nullptr || size == 0);
    for (Size i = 0; i < size; ++i) {
      if (*(data + i) == object) {
        return true;
      }
    }

    return false;
  }

  // Return false if 'ArrayRef' does not contain 'other'.
  // The behavior is undefined unless both arrays were initialized.
  bool contains(ArrayView<Object, Size> other) const {
    assert(data != nullptr || size == 0);
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

  // Return false if 'ArrayRef' does not contain 'object' at the end.
  // The behavior is undefined unless 'ArrayRef' was initialized.
  bool endsWith(const Object& object) const {
    assert(data != nullptr || size == 0);
    if (size == 0) {
      return false;
    }

    return *(data + (size - 1)) == object;
  }

  // Return false if 'ArrayRef' does not contain 'other' at the end.
  // The behavior is undefined unless both arrays were initialized.
  bool endsWith(ArrayView<Object, Size> other) const {
    assert(data != nullptr || size == 0);
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

  // Looks for the first occurence of 'object' in 'ArrayRef',
  // returns index or INVALID if there are no occurences.
  // The behavior is undefined unless 'ArrayRef' was initialized.
  Size find(const Object& object) const {
    assert(data != nullptr || size == 0);
    for (Size i = 0; i < size; ++i) {
      if (*(data + i) == object) {
        return i;
      }
    }

    return INVALID;
  }

  // Looks for the first occurence of 'other' in 'ArrayRef',
  // returns index or INVALID if there are no occurences.
  // The behavior is undefined unless both arrays were initialized.
  Size find(ArrayView<Object, Size> other) const {
    assert(data != nullptr || size == 0);
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

  // Looks for the last occurence of 'object' in 'ArrayRef',
  // returns index or INVALID if there are no occurences.
  // The behavior is undefined unless 'ArrayRef' was initialized.
  Size findLast(const Object& object) const {
    assert(data != nullptr || size == 0);
    for (Size i = 0; i < size; ++i) {
      if (*(data + (size - 1 - i)) == object) {
        return size - 1 - i;
      }
    }

    return INVALID;
  }

  // Looks for the first occurence of 'other' in 'ArrayRef',
  // returns index or INVALID if there are no occurences.
  // The behavior is undefined unless both arrays were initialized.
  Size findLast(ArrayView<Object, Size> other) const {
    assert(data != nullptr || size == 0);
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

  // Returns subarray of 'headSize' first elements.
  // The behavior is undefined unless 'ArrayRef' was initialized and 'headSize' <= 'size'.
  ArrayRef head(Size headSize) const {
    assert(data != nullptr || size == 0);
    assert(headSize <= size);
    return ArrayRef(data, headSize);
  }

  // Returns subarray of 'tailSize' last elements.
  // The behavior is undefined unless 'ArrayRef' was initialized and 'tailSize' <= 'size'.
  ArrayRef tail(Size tailSize) const {
    assert(data != nullptr || size == 0);
    assert(tailSize <= size);
    return ArrayRef(data + (size - tailSize), tailSize);
  }

  // Returns 'ArrayRef' without 'headSize' first elements.
  // The behavior is undefined unless 'ArrayRef' was initialized and 'headSize' <= 'size'.
  ArrayRef unhead(Size headSize) const {
    assert(data != nullptr || size == 0);
    assert(headSize <= size);
    return ArrayRef(data + headSize, size - headSize);
  }

  // Returns 'ArrayRef' without 'tailSize' last elements.
  // The behavior is undefined unless 'ArrayRef' was initialized and 'tailSize' <= 'size'.
  ArrayRef untail(Size tailSize) const {
    assert(data != nullptr || size == 0);
    assert(tailSize <= size);
    return ArrayRef(data, size - tailSize);
  }

  // Returns subarray starting at 'startIndex' and contaning 'endIndex' - 'startIndex' elements.
  // The behavior is undefined unless 'ArrayRef' was initialized and 'startIndex' <= 'endIndex' and 'endIndex' <= 'size'.
  ArrayRef range(Size startIndex, Size endIndex) const {
    assert(data != nullptr || size == 0);
    assert(startIndex <= endIndex && endIndex <= size);
    return ArrayRef(data + startIndex, endIndex - startIndex);
  }

  // Returns subarray starting at 'startIndex' and contaning 'sliceSize' elements.
  // The behavior is undefined unless 'ArrayRef' was initialized and 'startIndex' <= 'size' and 'startIndex' + 'sliceSize' <= 'size'.
  ArrayRef slice(Size startIndex, Size sliceSize) const {
    assert(data != nullptr || size == 0);
    assert(startIndex <= size && startIndex + sliceSize <= size);
    return ArrayRef(data + startIndex, sliceSize);
  }

  // Copy 'object' to each element of 'ArrayRef'.
  // The behavior is undefined unless 'ArrayRef' was initialized.
  const ArrayRef& fill(const Object& object) const {
    assert(data != nullptr || size == 0);
    for (Size i = 0; i < size; ++i) {
      *(data + i) = object;
    }

    return *this;
  }

  // Reverse 'ArrayRef' elements.
  // The behavior is undefined unless 'ArrayRef' was initialized.
  const ArrayRef& reverse() const {
    assert(data != nullptr || size == 0);
    for (Size i = 0; i < size / 2; ++i) {
      Object object = *(data + i);
      *(data + i) = *(data + (size - 1 - i));
      *(data + (size - 1 - i)) = object;
    }

    return *this;
  }

protected:
  Object* data;
  Size size;
};

template<class Object, class Size> const Size ArrayRef<Object, Size>::INVALID = std::numeric_limits<Size>::max();
template<class Object, class Size> const ArrayRef<Object, Size> ArrayRef<Object, Size>::EMPTY(reinterpret_cast<Object*>(1), 0);
template<class Object, class Size> const ArrayRef<Object, Size> ArrayRef<Object, Size>::NIL(nullptr, 0);

}
