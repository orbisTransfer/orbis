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
#include <Common/StringView.h>
#include <gtest/gtest.h>

using namespace Common;

TEST(StringViewTests, representations) {
  ASSERT_NE(nullptr, StringView::EMPTY.getData());
  ASSERT_EQ(0, StringView::EMPTY.getSize());
  ASSERT_EQ(nullptr, StringView::NIL.getData());
  ASSERT_EQ(0, StringView::NIL.getSize());
}

TEST(StringViewTests, directConstructor) {
  const char data[] = "ABCD";
  ASSERT_EQ(data, StringView(data, 4).getData());
  ASSERT_EQ(4, StringView(data, 4).getSize());
}

TEST(StringViewTests, arrayConstructor) {
  const char data[] = "ABCD";
  const StringView view = data;
  ASSERT_EQ(data, view.getData());
  ASSERT_EQ(4, view.getSize());
}

TEST(StringViewTests, stdStringConstructor) {
  std::string string("ABCD");
  const StringView view = string;
  ASSERT_EQ(string.data(), view.getData());
  ASSERT_EQ(string.size(), view.getSize());
}

TEST(StringViewTests, copyConstructor) {
  const char data[] = "ABCD";
  const StringView view(data);
  ASSERT_EQ(view.getData(), StringView(view).getData());
  ASSERT_EQ(view.getSize(), StringView(view).getSize());
}

TEST(StringViewTests, copyAssignment) {
  const char data[] = "ABCD";
  const StringView view1(data);
  StringView view2;
  view2 = view1;
  ASSERT_EQ(view1.getData(), view2.getData());
  ASSERT_EQ(view1.getSize(), view2.getSize());
}

TEST(ArrayRefTests, stdString) {
  const char data[] = "ABCD";
  const StringView view(data);
  std::string string(view);
  ASSERT_EQ(*view.getData(), *string.data());
  ASSERT_EQ(*(view.getData() + 1), *(string.data() + 1));
  ASSERT_EQ(*(view.getData() + 2), *(string.data() + 2));
  ASSERT_EQ(*(view.getData() + 3), *(string.data() + 3));
  ASSERT_EQ(view.getSize(), string.size());
}

TEST(StringViewTests, emptyNil) {
  ASSERT_TRUE(StringView::EMPTY.isEmpty());
  ASSERT_FALSE(StringView::EMPTY.isNil());
  ASSERT_TRUE(StringView::NIL.isEmpty());
  ASSERT_TRUE(StringView::NIL.isNil());
  const char data[] = "ABCD";
  ASSERT_TRUE(StringView(data, 0).isEmpty());
  ASSERT_FALSE(StringView(data, 0).isNil());
  ASSERT_FALSE(StringView(data).isEmpty());
  ASSERT_FALSE(StringView(data).isNil());
}

TEST(StringViewTests, squareBrackets) {
  const char data[] = "ABCD";
  const StringView view(data);
  ASSERT_EQ(data + 0, &view[0]);
  ASSERT_EQ(data + 1, &view[1]);
  ASSERT_EQ(data + 2, &view[2]);
  ASSERT_EQ(data + 3, &view[3]);
}

TEST(StringViewTests, firstLast) {
  const char data[] = "ABCD";
  const StringView view(data);
  ASSERT_EQ(data + 0, &view.first());
  ASSERT_EQ(data + 3, &view.last());
}

TEST(StringViewTests, beginEnd) {
  const char data[] = "ABCD";
  ASSERT_EQ(nullptr, StringView::NIL.begin());
  ASSERT_EQ(nullptr, StringView::NIL.end());
  ASSERT_EQ(data, StringView(data).begin());
  ASSERT_EQ(data + 4, StringView(data).end());
  size_t offset = 0;
  for (const char& value : StringView(data)) {
    ASSERT_EQ(data[offset], value);
    ++offset;
  }
}

