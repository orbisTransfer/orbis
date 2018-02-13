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

#include "orbis/orbis.h"
#include "OrbisCore/Currency.h"
#include "OrbisCore/TransactionApi.h"
#include "Logging/ConsoleLogger.h"

#include "TransactionApiHelpers.h"

using namespace Orbis;

namespace {
const size_t TEST_FUSION_TX_MAX_SIZE = 6000;
const size_t TEST_FUSION_TX_MIN_INPUT_COUNT = 6;
const size_t TEST_FUSION_TX_MIN_IN_OUT_COUNT_RATIO = 3;
const uint64_t TEST_DUST_THRESHOLD = UINT64_C(1000000);
const uint64_t TEST_AMOUNT = 370 * TEST_DUST_THRESHOLD;

class Currency_isFusionTransactionTest : public ::testing::Test {
public:
  Currency_isFusionTransactionTest() :
    m_currency(CurrencyBuilder(m_logger).
      defaultDustThreshold(TEST_DUST_THRESHOLD).
      fusionTxMaxSize(TEST_FUSION_TX_MAX_SIZE).
      fusionTxMinInputCount(TEST_FUSION_TX_MIN_INPUT_COUNT).
      fusionTxMinInOutCountRatio(TEST_FUSION_TX_MIN_IN_OUT_COUNT_RATIO).
      currency()) {
  }

protected:
  Logging::ConsoleLogger m_logger;
  Currency m_currency;
};
}

TEST_F(Currency_isFusionTransactionTest, succeedsOnFusionTransaction) {
  auto tx = FusionTransactionBuilder(m_currency, TEST_AMOUNT).buildTx();
  ASSERT_TRUE(m_currency.isFusionTransaction(tx));
}

TEST_F(Currency_isFusionTransactionTest, succeedsIfFusionTransactionSizeEqMaxSize) {
  FusionTransactionBuilder builder(m_currency, TEST_AMOUNT);
  auto tx = builder.createFusionTransactionBySize(m_currency.fusionTxMaxSize());
  ASSERT_EQ(m_currency.fusionTxMaxSize(), getObjectBinarySize(tx));
  ASSERT_TRUE(m_currency.isFusionTransaction(tx));
}

TEST_F(Currency_isFusionTransactionTest, failsIfFusionTransactionSizeGreaterThanMaxSize) {
  FusionTransactionBuilder builder(m_currency, TEST_AMOUNT);
  auto tx = builder.createFusionTransactionBySize(m_currency.fusionTxMaxSize() + 1);
  ASSERT_EQ(m_currency.fusionTxMaxSize() + 1, getObjectBinarySize(tx));
  ASSERT_FALSE(m_currency.isFusionTransaction(tx));
}

TEST_F(Currency_isFusionTransactionTest, failsIfTransactionInputsCountIsNotEnought) {
  FusionTransactionBuilder builder(m_currency, TEST_AMOUNT);
  builder.setInputCount(m_currency.fusionTxMinInputCount() - 1);
  auto tx = builder.buildTx();
  ASSERT_EQ(m_currency.fusionTxMinInputCount() - 1, tx.inputs.size());
  ASSERT_FALSE(m_currency.isFusionTransaction(tx));
}

TEST_F(Currency_isFusionTransactionTest, failsIfTransactionInputOutputCountRatioIsLessThenNecessary) {
  FusionTransactionBuilder builder(m_currency, 3710 * m_currency.defaultDustThreshold());
  auto tx = builder.buildTx();
  ASSERT_EQ(3, tx.outputs.size());
  ASSERT_GT(tx.outputs.size() * m_currency.fusionTxMinInOutCountRatio(), tx.inputs.size());
  ASSERT_FALSE(m_currency.isFusionTransaction(tx));
}

TEST_F(Currency_isFusionTransactionTest, failsIfTransactionHasNotExponentialOutput) {
  FusionTransactionBuilder builder(m_currency, TEST_AMOUNT);
  builder.setFirstOutput(TEST_AMOUNT);
  auto tx = builder.buildTx();
  ASSERT_EQ(1, tx.outputs.size());
  ASSERT_FALSE(m_currency.isFusionTransaction(tx));
}

TEST_F(Currency_isFusionTransactionTest, failsIfTransactionHasOutputsWithTheSameExponent) {
  FusionTransactionBuilder builder(m_currency, 130 * m_currency.defaultDustThreshold());
  builder.setFirstOutput(70 * m_currency.defaultDustThreshold());
  auto tx = builder.buildTx();
  ASSERT_EQ(2, tx.outputs.size());
  ASSERT_FALSE(m_currency.isFusionTransaction(tx));
}

TEST_F(Currency_isFusionTransactionTest, succeedsIfTransactionHasDustOutput) {
  FusionTransactionBuilder builder(m_currency, 11 * m_currency.defaultDustThreshold());
  auto tx = builder.buildTx();
  ASSERT_EQ(2, tx.outputs.size());
  ASSERT_EQ(m_currency.defaultDustThreshold(), tx.outputs[0].amount);
  ASSERT_TRUE(m_currency.isFusionTransaction(tx));
}

TEST_F(Currency_isFusionTransactionTest, failsIfTransactionFeeIsNotZero) {
  FusionTransactionBuilder builder(m_currency, 370 * m_currency.defaultDustThreshold());
  builder.setFee(70 * m_currency.defaultDustThreshold());
  auto tx = builder.buildTx();
  ASSERT_FALSE(m_currency.isFusionTransaction(tx));
}

TEST_F(Currency_isFusionTransactionTest, succedsIfTransactionHasInputEqualsDustThreshold) {
  FusionTransactionBuilder builder(m_currency, TEST_AMOUNT);
  builder.setFirstInput(m_currency.defaultDustThreshold());
  auto tx = builder.buildTx();
  ASSERT_TRUE(m_currency.isFusionTransaction(tx));
}

TEST_F(Currency_isFusionTransactionTest, failsIfTransactionHasInputLessThanDustThreshold) {
  FusionTransactionBuilder builder(m_currency, TEST_AMOUNT);
  builder.setFirstInput(m_currency.defaultDustThreshold() - 1);
  auto tx = builder.buildTx();
  ASSERT_FALSE(m_currency.isFusionTransaction(tx));
}
