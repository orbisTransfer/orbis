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

#include "OrbisCore/OrbisBasicImpl.h"
#include "OrbisCore/Currency.h"
#include <Logging/LoggerGroup.h>

using namespace Orbis;

namespace
{
  const uint64_t TEST_GRANTED_FULL_REWARD_ZONE = 10000;
  const uint64_t TEST_MONEY_SUPPLY = static_cast<uint64_t>(-1);
  const uint64_t TEST_EMISSION_SPEED_FACTOR = 18;

  //--------------------------------------------------------------------------------------------------------------------
  class getBlockReward_and_already_generated_coins : public ::testing::Test {
  public:
    getBlockReward_and_already_generated_coins() :
      ::testing::Test(),
      m_currency(Orbis::CurrencyBuilder(m_logger).
        blockGrantedFullRewardZone(TEST_GRANTED_FULL_REWARD_ZONE).
        moneySupply(TEST_MONEY_SUPPLY).
        emissionSpeedFactor(TEST_EMISSION_SPEED_FACTOR).
        currency()) {
    }

  protected:
    static const size_t currentBlockSize = TEST_GRANTED_FULL_REWARD_ZONE / 2;

    Logging::LoggerGroup m_logger;
    Orbis::Currency m_currency;
    bool m_blockTooBig;
    int64_t m_emissionChange;
    uint64_t m_blockReward;
  };

  #define TEST_ALREADY_GENERATED_COINS(alreadyGeneratedCoins, expectedReward)                                  \
    m_blockTooBig = !m_currency.getBlockReward(0, currentBlockSize, alreadyGeneratedCoins, 0,                  \
      m_blockReward, m_emissionChange);                                                                        \
    ASSERT_FALSE(m_blockTooBig);                                                                               \
    ASSERT_EQ(UINT64_C(expectedReward), m_blockReward);                                                        \
    ASSERT_EQ(UINT64_C(expectedReward), m_emissionChange);

  TEST_F(getBlockReward_and_already_generated_coins, handles_first_values) {
    TEST_ALREADY_GENERATED_COINS(0, 70368744177663);
    TEST_ALREADY_GENERATED_COINS(m_blockReward, 70368475742208);
    TEST_ALREADY_GENERATED_COINS(UINT64_C(2756434948434199641), 59853779316998);
  }

  TEST_F(getBlockReward_and_already_generated_coins, correctly_steps_from_reward_2_to_1) {
    TEST_ALREADY_GENERATED_COINS(m_currency.moneySupply() - ((UINT64_C(2) << m_currency.emissionSpeedFactor()) + 1), 2);
    TEST_ALREADY_GENERATED_COINS(m_currency.moneySupply() -  (UINT64_C(2) << m_currency.emissionSpeedFactor())     , 2);
    TEST_ALREADY_GENERATED_COINS(m_currency.moneySupply() - ((UINT64_C(2) << m_currency.emissionSpeedFactor()) - 1), 1);
  }

  TEST_F(getBlockReward_and_already_generated_coins, handles_max_already_generaged_coins) {
    TEST_ALREADY_GENERATED_COINS(m_currency.moneySupply() - ((UINT64_C(1) << m_currency.emissionSpeedFactor()) + 1), 1);
    TEST_ALREADY_GENERATED_COINS(m_currency.moneySupply() -  (UINT64_C(1) << m_currency.emissionSpeedFactor())     , 1);
    TEST_ALREADY_GENERATED_COINS(m_currency.moneySupply() - ((UINT64_C(1) << m_currency.emissionSpeedFactor()) - 1), 0);
    TEST_ALREADY_GENERATED_COINS(m_currency.moneySupply() - 1, 0);
    TEST_ALREADY_GENERATED_COINS(m_currency.moneySupply(), 0);
  }

