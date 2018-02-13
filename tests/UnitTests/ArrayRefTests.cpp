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
#include <Common/ArrayRef.h>
#include <gtest/gtest.h>

using namespace Common;

TEST(ArrayRefTests, representations) {
  ASSERT_NE(nullptr, ArrayRef<>::EMPTY.getData());
  ASSERT_EQ(0, ArrayRef<>::EMPTY.getSize());
  ASSERT_EQ(nullptr, ArrayRef<>::NIL.getData());
  ASSERT_EQ(0, ArrayRef<>::NIL.getSize());
}

TEST(ArrayRefTests, directConstructor) {
  uint8_t data[4] = {2, 3, 5, 7};
  ASSERT_EQ(data, ArrayRef<>(data, 4).getData());
  ASSERT_EQ(4, ArrayRef<>(data, 4).getSize());
}

TEST(ArrayRefTests, arrayConstructor) {
  uint8_t data[4] = {2, 3, 5, 7};
  ASSERT_EQ(data, ArrayRef<>(data).getData());
  ASSERT_EQ(4, ArrayRef<>(data).getSize());
}

TEST(ArrayRefTests, copyConstructor) {
  uint8_t data[4] = {2, 3, 5, 7};
  const ArrayRef<> ref(data);
  ASSERT_EQ(ref.getData(), ArrayRef<>(ref).getData());
  ASSERT_EQ(ref.getSize(), ArrayRef<>(ref).getSize());
}

TEST(ArrayRefTests, copyAssignment) {
  uint8_t data[4] = {2, 3, 5, 7};
  const ArrayRef<> ref1(data);
  ArrayRef<> ref2;
  ref2 = ref1;
  ASSERT_EQ(ref1.getData(), ref2.getData());
  ASSERT_EQ(ref1.getSize(), ref2.getSize());
}

TEST(ArrayRefTests, arrayView) {
  uint8_t data[4] = {2, 3, 5, 7};
  const ArrayRef<> ref(data);
  ArrayView<> view = ref;
  ASSERT_EQ(ref.getData(), view.getData());
  ASSERT_EQ(ref.getSize(), view.getSize());
}

TEST(ArrayRefTests, emptyNil) {
  ASSERT_TRUE(ArrayRef<>::EMPTY.isEmpty());
  ASSERT_FALSE(ArrayRef<>::EMPTY.isNil());
  ASSERT_TRUE(ArrayRef<>::NIL.isEmpty());
  ASSERT_TRUE(ArrayRef<>::NIL.isNil());
  uint8_t data[4] = {2, 3, 5, 7};
  ASSERT_TRUE(ArrayRef<>(data, 0).isEmpty());
  ASSERT_FALSE(ArrayRef<>(data, 0).isNil());
  ASSERT_FALSE(ArrayRef<>(data).isEmpty());
  ASSERT_FALSE(ArrayRef<>(data).isNil());
}

TEST(ArrayRefTests, squareBrackets) {
  uint8_t data[4] = {2, 3, 5, 7};
  const ArrayRef<> ref(data);
  ASSERT_EQ(data + 0, &ref[0]);
  ASSERT_EQ(data + 1, &ref[1]);
  ASSERT_EQ(data + 2, &ref[2]);
  ASSERT_EQ(data + 3, &ref[3]);
}

TEST(ArrayRefTests, firstLast) {
  uint8_t data[4] = {2, 3, 5, 7};
  const ArrayRef<> ref(data);
  ASSERT_EQ(data + 0, &ref.first());
  ASSERT_EQ(data + 3, &ref.last());
}

TEST(ArrayRefTests, beginEnd) {
  uint8_t data[4] = {2, 3, 5, 7};
  ASSERT_EQ(nullptr, ArrayRef<>::NIL.begin());
  ASSERT_EQ(nullptr, ArrayRef<>::NIL.end());
  ASSERT_EQ(data, ArrayRef<>(data).begin());
  ASSERT_EQ(data + 4, ArrayRef<>(data).end());
  size_t offset = 0;
  for (uint8_t& value : ArrayRef<>(data)) {
    ASSERT_EQ(data[offset], value);
    ++offset;
  }
}

