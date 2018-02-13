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
#include <cstdint>
#include <string>
#include <vector>

#include "Orbis.h"

namespace Orbis {

namespace TransactionTypes {
  
  enum class InputType : uint8_t { Invalid, Key, Multisignature, Generating };
  enum class OutputType : uint8_t { Invalid, Key, Multisignature };

  struct GlobalOutput {
    Crypto::PublicKey targetKey;
    uint32_t outputIndex;
  };

  typedef std::vector<GlobalOutput> GlobalOutputsContainer;

  struct OutputKeyInfo {
    Crypto::PublicKey transactionPublicKey;
    size_t transactionIndex;
    size_t outputInTransaction;
  };

  struct InputKeyInfo {
    uint64_t amount;
    GlobalOutputsContainer outputs;
    OutputKeyInfo realOutput;
  };
}

//
// ITransactionReader
// 
class ITransactionReader {
public:
  virtual ~ITransactionReader() { }

  virtual Crypto::Hash getTransactionHash() const = 0;
  virtual Crypto::Hash getTransactionPrefixHash() const = 0;
  virtual Crypto::PublicKey getTransactionPublicKey() const = 0;
  virtual bool getTransactionSecretKey(Crypto::SecretKey& key) const = 0;
  virtual uint64_t getUnlockTime() const = 0;

  // extra
  virtual bool getPaymentId(Crypto::Hash& paymentId) const = 0;
  virtual bool getExtraNonce(BinaryArray& nonce) const = 0;
  virtual BinaryArray getExtra() const = 0;

  // inputs
  virtual size_t getInputCount() const = 0;
  virtual uint64_t getInputTotalAmount() const = 0;
  virtual TransactionTypes::InputType getInputType(size_t index) const = 0;
  virtual void getInput(size_t index, KeyInput& input) const = 0;
  virtual void getInput(size_t index, MultisignatureInput& input) const = 0;

  // outputs
  virtual size_t getOutputCount() const = 0;
  virtual uint64_t getOutputTotalAmount() const = 0;
  virtual TransactionTypes::OutputType getOutputType(size_t index) const = 0;
  virtual void getOutput(size_t index, KeyOutput& output, uint64_t& amount) const = 0;
  virtual void getOutput(size_t index, MultisignatureOutput& output, uint64_t& amount) const = 0;

  // signatures
  virtual size_t getRequiredSignaturesCount(size_t inputIndex) const = 0;
  virtual bool findOutputsToAccount(const AccountPublicAddress& addr, const Crypto::SecretKey& viewSecretKey, std::vector<uint32_t>& outs, uint64_t& outputAmount) const = 0;

  // various checks
  virtual bool validateInputs() const = 0;
  virtual bool validateOutputs() const = 0;
  virtual bool validateSignatures() const = 0;

  // serialized transaction
  virtual BinaryArray getTransactionData() const = 0;
};

//
// ITransactionWriter
// 
class ITransactionWriter {
public: 

  virtual ~ITransactionWriter() { }

  // transaction parameters
  virtual void setUnlockTime(uint64_t unlockTime) = 0;

  // extra
  virtual void setPaymentId(const Crypto::Hash& paymentId) = 0;
  virtual void setExtraNonce(const BinaryArray& nonce) = 0;
  virtual void appendExtra(const BinaryArray& extraData) = 0;

  // Inputs/Outputs 
  virtual size_t addInput(const KeyInput& input) = 0;
  virtual size_t addInput(const MultisignatureInput& input) = 0;
  virtual size_t addInput(const AccountKeys& senderKeys, const TransactionTypes::InputKeyInfo& info, KeyPair& ephKeys) = 0;

  virtual size_t addOutput(uint64_t amount, const AccountPublicAddress& to) = 0;
  virtual size_t addOutput(uint64_t amount, const std::vector<AccountPublicAddress>& to, uint32_t requiredSignatures) = 0;
  virtual size_t addOutput(uint64_t amount, const KeyOutput& out) = 0;
  virtual size_t addOutput(uint64_t amount, const MultisignatureOutput& out) = 0;

  // transaction info
  virtual void setTransactionSecretKey(const Crypto::SecretKey& key) = 0;

  // signing
  virtual void signInputKey(size_t input, const TransactionTypes::InputKeyInfo& info, const KeyPair& ephKeys) = 0;
  virtual void signInputMultisignature(size_t input, const Crypto::PublicKey& sourceTransactionKey, size_t outputIndex, const AccountKeys& accountKeys) = 0;
  virtual void signInputMultisignature(size_t input, const KeyPair& ephemeralKeys) = 0;
};

class ITransaction : 
  public ITransactionReader, 
  public ITransactionWriter {
public:
  virtual ~ITransaction() { }

};

}
