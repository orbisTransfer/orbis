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
#include <cstddef>
#include <ios>
#include <iostream>
#include <type_traits>
#include <vector>

inline bool hexdecode(const char *from, std::size_t length, void *to) {
  std::size_t i;
  for (i = 0; i < length; i++) {
    int v = 0;
    if (from[2 * i] >= '0' && from[2 * i] <= '9') {
      v = from[2 * i] - '0';
    } else if (from[2 * i] >= 'a' && from[2 * i] <= 'f') {
      v = from[2 * i] - 'a' + 10;
    } else {
      return false;
    }
    v <<= 4;
    if (from[2 * i + 1] >= '0' && from[2 * i + 1] <= '9') {
      v |= from[2 * i + 1] - '0';
    } else if (from[2 * i + 1] >= 'a' && from[2 * i + 1] <= 'f') {
      v |= from[2 * i + 1] - 'a' + 10;
    } else {
      return false;
    }
    *(reinterpret_cast<unsigned char *>(to) + i) = v;
  }
  return true;
}

inline void get(std::istream &input, bool &res) {
  std::string sres;
  input >> sres;
  if (sres == "false") {
    res = false;
  } else if (sres == "true") {
    res = true;
  } else {
    input.setstate(std::ios_base::failbit);
  }
}

template<typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
get(std::istream &input, T &res) {
  input >> res;
}

inline void getvar(std::istream &input, std::size_t length, void *res) {
  std::string sres;
  input >> sres;
  if (sres.length() != 2 * length || !hexdecode(sres.data(), length, res)) {
    input.setstate(std::ios_base::failbit);
  }
}

template<typename T>
typename std::enable_if<std::is_standard_layout<T>::value && !std::is_scalar<T>::value, void>::type
get(std::istream &input, T &res) {
  getvar(input, sizeof(T), &res);
}

inline void get(std::istream &input, std::vector<char> &res) {
  std::string sres;
  input >> sres;
  if (sres == "x") {
    res.clear();
  } else if (sres.length() % 2 != 0) {
    input.setstate(std::ios_base::failbit);
  } else {
    std::size_t length = sres.length() / 2;
    res.resize(length);
    if (!hexdecode(sres.data(), length, res.data())) {
      input.setstate(std::ios_base::failbit);
    }
  }
}

#if !defined(_MSC_VER) || _MSC_VER >= 1800

template<typename T, typename... TT>
typename std::enable_if<(sizeof...(TT) > 0), void>::type
get(std::istream &input, T &res, TT &... resres) {
  get(input, res);
  get(input, resres...);
}

#else
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>

#define NESTED_GET(z, n, data) get(input, BOOST_PP_CAT(res, n));
#define GET(z, n, data) \
template<BOOST_PP_ENUM_PARAMS(n, typename T)> \
void get(std::istream &input, BOOST_PP_ENUM_BINARY_PARAMS(n, T, &res)) { \
  BOOST_PP_REPEAT(n, NESTED_GET, ~) \
}
BOOST_PP_REPEAT_FROM_TO(2, 5, GET, ~)

#endif