TEST(ArrayRefTests, comparisons) {
  uint8_t data1[3] = {2, 3, 5};
  uint8_t data2[4] = {2, 3, 5, 7};
  uint8_t data3[4] = {2, 3, 5, 7};
  uint8_t data4[5] = {2, 3, 5, 7, 11};
  uint8_t data5[4] = {13, 17, 19, 23};
  ASSERT_TRUE(ArrayRef<>::EMPTY == ArrayView<>::EMPTY);
  ASSERT_TRUE(ArrayRef<>::EMPTY == ArrayView<>::NIL);
  ASSERT_FALSE(ArrayRef<>::EMPTY == ArrayView<>(data1));
  ASSERT_TRUE(ArrayRef<>::NIL == ArrayView<>::EMPTY);
  ASSERT_TRUE(ArrayRef<>::NIL == ArrayView<>::NIL);
  ASSERT_FALSE(ArrayRef<>::NIL == ArrayView<>(data1));
  ASSERT_FALSE(ArrayRef<>(data2) == ArrayView<>::EMPTY);
  ASSERT_FALSE(ArrayRef<>(data2) == ArrayView<>::NIL);
  ASSERT_FALSE(ArrayRef<>(data2) == ArrayView<>(data1));
  ASSERT_TRUE(ArrayRef<>(data2) == ArrayView<>(data2));
  ASSERT_TRUE(ArrayRef<>(data2) == ArrayView<>(data3));
  ASSERT_FALSE(ArrayRef<>(data2) == ArrayView<>(data4));
  ASSERT_FALSE(ArrayRef<>(data2) == ArrayView<>(data5));
  ASSERT_FALSE(ArrayRef<>::EMPTY != ArrayView<>::EMPTY);
  ASSERT_FALSE(ArrayRef<>::EMPTY != ArrayView<>::NIL);
  ASSERT_TRUE(ArrayRef<>::EMPTY != ArrayView<>(data1));
  ASSERT_FALSE(ArrayRef<>::NIL != ArrayView<>::EMPTY);
  ASSERT_FALSE(ArrayRef<>::NIL != ArrayView<>::NIL);
  ASSERT_TRUE(ArrayRef<>::NIL != ArrayView<>(data1));
  ASSERT_TRUE(ArrayRef<>(data2) != ArrayView<>::EMPTY);
  ASSERT_TRUE(ArrayRef<>(data2) != ArrayView<>::NIL);
  ASSERT_TRUE(ArrayRef<>(data2) != ArrayView<>(data1));
  ASSERT_FALSE(ArrayRef<>(data2) != ArrayView<>(data2));
  ASSERT_FALSE(ArrayRef<>(data2) != ArrayView<>(data3));
  ASSERT_TRUE(ArrayRef<>(data2) != ArrayView<>(data4));
  ASSERT_TRUE(ArrayRef<>(data2) != ArrayView<>(data5));
}

