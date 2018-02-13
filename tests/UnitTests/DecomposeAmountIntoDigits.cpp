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

#include <cstdint>
#include <vector>

#include "OrbisCore/OrbisFormatUtils.h"

#define VEC_FROM_ARR(vec)                                               \
  std::vector<uint64_t> vec;                                            \
  for (size_t i = 0; i < sizeof(vec##_arr) / sizeof(vec##_arr[0]); ++i) \
  {                                                                     \
    vec.push_back(vec##_arr[i]);                                        \
  }

namespace
{
  struct chunk_handler_t
  {
    void operator()(uint64_t chunk) const
    {
      m_chunks.push_back(chunk);
    }

    mutable std::vector<uint64_t> m_chunks;
  };

  struct dust_handler_t
  {
    dust_handler_t()
      : m_dust(0)
      , m_has_dust(false)
    {
    }

    void operator()(uint64_t dust) const
    {
      m_dust = dust;
      m_has_dust = true;
    }

    mutable uint64_t m_dust;
    mutable bool m_has_dust;
  };

  class decompose_amount_into_digits_test : public ::testing::Test
  {
  protected:
    chunk_handler_t m_chunk_handler;
    dust_handler_t m_dust_handler;
  };
}

TEST_F(decompose_amount_into_digits_test, is_correct_0)
{
  std::vector<uint64_t> expected_chunks;
  Orbis::decompose_amount_into_digits(0, 0, m_chunk_handler, m_dust_handler);
  ASSERT_EQ(m_chunk_handler.m_chunks, expected_chunks);
  ASSERT_EQ(m_dust_handler.m_has_dust, false);
}

TEST_F(decompose_amount_into_digits_test, is_correct_1)
{
  std::vector<uint64_t> expected_chunks;
  Orbis::decompose_amount_into_digits(0, 10, m_chunk_handler, m_dust_handler);
  ASSERT_EQ(m_chunk_handler.m_chunks, expected_chunks);
  ASSERT_EQ(m_dust_handler.m_has_dust, false);
}

TEST_F(decompose_amount_into_digits_test, is_correct_2)
{
  uint64_t expected_chunks_arr[] = {10};
  VEC_FROM_ARR(expected_chunks);
  Orbis::decompose_amount_into_digits(10, 0, m_chunk_handler, m_dust_handler);
  ASSERT_EQ(m_chunk_handler.m_chunks, expected_chunks);
  ASSERT_EQ(m_dust_handler.m_has_dust, false);
}

TEST_F(decompose_amount_into_digits_test, is_correct_3)
{
  std::vector<uint64_t> expected_chunks;
  uint64_t expected_dust = 10;
  Orbis::decompose_amount_into_digits(10, 10, m_chunk_handler, m_dust_handler);
  ASSERT_EQ(m_chunk_handler.m_chunks, expected_chunks);
  ASSERT_EQ(m_dust_handler.m_dust, expected_dust);
}

TEST_F(decompose_amount_into_digits_test, is_correct_4)
{
  uint64_t expected_dust = 8100;
  std::vector<uint64_t> expected_chunks;
  Orbis::decompose_amount_into_digits(8100, 1000000, m_chunk_handler, m_dust_handler);
  ASSERT_EQ(m_chunk_handler.m_chunks, expected_chunks);
  ASSERT_EQ(m_dust_handler.m_dust, expected_dust);
}

TEST_F(decompose_amount_into_digits_test, is_correct_5)
{
  uint64_t expected_chunks_arr[] = {100, 900000, 8000000};
  VEC_FROM_ARR(expected_chunks);
  Orbis::decompose_amount_into_digits(8900100, 10, m_chunk_handler, m_dust_handler);
  ASSERT_EQ(m_chunk_handler.m_chunks, expected_chunks);
  ASSERT_EQ(m_dust_handler.m_has_dust, false);
}

TEST_F(decompose_amount_into_digits_test, is_correct_6)
{
  uint64_t expected_chunks_arr[] = {900000, 8000000};
  VEC_FROM_ARR(expected_chunks);
  uint64_t expected_dust = 100;
  Orbis::decompose_amount_into_digits(8900100, 1000, m_chunk_handler, m_dust_handler);
  ASSERT_EQ(m_chunk_handler.m_chunks, expected_chunks);
  ASSERT_EQ(m_dust_handler.m_dust, expected_dust);
}
