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
#include "BlockReward.h"
#include "OrbisCore/OrbisTools.h"
#include <Common/Math.h>

using namespace Orbis;

namespace
{
  bool rewind_blocks(std::vector<test_event_entry>& events, test_generator& generator, Block& blk, const Block& blk_prev,
    const AccountBase& miner_account, size_t block_count)
  {
    blk = blk_prev;
    for (size_t i = 0; i < block_count; ++i)
    {
      Block blk_i;
      if (!generator.constructMaxSizeBlock(blk_i, blk, miner_account))
        return false;

      events.push_back(blk_i);
      blk = blk_i;
    }

    return true;
  }

  uint64_t get_tx_out_amount(const Transaction& tx)
  {
    uint64_t amount = 0;
    BOOST_FOREACH(auto& o, tx.outputs)
      amount += o.amount;
    return amount;
  }
}

gen_block_reward::gen_block_reward()
  : m_invalid_block_index(0) {
  Orbis::CurrencyBuilder currencyBuilder(m_logger);
  currencyBuilder.maxBlockSizeInitial(std::numeric_limits<size_t>::max() / 2);
  m_currency = currencyBuilder.currency();

  REGISTER_CALLBACK_METHOD(gen_block_reward, mark_invalid_block);
  REGISTER_CALLBACK_METHOD(gen_block_reward, mark_checked_block);
  REGISTER_CALLBACK_METHOD(gen_block_reward, check_block_rewards);
}

bool gen_block_reward::generate(std::vector<test_event_entry>& events) const
{
  uint64_t ts_start = 1338224400;

  GENERATE_ACCOUNT(miner_account);
  MAKE_GENESIS_BLOCK(events, blk_0, miner_account, ts_start);
  DO_CALLBACK(events, "mark_checked_block");
  MAKE_ACCOUNT(events, bob_account);

  // Test case 1: miner transactions without outputs (block reward == 0)
  Block blk_0r;
  if (!rewind_blocks(events, generator, blk_0r, blk_0, miner_account, m_currency.rewardBlocksWindow())) {
    return false;
  }

  // Test: block reward is calculated using median of the latest m_currency.rewardBlocksWindow() blocks
  DO_CALLBACK(events, "mark_invalid_block");
  Block blk_1_bad_1;
  if (!generator.constructMaxSizeBlock(blk_1_bad_1, blk_0r, miner_account, m_currency.rewardBlocksWindow() + 1)) {
    return false;
  }
  events.push_back(blk_1_bad_1);

  DO_CALLBACK(events, "mark_invalid_block");
  Block blk_1_bad_2;
  if (!generator.constructMaxSizeBlock(blk_1_bad_2, blk_0r, miner_account, m_currency.rewardBlocksWindow() - 1)) {
    return false;
  }
  events.push_back(blk_1_bad_2);

  // Test 1.2: miner transactions without outputs (block reward == 0)
  Block blk_1;
  if (!generator.constructMaxSizeBlock(blk_1, blk_0r, miner_account)) {
    return false;
  }
  events.push_back(blk_1);
  // End of Test case 1

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  MAKE_NEXT_BLOCK(events, blk_2, blk_1, miner_account);
  DO_CALLBACK(events, "mark_checked_block");
  MAKE_NEXT_BLOCK(events, blk_3, blk_2, miner_account);
  DO_CALLBACK(events, "mark_checked_block");
  MAKE_NEXT_BLOCK(events, blk_4, blk_3, miner_account);
  DO_CALLBACK(events, "mark_checked_block");
  MAKE_NEXT_BLOCK(events, blk_5, blk_4, miner_account);
  DO_CALLBACK(events, "mark_checked_block");

  Block blk_5r;
  if (!rewind_blocks(events, generator, blk_5r, blk_5, miner_account, m_currency.minedMoneyUnlockWindow()))
    return false;

  // Test: fee increases block reward
  Transaction tx_0(construct_tx_with_fee(m_logger, events, blk_5, miner_account, bob_account, MK_COINS(1), 3 * m_currency.minimumFee()));
  MAKE_NEXT_BLOCK_TX1(events, blk_6, blk_5r, miner_account, tx_0);
  DO_CALLBACK(events, "mark_checked_block");

  // Test: fee from all block transactions increase block reward
  std::list<Transaction> txs_0;
  txs_0.push_back(construct_tx_with_fee(m_logger, events, blk_5, miner_account, bob_account, MK_COINS(1), 5 * m_currency.minimumFee()));
  txs_0.push_back(construct_tx_with_fee(m_logger, events, blk_5, miner_account, bob_account, MK_COINS(1), 7 * m_currency.minimumFee()));
  MAKE_NEXT_BLOCK_TX_LIST(events, blk_7, blk_6, miner_account, txs_0);
  DO_CALLBACK(events, "mark_checked_block");

  // Test: block reward == 0
  {
    Transaction tx_1 = construct_tx_with_fee(m_logger, events, blk_5, miner_account, bob_account, MK_COINS(1), 11 * m_currency.minimumFee());
    Transaction tx_2 = construct_tx_with_fee(m_logger, events, blk_5, miner_account, bob_account, MK_COINS(1), 13 * m_currency.minimumFee());
    size_t txs_1_size = getObjectBinarySize(tx_1) + getObjectBinarySize(tx_2);
    uint64_t txs_fee = get_tx_fee(tx_1) + get_tx_fee(tx_2);

    std::vector<size_t> block_sizes;
    generator.getLastNBlockSizes(block_sizes, get_block_hash(blk_7), m_currency.rewardBlocksWindow());
    size_t median = Common::medianValue(block_sizes);

    Transaction miner_tx;
    bool r = constructMinerTxBySize(m_currency, miner_tx, get_block_height(blk_7) + 1, generator.getAlreadyGeneratedCoins(blk_7),
      miner_account.getAccountKeys().address, block_sizes, 2 * median - txs_1_size, 2 * median, txs_fee);
    if (!r)
      return false;

    std::vector<Crypto::Hash> txs_1_hashes;
    txs_1_hashes.push_back(getObjectHash(tx_1));
    txs_1_hashes.push_back(getObjectHash(tx_2));

    Block blk_8;
    generator.constructBlockManually(blk_8, blk_7, miner_account, test_generator::bf_miner_tx | test_generator::bf_tx_hashes,
      0, 0, 0, Crypto::Hash(), 0, miner_tx, txs_1_hashes, txs_1_size, txs_fee);

    events.push_back(blk_8);
    DO_CALLBACK(events, "mark_checked_block");
  }

  DO_CALLBACK(events, "check_block_rewards");

  return true;
}