TEST(ArrayRefTests, beginsWith) {
  uint8_t data1[3] = {2, 3, 5};
  uint8_t data2[4] = {2, 3, 5, 7};
  uint8_t data3[4] = {2, 3, 5, 7};
  uint8_t data4[5] = {2, 3, 5, 7, 11};
  uint8_t data5[4] = {13, 17, 19, 23};
  ASSERT_FALSE(ArrayRef<>::EMPTY.beginsWith(data1[0]));
  ASSERT_TRUE(ArrayRef<>::EMPTY.beginsWith(ArrayView<>::EMPTY));
  ASSERT_TRUE(ArrayRef<>::EMPTY.beginsWith(ArrayView<>::NIL));
  ASSERT_FALSE(ArrayRef<>::EMPTY.beginsWith(ArrayView<>(data1)));
  ASSERT_FALSE(ArrayRef<>::NIL.beginsWith(data1[0]));
  ASSERT_TRUE(ArrayRef<>::NIL.beginsWith(ArrayView<>::EMPTY));
  ASSERT_TRUE(ArrayRef<>::NIL.beginsWith(ArrayView<>::NIL));
  ASSERT_FALSE(ArrayRef<>::NIL.beginsWith(ArrayView<>(data1)));
  ASSERT_TRUE(ArrayRef<>(data2).beginsWith(data1[0]));
  ASSERT_FALSE(ArrayRef<>(data2).beginsWith(data5[0]));
  ASSERT_TRUE(ArrayRef<>(data2).beginsWith(ArrayView<>::EMPTY));
  ASSERT_TRUE(ArrayRef<>(data2).beginsWith(ArrayView<>::NIL));
  ASSERT_TRUE(ArrayRef<>(data2).beginsWith(ArrayView<>(data1)));
  ASSERT_TRUE(ArrayRef<>(data2).beginsWith(ArrayView<>(data2)));
  ASSERT_TRUE(ArrayRef<>(data2).beginsWith(ArrayView<>(data3)));
  ASSERT_FALSE(ArrayRef<>(data2).beginsWith(ArrayView<>(data4)));
  ASSERT_FALSE(ArrayRef<>(data2).beginsWith(ArrayView<>(data5)));
}

TEST(ArrayRefTests, contains) {
  uint8_t data1[2] = {3, 5};
  uint8_t data2[4] = {2, 3, 5, 7};
  uint8_t data3[4] = {2, 3, 5, 7};
  uint8_t data4[5] = {2, 3, 5, 7, 11};
  uint8_t data5[4] = {13, 17, 19, 23};
  ASSERT_FALSE(ArrayRef<>::EMPTY.contains(data1[1]));
  ASSERT_TRUE(ArrayRef<>::EMPTY.contains(ArrayView<>::EMPTY));
  ASSERT_TRUE(ArrayRef<>::EMPTY.contains(ArrayView<>::NIL));
  ASSERT_FALSE(ArrayRef<>::EMPTY.contains(ArrayView<>(data1)));
  ASSERT_FALSE(ArrayRef<>::NIL.contains(data1[1]));
  ASSERT_TRUE(ArrayRef<>::NIL.contains(ArrayView<>::EMPTY));
  ASSERT_TRUE(ArrayRef<>::NIL.contains(ArrayView<>::NIL));
  ASSERT_FALSE(ArrayRef<>::NIL.contains(ArrayView<>(data1)));
  ASSERT_TRUE(ArrayRef<>(data2).contains(data1[1]));
  ASSERT_FALSE(ArrayRef<>(data2).contains(data5[1]));
  ASSERT_TRUE(ArrayRef<>(data2).contains(ArrayView<>::EMPTY));
  ASSERT_TRUE(ArrayRef<>(data2).contains(ArrayView<>::NIL));
  ASSERT_TRUE(ArrayRef<>(data2).contains(ArrayView<>(data1)));
  ASSERT_TRUE(ArrayRef<>(data2).contains(ArrayView<>(data2)));
  ASSERT_TRUE(ArrayRef<>(data2).contains(ArrayView<>(data3)));
  ASSERT_FALSE(ArrayRef<>(data2).contains(ArrayView<>(data4)));
  ASSERT_FALSE(ArrayRef<>(data2).contains(ArrayView<>(data5)));
}

