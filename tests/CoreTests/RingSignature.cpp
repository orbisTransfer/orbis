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
#include "RingSignature.h"

using namespace Orbis;


////////
// class gen_ring_signature_1;

gen_ring_signature_1::gen_ring_signature_1()
{
  REGISTER_CALLBACK("check_balances_1", gen_ring_signature_1::check_balances_1);
  REGISTER_CALLBACK("check_balances_2", gen_ring_signature_1::check_balances_2);
}

namespace
{
  // To be sure that miner tx outputs don't match any bob_account and some_accounts inputs
  const uint64_t rnd_11 = 475921;
  const uint64_t rnd_20 = 360934;
  const uint64_t rnd_29 = 799665;
}

bool gen_ring_signature_1::generate(std::vector<test_event_entry>& events) const
{
  uint64_t ts_start = 1338224400;

  GENERATE_ACCOUNT(miner_account);

  //                                                                                                   events
  MAKE_GENESIS_BLOCK(events, blk_0, miner_account, ts_start);                                          //  0
  MAKE_ACCOUNT(events, some_account_1);                                                                //  1
  MAKE_ACCOUNT(events, some_account_2);                                                                //  2
  MAKE_ACCOUNT(events, bob_account);                                                                   //  3
  MAKE_ACCOUNT(events, alice_account);                                                                 //  4
  MAKE_NEXT_BLOCK(events, blk_1, blk_0, miner_account);                                                //  5
  MAKE_NEXT_BLOCK(events, blk_2, blk_1, miner_account);                                                //  6
  MAKE_NEXT_BLOCK(events, blk_3, blk_2, miner_account);                                                //  7
  MAKE_NEXT_BLOCK(events, blk_4, blk_3, miner_account);                                                //  8
  REWIND_BLOCKS(events, blk_5, blk_4, miner_account);                                                  // <N blocks>
  REWIND_BLOCKS(events, blk_5r, blk_5, miner_account);                                                 // <N blocks>
  MAKE_TX_LIST_START(events, txs_blk_6, miner_account, bob_account, MK_COINS(1), blk_5);               //  9 + 2N
  MAKE_TX_LIST(events, txs_blk_6, miner_account, bob_account, MK_COINS(11) + rnd_11, blk_5);           // 10 + 2N
  MAKE_TX_LIST(events, txs_blk_6, miner_account, bob_account, MK_COINS(11) + rnd_11, blk_5);           // 11 + 2N
  MAKE_TX_LIST(events, txs_blk_6, miner_account, bob_account, MK_COINS(20) + rnd_20, blk_5);           // 12 + 2N
  MAKE_TX_LIST(events, txs_blk_6, miner_account, bob_account, MK_COINS(29) + rnd_29, blk_5);           // 13 + 2N
  MAKE_TX_LIST(events, txs_blk_6, miner_account, bob_account, MK_COINS(29) + rnd_29, blk_5);           // 14 + 2N
  MAKE_TX_LIST(events, txs_blk_6, miner_account, bob_account, MK_COINS(29) + rnd_29, blk_5);           // 15 + 2N
  MAKE_TX_LIST(events, txs_blk_6, miner_account, some_account_1, MK_COINS(11) + rnd_11, blk_5);        // 16 + 2N
  MAKE_TX_LIST(events, txs_blk_6, miner_account, some_account_1, MK_COINS(11) + rnd_11, blk_5);        // 17 + 2N
  MAKE_TX_LIST(events, txs_blk_6, miner_account, some_account_1, MK_COINS(11) + rnd_11, blk_5);        // 18 + 2N
  MAKE_TX_LIST(events, txs_blk_6, miner_account, some_account_1, MK_COINS(11) + rnd_11, blk_5);        // 19 + 2N
  MAKE_TX_LIST(events, txs_blk_6, miner_account, some_account_1, MK_COINS(20) + rnd_20, blk_5);        // 20 + 2N
  MAKE_TX_LIST(events, txs_blk_6, miner_account, some_account_2, MK_COINS(20) + rnd_20, blk_5);        // 21 + 2N
  MAKE_NEXT_BLOCK_TX_LIST(events, blk_6, blk_5r, miner_account, txs_blk_6);                            // 22 + 2N
  DO_CALLBACK(events, "check_balances_1");                                                             // 23 + 2N
  REWIND_BLOCKS(events, blk_6r, blk_6, miner_account);                                                 // <N blocks>
  // 129 = 11 + 11 + 20 + 29 + 29 + 29
  MAKE_TX_MIX(events, tx_0, bob_account, alice_account, MK_COINS(129) + 2 * rnd_11 + rnd_20 + 3 * rnd_29 - m_currency.minimumFee(), 2, blk_6);  // 24 + 3N
  MAKE_NEXT_BLOCK_TX1(events, blk_7, blk_6r, miner_account, tx_0);                                     // 25 + 3N
  DO_CALLBACK(events, "check_balances_2");                                                             // 26 + 3N

  return true;
}

