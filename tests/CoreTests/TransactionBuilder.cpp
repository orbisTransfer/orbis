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
#include "TransactionBuilder.h"
#include "OrbisCore/TransactionExtra.h"
#include "OrbisCore/OrbisTools.h"

using namespace Orbis;
using namespace Crypto;
using namespace Common;

TransactionBuilder::TransactionBuilder(const Orbis::Currency& currency, uint64_t unlockTime)
  : m_currency(currency), m_version(Orbis::CURRENT_TRANSACTION_VERSION), m_unlockTime(unlockTime), m_txKey(generateKeyPair()) {}

TransactionBuilder& TransactionBuilder::newTxKeys() {
  m_txKey = generateKeyPair();
  return *this;
}

TransactionBuilder& TransactionBuilder::setTxKeys(const Orbis::KeyPair& txKeys) {
  m_txKey = txKeys;
  return *this;
}

TransactionBuilder& TransactionBuilder::setInput(const std::vector<Orbis::TransactionSourceEntry>& sources, const Orbis::AccountKeys& senderKeys) {
  m_sources = sources;
  m_senderKeys = senderKeys;
  return *this;
}

TransactionBuilder& TransactionBuilder::addMultisignatureInput(const MultisignatureSource& source) {
  m_msigSources.push_back(source);
  return *this;
}

TransactionBuilder& TransactionBuilder::setOutput(const std::vector<Orbis::TransactionDestinationEntry>& destinations) {
  m_destinations = destinations;
  return *this;
}

TransactionBuilder& TransactionBuilder::addOutput(const Orbis::TransactionDestinationEntry& dest) {
  m_destinations.push_back(dest);
  return *this;
}

TransactionBuilder& TransactionBuilder::addMultisignatureOut(uint64_t amount, const KeysVector& keys, uint32_t required) {

  MultisignatureDestination dst;

  dst.amount = amount;
  dst.keys = keys;
  dst.requiredSignatures = required;

  m_msigDestinations.push_back(dst);

  return *this;
}

Transaction TransactionBuilder::build() const {
  Crypto::Hash prefixHash;

  Transaction tx;
  addTransactionPublicKeyToExtra(tx.extra, m_txKey.publicKey);

  tx.version = static_cast<uint8_t>(m_version);
  tx.unlockTime = m_unlockTime;

  std::vector<Orbis::KeyPair> contexts;

  fillInputs(tx, contexts);
  fillOutputs(tx);

  getObjectHash(*static_cast<TransactionPrefix*>(&tx), prefixHash);

  signSources(prefixHash, contexts, tx);

  return tx;
}

void TransactionBuilder::fillInputs(Transaction& tx, std::vector<Orbis::KeyPair>& contexts) const {
  for (const TransactionSourceEntry& src_entr : m_sources) {
    contexts.push_back(KeyPair());
    KeyPair& in_ephemeral = contexts.back();
    Crypto::KeyImage img;
    generate_key_image_helper(m_senderKeys, src_entr.realTransactionPublicKey, src_entr.realOutputIndexInTransaction, in_ephemeral, img);

    // put key image into tx input
    KeyInput input_to_key;
    input_to_key.amount = src_entr.amount;
    input_to_key.keyImage = img;

    // fill outputs array and use relative offsets
    for (const TransactionSourceEntry::OutputEntry& out_entry : src_entr.outputs) {
      input_to_key.outputIndexes.push_back(out_entry.first);
    }

    input_to_key.outputIndexes = absolute_output_offsets_to_relative(input_to_key.outputIndexes);
    tx.inputs.push_back(input_to_key);
  }

  for (const auto& msrc : m_msigSources) {
    tx.inputs.push_back(msrc.input);
  }
}

void TransactionBuilder::fillOutputs(Transaction& tx) const {
  size_t output_index = 0;
  
  for(const auto& dst_entr : m_destinations) {
    Crypto::KeyDerivation derivation;
    Crypto::PublicKey out_eph_public_key;
    Crypto::generate_key_derivation(dst_entr.addr.viewPublicKey, m_txKey.secretKey, derivation);
    Crypto::derive_public_key(derivation, output_index, dst_entr.addr.spendPublicKey, out_eph_public_key);

    TransactionOutput out;
    out.amount = dst_entr.amount;
    KeyOutput tk;
    tk.key = out_eph_public_key;
    out.target = tk;
    tx.outputs.push_back(out);
    output_index++;
  }

  for (const auto& mdst : m_msigDestinations) {   
    TransactionOutput out;
    MultisignatureOutput target;

    target.requiredSignatureCount = mdst.requiredSignatures;

    for (const auto& key : mdst.keys) {
      Crypto::KeyDerivation derivation;
      Crypto::PublicKey ephemeralPublicKey;
      Crypto::generate_key_derivation(key.address.viewPublicKey, m_txKey.secretKey, derivation);
      Crypto::derive_public_key(derivation, output_index, key.address.spendPublicKey, ephemeralPublicKey);
      target.keys.push_back(ephemeralPublicKey);
    }
    out.amount = mdst.amount;
    out.target = target;
    tx.outputs.push_back(out);
    output_index++;
  }
}


void TransactionBuilder::signSources(const Crypto::Hash& prefixHash, const std::vector<Orbis::KeyPair>& contexts, Transaction& tx) const {
  
  tx.signatures.clear();

  size_t i = 0;

  // sign TransactionInputToKey sources
  for (const auto& src_entr : m_sources) {
    std::vector<const Crypto::PublicKey*> keys_ptrs;

    for (const auto& o : src_entr.outputs) {
      keys_ptrs.push_back(&o.second);
    }

    tx.signatures.push_back(std::vector<Crypto::Signature>());
    std::vector<Crypto::Signature>& sigs = tx.signatures.back();
    sigs.resize(src_entr.outputs.size());
    generate_ring_signature(prefixHash, boost::get<KeyInput>(tx.inputs[i]).keyImage, keys_ptrs, contexts[i].secretKey, src_entr.realOutput, sigs.data());
    i++;
  }

  // sign multisignature source
  for (const auto& msrc : m_msigSources) {
    tx.signatures.resize(tx.signatures.size() + 1);
    auto& outsigs = tx.signatures.back();

    for (const auto& key : msrc.keys) {
      Crypto::KeyDerivation derivation;
      Crypto::PublicKey ephemeralPublicKey;
      Crypto::SecretKey ephemeralSecretKey;

      Crypto::generate_key_derivation(msrc.srcTxPubKey, key.viewSecretKey, derivation);
      Crypto::derive_public_key(derivation, msrc.srcOutputIndex, key.address.spendPublicKey, ephemeralPublicKey);
      Crypto::derive_secret_key(derivation, msrc.srcOutputIndex, key.spendSecretKey, ephemeralSecretKey);

      Crypto::Signature sig;
      Crypto::generate_signature(prefixHash, ephemeralPublicKey, ephemeralSecretKey, sig);
      outsigs.push_back(sig);
    }
  }
}