bool gen_block_reward::check_block_verification_context(const Orbis::block_verification_context& bvc, size_t event_idx, const Orbis::Block& /*blk*/)
{
  if (m_invalid_block_index == event_idx)
  {
    m_invalid_block_index = 0;
    return bvc.m_verifivation_failed;
  }
  else
  {
    return !bvc.m_verifivation_failed;
  }
}

bool gen_block_reward::mark_invalid_block(Orbis::core& /*c*/, size_t ev_index, const std::vector<test_event_entry>& /*events*/)
{
  m_invalid_block_index = ev_index + 1;
  return true;
}

bool gen_block_reward::mark_checked_block(Orbis::core& /*c*/, size_t ev_index, const std::vector<test_event_entry>& /*events*/)
{
  m_checked_blocks_indices.push_back(ev_index - 1);
  return true;
}

bool gen_block_reward::check_block_rewards(Orbis::core& /*c*/, size_t /*ev_index*/, const std::vector<test_event_entry>& events)
{
  DEFINE_TESTS_ERROR_CONTEXT("gen_block_reward_without_txs::check_block_rewards");

  std::array<uint64_t, 7> blk_rewards;
  blk_rewards[0] = m_currency.moneySupply() >> m_currency.emissionSpeedFactor();
  uint64_t cumulative_reward = blk_rewards[0];
  for (size_t i = 1; i < blk_rewards.size(); ++i)
  {
    blk_rewards[i] = (m_currency.moneySupply() - cumulative_reward) >> m_currency.emissionSpeedFactor();
    cumulative_reward += blk_rewards[i];
  }

  for (size_t i = 0; i < 5; ++i)
  {
    Block blk_i = boost::get<Block>(events[m_checked_blocks_indices[i]]);
    CHECK_EQ(blk_rewards[i], get_tx_out_amount(blk_i.baseTransaction));
  }

  Block blk_n1 = boost::get<Block>(events[m_checked_blocks_indices[5]]);
  CHECK_EQ(blk_rewards[5] + 3 * m_currency.minimumFee(), get_tx_out_amount(blk_n1.baseTransaction));

  Block blk_n2 = boost::get<Block>(events[m_checked_blocks_indices[6]]);
  CHECK_EQ(blk_rewards[6] + (5 + 7) * m_currency.minimumFee(), get_tx_out_amount(blk_n2.baseTransaction));

  Block blk_n3 = boost::get<Block>(events[m_checked_blocks_indices[7]]);
  CHECK_EQ(0, get_tx_out_amount(blk_n3.baseTransaction));

  return true;
}