  //--------------------------------------------------------------------------------------------------------------------
  class getBlockReward_and_median_and_blockSize : public ::testing::Test {
  public:
    getBlockReward_and_median_and_blockSize() :
      ::testing::Test(),
      m_currency(Orbis::CurrencyBuilder(m_logger).
        blockGrantedFullRewardZone(TEST_GRANTED_FULL_REWARD_ZONE).
        moneySupply(TEST_MONEY_SUPPLY).
        emissionSpeedFactor(TEST_EMISSION_SPEED_FACTOR).
        currency()) {
    }

  protected:
    static const uint64_t alreadyGeneratedCoins = 0;

    virtual void SetUp() override {
      m_blockTooBig = !m_currency.getBlockReward(0, 0, alreadyGeneratedCoins, 0,
        m_standardBlockReward, m_emissionChange);
      ASSERT_FALSE(m_blockTooBig);
      ASSERT_EQ(UINT64_C(70368744177663), m_standardBlockReward);
    }

    void do_test(size_t medianBlockSize, size_t currentBlockSize) {
      m_blockTooBig = !m_currency.getBlockReward(medianBlockSize, currentBlockSize, alreadyGeneratedCoins, 0,
        m_blockReward, m_emissionChange);
    }

    Logging::LoggerGroup m_logger;
    Orbis::Currency m_currency;
    bool m_blockTooBig;
    int64_t m_emissionChange;
    uint64_t m_blockReward;
    uint64_t m_standardBlockReward;
  };

  TEST_F(getBlockReward_and_median_and_blockSize, handles_zero_median) {
    do_test(0, TEST_GRANTED_FULL_REWARD_ZONE);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(m_standardBlockReward, m_blockReward);
  }

  TEST_F(getBlockReward_and_median_and_blockSize, handles_median_lt_relevance_level) {
    do_test(TEST_GRANTED_FULL_REWARD_ZONE - 1, TEST_GRANTED_FULL_REWARD_ZONE);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(m_standardBlockReward, m_blockReward);
  }

  TEST_F(getBlockReward_and_median_and_blockSize, handles_median_eq_relevance_level) {
    do_test(TEST_GRANTED_FULL_REWARD_ZONE, TEST_GRANTED_FULL_REWARD_ZONE - 1);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(m_standardBlockReward, m_blockReward);
  }

  TEST_F(getBlockReward_and_median_and_blockSize, handles_median_gt_relevance_level) {
    do_test(TEST_GRANTED_FULL_REWARD_ZONE + 1, TEST_GRANTED_FULL_REWARD_ZONE);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(m_standardBlockReward, m_blockReward);
  }

  TEST_F(getBlockReward_and_median_and_blockSize, handles_big_median) {
    size_t blockSize = 1;
    size_t medianSize = std::numeric_limits<uint32_t>::max();

    do_test(medianSize, blockSize);

    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(m_standardBlockReward, m_blockReward);
  }

  TEST_F(getBlockReward_and_median_and_blockSize, handles_big_block_size) {
    size_t blockSize = std::numeric_limits<uint32_t>::max() - 1; // even
    size_t medianSize = blockSize / 2; // 2 * medianSize == blockSize

    do_test(medianSize, blockSize);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(0, m_blockReward);
  }

  TEST_F(getBlockReward_and_median_and_blockSize, handles_big_block_size_fail) {
    size_t blockSize = std::numeric_limits<uint32_t>::max();
    size_t medianSize = blockSize / 2 - 1;

    do_test(medianSize, blockSize);
    ASSERT_TRUE(m_blockTooBig);
  }

  TEST_F(getBlockReward_and_median_and_blockSize, handles_big_median_and_block_size) {
    // blockSize should be greater medianSize
    size_t blockSize = std::numeric_limits<uint32_t>::max();
    size_t medianSize = std::numeric_limits<uint32_t>::max() - 1;

    do_test(medianSize, blockSize);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_LT(m_blockReward, m_standardBlockReward);
  }