TEST(ArrayRefTests, endsWith) {
  uint8_t data1[3] = {3, 5, 7};
  uint8_t data2[4] = {2, 3, 5, 7};
  uint8_t data3[4] = {2, 3, 5, 7};
  uint8_t data4[5] = {2, 3, 5, 7, 11};
  uint8_t data5[4] = {13, 17, 19, 23};
  ASSERT_FALSE(ArrayRef<>::EMPTY.endsWith(data1[2]));
  ASSERT_TRUE(ArrayRef<>::EMPTY.endsWith(ArrayView<>::EMPTY));
  ASSERT_TRUE(ArrayRef<>::EMPTY.endsWith(ArrayView<>::NIL));
  ASSERT_FALSE(ArrayRef<>::EMPTY.endsWith(ArrayView<>(data1)));
  ASSERT_FALSE(ArrayRef<>::NIL.endsWith(data1[2]));
  ASSERT_TRUE(ArrayRef<>::NIL.endsWith(ArrayView<>::EMPTY));
  ASSERT_TRUE(ArrayRef<>::NIL.endsWith(ArrayView<>::NIL));
  ASSERT_FALSE(ArrayRef<>::NIL.endsWith(ArrayView<>(data1)));
  ASSERT_TRUE(ArrayRef<>(data2).endsWith(data1[2]));
  ASSERT_FALSE(ArrayRef<>(data2).endsWith(data5[3]));
  ASSERT_TRUE(ArrayRef<>(data2).endsWith(ArrayView<>::EMPTY));
  ASSERT_TRUE(ArrayRef<>(data2).endsWith(ArrayView<>::NIL));
  ASSERT_TRUE(ArrayRef<>(data2).endsWith(ArrayView<>(data1)));
  ASSERT_TRUE(ArrayRef<>(data2).endsWith(ArrayView<>(data2)));
  ASSERT_TRUE(ArrayRef<>(data2).endsWith(ArrayView<>(data3)));
  ASSERT_FALSE(ArrayRef<>(data2).endsWith(ArrayView<>(data4)));
  ASSERT_FALSE(ArrayRef<>(data2).endsWith(ArrayView<>(data5)));
}

TEST(ArrayRefTests, find) {
  uint8_t data1[2] = {3, 5};
  uint8_t data2[6] = {2, 3, 5, 3, 5, 7};
  uint8_t data3[6] = {2, 3, 5, 3, 5, 7};
  uint8_t data4[7] = {2, 3, 5, 3, 5, 7, 11};
  uint8_t data5[4] = {13, 17, 19, 23};
  ASSERT_EQ(ArrayRef<>::INVALID, ArrayRef<>::EMPTY.find(data1[0]));
  ASSERT_EQ(0, ArrayRef<>::EMPTY.find(ArrayView<>::EMPTY));
  ASSERT_EQ(0, ArrayRef<>::EMPTY.find(ArrayView<>::NIL));
  ASSERT_EQ(ArrayRef<>::INVALID, ArrayRef<>::EMPTY.find(ArrayView<>(data1)));
  ASSERT_EQ(ArrayRef<>::INVALID, ArrayRef<>::NIL.find(data1[0]));
  ASSERT_EQ(0, ArrayRef<>::NIL.find(ArrayView<>::EMPTY));
  ASSERT_EQ(0, ArrayRef<>::NIL.find(ArrayView<>::NIL));
  ASSERT_EQ(ArrayRef<>::INVALID, ArrayRef<>::NIL.find(ArrayView<>(data1)));
  ASSERT_EQ(1, ArrayRef<>(data2).find(data1[0]));
  ASSERT_EQ(ArrayRef<>::INVALID, ArrayRef<>(data2).find(data5[1]));
  ASSERT_EQ(0, ArrayRef<>(data2).find(ArrayView<>::EMPTY));
  ASSERT_EQ(0, ArrayRef<>(data2).find(ArrayView<>::NIL));
  ASSERT_EQ(1, ArrayRef<>(data2).find(ArrayView<>(data1)));
  ASSERT_EQ(0, ArrayRef<>(data2).find(ArrayView<>(data2)));
  ASSERT_EQ(0, ArrayRef<>(data2).find(ArrayView<>(data3)));
  ASSERT_EQ(ArrayRef<>::INVALID, ArrayRef<>(data2).find(ArrayView<>(data4)));
  ASSERT_EQ(ArrayRef<>::INVALID, ArrayRef<>(data2).find(ArrayView<>(data5)));
}

