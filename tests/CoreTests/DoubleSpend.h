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
#pragma once 
#include "Chaingen.h"
#include "TransactionBuilder.h"

const size_t invalid_index_value = std::numeric_limits<size_t>::max();


template<class concrete_test>
class gen_double_spend_base : public test_chain_unit_base
{
public:
  static const uint64_t send_amount = MK_COINS(17);

  gen_double_spend_base();

  bool check_tx_verification_context(const Orbis::tx_verification_context& tvc, bool tx_added, size_t event_idx, const Orbis::Transaction& tx);
  bool check_block_verification_context(const Orbis::block_verification_context& bvc, size_t event_idx, const Orbis::Block& block);

  bool mark_last_valid_block(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events);
  bool mark_invalid_tx(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events);
  bool mark_invalid_block(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events);
  bool check_double_spend(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events);

private:
  Orbis::Block m_last_valid_block;
  size_t m_invalid_tx_index;
  size_t m_invalid_block_index;
};


template<bool txs_keeped_by_block>
struct gen_double_spend_in_tx : public gen_double_spend_base< gen_double_spend_in_tx<txs_keeped_by_block> >
{
  static const uint64_t send_amount = MK_COINS(17);
  static const bool has_invalid_tx = true;
  static const size_t expected_pool_txs_count = 0;
  static const uint64_t expected_bob_balance = send_amount;
  static const uint64_t expected_alice_balance = 0;

  bool generate(std::vector<test_event_entry>& events) const;
};


template<bool txs_keeped_by_block>
struct gen_double_spend_in_the_same_block : public gen_double_spend_base< gen_double_spend_in_the_same_block<txs_keeped_by_block> >
{
  static const uint64_t send_amount = MK_COINS(17);
  static const bool has_invalid_tx = !txs_keeped_by_block;
  static const size_t expected_pool_txs_count = has_invalid_tx ? 1 : 2;
  static const uint64_t expected_bob_balance = send_amount;
  static const uint64_t expected_alice_balance = 0;

  bool generate(std::vector<test_event_entry>& events) const;
};


template<bool txs_keeped_by_block>
struct gen_double_spend_in_different_blocks : public gen_double_spend_base< gen_double_spend_in_different_blocks<txs_keeped_by_block> >
{
  static const uint64_t send_amount = MK_COINS(17);
  static const bool has_invalid_tx = !txs_keeped_by_block;
  static const size_t expected_pool_txs_count = has_invalid_tx ? 0 : 1;
  static const uint64_t expected_bob_balance = 0;
  static uint64_t expected_alice_balance;

  gen_double_spend_in_different_blocks() :
    gen_double_spend_base< gen_double_spend_in_different_blocks<txs_keeped_by_block> >() {
    expected_alice_balance = send_amount - this->m_currency.minimumFee();
  }

  bool generate(std::vector<test_event_entry>& events) const;
};

template<bool txs_keeped_by_block>
uint64_t gen_double_spend_in_different_blocks<txs_keeped_by_block>::expected_alice_balance;


template<bool txs_keeped_by_block>
struct gen_double_spend_in_alt_chain_in_the_same_block : public gen_double_spend_base< gen_double_spend_in_alt_chain_in_the_same_block<txs_keeped_by_block> >
{
  static const uint64_t send_amount = MK_COINS(17);
  static const bool has_invalid_tx = !txs_keeped_by_block;
  static const size_t expected_pool_txs_count = has_invalid_tx ? 1 : 2;
  static const uint64_t expected_bob_balance = send_amount;
  static const uint64_t expected_alice_balance = 0;

  bool generate(std::vector<test_event_entry>& events) const;
};


template<bool txs_keeped_by_block>
struct gen_double_spend_in_alt_chain_in_different_blocks : public gen_double_spend_base< gen_double_spend_in_alt_chain_in_different_blocks<txs_keeped_by_block> >
{
  static const uint64_t send_amount = MK_COINS(17);
  static const bool has_invalid_tx = !txs_keeped_by_block;
  static const size_t expected_pool_txs_count = has_invalid_tx ? 1 : 2;
  static const uint64_t expected_bob_balance = send_amount;
  static const uint64_t expected_alice_balance = 0;

  bool generate(std::vector<test_event_entry>& events) const;
};


