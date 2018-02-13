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
#include "IntegerOverflow.h"

using namespace Orbis;

namespace
{
  void split_miner_tx_outs(Transaction& miner_tx, uint64_t amount_1)
  {
    uint64_t total_amount = get_outs_money_amount(miner_tx);
    uint64_t amount_2 = total_amount - amount_1;
    TransactionOutputTarget target = miner_tx.outputs[0].target;

    miner_tx.outputs.clear();

    TransactionOutput out1;
    out1.amount = amount_1;
    out1.target = target;
    miner_tx.outputs.push_back(out1);

    TransactionOutput out2;
    out2.amount = amount_2;
    out2.target = target;
    miner_tx.outputs.push_back(out2);
  }

  void append_TransactionSourceEntry(std::vector<Orbis::TransactionSourceEntry>& sources, const Transaction& tx, size_t out_idx)
  {
    Orbis::TransactionSourceEntry se;
    se.amount = tx.outputs[out_idx].amount;
    se.outputs.push_back(std::make_pair(0, boost::get<Orbis::KeyOutput>(tx.outputs[out_idx].target).key));
    se.realOutput = 0;
    se.realTransactionPublicKey = getTransactionPublicKeyFromExtra(tx.extra);
    se.realOutputIndexInTransaction = out_idx;

    sources.push_back(se);
  }
}

//======================================================================================================================

gen_uint_overflow_base::gen_uint_overflow_base()
  : m_last_valid_block_event_idx(static_cast<size_t>(-1))
{
  REGISTER_CALLBACK_METHOD(gen_uint_overflow_1, mark_last_valid_block);
}

bool gen_uint_overflow_base::check_tx_verification_context(const Orbis::tx_verification_context& tvc, bool tx_added, size_t event_idx, const Orbis::Transaction& /*tx*/)
{
  return m_last_valid_block_event_idx < event_idx ? !tx_added && tvc.m_verifivation_failed : tx_added && !tvc.m_verifivation_failed;
}

bool gen_uint_overflow_base::check_block_verification_context(const Orbis::block_verification_context& bvc, size_t event_idx, const Orbis::Block& /*block*/)
{
  return m_last_valid_block_event_idx < event_idx ? bvc.m_verifivation_failed | bvc.m_marked_as_orphaned : !bvc.m_verifivation_failed;
}

bool gen_uint_overflow_base::mark_last_valid_block(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events)
{
  m_last_valid_block_event_idx = ev_index - 1;
  return true;
}

//======================================================================================================================

bool gen_uint_overflow_1::generate(std::vector<test_event_entry>& events) const
{
  uint64_t ts_start = 1338224400;

  GENERATE_ACCOUNT(miner_account);
  MAKE_GENESIS_BLOCK(events, blk_0, miner_account, ts_start);
  DO_CALLBACK(events, "mark_last_valid_block");
  MAKE_ACCOUNT(events, bob_account);
  MAKE_ACCOUNT(events, alice_account);

  // Problem 1. Miner tx output overflow
  MAKE_MINER_TX_MANUALLY(miner_tx_0, blk_0);
  split_miner_tx_outs(miner_tx_0, m_currency.moneySupply());
  Block blk_1;
  if (!generator.constructBlockManually(blk_1, blk_0, miner_account, test_generator::bf_miner_tx, 0, 0, 0, Crypto::Hash(), 0, miner_tx_0))
    return false;
  events.push_back(blk_1);

  // Problem 1. Miner tx outputs overflow
  MAKE_MINER_TX_MANUALLY(miner_tx_1, blk_1);
  split_miner_tx_outs(miner_tx_1, m_currency.moneySupply());
  Block blk_2;
  if (!generator.constructBlockManually(blk_2, blk_1, miner_account, test_generator::bf_miner_tx, 0, 0, 0, Crypto::Hash(), 0, miner_tx_1))
    return false;
  events.push_back(blk_2);

  REWIND_BLOCKS(events, blk_2r, blk_2, miner_account);
  MAKE_TX_LIST_START(events, txs_0, miner_account, bob_account, m_currency.moneySupply(), blk_2);
  MAKE_TX_LIST(events, txs_0, miner_account, bob_account, m_currency.moneySupply(), blk_2);
  MAKE_NEXT_BLOCK_TX_LIST(events, blk_3, blk_2r, miner_account, txs_0);
  REWIND_BLOCKS(events, blk_3r, blk_3, miner_account);

  // Problem 2. total_fee overflow, block_reward overflow
  std::list<Orbis::Transaction> txs_1;
  // Create txs with huge fee
  txs_1.push_back(construct_tx_with_fee(m_logger, events, blk_3, bob_account, alice_account, MK_COINS(1), m_currency.moneySupply() - MK_COINS(1)));
  txs_1.push_back(construct_tx_with_fee(m_logger, events, blk_3, bob_account, alice_account, MK_COINS(1), m_currency.moneySupply() - MK_COINS(1)));
  MAKE_NEXT_BLOCK_TX_LIST(events, blk_4, blk_3r, miner_account, txs_1);

  return true;
}