bool gen_ring_signature_1::check_balances_1(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events)
{
  DEFINE_TESTS_ERROR_CONTEXT("gen_ring_signature_1::check_balances_1");

  m_bob_account = boost::get<AccountBase>(events[3]);
  m_alice_account = boost::get<AccountBase>(events[4]);

  std::list<Block> blocks;
  bool r = c.get_blocks(0, 100 + 2 * m_currency.minedMoneyUnlockWindow(), blocks);
  CHECK_TEST_CONDITION(r);

  std::vector<Orbis::Block> chain;
  map_hash2tx_t mtx;
  r = find_block_chain(events, chain, mtx, get_block_hash(blocks.back()));
  CHECK_TEST_CONDITION(r);
  CHECK_EQ(MK_COINS(130) + 2 * rnd_11 + rnd_20 + 3 * rnd_29, get_balance(m_bob_account, chain, mtx));
  CHECK_EQ(0, get_balance(m_alice_account, chain, mtx));

  return true;
}

bool gen_ring_signature_1::check_balances_2(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events)
{
  DEFINE_TESTS_ERROR_CONTEXT("gen_ring_signature_1::check_balances_2");

  std::list<Block> blocks;
  bool r = c.get_blocks(0, 100 + 2 * m_currency.minedMoneyUnlockWindow(), blocks);
  CHECK_TEST_CONDITION(r);

  std::vector<Orbis::Block> chain;
  map_hash2tx_t mtx;
  r = find_block_chain(events, chain, mtx, get_block_hash(blocks.back()));
  CHECK_TEST_CONDITION(r);
  CHECK_EQ(MK_COINS(1), get_balance(m_bob_account, chain, mtx));
  CHECK_EQ(MK_COINS(129) + 2 * rnd_11 + rnd_20 + 3 * rnd_29 - m_currency.minimumFee(), get_balance(m_alice_account, chain, mtx));

  return true;
}


////////
// class gen_ring_signature_2;

gen_ring_signature_2::gen_ring_signature_2()
{
  REGISTER_CALLBACK("check_balances_1", gen_ring_signature_2::check_balances_1);
  REGISTER_CALLBACK("check_balances_2", gen_ring_signature_2::check_balances_2);
}

/**
 * Bob has 4 inputs by 61 coins. He sends 4 * 61 coins to Alice, using ring signature with nmix = 3. Each Bob's input
 * is used as mix for 3 others.
 */
bool gen_ring_signature_2::generate(std::vector<test_event_entry>& events) const
{
  uint64_t ts_start = 1338224400;

  GENERATE_ACCOUNT(miner_account);

  //                                                                                                    events
  MAKE_GENESIS_BLOCK(events, blk_0, miner_account, ts_start);                                           //  0
  MAKE_ACCOUNT(events, bob_account);                                                                    //  1
  MAKE_ACCOUNT(events, alice_account);                                                                  //  2
  MAKE_NEXT_BLOCK(events, blk_1, blk_0, miner_account);                                                 //  3
  MAKE_NEXT_BLOCK(events, blk_2, blk_1, miner_account);                                                 //  4
  MAKE_NEXT_BLOCK(events, blk_3, blk_2, miner_account);                                                 //  5
  REWIND_BLOCKS(events, blk_3r, blk_3, miner_account);                                                  // <N blocks>
  MAKE_TX_LIST_START(events, txs_blk_4, miner_account, bob_account, MK_COINS(61), blk_3);               //  6 + N
  MAKE_TX_LIST(events, txs_blk_4, miner_account, bob_account, MK_COINS(61), blk_3);                     //  7 + N
  MAKE_TX_LIST(events, txs_blk_4, miner_account, bob_account, MK_COINS(61), blk_3);                     //  8 + N
  MAKE_TX_LIST(events, txs_blk_4, miner_account, bob_account, MK_COINS(61), blk_3);                     //  9 + N
  MAKE_NEXT_BLOCK_TX_LIST(events, blk_4, blk_3r, miner_account, txs_blk_4);                             // 10 + N
  DO_CALLBACK(events, "check_balances_1");                                                              // 11 + N
  REWIND_BLOCKS(events, blk_4r, blk_4, miner_account);                                                  // <N blocks>
  MAKE_TX_MIX(events, tx_0, bob_account, alice_account, MK_COINS(244) - m_currency.minimumFee(), 3, blk_4);   // 12 + 2N
  MAKE_NEXT_BLOCK_TX1(events, blk_5, blk_4r, miner_account, tx_0);                                      // 13 + 2N
  DO_CALLBACK(events, "check_balances_2");                                                              // 14 + 2N

  return true;
}