class gen_double_spend_in_different_chains : public test_chain_unit_base
{
public:
  static const uint64_t send_amount = MK_COINS(31);
  size_t expected_blockchain_height;

  gen_double_spend_in_different_chains();

  bool generate(std::vector<test_event_entry>& events) const;

  bool check_double_spend(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events);
};


class TestGenerator;

class DoubleSpendBase : public test_chain_unit_base
{
public:
  
  // parameters to be checked
  uint64_t send_amount;
  bool has_invalid_tx;

  DoubleSpendBase();

  bool check_tx_verification_context(const Orbis::tx_verification_context& tvc, bool tx_added, size_t event_idx, const Orbis::Transaction& tx);
  bool check_block_verification_context(const Orbis::block_verification_context& bvc, size_t event_idx, const Orbis::Block& block);

  bool mark_last_valid_block(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events);
  bool mark_invalid_tx(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events);
  bool mark_invalid_block(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events);
  bool check_double_spend(Orbis::core& c, size_t ev_index, const std::vector<test_event_entry>& events);

  TestGenerator prepare(std::vector<test_event_entry>& events) const;
  TransactionBuilder createBobToAliceTx() const;
  TransactionBuilder::MultisignatureSource createSource() const;

protected:

  Orbis::AccountBase m_bob_account;
  Orbis::AccountBase m_alice_account;
  Orbis::KeyPair m_outputTxKey;

private:

  Crypto::Hash m_last_valid_block;
  size_t m_invalid_tx_index;
  size_t m_invalid_block_index;
};


struct MultiSigTx_DoubleSpendInTx : public DoubleSpendBase
{ 
  const bool m_txsKeepedByBlock;

  MultiSigTx_DoubleSpendInTx(bool txsKeepedByBlock);

  bool generate(std::vector<test_event_entry>& events) const;
};

struct MultiSigTx_DoubleSpendSameBlock : public DoubleSpendBase
{
  const bool m_txsKeepedByBlock;

  MultiSigTx_DoubleSpendSameBlock(bool txsKeepedByBlock);

  bool generate(std::vector<test_event_entry>& events) const;
};


struct MultiSigTx_DoubleSpendDifferentBlocks : public DoubleSpendBase
{
  const bool m_txsKeepedByBlock;

  MultiSigTx_DoubleSpendDifferentBlocks(bool txsKeepedByBlock);

  bool generate(std::vector<test_event_entry>& events) const;
};

struct MultiSigTx_DoubleSpendAltChainSameBlock : public DoubleSpendBase
{
  const bool m_txsKeepedByBlock;

  MultiSigTx_DoubleSpendAltChainSameBlock(bool txsKeepedByBlock);

  bool check_tx_verification_context(const Orbis::tx_verification_context& tvc, bool tx_added, size_t event_idx, const Orbis::Transaction& tx) {
    return true;
  }

  bool check_block_verification_context(const Orbis::block_verification_context& bvc, size_t event_idx, const Orbis::Block& block) {
    return true;
  }

  bool generate(std::vector<test_event_entry>& events) const;
};

struct MultiSigTx_DoubleSpendAltChainDifferentBlocks : public DoubleSpendBase
{
  const bool m_txsKeepedByBlock;
  MultiSigTx_DoubleSpendAltChainDifferentBlocks(bool txsKeepedByBlock);
  bool generate(std::vector<test_event_entry>& events) const;
};


#define INIT_DOUBLE_SPEND_TEST()                                           \
  uint64_t ts_start = 1338224400;                                          \
  GENERATE_ACCOUNT(miner_account);                                         \
  MAKE_GENESIS_BLOCK(events, blk_0, miner_account, ts_start);              \
  MAKE_ACCOUNT(events, bob_account);                                       \
  MAKE_ACCOUNT(events, alice_account);                                     \
  REWIND_BLOCKS(events, blk_0r, blk_0, miner_account);                     \
  MAKE_TX(events, tx_0, miner_account, bob_account, send_amount, blk_0);   \
  MAKE_NEXT_BLOCK_TX1(events, blk_1, blk_0r, miner_account, tx_0);         \
  REWIND_BLOCKS(events, blk_1r, blk_1, miner_account);


#include "double_spend.inl"
