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
#include "OrbisCore/OrbisBasicImpl.h"
#include "OrbisCore/Account.h"
#include "OrbisCore/OrbisFormatUtils.h"
#include "OrbisCore/OrbisTools.h"
#include "OrbisCore/Currency.h"

#include <Common/Math.h>

#include "Chaingen.h"

using namespace Orbis;

bool test_transaction_generation_and_ring_signature()
{
  Logging::ConsoleLogger logger;
  Orbis::Currency currency = Orbis::CurrencyBuilder(logger).currency();

  AccountBase miner_acc1;
  miner_acc1.generate();
  AccountBase miner_acc2;
  miner_acc2.generate();
  AccountBase miner_acc3;
  miner_acc3.generate();
  AccountBase miner_acc4;
  miner_acc4.generate();
  AccountBase miner_acc5;
  miner_acc5.generate();
  AccountBase miner_acc6;
  miner_acc6.generate();

  std::string add_str = currency.accountAddressAsString(miner_acc3);

  AccountBase rv_acc;
  rv_acc.generate();
  AccountBase rv_acc2;
  rv_acc2.generate();
  Transaction tx_mine_1;
  currency.constructMinerTx(0, 0, 0, 10, 0, miner_acc1.getAccountKeys().address, tx_mine_1);
  Transaction tx_mine_2;
  currency.constructMinerTx(0, 0, 0, 0, 0, miner_acc2.getAccountKeys().address, tx_mine_2);
  Transaction tx_mine_3;
  currency.constructMinerTx(0, 0, 0, 0, 0, miner_acc3.getAccountKeys().address, tx_mine_3);
  Transaction tx_mine_4;
  currency.constructMinerTx(0, 0, 0, 0, 0, miner_acc4.getAccountKeys().address, tx_mine_4);
  Transaction tx_mine_5;
  currency.constructMinerTx(0, 0, 0, 0, 0, miner_acc5.getAccountKeys().address, tx_mine_5);
  Transaction tx_mine_6;
  currency.constructMinerTx(0, 0, 0, 0, 0, miner_acc6.getAccountKeys().address, tx_mine_6);

  //fill inputs entry
  typedef TransactionSourceEntry::OutputEntry tx_output_entry;
  std::vector<TransactionSourceEntry> sources;
  sources.resize(sources.size()+1);
  TransactionSourceEntry& src = sources.back();
  src.amount = 70368744177663;
  {
    tx_output_entry oe;
    oe.first = 0;
    oe.second = boost::get<KeyOutput>(tx_mine_1.outputs[0].target).key;
    src.outputs.push_back(oe);

    oe.first = 1;
    oe.second = boost::get<KeyOutput>(tx_mine_2.outputs[0].target).key;
    src.outputs.push_back(oe);

    oe.first = 2;
    oe.second = boost::get<KeyOutput>(tx_mine_3.outputs[0].target).key;
    src.outputs.push_back(oe);

    oe.first = 3;
    oe.second = boost::get<KeyOutput>(tx_mine_4.outputs[0].target).key;
    src.outputs.push_back(oe);

    oe.first = 4;
    oe.second = boost::get<KeyOutput>(tx_mine_5.outputs[0].target).key;
    src.outputs.push_back(oe);

    oe.first = 5;
    oe.second = boost::get<KeyOutput>(tx_mine_6.outputs[0].target).key;
    src.outputs.push_back(oe);

    src.realTransactionPublicKey = Orbis::getTransactionPublicKeyFromExtra(tx_mine_2.extra);
    src.realOutput = 1;
    src.realOutputIndexInTransaction = 0;
  }
  //fill outputs entry
  TransactionDestinationEntry td;
  td.addr = rv_acc.getAccountKeys().address;
  td.amount = 69368744177663;
  std::vector<TransactionDestinationEntry> destinations;
  destinations.push_back(td);

  Transaction tx_rc1;
  bool r = constructTransaction(miner_acc2.getAccountKeys(), sources, destinations, std::vector<uint8_t>(), tx_rc1, 0, logger);
  CHECK_AND_ASSERT_MES(r, false, "failed to construct transaction");

  Crypto::Hash pref_hash = getObjectHash(*static_cast<TransactionPrefix*>(&tx_rc1));
  std::vector<const Crypto::PublicKey *> output_keys;
  output_keys.push_back(&boost::get<KeyOutput>(tx_mine_1.outputs[0].target).key);
  output_keys.push_back(&boost::get<KeyOutput>(tx_mine_2.outputs[0].target).key);
  output_keys.push_back(&boost::get<KeyOutput>(tx_mine_3.outputs[0].target).key);
  output_keys.push_back(&boost::get<KeyOutput>(tx_mine_4.outputs[0].target).key);
  output_keys.push_back(&boost::get<KeyOutput>(tx_mine_5.outputs[0].target).key);
  output_keys.push_back(&boost::get<KeyOutput>(tx_mine_6.outputs[0].target).key);
  r = Crypto::check_ring_signature(pref_hash, boost::get<KeyInput>(tx_rc1.inputs[0]).keyImage,
    output_keys, &tx_rc1.signatures[0][0]);
  CHECK_AND_ASSERT_MES(r, false, "failed to check ring signature");

  std::vector<size_t> outs;
  uint64_t money = 0;

  r = lookup_acc_outs(rv_acc.getAccountKeys(), tx_rc1, getTransactionPublicKeyFromExtra(tx_rc1.extra), outs, money);
  CHECK_AND_ASSERT_MES(r, false, "failed to lookup_acc_outs");
  CHECK_AND_ASSERT_MES(td.amount == money, false, "wrong money amount in new transaction");
  money = 0;
  r = lookup_acc_outs(rv_acc2.getAccountKeys(), tx_rc1, getTransactionPublicKeyFromExtra(tx_rc1.extra), outs,  money);
  CHECK_AND_ASSERT_MES(r, false, "failed to lookup_acc_outs");
  CHECK_AND_ASSERT_MES(0 == money, false, "wrong money amount in new transaction");
  return true;
}

bool test_block_creation()
{
  Logging::ConsoleLogger logger;

  uint64_t vszs[] = {80,476,476,475,475,474,475,474,474,475,472,476,476,475,475,474,475,474,474,475,472,476,476,475,475,474,475,474,474,475,9391,476,476,475,475,474,475,8819,8301,475,472,4302,5316,14347,16620,19583,19403,19728,19442,19852,19015,19000,19016,19795,19749,18087,19787,19704,19750,19267,19006,19050,19445,19407,19522,19546,19788,19369,19486,19329,19370,18853,19600,19110,19320,19746,19474,19474,19743,19494,19755,19715,19769,19620,19368,19839,19532,23424,28287,30707};
  std::vector<uint64_t> szs(&vszs[0], &vszs[90]);
  Orbis::Currency currency = Orbis::CurrencyBuilder(logger).currency();

  AccountPublicAddress adr;
  bool r = currency.parseAccountAddressString("272xWzbWsP4cfNFfxY5ETN5moU8x81PKfWPwynrrqsNGDBQGLmD1kCkKCvPeDUXu5XfmZkCrQ53wsWmdfvHBGLNjGcRiDcK", adr);
  CHECK_AND_ASSERT_MES(r, false, "failed to import");
  Block b;
  r = currency.constructMinerTx(90, Common::medianValue(szs), 3553616528562147, 33094, 10000000, adr, b.baseTransaction, BinaryArray(), 11);
  return r;
}

bool test_transactions()
{
  if(!test_transaction_generation_and_ring_signature())
    return false;
  if(!test_block_creation())
    return false;


  return true;
}
