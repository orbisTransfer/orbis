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

#include <cstdint>
#include <limits>
#include <vector>
#include "orbis/hash.h"
#include "ITransaction.h"
#include "IObservable.h"
#include "IStreamSerializable.h"

namespace Orbis {

const uint32_t UNCONFIRMED_TRANSACTION_GLOBAL_OUTPUT_INDEX = std::numeric_limits<uint32_t>::max();

struct TransactionInformation {
  // transaction info
  Crypto::Hash transactionHash;
  Crypto::PublicKey publicKey;
  uint32_t blockHeight;
  uint64_t timestamp;
  uint64_t unlockTime;
  uint64_t totalAmountIn;
  uint64_t totalAmountOut;
  std::vector<uint8_t> extra;
  Crypto::Hash paymentId;
};


struct TransactionOutputInformation {
  // output info
  TransactionTypes::OutputType type;
  uint64_t amount;
  uint32_t globalOutputIndex;
  uint32_t outputInTransaction;

  // transaction info
  Crypto::Hash transactionHash;
  Crypto::PublicKey transactionPublicKey;

  union {
    Crypto::PublicKey outputKey;         // Type: Key 
    uint32_t requiredSignatures; // Type: Multisignature
  };
};

struct TransactionSpentOutputInformation: public TransactionOutputInformation {
  uint32_t spendingBlockHeight;
  uint64_t timestamp;
  Crypto::Hash spendingTransactionHash;
  Crypto::KeyImage keyImage;  //!< \attention Used only for TransactionTypes::OutputType::Key
  uint32_t inputInTransaction;
};

class ITransfersContainer : public IStreamSerializable {
public:
  enum Flags : uint32_t {
    // state
    IncludeStateUnlocked = 0x01,
    IncludeStateLocked = 0x02,
    IncludeStateSoftLocked = 0x04,
    IncludeStateSpent = 0x08,
    // output type
    IncludeTypeKey = 0x100,
    IncludeTypeMultisignature = 0x200,
    // combinations
    IncludeStateAll = 0xff,
    IncludeTypeAll = 0xff00,

    IncludeKeyUnlocked = IncludeTypeKey | IncludeStateUnlocked,
    IncludeKeyNotUnlocked = IncludeTypeKey | IncludeStateLocked | IncludeStateSoftLocked,

    IncludeAllLocked = IncludeTypeAll | IncludeStateLocked | IncludeStateSoftLocked,
    IncludeAllUnlocked = IncludeTypeAll | IncludeStateUnlocked,
    IncludeAll = IncludeTypeAll | IncludeStateAll,

    IncludeDefault = IncludeKeyUnlocked
  };

  virtual size_t transfersCount() const = 0;
  virtual size_t transactionsCount() const = 0;
  virtual uint64_t balance(uint32_t flags = IncludeDefault) const = 0;
  virtual void getOutputs(std::vector<TransactionOutputInformation>& transfers, uint32_t flags = IncludeDefault) const = 0;
  virtual bool getTransactionInformation(const Crypto::Hash& transactionHash, TransactionInformation& info,
    uint64_t* amountIn = nullptr, uint64_t* amountOut = nullptr) const = 0;
  virtual std::vector<TransactionOutputInformation> getTransactionOutputs(const Crypto::Hash& transactionHash, uint32_t flags = IncludeDefault) const = 0;
  //only type flags are feasible for this function
  virtual std::vector<TransactionOutputInformation> getTransactionInputs(const Crypto::Hash& transactionHash, uint32_t flags) const = 0;
  virtual void getUnconfirmedTransactions(std::vector<Crypto::Hash>& transactions) const = 0;
  virtual std::vector<TransactionSpentOutputInformation> getSpentOutputs() const = 0;
};

}