TEST(StringViewTests, comparisons) {
  const char data1[] = "ABC";
  const char data2[] = "ABCD";
  const char data3[] = "ABCD";
  const char data4[] = "ABCDE";
  const char data5[] = "FGHI";
  ASSERT_TRUE(StringView::EMPTY == StringView::EMPTY);
  ASSERT_TRUE(StringView::EMPTY == StringView::NIL);
  ASSERT_FALSE(StringView::EMPTY == StringView(data1));
  ASSERT_TRUE(StringView::NIL == StringView::EMPTY);
  ASSERT_TRUE(StringView::NIL == StringView::NIL);
  ASSERT_FALSE(StringView::NIL == StringView(data1));
  ASSERT_FALSE(StringView(data2) == StringView::EMPTY);
  ASSERT_FALSE(StringView(data2) == StringView::NIL);
  ASSERT_FALSE(StringView(data2) == StringView(data1));
  ASSERT_TRUE(StringView(data2) == StringView(data2));
  ASSERT_TRUE(StringView(data2) == StringView(data3));
  ASSERT_FALSE(StringView(data2) == StringView(data4));
  ASSERT_FALSE(StringView(data2) == StringView(data5));
  ASSERT_FALSE(StringView::EMPTY != StringView::EMPTY);
  ASSERT_FALSE(StringView::EMPTY != StringView::NIL);
  ASSERT_TRUE(StringView::EMPTY != StringView(data1));
  ASSERT_FALSE(StringView::NIL != StringView::EMPTY);
  ASSERT_FALSE(StringView::NIL != StringView::NIL);
  ASSERT_TRUE(StringView::NIL != StringView(data1));
  ASSERT_TRUE(StringView(data2) != StringView::EMPTY);
  ASSERT_TRUE(StringView(data2) != StringView::NIL);
  ASSERT_TRUE(StringView(data2) != StringView(data1));
  ASSERT_FALSE(StringView(data2) != StringView(data2));
  ASSERT_FALSE(StringView(data2) != StringView(data3));
  ASSERT_TRUE(StringView(data2) != StringView(data4));
  ASSERT_TRUE(StringView(data2) != StringView(data5));
  ASSERT_FALSE(StringView::EMPTY < StringView::EMPTY);
  ASSERT_FALSE(StringView::EMPTY < StringView::NIL);
  ASSERT_TRUE(StringView::EMPTY < StringView(data1));
  ASSERT_FALSE(StringView::NIL < StringView::EMPTY);
  ASSERT_FALSE(StringView::NIL < StringView::NIL);
  ASSERT_TRUE(StringView::NIL < StringView(data1));
  ASSERT_FALSE(StringView(data2) < StringView::EMPTY);
  ASSERT_FALSE(StringView(data2) < StringView::NIL);
  ASSERT_FALSE(StringView(data2) < StringView(data1));
  ASSERT_FALSE(StringView(data2) < StringView(data2));
  ASSERT_FALSE(StringView(data2) < StringView(data3));
  ASSERT_TRUE(StringView(data2) < StringView(data4));
  ASSERT_TRUE(StringView(data2) < StringView(data5));
  ASSERT_TRUE(StringView::EMPTY <= StringView::EMPTY);
  ASSERT_TRUE(StringView::EMPTY <= StringView::NIL);
  ASSERT_TRUE(StringView::EMPTY <= StringView(data1));
  ASSERT_TRUE(StringView::NIL <= StringView::EMPTY);
  ASSERT_TRUE(StringView::NIL <= StringView::NIL);
  ASSERT_TRUE(StringView::NIL <= StringView(data1));
  ASSERT_FALSE(StringView(data2) <= StringView::EMPTY);
  ASSERT_FALSE(StringView(data2) <= StringView::NIL);
  ASSERT_FALSE(StringView(data2) <= StringView(data1));
  ASSERT_TRUE(StringView(data2) <= StringView(data2));
  ASSERT_TRUE(StringView(data2) <= StringView(data3));
  ASSERT_TRUE(StringView(data2) <= StringView(data4));
  ASSERT_TRUE(StringView(data2) <= StringView(data5));
  ASSERT_FALSE(StringView::EMPTY > StringView::EMPTY);
  ASSERT_FALSE(StringView::EMPTY > StringView::NIL);
  ASSERT_FALSE(StringView::EMPTY > StringView(data1));
  ASSERT_FALSE(StringView::NIL > StringView::EMPTY);
  ASSERT_FALSE(StringView::NIL > StringView::NIL);
  ASSERT_FALSE(StringView::NIL > StringView(data1));
  ASSERT_TRUE(StringView(data2) > StringView::EMPTY);
  ASSERT_TRUE(StringView(data2) > StringView::NIL);
  ASSERT_TRUE(StringView(data2) > StringView(data1));
  ASSERT_FALSE(StringView(data2) > StringView(data2));
  ASSERT_FALSE(StringView(data2) > StringView(data3));
  ASSERT_FALSE(StringView(data2) > StringView(data4));
  ASSERT_FALSE(StringView(data2) > StringView(data5));
  ASSERT_TRUE(StringView::EMPTY >= StringView::EMPTY);
  ASSERT_TRUE(StringView::EMPTY >= StringView::NIL);
  ASSERT_FALSE(StringView::EMPTY >= StringView(data1));
  ASSERT_TRUE(StringView::NIL >= StringView::EMPTY);
  ASSERT_TRUE(StringView::NIL >= StringView::NIL);
  ASSERT_FALSE(StringView::NIL >= StringView(data1));
  ASSERT_TRUE(StringView(data2) >= StringView::EMPTY);
  ASSERT_TRUE(StringView(data2) >= StringView::NIL);
  ASSERT_TRUE(StringView(data2) >= StringView(data1));
  ASSERT_TRUE(StringView(data2) >= StringView(data2));
  ASSERT_TRUE(StringView(data2) >= StringView(data3));
  ASSERT_FALSE(StringView(data2) >= StringView(data4));
  ASSERT_FALSE(StringView(data2) >= StringView(data5));
}

