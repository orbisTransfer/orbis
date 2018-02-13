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

#include <array>
#include <string>
#include <vector>

#include "CryptoTypes.h"

#include <boost/variant.hpp>

namespace Orbis {

enum class TransactionRemoveReason : uint8_t 
{ 
  INCLUDED_IN_BLOCK = 0, 
  TIMEOUT = 1
};

struct TransactionOutputToKeyDetails {
  Crypto::PublicKey txOutKey;
};

struct TransactionOutputMultisignatureDetails {
  std::vector<Crypto::PublicKey> keys;
  uint32_t requiredSignatures;
};

struct TransactionOutputDetails {
  uint64_t amount;
  uint32_t globalIndex;

  boost::variant<
    TransactionOutputToKeyDetails,
    TransactionOutputMultisignatureDetails> output;
};

struct TransactionOutputReferenceDetails {
  Crypto::Hash transactionHash;
  size_t number;
};

struct TransactionInputGenerateDetails {
  uint32_t height;
};

struct TransactionInputToKeyDetails {
  std::vector<uint32_t> outputIndexes;
  Crypto::KeyImage keyImage;
  uint64_t mixin;
  TransactionOutputReferenceDetails output;
};

struct TransactionInputMultisignatureDetails {
  uint32_t signatures;
  TransactionOutputReferenceDetails output;
};

struct TransactionInputDetails {
  uint64_t amount;

  boost::variant<
    TransactionInputGenerateDetails,
    TransactionInputToKeyDetails,
    TransactionInputMultisignatureDetails> input;
};

struct TransactionExtraDetails {
  std::vector<size_t> padding;
  std::vector<Crypto::PublicKey> publicKey; 
  std::vector<std::string> nonce;
  std::vector<uint8_t> raw;
};

struct TransactionDetails {
  Crypto::Hash hash;
  uint64_t size;
  uint64_t fee;
  uint64_t totalInputsAmount;
  uint64_t totalOutputsAmount;
  uint64_t mixin;
  uint64_t unlockTime;
  uint64_t timestamp;
  Crypto::Hash paymentId;
  bool inBlockchain;
  Crypto::Hash blockHash;
  uint32_t blockHeight;
  TransactionExtraDetails extra;
  std::vector<std::vector<Crypto::Signature>> signatures;
  std::vector<TransactionInputDetails> inputs;
  std::vector<TransactionOutputDetails> outputs;
};

struct BlockDetails {
  uint8_t majorVersion;
  uint8_t minorVersion;
  uint64_t timestamp;
  Crypto::Hash prevBlockHash;
  uint32_t nonce;
  bool isOrphaned;
  uint32_t height;
  Crypto::Hash hash;
  uint64_t difficulty;
  uint64_t reward;
  uint64_t baseReward;
  uint64_t blockSize;
  uint64_t transactionsCumulativeSize;
  uint64_t alreadyGeneratedCoins;
  uint64_t alreadyGeneratedTransactions;
  uint64_t sizeMedian;
  double penalty;
  uint64_t totalFeeAmount;
  std::vector<TransactionDetails> transactions;
};

}