//======================================================================================================================

bool gen_uint_overflow_2::generate(std::vector<test_event_entry>& events) const
{
  uint64_t ts_start = 1338224400;

  GENERATE_ACCOUNT(miner_account);
  MAKE_GENESIS_BLOCK(events, blk_0, miner_account, ts_start);
  MAKE_ACCOUNT(events, bob_account);
  MAKE_ACCOUNT(events, alice_account);
  REWIND_BLOCKS(events, blk_0r, blk_0, miner_account);
  DO_CALLBACK(events, "mark_last_valid_block");

  // Problem 1. Regular tx outputs overflow
  std::vector<Orbis::TransactionSourceEntry> sources;
  for (size_t i = 0; i < blk_0.baseTransaction.outputs.size(); ++i)
  {
    if (m_currency.minimumFee() < blk_0.baseTransaction.outputs[i].amount)
    {
      append_TransactionSourceEntry(sources, blk_0.baseTransaction, i);
      break;
    }
  }
  if (sources.empty())
  {
    return false;
  }

  std::vector<Orbis::TransactionDestinationEntry> destinations;
  const AccountPublicAddress& bob_addr = bob_account.getAccountKeys().address;
  destinations.push_back(TransactionDestinationEntry(m_currency.moneySupply(), bob_addr));
  destinations.push_back(TransactionDestinationEntry(m_currency.moneySupply() - 1, bob_addr));
  // sources.front().amount = destinations[0].amount + destinations[2].amount + destinations[3].amount + m_currency.minimumFee()
  destinations.push_back(TransactionDestinationEntry(sources.front().amount - m_currency.moneySupply() - m_currency.moneySupply() + 1 - m_currency.minimumFee(), bob_addr));

  Orbis::Transaction tx_1;
  if (!constructTransaction(miner_account.getAccountKeys(), sources, destinations, std::vector<uint8_t>(), tx_1, 0, m_logger))
    return false;
  events.push_back(tx_1);

  MAKE_NEXT_BLOCK_TX1(events, blk_1, blk_0r, miner_account, tx_1);
  REWIND_BLOCKS(events, blk_1r, blk_1, miner_account);

  // Problem 2. Regular tx inputs overflow
  sources.clear();
  for (size_t i = 0; i < tx_1.outputs.size(); ++i)
  {
    auto& tx_1_out = tx_1.outputs[i];
    if (tx_1_out.amount < m_currency.moneySupply() - 1)
      continue;

    append_TransactionSourceEntry(sources, tx_1, i);
  }

  destinations.clear();
  Orbis::TransactionDestinationEntry de;
  de.addr = alice_account.getAccountKeys().address;
  de.amount = m_currency.moneySupply() - m_currency.minimumFee();
  destinations.push_back(de);
  destinations.push_back(de);

  Orbis::Transaction tx_2;
  if (!constructTransaction(bob_account.getAccountKeys(), sources, destinations, std::vector<uint8_t>(), tx_2, 0, m_logger))
    return false;
  events.push_back(tx_2);

  MAKE_NEXT_BLOCK_TX1(events, blk_2, blk_1r, miner_account, tx_2);

  return true;
}
