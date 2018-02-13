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
#include "gtest/gtest.h"

#include "Common/int-util.h"

namespace
{
  TEST(mul128, handles_zero)
  {
    uint64_t hi, lo;

    lo = mul128(0, 0, &hi);
    ASSERT_EQ(lo, 0);
    ASSERT_EQ(hi, 0);

    lo = mul128(7, 0, &hi);
    ASSERT_EQ(lo, 0);
    ASSERT_EQ(hi, 0);

    lo = mul128(0, 7, &hi);
    ASSERT_EQ(lo, 0);
    ASSERT_EQ(hi, 0);
  }

  TEST(mul128, handles_one)
  {
    uint64_t hi, lo;

    lo = mul128(1, 1, &hi);
    ASSERT_EQ(lo, 1);
    ASSERT_EQ(hi, 0);

    lo = mul128(7, 1, &hi);
    ASSERT_EQ(lo, 7);
    ASSERT_EQ(hi, 0);

    lo = mul128(1, 7, &hi);
    ASSERT_EQ(lo, 7);
    ASSERT_EQ(hi, 0);
  }

  TEST(mul128_without_carry, multiplies_correctly)
  {
    uint64_t hi, lo;

    lo = mul128(0x3333333333333333, 5, &hi);
    ASSERT_EQ(lo, 0xffffffffffffffff);
    ASSERT_EQ(hi, 0);

    lo = mul128(5, 0x3333333333333333, &hi);
    ASSERT_EQ(lo, 0xffffffffffffffff);
    ASSERT_EQ(hi, 0);

    lo = mul128(0x1111111111111111, 0x1111111111111111, &hi);
    ASSERT_EQ(lo, 0x0fedcba987654321);
    ASSERT_EQ(hi, 0x0123456789abcdf0);;
  }

  TEST(mul128_with_carry_1_only, multiplies_correctly)
  {
    uint64_t hi, lo;

    lo = mul128(0xe0000000e0000000, 0xe0000000e0000000, &hi);
    ASSERT_EQ(lo, 0xc400000000000000);
    ASSERT_EQ(hi, 0xc400000188000000);
  }

  TEST(mul128_with_carry_2_only, multiplies_correctly)
  {
    uint64_t hi, lo;

    lo = mul128(0x10000000ffffffff, 0x10000000ffffffff, &hi);
    ASSERT_EQ(lo, 0xdffffffe00000001);
    ASSERT_EQ(hi, 0x0100000020000000);
  }

  TEST(mul128_with_carry_1_and_2, multiplies_correctly)
  {
    uint64_t hi, lo;

    lo = mul128(0xf1f2f3f4f5f6f7f8, 0xf9f0fafbfcfdfeff, &hi);
    ASSERT_EQ(lo, 0x52118e5f3b211008);
    ASSERT_EQ(hi, 0xec39104363716e59);

    lo = mul128(0xffffffffffffffff, 0xffffffffffffffff, &hi);
    ASSERT_EQ(lo, 0x0000000000000001);
    ASSERT_EQ(hi, 0xfffffffffffffffe);
  }

  TEST(div128_32, handles_zero)
  {
    uint32_t reminder;
    uint64_t hi;
    uint64_t lo;

    reminder = div128_32(0, 0, 7, &hi, &lo);
    ASSERT_EQ(reminder, 0);
    ASSERT_EQ(hi, 0);
    ASSERT_EQ(lo, 0);

    // Division by zero is UB, so can be tested correctly
  }

  TEST(div128_32, handles_one)
  {
    uint32_t reminder;
    uint64_t hi;
    uint64_t lo;

    reminder = div128_32(0, 7, 1, &hi, &lo);
    ASSERT_EQ(reminder, 0);
    ASSERT_EQ(hi, 0);
    ASSERT_EQ(lo, 7);

    reminder = div128_32(7, 0, 1, &hi, &lo);
    ASSERT_EQ(reminder, 0);
    ASSERT_EQ(hi, 7);
    ASSERT_EQ(lo, 0);
  }

  TEST(div128_32, handles_if_dividend_less_divider)
  {
    uint32_t reminder;
    uint64_t hi;
    uint64_t lo;

    reminder = div128_32(0, 1383746, 1645825, &hi, &lo);
    ASSERT_EQ(reminder, 1383746);
    ASSERT_EQ(hi, 0);
    ASSERT_EQ(lo, 0);
  }

  TEST(div128_32, handles_if_dividend_dwords_less_divider)
  {
    uint32_t reminder;
    uint64_t hi;
    uint64_t lo;

    reminder = div128_32(0x5AD629E441074F28, 0x0DBCAB2B231081F1, 0xFE735CD6, &hi, &lo);
    ASSERT_EQ(reminder, 0xB9C924E9);
    ASSERT_EQ(hi, 0x000000005B63C274);
    ASSERT_EQ(lo, 0x9084FC024383E48C);
  }

  TEST(div128_32, works_correctly)
  {
    uint32_t reminder;
    uint64_t hi;
    uint64_t lo;

    reminder = div128_32(2, 0, 2, &hi, &lo);
    ASSERT_EQ(reminder, 0);
    ASSERT_EQ(hi, 1);
    ASSERT_EQ(lo, 0);

    reminder = div128_32(0xffffffffffffffff, 0, 0xffffffff, &hi, &lo);
    ASSERT_EQ(reminder, 0);
    ASSERT_EQ(hi, 0x0000000100000001);
    ASSERT_EQ(lo, 0);

    reminder = div128_32(0xffffffffffffffff, 5846, 0xffffffff, &hi, &lo);
    ASSERT_EQ(reminder, 5846);
    ASSERT_EQ(hi, 0x0000000100000001);
    ASSERT_EQ(lo, 0);

    reminder = div128_32(0xffffffffffffffff - 1, 0, 0xffffffff, &hi, &lo);
    ASSERT_EQ(reminder, 0xfffffffe);
    ASSERT_EQ(hi, 0x0000000100000000);
    ASSERT_EQ(lo, 0xfffffffefffffffe);

    reminder = div128_32(0x2649372534875028, 0xaedbfedc5adbc739, 0x27826534, &hi, &lo);
    ASSERT_EQ(reminder, 0x1a6dc2e5);
    ASSERT_EQ(hi, 0x00000000f812c1f8);
    ASSERT_EQ(lo, 0xddf2fdb09bc2e2e9);
  }
}