TEST(ArrayRefTests, findLast) {
  uint8_t data1[2] = {3, 5};
  uint8_t data2[6] = {2, 3, 5, 3, 5, 7};
  uint8_t data3[6] = {2, 3, 5, 3, 5, 7};
  uint8_t data4[7] = {2, 3, 5, 3, 5, 7, 11};
  uint8_t data5[4] = {13, 17, 19, 23};
  ASSERT_EQ(ArrayRef<>::INVALID, ArrayRef<>::EMPTY.findLast(data1[0]));
  ASSERT_EQ(0, ArrayRef<>::EMPTY.findLast(ArrayView<>::EMPTY));
  ASSERT_EQ(0, ArrayRef<>::EMPTY.findLast(ArrayView<>::NIL));
  ASSERT_EQ(ArrayRef<>::INVALID, ArrayRef<>::EMPTY.findLast(ArrayView<>(data1)));
  ASSERT_EQ(ArrayRef<>::INVALID, ArrayRef<>::NIL.findLast(data1[0]));
  ASSERT_EQ(0, ArrayRef<>::NIL.findLast(ArrayView<>::EMPTY));
  ASSERT_EQ(0, ArrayRef<>::NIL.findLast(ArrayView<>::NIL));
  ASSERT_EQ(ArrayRef<>::INVALID, ArrayRef<>::NIL.findLast(ArrayView<>(data1)));
  ASSERT_EQ(3, ArrayRef<>(data2).findLast(data1[0]));
  ASSERT_EQ(ArrayRef<>::INVALID, ArrayRef<>(data2).findLast(data5[1]));
  ASSERT_EQ(6, ArrayRef<>(data2).findLast(ArrayView<>::EMPTY));
  ASSERT_EQ(6, ArrayRef<>(data2).findLast(ArrayView<>::NIL));
  ASSERT_EQ(3, ArrayRef<>(data2).findLast(ArrayView<>(data1)));
  ASSERT_EQ(0, ArrayRef<>(data2).findLast(ArrayView<>(data2)));
  ASSERT_EQ(0, ArrayRef<>(data2).findLast(ArrayView<>(data3)));
  ASSERT_EQ(ArrayRef<>::INVALID, ArrayRef<>(data2).findLast(ArrayView<>(data4)));
  ASSERT_EQ(ArrayRef<>::INVALID, ArrayRef<>(data2).findLast(ArrayView<>(data5)));
}

TEST(ArrayRefTests, head) {
  uint8_t data[4] = {2, 3, 5, 7};
  ASSERT_EQ(0, ArrayRef<>::EMPTY.head(0).getSize());
  ASSERT_EQ(ArrayRef<>(nullptr, 0), ArrayRef<>::NIL.head(0));
  ASSERT_EQ(ArrayRef<>(data, 0), ArrayRef<>(data).head(0));
  ASSERT_EQ(ArrayRef<>(data, 2), ArrayRef<>(data).head(2));
  ASSERT_EQ(ArrayRef<>(data, 4), ArrayRef<>(data).head(4));
}

TEST(ArrayRefTests, tail) {
  uint8_t data[4] = {2, 3, 5, 7};
  ASSERT_EQ(0, ArrayRef<>::EMPTY.tail(0).getSize());
  ASSERT_EQ(ArrayRef<>(nullptr, 0), ArrayRef<>::NIL.tail(0));
  ASSERT_EQ(ArrayRef<>(data + 4, 0), ArrayRef<>(data).tail(0));
  ASSERT_EQ(ArrayRef<>(data + 2, 2), ArrayRef<>(data).tail(2));
  ASSERT_EQ(ArrayRef<>(data, 4), ArrayRef<>(data).tail(4));
}