  //--------------------------------------------------------------------------------------------------------------------
  class getBlockReward_and_currentBlockSize : public ::testing::Test {
  public:
    getBlockReward_and_currentBlockSize() :
      ::testing::Test(),
      m_currency(Orbis::CurrencyBuilder(m_logger).
        blockGrantedFullRewardZone(TEST_GRANTED_FULL_REWARD_ZONE).
        moneySupply(TEST_MONEY_SUPPLY).
        emissionSpeedFactor(TEST_EMISSION_SPEED_FACTOR).
        currency()) {
    }

  protected:
    static const size_t testMedian = 7 * TEST_GRANTED_FULL_REWARD_ZONE;
    static const uint64_t alreadyGeneratedCoins = 0;

    virtual void SetUp() override {
      m_blockTooBig = !m_currency.getBlockReward(testMedian, 0, alreadyGeneratedCoins, 0,
        m_standardBlockReward, m_emissionChange);

      ASSERT_FALSE(m_blockTooBig);
      ASSERT_EQ(UINT64_C(70368744177663), m_standardBlockReward);
    }

    void do_test(size_t currentBlockSize) {
      m_blockTooBig = !m_currency.getBlockReward(testMedian, currentBlockSize, alreadyGeneratedCoins, 0,
        m_blockReward, m_emissionChange);
    }

    Logging::LoggerGroup m_logger;
    Orbis::Currency m_currency;
    bool m_blockTooBig;
    int64_t m_emissionChange;
    uint64_t m_blockReward;
    uint64_t m_standardBlockReward;
  };

  TEST_F(getBlockReward_and_currentBlockSize, handles_zero_block_size) {
    do_test(0);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(m_standardBlockReward, m_blockReward);
  }

  TEST_F(getBlockReward_and_currentBlockSize, handles_block_size_less_median) {
    do_test(testMedian - 1);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(m_standardBlockReward, m_blockReward);
  }

  TEST_F(getBlockReward_and_currentBlockSize, handles_block_size_eq_median) {
    do_test(testMedian);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(m_standardBlockReward, m_blockReward);
  }

  TEST_F(getBlockReward_and_currentBlockSize, handles_block_size_gt_median) {
    do_test(testMedian + 1);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_LT(m_blockReward, m_standardBlockReward);
  }

  TEST_F(getBlockReward_and_currentBlockSize, handles_block_size_less_2_medians) {
    do_test(2 * testMedian - 1);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_LT(m_blockReward, m_standardBlockReward);
    ASSERT_GT(m_blockReward, 0);
  }

  TEST_F(getBlockReward_and_currentBlockSize, handles_block_size_eq_2_medians) {
    do_test(2 * testMedian);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(0, m_blockReward);
  }

  TEST_F(getBlockReward_and_currentBlockSize, handles_block_size_gt_2_medians) {
    do_test(2 * testMedian + 1);
    ASSERT_TRUE(m_blockTooBig);
  }

  TEST_F(getBlockReward_and_currentBlockSize, calculates_correctly) {
    ASSERT_EQ(0, testMedian % 8);

    // reward = 1 - (k - 1)^2
    // k = 9/8 => reward = 63/64
    do_test(testMedian * 9 / 8);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(m_standardBlockReward * 63 / 64, m_blockReward);

    // 3/2 = 12/8
    do_test(testMedian * 3 / 2);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(m_standardBlockReward * 3 / 4, m_blockReward);

    do_test(testMedian * 15 / 8);
    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(m_standardBlockReward * 15 / 64, m_blockReward);
  }
  //--------------------------------------------------------------------------------------------------------------------
  const unsigned int testEmissionSpeedFactor = 4;
  const size_t testGrantedFullRewardZone = 10000;
  const size_t testMedian = testGrantedFullRewardZone;
  const size_t testBlockSize = testMedian + testMedian * 8 / 10; // expected penalty 0.64 * reward
  const uint64_t testPenalty = 64; // percentage
  const uint64_t testMoneySupply = UINT64_C(1000000000);
  const uint64_t expectedBaseReward = 62500000;  // testMoneySupply >> testEmissionSpeedFactor
  const uint64_t expectedBlockReward = 22500000; // expectedBaseReward - expectedBaseReward * testPenalty / 100
  //--------------------------------------------------------------------------------------------------------------------
  class getBlockReward_fee_and_penalizeFee_test : public ::testing::Test {
  public:
    getBlockReward_fee_and_penalizeFee_test() :
      ::testing::Test(),
      m_currency(Orbis::CurrencyBuilder(m_logger).
        blockGrantedFullRewardZone(testGrantedFullRewardZone).
        moneySupply(testMoneySupply).
        emissionSpeedFactor(testEmissionSpeedFactor).
        currency()) {
    }