TEST(StringViewTests, beginsWith) {
  const char data1[] = "ABC";
  const char data2[] = "ABCD";
  const char data3[] = "ABCD";
  const char data4[] = "ABCDE";
  const char data5[] = "FGHI";
  ASSERT_FALSE(StringView::EMPTY.beginsWith(data1[0]));
  ASSERT_TRUE(StringView::EMPTY.beginsWith(StringView::EMPTY));
  ASSERT_TRUE(StringView::EMPTY.beginsWith(StringView::NIL));
  ASSERT_FALSE(StringView::EMPTY.beginsWith(StringView(data1)));
  ASSERT_FALSE(StringView::NIL.beginsWith(data1[0]));
  ASSERT_TRUE(StringView::NIL.beginsWith(StringView::EMPTY));
  ASSERT_TRUE(StringView::NIL.beginsWith(StringView::NIL));
  ASSERT_FALSE(StringView::NIL.beginsWith(StringView(data1)));
  ASSERT_TRUE(StringView(data2).beginsWith(data1[0]));
  ASSERT_FALSE(StringView(data2).beginsWith(data5[0]));
  ASSERT_TRUE(StringView(data2).beginsWith(StringView::EMPTY));
  ASSERT_TRUE(StringView(data2).beginsWith(StringView::NIL));
  ASSERT_TRUE(StringView(data2).beginsWith(StringView(data1)));
  ASSERT_TRUE(StringView(data2).beginsWith(StringView(data2)));
  ASSERT_TRUE(StringView(data2).beginsWith(StringView(data3)));
  ASSERT_FALSE(StringView(data2).beginsWith(StringView(data4)));
  ASSERT_FALSE(StringView(data2).beginsWith(StringView(data5)));
}

TEST(StringViewTests, contains) {
  const char data1[] = "BC";
  const char data2[] = "ABCD";
  const char data3[] = "ABCD";
  const char data4[] = "ABCDE";
  const char data5[] = "FGHI";
  ASSERT_FALSE(StringView::EMPTY.contains(data1[1]));
  ASSERT_TRUE(StringView::EMPTY.contains(StringView::EMPTY));
  ASSERT_TRUE(StringView::EMPTY.contains(StringView::NIL));
  ASSERT_FALSE(StringView::EMPTY.contains(StringView(data1)));
  ASSERT_FALSE(StringView::NIL.contains(data1[1]));
  ASSERT_TRUE(StringView::NIL.contains(StringView::EMPTY));
  ASSERT_TRUE(StringView::NIL.contains(StringView::NIL));
  ASSERT_FALSE(StringView::NIL.contains(StringView(data1)));
  ASSERT_TRUE(StringView(data2).contains(data1[1]));
  ASSERT_FALSE(StringView(data2).contains(data5[1]));
  ASSERT_TRUE(StringView(data2).contains(StringView::EMPTY));
  ASSERT_TRUE(StringView(data2).contains(StringView::NIL));
  ASSERT_TRUE(StringView(data2).contains(StringView(data1)));
  ASSERT_TRUE(StringView(data2).contains(StringView(data2)));
  ASSERT_TRUE(StringView(data2).contains(StringView(data3)));
  ASSERT_FALSE(StringView(data2).contains(StringView(data4)));
  ASSERT_FALSE(StringView(data2).contains(StringView(data5)));
}