bool gen_ring_signature_2::check_balances_1(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events)
{
  DEFINE_TESTS_ERROR_CONTEXT("gen_ring_signature_2::check_balances_1");

  m_bob_account = boost::get<AccountBase>(events[1]);
  m_alice_account = boost::get<AccountBase>(events[2]);

  std::list<Block> blocks;
  bool r = c.get_blocks(0, 100 + 2 * m_currency.minedMoneyUnlockWindow(), blocks);
  CHECK_TEST_CONDITION(r);

  std::vector<Orbis::Block> chain;
  map_hash2tx_t mtx;
  r = find_block_chain(events, chain, mtx, get_block_hash(blocks.back()));
  CHECK_TEST_CONDITION(r);
  CHECK_EQ(MK_COINS(244), get_balance(m_bob_account, chain, mtx));
  CHECK_EQ(0, get_balance(m_alice_account, chain, mtx));

  return true;
}

bool gen_ring_signature_2::check_balances_2(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events)
{
  DEFINE_TESTS_ERROR_CONTEXT("gen_ring_signature_2::check_balances_2");

  std::list<Block> blocks;
  bool r = c.get_blocks(0, 100 + 2 * m_currency.minedMoneyUnlockWindow(), blocks);
  CHECK_TEST_CONDITION(r);

  std::vector<Orbis::Block> chain;
  map_hash2tx_t mtx;
  r = find_block_chain(events, chain, mtx, get_block_hash(blocks.back()));
  CHECK_TEST_CONDITION(r);
  CHECK_EQ(0, get_balance(m_bob_account, chain, mtx));
  CHECK_EQ(MK_COINS(244) - m_currency.minimumFee(), get_balance(m_alice_account, chain, mtx));

  return true;
}


////////
// class gen_ring_signature_big;

gen_ring_signature_big::gen_ring_signature_big()
  : m_test_size(100)
  , m_tx_amount(MK_COINS(29))
{
  REGISTER_CALLBACK("check_balances_1", gen_ring_signature_big::check_balances_1);
  REGISTER_CALLBACK("check_balances_2", gen_ring_signature_big::check_balances_2);
}

/**
 * Check ring signature with m_test_size-1 sources.
 * - Create 100 accounts.
 * - Create 100 blocks, each block contains transaction from the miner to account[i].
 * - Create transaction with ring signature from account[99] to Alice with nmix = 99.
 * - Check balances.
 */