TEST(ArrayRefTests, unhead) {
  uint8_t data[4] = {2, 3, 5, 7};
  ASSERT_EQ(0, ArrayRef<>::EMPTY.unhead(0).getSize());
  ASSERT_EQ(ArrayRef<>(nullptr, 0), ArrayRef<>::NIL.unhead(0));
  ASSERT_EQ(ArrayRef<>(data, 4), ArrayRef<>(data).unhead(0));
  ASSERT_EQ(ArrayRef<>(data + 2, 2), ArrayRef<>(data).unhead(2));
  ASSERT_EQ(ArrayRef<>(data + 4, 0), ArrayRef<>(data).unhead(4));
}

TEST(ArrayRefTests, untail) {
  uint8_t data[4] = {2, 3, 5, 7};
  ASSERT_EQ(0, ArrayRef<>::EMPTY.untail(0).getSize());
  ASSERT_EQ(ArrayRef<>(nullptr, 0), ArrayRef<>::NIL.untail(0));
  ASSERT_EQ(ArrayRef<>(data, 4), ArrayRef<>(data).untail(0));
  ASSERT_EQ(ArrayRef<>(data, 2), ArrayRef<>(data).untail(2));
  ASSERT_EQ(ArrayRef<>(data, 0), ArrayRef<>(data).untail(4));
}

TEST(ArrayRefTests, range) {
  uint8_t data[4] = {2, 3, 5, 7};
  ASSERT_EQ(0, ArrayRef<>::EMPTY.range(0, 0).getSize());
  ASSERT_EQ(ArrayRef<>(nullptr, 0), ArrayRef<>::NIL.range(0, 0));
  ASSERT_EQ(ArrayRef<>(data + 0, 0), ArrayRef<>(data).range(0, 0));
  ASSERT_EQ(ArrayRef<>(data + 0, 2), ArrayRef<>(data).range(0, 2));
  ASSERT_EQ(ArrayRef<>(data + 0, 4), ArrayRef<>(data).range(0, 4));
  ASSERT_EQ(ArrayRef<>(data + 2, 0), ArrayRef<>(data).range(2, 2));
  ASSERT_EQ(ArrayRef<>(data + 2, 2), ArrayRef<>(data).range(2, 4));
  ASSERT_EQ(ArrayRef<>(data + 4, 0), ArrayRef<>(data).range(4, 4));
}

TEST(ArrayRefTests, slice) {
  uint8_t data[4] = {2, 3, 5, 7};
  ASSERT_EQ(0, ArrayRef<>::EMPTY.slice(0, 0).getSize());
  ASSERT_EQ(ArrayRef<>(nullptr, 0), ArrayRef<>::NIL.slice(0, 0));
  ASSERT_EQ(ArrayRef<>(data + 0, 0), ArrayRef<>(data).slice(0, 0));
  ASSERT_EQ(ArrayRef<>(data + 0, 2), ArrayRef<>(data).slice(0, 2));
  ASSERT_EQ(ArrayRef<>(data + 0, 4), ArrayRef<>(data).slice(0, 4));
  ASSERT_EQ(ArrayRef<>(data + 2, 0), ArrayRef<>(data).slice(2, 0));
  ASSERT_EQ(ArrayRef<>(data + 2, 2), ArrayRef<>(data).slice(2, 2));
  ASSERT_EQ(ArrayRef<>(data + 4, 0), ArrayRef<>(data).slice(4, 0));
}

TEST(ArrayRefTests, fill) {
  uint8_t data[4] = {2, 3, 5, 7};
  const ArrayRef<> ref(data);
  ASSERT_EQ(ArrayRef<>(data), ref.fill(11));
  ASSERT_EQ(11, data[0]);
  ASSERT_EQ(11, data[1]);
  ASSERT_EQ(11, data[2]);
  ASSERT_EQ(11, data[3]);
}

TEST(ArrayRefTests, reverse) {
  uint8_t data[4] = {2, 3, 5, 7};
  const ArrayRef<> ref(data);
  ASSERT_EQ(ArrayRef<>(data), ref.reverse());
  ASSERT_EQ(7, data[0]);
  ASSERT_EQ(5, data[1]);
  ASSERT_EQ(3, data[2]);
  ASSERT_EQ(2, data[3]);
}
