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
#include "TransactionUtils.h"

#include <unordered_set>

#include "orbis/orbis.h"
#include "OrbisCore/Account.h"
#include "OrbisFormatUtils.h"
#include "TransactionExtra.h"

using namespace Crypto;

namespace Orbis {

bool checkInputsKeyimagesDiff(const Orbis::TransactionPrefix& tx) {
  std::unordered_set<Crypto::KeyImage> ki;
  for (const auto& in : tx.inputs) {
    if (in.type() == typeid(KeyInput)) {
      if (!ki.insert(boost::get<KeyInput>(in).keyImage).second)
        return false;
    }
  }
  return true;
}

// TransactionInput helper functions

size_t getRequiredSignaturesCount(const TransactionInput& in) {
  if (in.type() == typeid(KeyInput)) {
    return boost::get<KeyInput>(in).outputIndexes.size();
  }
  if (in.type() == typeid(MultisignatureInput)) {
    return boost::get<MultisignatureInput>(in).signatureCount;
  }
  return 0;
}

uint64_t getTransactionInputAmount(const TransactionInput& in) {
  if (in.type() == typeid(KeyInput)) {
    return boost::get<KeyInput>(in).amount;
  }
  if (in.type() == typeid(MultisignatureInput)) {
    return boost::get<MultisignatureInput>(in).amount;
  }
  return 0;
}

TransactionTypes::InputType getTransactionInputType(const TransactionInput& in) {
  if (in.type() == typeid(KeyInput)) {
    return TransactionTypes::InputType::Key;
  }
  if (in.type() == typeid(MultisignatureInput)) {
    return TransactionTypes::InputType::Multisignature;
  }
  if (in.type() == typeid(BaseInput)) {
    return TransactionTypes::InputType::Generating;
  }
  return TransactionTypes::InputType::Invalid;
}

const TransactionInput& getInputChecked(const Orbis::TransactionPrefix& transaction, size_t index) {
  if (transaction.inputs.size() <= index) {
    throw std::runtime_error("Transaction input index out of range");
  }
  return transaction.inputs[index];
}

const TransactionInput& getInputChecked(const Orbis::TransactionPrefix& transaction, size_t index, TransactionTypes::InputType type) {
  const auto& input = getInputChecked(transaction, index);
  if (getTransactionInputType(input) != type) {
    throw std::runtime_error("Unexpected transaction input type");
  }
  return input;
}

// TransactionOutput helper functions

TransactionTypes::OutputType getTransactionOutputType(const TransactionOutputTarget& out) {
  if (out.type() == typeid(KeyOutput)) {
    return TransactionTypes::OutputType::Key;
  }
  if (out.type() == typeid(MultisignatureOutput)) {
    return TransactionTypes::OutputType::Multisignature;
  }
  return TransactionTypes::OutputType::Invalid;
}

const TransactionOutput& getOutputChecked(const Orbis::TransactionPrefix& transaction, size_t index) {
  if (transaction.outputs.size() <= index) {
    throw std::runtime_error("Transaction output index out of range");
  }
  return transaction.outputs[index];
}

const TransactionOutput& getOutputChecked(const Orbis::TransactionPrefix& transaction, size_t index, TransactionTypes::OutputType type) {
  const auto& output = getOutputChecked(transaction, index);
  if (getTransactionOutputType(output.target) != type) {
    throw std::runtime_error("Unexpected transaction output target type");
  }
  return output;
}

bool isOutToKey(const Crypto::PublicKey& spendPublicKey, const Crypto::PublicKey& outKey, const Crypto::KeyDerivation& derivation, size_t keyIndex) {
  Crypto::PublicKey pk;
  derive_public_key(derivation, keyIndex, spendPublicKey, pk);
  return pk == outKey;
}

bool findOutputsToAccount(const Orbis::TransactionPrefix& transaction, const AccountPublicAddress& addr,
                          const SecretKey& viewSecretKey, std::vector<uint32_t>& out, uint64_t& amount) {
  AccountKeys keys;
  keys.address = addr;
  // only view secret key is used, spend key is not needed
  keys.viewSecretKey = viewSecretKey;

  Crypto::PublicKey txPubKey = getTransactionPublicKeyFromExtra(transaction.extra);

  amount = 0;
  size_t keyIndex = 0;
  uint32_t outputIndex = 0;

  Crypto::KeyDerivation derivation;
  generate_key_derivation(txPubKey, keys.viewSecretKey, derivation);

  for (const TransactionOutput& o : transaction.outputs) {
    assert(o.target.type() == typeid(KeyOutput) || o.target.type() == typeid(MultisignatureOutput));
    if (o.target.type() == typeid(KeyOutput)) {
      if (is_out_to_acc(keys, boost::get<KeyOutput>(o.target), derivation, keyIndex)) {
        out.push_back(outputIndex);
        amount += o.amount;
      }
      ++keyIndex;
    } else if (o.target.type() == typeid(MultisignatureOutput)) {
      const auto& target = boost::get<MultisignatureOutput>(o.target);
      for (const auto& key : target.keys) {
        if (isOutToKey(keys.address.spendPublicKey, key, derivation, static_cast<size_t>(outputIndex))) {
          out.push_back(outputIndex);
        }
        ++keyIndex;
      }
    }
    ++outputIndex;
  }

  return true;
}

}
