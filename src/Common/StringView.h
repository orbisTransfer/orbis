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

#include <cassert>
#include <string>

namespace Common {

// 'StringView' is a pair of pointer to constant char and size.
// It is recommended to pass 'StringView' to procedures by value.
// 'StringView' supports 'EMPTY' and 'NIL' representations as follows:
//   'data' == 'nullptr' && 'size' == 0 - EMPTY NIL
//   'data' != 'nullptr' && 'size' == 0 - EMPTY NOTNIL
//   'data' == 'nullptr' && 'size' > 0 - Undefined
//   'data' != 'nullptr' && 'size' > 0 - NOTEMPTY NOTNIL
class StringView {
public:
  typedef char Object;
  typedef size_t Size;

  const static Size INVALID;
  const static StringView EMPTY;
  const static StringView NIL;

  // Default constructor.
  // Leaves object uninitialized. Any usage before initializing it is undefined.
  StringView();

  // Direct constructor.
  // The behavior is undefined unless 'stringData' != 'nullptr' || 'stringSize' == 0
  StringView(const Object* stringData, Size stringSize);

  // Constructor from C array.
  // The behavior is undefined unless 'stringData' != 'nullptr' || 'stringSize' == 0. Input state can be malformed using poiner conversions.
  template<Size stringSize> StringView(const Object(&stringData)[stringSize]) : data(stringData), size(stringSize - 1) {
    assert(data != nullptr || size == 0);
  }

  // Constructor from std::string
  StringView(const std::string& string);

  // Copy constructor.
  // Performs default action - bitwise copying of source object.
  // The behavior is undefined unless 'other' 'StringView' is in defined state, that is 'data' != 'nullptr' || 'size' == 0
  StringView(const StringView& other);

  // Destructor.
  // No special action is performed.
  ~StringView();

  // Copy assignment operator.
  // The behavior is undefined unless 'other' 'StringView' is in defined state, that is 'data' != 'nullptr' || 'size' == 0
  StringView& operator=(const StringView& other);

  explicit operator std::string() const;

  const Object* getData() const;

  Size getSize() const;

  // Return false if 'StringView' is not EMPTY.
  // The behavior is undefined unless 'StringView' was initialized.
  bool isEmpty() const;

  // Return false if 'StringView' is not NIL.
  // The behavior is undefined unless 'StringView' was initialized.
  bool isNil() const;

  // Get 'StringView' element by index.
  // The behavior is undefined unless 'StringView' was initialized and 'index' < 'size'.
  const Object& operator[](Size index) const;

  // Get first element.
  // The behavior is undefined unless 'StringView' was initialized and 'size' > 0
  const Object& first() const;

  // Get last element.
  // The behavior is undefined unless 'StringView' was initialized and 'size' > 0
  const Object& last() const;

  // Return a pointer to the first element.
  // The behavior is undefined unless 'StringView' was initialized.
  const Object* begin() const;

  // Return a pointer after the last element.
  // The behavior is undefined unless 'StringView' was initialized.
  const Object* end() const;

  // Compare elements of two strings, return false if there is a difference.
  // EMPTY and NIL strings are considered equal.
  // The behavior is undefined unless both strings were initialized.
  bool operator==(StringView other) const;

  // Compare elements two strings, return false if there is no difference.
  // EMPTY and NIL strings are considered equal.
  // The behavior is undefined unless both strings were initialized.
  bool operator!=(StringView other) const;

  // Compare two strings character-wise.
  // The behavior is undefined unless both strings were initialized.
  bool operator<(StringView other) const;

  // Compare two strings character-wise.
  // The behavior is undefined unless both strings were initialized.
  bool operator<=(StringView other) const;

  // Compare two strings character-wise.
  // The behavior is undefined unless both strings were initialized.
  bool operator>(StringView other) const;

  // Compare two strings character-wise.
  // The behavior is undefined unless both strings were initialized.
  bool operator>=(StringView other) const;

  // Return false if 'StringView' does not contain 'object' at the beginning.
  // The behavior is undefined unless 'StringView' was initialized.
  bool beginsWith(const Object& object) const;

  // Return false if 'StringView' does not contain 'other' at the beginning.
  // The behavior is undefined unless both strings were initialized.
  bool beginsWith(StringView other) const;

  // Return false if 'StringView' does not contain 'object'.
  // The behavior is undefined unless 'StringView' was initialized.
  bool contains(const Object& object) const;

  // Return false if 'StringView' does not contain 'other'.
  // The behavior is undefined unless both strings were initialized.
  bool contains(StringView other) const;

  // Return false if 'StringView' does not contain 'object' at the end.
  // The behavior is undefined unless 'StringView' was initialized.
  bool endsWith(const Object& object) const;

  // Return false if 'StringView' does not contain 'other' at the end.
  // The behavior is undefined unless both strings were initialized.
  bool endsWith(StringView other) const;

  // Looks for the first occurence of 'object' in 'StringView',
  // returns index or INVALID if there are no occurences.
  // The behavior is undefined unless 'StringView' was initialized.
  Size find(const Object& object) const;

  // Looks for the first occurence of 'other' in 'StringView',
  // returns index or INVALID if there are no occurences.
  // The behavior is undefined unless both strings were initialized.
  Size find(StringView other) const;

  // Looks for the last occurence of 'object' in 'StringView',
  // returns index or INVALID if there are no occurences.
  // The behavior is undefined unless 'StringView' was initialized.
  Size findLast(const Object& object) const;

  // Looks for the first occurence of 'other' in 'StringView',
  // returns index or INVALID if there are no occurences.
  // The behavior is undefined unless both strings were initialized.
  Size findLast(StringView other) const;

  // Returns substring of 'headSize' first elements.
  // The behavior is undefined unless 'StringView' was initialized and 'headSize' <= 'size'.
  StringView head(Size headSize) const;

  // Returns substring of 'tailSize' last elements.
  // The behavior is undefined unless 'StringView' was initialized and 'tailSize' <= 'size'.
  StringView tail(Size tailSize) const;

  // Returns 'StringView' without 'headSize' first elements.
  // The behavior is undefined unless 'StringView' was initialized and 'headSize' <= 'size'.
  StringView unhead(Size headSize) const;

  // Returns 'StringView' without 'tailSize' last elements.
  // The behavior is undefined unless 'StringView' was initialized and 'tailSize' <= 'size'.
  StringView untail(Size tailSize) const;

  // Returns substring starting at 'startIndex' and contaning 'endIndex' - 'startIndex' elements.
  // The behavior is undefined unless 'StringView' was initialized and 'startIndex' <= 'endIndex' and 'endIndex' <= 'size'.
  StringView range(Size startIndex, Size endIndex) const;

  // Returns substring starting at 'startIndex' and contaning 'sliceSize' elements.
  // The behavior is undefined unless 'StringView' was initialized and 'startIndex' <= 'size' and 'startIndex' + 'sliceSize' <= 'size'.
  StringView slice(Size startIndex, Size sliceSize) const;

protected:
  const Object* data;
  Size size;
};

}
