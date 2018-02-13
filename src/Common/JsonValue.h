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

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace Common {

class JsonValue {
public:
  typedef std::string Key;

  typedef std::vector<JsonValue> Array;
  typedef bool Bool;
  typedef int64_t Integer;
  typedef std::nullptr_t Nil;
  typedef std::map<Key, JsonValue> Object;
  typedef double Real;
  typedef std::string String;

  enum Type {
    ARRAY,
    BOOL,
    INTEGER,
    NIL,
    OBJECT,
    REAL,
    STRING
  };

  JsonValue();
  JsonValue(const JsonValue& other);
  JsonValue(JsonValue&& other);
  JsonValue(Type valueType);
  JsonValue(const Array& value);
  JsonValue(Array&& value);
  explicit JsonValue(Bool value);
  JsonValue(Integer value);
  JsonValue(Nil value);
  JsonValue(const Object& value);
  JsonValue(Object&& value);
  JsonValue(Real value);
  JsonValue(const String& value);
  JsonValue(String&& value);
  template<size_t size> JsonValue(const char(&value)[size]) {
    new(valueString)String(value, size - 1);
    type = STRING;
  }

  ~JsonValue();

  JsonValue& operator=(const JsonValue& other);
  JsonValue& operator=(JsonValue&& other);
  JsonValue& operator=(const Array& value);
  JsonValue& operator=(Array&& value);
  //JsonValue& operator=(Bool value);
  JsonValue& operator=(Integer value);
  JsonValue& operator=(Nil value);
  JsonValue& operator=(const Object& value);
  JsonValue& operator=(Object&& value);
  JsonValue& operator=(Real value);
  JsonValue& operator=(const String& value);
  JsonValue& operator=(String&& value);
  template<size_t size> JsonValue& operator=(const char(&value)[size]) {
    if (type != STRING) {
      destructValue();
      type = NIL;
      new(valueString)String(value, size - 1);
      type = STRING;
    } else {
      reinterpret_cast<String*>(valueString)->assign(value, size - 1);
    }

    return *this;
  }

  bool isArray() const;
  bool isBool() const;
  bool isInteger() const;
  bool isNil() const;
  bool isObject() const;
  bool isReal() const;
  bool isString() const;

  Type getType() const;
  Array& getArray();
  const Array& getArray() const;
  Bool getBool() const;
  Integer getInteger() const;
  Object& getObject();
  const Object& getObject() const;
  Real getReal() const;
  String& getString();
  const String& getString() const;

  size_t size() const;

  JsonValue& operator[](size_t index);
  const JsonValue& operator[](size_t index) const;
  JsonValue& pushBack(const JsonValue& value);
  JsonValue& pushBack(JsonValue&& value);

  JsonValue& operator()(const Key& key);
  const JsonValue& operator()(const Key& key) const;
  bool contains(const Key& key) const;
  JsonValue& insert(const Key& key, const JsonValue& value);
  JsonValue& insert(const Key& key, JsonValue&& value);

  // sets or creates value, returns reference to self
  JsonValue& set(const Key& key, const JsonValue& value);
  JsonValue& set(const Key& key, JsonValue&& value);

  size_t erase(const Key& key);

  static JsonValue fromString(const std::string& source);
  std::string toString() const;

  friend std::ostream& operator<<(std::ostream& out, const JsonValue& jsonValue);
  friend std::istream& operator>>(std::istream& in, JsonValue& jsonValue);

private:
  Type type;
  union {
    uint8_t valueArray[sizeof(Array)];
    Bool valueBool;
    Integer valueInteger;
    uint8_t valueObject[sizeof(Object)];
    Real valueReal;
    uint8_t valueString[sizeof(std::string)];
  };

  void destructValue();

  void readArray(std::istream& in);
  void readTrue(std::istream& in);
  void readFalse(std::istream& in);
  void readNull(std::istream& in);
  void readNumber(std::istream& in, char c);
  void readObject(std::istream& in);
  void readString(std::istream& in);
};

}