bool gen_ring_signature_big::generate(std::vector<test_event_entry>& events) const
{
  std::vector<AccountBase> accounts(m_test_size);
  std::vector<Block> blocks;
  blocks.reserve(m_test_size + m_currency.minedMoneyUnlockWindow());

  uint64_t ts_start = 1338224400;
  GENERATE_ACCOUNT(miner_account);

  MAKE_GENESIS_BLOCK(events, blk_0, miner_account, ts_start);

  for (size_t i = 0; i < m_test_size; ++i)
  {
    MAKE_ACCOUNT(events, an_account);
    accounts[i] = an_account;
  }
  MAKE_ACCOUNT(events, alice_account);

  size_t blk_0r_idx = events.size();
  REWIND_BLOCKS(events, blk_0r, blk_0, miner_account);
  blocks.push_back(blk_0);
  for (size_t i = blk_0r_idx; i < events.size(); ++i)
  {
    blocks.push_back(boost::get<Block>(events[i]));
  }

  for (size_t i = 0; i < m_test_size; ++i)
  {
    Block blk_with_unlocked_out = blocks[blocks.size() - 1 - m_currency.minedMoneyUnlockWindow()];
    MAKE_TX_LIST_START(events, txs_blk_i, miner_account, accounts[i], m_tx_amount, blk_with_unlocked_out);
    for (size_t j = 0; j <= i; ++j)
    {
      MAKE_TX_LIST(events, txs_blk_i, miner_account, accounts[i], m_currency.minimumFee(), blk_with_unlocked_out);
    }
    MAKE_NEXT_BLOCK_TX_LIST(events, blk_i, blocks.back(), miner_account, txs_blk_i);
    blocks.push_back(blk_i);

    std::vector<Orbis::Block> chain;
    map_hash2tx_t mtx;
    bool r = find_block_chain(events, chain, mtx, get_block_hash(blk_i));
    CHECK_AND_NO_ASSERT_MES(r, false, "failed to call find_block_chain");
    std::cout << i << ": " << get_balance(accounts[i], chain, mtx) << std::endl;
  }

  DO_CALLBACK(events, "check_balances_1");
  MAKE_TX_MIX(events, tx_0, accounts[0], alice_account, m_tx_amount, m_test_size - 1, blocks.back());
  MAKE_NEXT_BLOCK_TX1(events, blk_1, blocks.back(), miner_account, tx_0);
  DO_CALLBACK(events, "check_balances_2");

  return true;
}

bool gen_ring_signature_big::check_balances_1(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events)
{
  DEFINE_TESTS_ERROR_CONTEXT("gen_ring_signature_big::check_balances_1");

  m_bob_account = boost::get<AccountBase>(events[1]);
  m_alice_account = boost::get<AccountBase>(events[1 + m_test_size]);

  std::list<Block> blocks;
  bool r = c.get_blocks(0, 2 * m_test_size + m_currency.minedMoneyUnlockWindow(), blocks);
  CHECK_TEST_CONDITION(r);

  std::vector<Orbis::Block> chain;
  map_hash2tx_t mtx;
  r = find_block_chain(events, chain, mtx, get_block_hash(blocks.back()));
  CHECK_TEST_CONDITION(r);
  CHECK_EQ(m_tx_amount + m_currency.minimumFee(), get_balance(m_bob_account, chain, mtx));
  CHECK_EQ(0, get_balance(m_alice_account, chain, mtx));

  for (size_t i = 2; i < 1 + m_test_size; ++i)
  {
    const AccountBase& an_account = boost::get<AccountBase>(events[i]);
    uint64_t balance = m_tx_amount + m_currency.minimumFee() * i;
    CHECK_EQ(balance, get_balance(an_account, chain, mtx));
  }

  return true;
}

bool gen_ring_signature_big::check_balances_2(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events)
{
  DEFINE_TESTS_ERROR_CONTEXT("gen_ring_signature_big::check_balances_2");

  std::list<Block> blocks;
  bool r = c.get_blocks(0, 2 * m_test_size + m_currency.minedMoneyUnlockWindow(), blocks);
  CHECK_TEST_CONDITION(r);

  std::vector<Orbis::Block> chain;
  map_hash2tx_t mtx;
  r = find_block_chain(events, chain, mtx, get_block_hash(blocks.back()));
  CHECK_TEST_CONDITION(r);
  CHECK_EQ(0, get_balance(m_bob_account, chain, mtx));
  CHECK_EQ(m_tx_amount, get_balance(m_alice_account, chain, mtx));

  for (size_t i = 2; i < 1 + m_test_size; ++i)
  {
    const AccountBase& an_account = boost::get<AccountBase>(events[i]);
    uint64_t balance = m_tx_amount + m_currency.minimumFee() * i;
    CHECK_EQ(balance, get_balance(an_account, chain, mtx));
  }

  std::vector<size_t> tx_outs;
  uint64_t transfered;
  lookup_acc_outs(m_alice_account.getAccountKeys(), boost::get<Transaction>(events[events.size() - 3]), getTransactionPublicKeyFromExtra(boost::get<Transaction>(events[events.size() - 3]).extra), tx_outs, transfered);
  CHECK_EQ(m_tx_amount, transfered);

  return true;
}