  protected:
    virtual void SetUp() override {
      uint64_t blockReward;
      int64_t emissionChange;

      m_blockTooBig = !m_currency.getBlockReward(testMedian, testBlockSize, 0, 0, blockReward, emissionChange);

      ASSERT_FALSE(m_blockTooBig);
      ASSERT_EQ(expectedBlockReward, blockReward);
      ASSERT_EQ(expectedBlockReward, emissionChange);
    }

    void do_test(uint64_t alreadyGeneratedCoins, uint64_t fee) {
      m_blockTooBig = !m_currency.getBlockReward(testMedian, testBlockSize, alreadyGeneratedCoins, fee,
        m_blockReward, m_emissionChange);
    }

    Logging::LoggerGroup m_logger;
    Orbis::Currency m_currency;
    bool m_blockTooBig;
    int64_t m_emissionChange;
    uint64_t m_blockReward;
  };

  TEST_F(getBlockReward_fee_and_penalizeFee_test, handles_zero_fee_and_penalize_fee) {
    do_test(0, 0);

    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(expectedBlockReward, m_blockReward);
    ASSERT_EQ(expectedBlockReward, m_emissionChange);
    ASSERT_GT(m_emissionChange, 0);
  }

  TEST_F(getBlockReward_fee_and_penalizeFee_test, handles_fee_lt_block_reward_and_penalize_fee) {
    uint64_t fee = expectedBlockReward / 2;
    do_test(0, fee);

    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(expectedBlockReward + fee - fee * testPenalty / 100, m_blockReward);
    ASSERT_EQ(expectedBlockReward - fee * testPenalty / 100, m_emissionChange);
    ASSERT_GT(m_emissionChange, 0);
  }

  TEST_F(getBlockReward_fee_and_penalizeFee_test, handles_fee_eq_block_reward_and_penalize_fee) {
    uint64_t fee = expectedBlockReward;
    do_test(0, fee);

    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(expectedBlockReward + fee - fee * testPenalty / 100, m_blockReward);
    ASSERT_EQ(expectedBlockReward - fee * testPenalty / 100, m_emissionChange);
    ASSERT_GT(m_emissionChange, 0);
  }

  TEST_F(getBlockReward_fee_and_penalizeFee_test, handles_fee_gt_block_reward_and_penalize_fee) {
    uint64_t fee = 2 * expectedBlockReward;
    do_test(0, fee);

    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(expectedBlockReward + fee - fee * testPenalty / 100, m_blockReward);
    ASSERT_EQ(expectedBlockReward - fee * testPenalty / 100, m_emissionChange);
    ASSERT_LT(m_emissionChange, 0);
  }

  TEST_F(getBlockReward_fee_and_penalizeFee_test, handles_emission_change_eq_zero) {
    uint64_t fee = expectedBlockReward * 100 / testPenalty;
    do_test(0, fee);

    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(expectedBlockReward + fee - fee * testPenalty / 100, m_blockReward);
    ASSERT_EQ(0, m_emissionChange);
  }

  TEST_F(getBlockReward_fee_and_penalizeFee_test, handles_fee_if_block_reward_is_zero_and_penalize_fee) {
    uint64_t fee = UINT64_C(100);
    do_test(m_currency.moneySupply(), fee);

    ASSERT_FALSE(m_blockTooBig);
    ASSERT_EQ(fee - fee * testPenalty / 100, m_blockReward);
    ASSERT_EQ(-static_cast<int64_t>(fee * testPenalty / 100), m_emissionChange);
  }
}