TEST(StringViewTests, endsWith) {
  const char data1[] = "BCD";
  const char data2[] = "ABCD";
  const char data3[] = "ABCD";
  const char data4[] = "ABCDE";
  const char data5[] = "FGHI";
  ASSERT_FALSE(StringView::EMPTY.endsWith(data1[2]));
  ASSERT_TRUE(StringView::EMPTY.endsWith(StringView::EMPTY));
  ASSERT_TRUE(StringView::EMPTY.endsWith(StringView::NIL));
  ASSERT_FALSE(StringView::EMPTY.endsWith(StringView(data1)));
  ASSERT_FALSE(StringView::NIL.endsWith(data1[2]));
  ASSERT_TRUE(StringView::NIL.endsWith(StringView::EMPTY));
  ASSERT_TRUE(StringView::NIL.endsWith(StringView::NIL));
  ASSERT_FALSE(StringView::NIL.endsWith(StringView(data1)));
  ASSERT_TRUE(StringView(data2).endsWith(data1[2]));
  ASSERT_FALSE(StringView(data2).endsWith(data5[3]));
  ASSERT_TRUE(StringView(data2).endsWith(StringView::EMPTY));
  ASSERT_TRUE(StringView(data2).endsWith(StringView::NIL));
  ASSERT_TRUE(StringView(data2).endsWith(StringView(data1)));
  ASSERT_TRUE(StringView(data2).endsWith(StringView(data2)));
  ASSERT_TRUE(StringView(data2).endsWith(StringView(data3)));
  ASSERT_FALSE(StringView(data2).endsWith(StringView(data4)));
  ASSERT_FALSE(StringView(data2).endsWith(StringView(data5)));
}

TEST(StringViewTests, find) {
  const char data1[] = "BC";
  const char data2[] = "ABCBCD";
  const char data3[] = "ABCBCD";
  const char data4[] = "ABCBCDE";
  const char data5[] = "FGHI";
  ASSERT_EQ(StringView::INVALID, StringView::EMPTY.find(data1[0]));
  ASSERT_EQ(0, StringView::EMPTY.find(StringView::EMPTY));
  ASSERT_EQ(0, StringView::EMPTY.find(StringView::NIL));
  ASSERT_EQ(StringView::INVALID, StringView::EMPTY.find(StringView(data1)));
  ASSERT_EQ(StringView::INVALID, StringView::NIL.find(data1[0]));
  ASSERT_EQ(0, StringView::NIL.find(StringView::EMPTY));
  ASSERT_EQ(0, StringView::NIL.find(StringView::NIL));
  ASSERT_EQ(StringView::INVALID, StringView::NIL.find(StringView(data1)));
  ASSERT_EQ(1, StringView(data2).find(data1[0]));
  ASSERT_EQ(StringView::INVALID, StringView(data2).find(data5[1]));
  ASSERT_EQ(0, StringView(data2).find(StringView::EMPTY));
  ASSERT_EQ(0, StringView(data2).find(StringView::NIL));
  ASSERT_EQ(1, StringView(data2).find(StringView(data1)));
  ASSERT_EQ(0, StringView(data2).find(StringView(data2)));
  ASSERT_EQ(0, StringView(data2).find(StringView(data3)));
  ASSERT_EQ(StringView::INVALID, StringView(data2).find(StringView(data4)));
  ASSERT_EQ(StringView::INVALID, StringView(data2).find(StringView(data5)));
}

TEST(StringViewTests, findLast) {
  const char data1[] = "BC";
  const char data2[] = "ABCBCD";
  const char data3[] = "ABCBCD";
  const char data4[] = "ABCBCDE";
  const char data5[] = "FGHI";
  ASSERT_EQ(StringView::INVALID, StringView::EMPTY.findLast(data1[0]));
  ASSERT_EQ(0, StringView::EMPTY.findLast(StringView::EMPTY));
  ASSERT_EQ(0, StringView::EMPTY.findLast(StringView::NIL));
  ASSERT_EQ(StringView::INVALID, StringView::EMPTY.findLast(StringView(data1)));
  ASSERT_EQ(StringView::INVALID, StringView::NIL.findLast(data1[0]));
  ASSERT_EQ(0, StringView::NIL.findLast(StringView::EMPTY));
  ASSERT_EQ(0, StringView::NIL.findLast(StringView::NIL));
  ASSERT_EQ(StringView::INVALID, StringView::NIL.findLast(StringView(data1)));
  ASSERT_EQ(3, StringView(data2).findLast(data1[0]));
  ASSERT_EQ(StringView::INVALID, StringView(data2).findLast(data5[1]));
  ASSERT_EQ(6, StringView(data2).findLast(StringView::EMPTY));
  ASSERT_EQ(6, StringView(data2).findLast(StringView::NIL));
  ASSERT_EQ(3, StringView(data2).findLast(StringView(data1)));
  ASSERT_EQ(0, StringView(data2).findLast(StringView(data2)));
  ASSERT_EQ(0, StringView(data2).findLast(StringView(data3)));
  ASSERT_EQ(StringView::INVALID, StringView(data2).findLast(StringView(data4)));
  ASSERT_EQ(StringView::INVALID, StringView(data2).findLast(StringView(data5)));
}

TEST(StringViewTests, head) {
  const char data[] = "ABCD";
  ASSERT_EQ(0, StringView::EMPTY.head(0).getSize());
  ASSERT_EQ(StringView(nullptr, 0), StringView::NIL.head(0));
  ASSERT_EQ(StringView(data, 0), StringView(data).head(0));
  ASSERT_EQ(StringView(data, 2), StringView(data).head(2));
  ASSERT_EQ(StringView(data, 4), StringView(data).head(4));
}

TEST(StringViewTests, tail) {
  const char data[] = "ABCD";
  ASSERT_EQ(0, StringView::EMPTY.tail(0).getSize());
  ASSERT_EQ(StringView(nullptr, 0), StringView::NIL.tail(0));
  ASSERT_EQ(StringView(data + 4, 0), StringView(data).tail(0));
  ASSERT_EQ(StringView(data + 2, 2), StringView(data).tail(2));
  ASSERT_EQ(StringView(data, 4), StringView(data).tail(4));
}

TEST(StringViewTests, unhead) {
  const char data[] = "ABCD";
  ASSERT_EQ(0, StringView::EMPTY.unhead(0).getSize());
  ASSERT_EQ(StringView(nullptr, 0), StringView::NIL.unhead(0));
  ASSERT_EQ(StringView(data, 4), StringView(data).unhead(0));
  ASSERT_EQ(StringView(data + 2, 2), StringView(data).unhead(2));
  ASSERT_EQ(StringView(data + 4, 0), StringView(data).unhead(4));
}

TEST(StringViewTests, untail) {
  const char data[] = "ABCD";
  ASSERT_EQ(0, StringView::EMPTY.untail(0).getSize());
  ASSERT_EQ(StringView(nullptr, 0), StringView::NIL.untail(0));
  ASSERT_EQ(StringView(data, 4), StringView(data).untail(0));
  ASSERT_EQ(StringView(data, 2), StringView(data).untail(2));
  ASSERT_EQ(StringView(data, 0), StringView(data).untail(4));
}

TEST(StringViewTests, range) {
  const char data[] = "ABCD";
  ASSERT_EQ(0, StringView::EMPTY.range(0, 0).getSize());
  ASSERT_EQ(StringView(nullptr, 0), StringView::NIL.range(0, 0));
  ASSERT_EQ(StringView(data + 0, 0), StringView(data).range(0, 0));
  ASSERT_EQ(StringView(data + 0, 2), StringView(data).range(0, 2));
  ASSERT_EQ(StringView(data + 0, 4), StringView(data).range(0, 4));
  ASSERT_EQ(StringView(data + 2, 0), StringView(data).range(2, 2));
  ASSERT_EQ(StringView(data + 2, 2), StringView(data).range(2, 4));
  ASSERT_EQ(StringView(data + 4, 0), StringView(data).range(4, 4));
}

TEST(StringViewTests, slice) {
  const char data[] = "ABCD";
  ASSERT_EQ(0, StringView::EMPTY.slice(0, 0).getSize());
  ASSERT_EQ(StringView(nullptr, 0), StringView::NIL.slice(0, 0));
  ASSERT_EQ(StringView(data + 0, 0), StringView(data).slice(0, 0));
  ASSERT_EQ(StringView(data + 0, 2), StringView(data).slice(0, 2));
  ASSERT_EQ(StringView(data + 0, 4), StringView(data).slice(0, 4));
  ASSERT_EQ(StringView(data + 2, 0), StringView(data).slice(2, 0));
  ASSERT_EQ(StringView(data + 2, 2), StringView(data).slice(2, 2));
  ASSERT_EQ(StringView(data + 4, 0), StringView(data).slice(4, 0));
}

TEST(StringViewTests, set) {
  std::set<std::string> set;
  set.insert("AB");
  set.insert("ABC");
  set.insert("ABCD");
  ASSERT_EQ(0, set.count(std::string(StringView("A"))));
  ASSERT_EQ(1, set.count(std::string(StringView("AB"))));
  ASSERT_EQ(1, set.count(std::string(StringView("ABC"))));
  ASSERT_EQ(1, set.count(std::string(StringView("ABCD"))));
  ASSERT_EQ(0, set.count(std::string(StringView("ABCDE"))));
}
