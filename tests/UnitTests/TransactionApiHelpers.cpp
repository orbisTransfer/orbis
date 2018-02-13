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
#include "TransactionApiHelpers.h"
#include "OrbisCore/TransactionApi.h"

using namespace Orbis;
using namespace Crypto;

namespace {

  const std::vector<AccountBase>& getMsigAccounts() {
    static std::vector<AccountBase> msigAccounts = { generateAccount(), generateAccount() };
    return msigAccounts;
  }

}

TestTransactionBuilder::TestTransactionBuilder() {
  tx = createTransaction();
}

TestTransactionBuilder::TestTransactionBuilder(const BinaryArray& txTemplate, const Crypto::SecretKey& secretKey) {
  tx = createTransaction(txTemplate);
  tx->setTransactionSecretKey(secretKey);
}

PublicKey TestTransactionBuilder::getTransactionPublicKey() const {
  return tx->getTransactionPublicKey();
}

void TestTransactionBuilder::appendExtra(const BinaryArray& extraData) {
  tx->appendExtra(extraData);
}

void TestTransactionBuilder::setUnlockTime(uint64_t time) {
  tx->setUnlockTime(time);
}

size_t TestTransactionBuilder::addTestInput(uint64_t amount, const AccountKeys& senderKeys) {
  using namespace TransactionTypes;

  TransactionTypes::InputKeyInfo info;
  PublicKey targetKey;

  Orbis::KeyPair srcTxKeys = Orbis::generateKeyPair();
  derivePublicKey(senderKeys, srcTxKeys.publicKey, 5, targetKey);

  TransactionTypes::GlobalOutput gout = { targetKey, 0 };

  info.amount = amount;
  info.outputs.push_back(gout);

  info.realOutput.transactionIndex = 0;
  info.realOutput.outputInTransaction = 5;
  info.realOutput.transactionPublicKey = reinterpret_cast<const PublicKey&>(srcTxKeys.publicKey);

  KeyPair ephKeys;
  size_t idx = tx->addInput(senderKeys, info, ephKeys);
  keys[idx] = std::make_pair(info, ephKeys);
  return idx;
}

size_t TestTransactionBuilder::addTestInput(uint64_t amount, std::vector<uint32_t> gouts, const AccountKeys& senderKeys) {
  using namespace TransactionTypes;

  TransactionTypes::InputKeyInfo info;
  PublicKey targetKey;

  Orbis::KeyPair srcTxKeys = Orbis::generateKeyPair();
  derivePublicKey(senderKeys, srcTxKeys.publicKey, 5, targetKey);

  TransactionTypes::GlobalOutput gout = { targetKey, 0 };

  info.amount = amount;
  info.outputs.push_back(gout);
  PublicKey pk;
  SecretKey sk;
  for (auto out : gouts) {
    Crypto::generate_keys(pk, sk);
    info.outputs.push_back(TransactionTypes::GlobalOutput{ pk, out });
  }

  info.realOutput.transactionIndex = 0;
  info.realOutput.outputInTransaction = 5;
  info.realOutput.transactionPublicKey = reinterpret_cast<const PublicKey&>(srcTxKeys.publicKey);

  KeyPair ephKeys;
  size_t idx = tx->addInput(senderKeys, info, ephKeys);
  keys[idx] = std::make_pair(info, ephKeys);
  return idx;
}

void TestTransactionBuilder::addInput(const AccountKeys& senderKeys, const TransactionOutputInformation& t) {
  TransactionTypes::InputKeyInfo info;
  info.amount = t.amount;

  TransactionTypes::GlobalOutput globalOut;
  globalOut.outputIndex = t.globalOutputIndex;
  globalOut.targetKey = t.outputKey;
  info.outputs.push_back(globalOut);

  info.realOutput.outputInTransaction = t.outputInTransaction;
  info.realOutput.transactionIndex = 0;
  info.realOutput.transactionPublicKey = t.transactionPublicKey;

  KeyPair ephKeys;
  size_t idx = tx->addInput(senderKeys, info, ephKeys);
  keys[idx] = std::make_pair(info, ephKeys);
}

void TestTransactionBuilder::addTestMultisignatureInput(uint64_t amount, const TransactionOutputInformation& t) {
  MultisignatureInput input;
  input.amount = amount;
  input.outputIndex = t.globalOutputIndex;
  input.signatureCount = t.requiredSignatures;
  size_t idx = tx->addInput(input);
 
  msigInputs[idx] = MsigInfo{ t.transactionPublicKey, t.outputInTransaction, getMsigAccounts() };
}

size_t TestTransactionBuilder::addFakeMultisignatureInput(uint64_t amount, uint32_t globalOutputIndex, size_t signatureCount) {
  MultisignatureInput input;
  input.amount = amount;
  input.outputIndex = globalOutputIndex;
  input.signatureCount = static_cast<uint8_t>(signatureCount);
  size_t idx = tx->addInput(input);

  std::vector<AccountBase> accs;
  for (size_t i = 0; i < signatureCount; ++i) {
    accs.push_back(generateAccount());
  }

  msigInputs[idx] = MsigInfo{ Crypto::rand<PublicKey>(), 0, std::move(accs) };
  return idx;
}

TransactionOutputInformationIn TestTransactionBuilder::addTestKeyOutput(uint64_t amount, uint32_t globalOutputIndex, const AccountKeys& senderKeys) {
  uint32_t index = static_cast<uint32_t>(tx->addOutput(amount, senderKeys.address));

  uint64_t amount_;
  KeyOutput output;
  tx->getOutput(index, output, amount_);

  TransactionOutputInformationIn outputInfo;
  outputInfo.type = TransactionTypes::OutputType::Key;
  outputInfo.amount = amount_;
  outputInfo.globalOutputIndex = globalOutputIndex;
  outputInfo.outputInTransaction = index;
  outputInfo.transactionPublicKey = tx->getTransactionPublicKey();
  outputInfo.outputKey = output.key;
  outputInfo.keyImage = generateKeyImage(senderKeys, index, tx->getTransactionPublicKey());

  return outputInfo;
}

TransactionOutputInformationIn TestTransactionBuilder::addTestMultisignatureOutput(uint64_t amount, std::vector<AccountPublicAddress>& addresses, uint32_t globalOutputIndex) {
  uint32_t index = static_cast<uint32_t>(tx->addOutput(amount, addresses, static_cast<uint32_t>(addresses.size())));

  uint64_t _amount;
  MultisignatureOutput output;
  tx->getOutput(index, output, _amount);

  TransactionOutputInformationIn outputInfo;
  outputInfo.type = TransactionTypes::OutputType::Multisignature;
  outputInfo.amount = _amount;
  outputInfo.globalOutputIndex = globalOutputIndex;
  outputInfo.outputInTransaction = index;
  outputInfo.transactionPublicKey = tx->getTransactionPublicKey();
  // Doesn't used in multisignature output, so can contain garbage
  outputInfo.keyImage = generateKeyImage();
  outputInfo.requiredSignatures = output.requiredSignatureCount;
  return outputInfo;
}

TransactionOutputInformationIn TestTransactionBuilder::addTestMultisignatureOutput(uint64_t amount, uint32_t globalOutputIndex) {
  std::vector<AccountPublicAddress> multisigAddresses;
  for (const auto& acc : getMsigAccounts()) {
    multisigAddresses.push_back(acc.getAccountKeys().address);
  }

  return addTestMultisignatureOutput(amount, multisigAddresses, globalOutputIndex);
}

size_t TestTransactionBuilder::addOutput(uint64_t amount, const AccountPublicAddress& to) {
  return tx->addOutput(amount, to);
}

size_t TestTransactionBuilder::addOutput(uint64_t amount, const KeyOutput& out) {
  return tx->addOutput(amount, out);
}

size_t TestTransactionBuilder::addOutput(uint64_t amount, const MultisignatureOutput& out) {
  return tx->addOutput(amount, out);
}

std::unique_ptr<ITransactionReader> TestTransactionBuilder::build() {
  for (const auto& kv : keys) {
    tx->signInputKey(kv.first, kv.second.first, kv.second.second);
  }

  for (const auto& kv : msigInputs) {
    for (const auto& acc : kv.second.accounts) {
      tx->signInputMultisignature(kv.first, kv.second.transactionKey, kv.second.outputIndex, acc.getAccountKeys());
    }
  }

  transactionHash = tx->getTransactionHash();

  keys.clear();
  return std::move(tx);
}

Crypto::Hash TestTransactionBuilder::getTransactionHash() const {
  return transactionHash;
}

FusionTransactionBuilder::FusionTransactionBuilder(const Currency& currency, uint64_t amount) :
  m_currency(currency),
  m_amount(amount),
  m_firstInput(0),
  m_firstOutput(0),
  m_fee(0),
  m_extraSize(0),
  m_inputCount(currency.fusionTxMinInputCount()) {
}

uint64_t FusionTransactionBuilder::getAmount() const {
  return m_amount;
}

void FusionTransactionBuilder::setAmount(uint64_t val) {
  m_amount = val;
}

uint64_t FusionTransactionBuilder::getFirstInput() const {
  return m_firstInput;
}

void FusionTransactionBuilder::setFirstInput(uint64_t val) {
  m_firstInput = val;
}

uint64_t FusionTransactionBuilder::getFirstOutput() const {
  return m_firstOutput;
}

void FusionTransactionBuilder::setFirstOutput(uint64_t val) {
  m_firstOutput = val;
}

uint64_t FusionTransactionBuilder::getFee() const {
  return m_fee;
}

void FusionTransactionBuilder::setFee(uint64_t val) {
  m_fee = val;
}

size_t FusionTransactionBuilder::getExtraSize() const {
  return m_extraSize;
}

void FusionTransactionBuilder::setExtraSize(size_t val) {
  m_extraSize = val;
}

size_t FusionTransactionBuilder::getInputCount() const {
  return m_inputCount;
}

void FusionTransactionBuilder::setInputCount(size_t val) {
  m_inputCount = val;
}

std::unique_ptr<ITransactionReader> FusionTransactionBuilder::buildReader() const {
  assert(m_inputCount > 0);
  assert(m_firstInput <= m_amount);
  assert(m_amount > m_currency.defaultDustThreshold());

  TestTransactionBuilder builder;

  if (m_extraSize != 0) {
    builder.appendExtra(BinaryArray(m_extraSize, 0));
  }

  if (m_firstInput != 0) {
    builder.addTestInput(m_firstInput);
  }

  if (m_amount > m_firstInput) {
    builder.addTestInput(m_amount - m_firstInput - (m_inputCount - 1) * m_currency.defaultDustThreshold());
    for (size_t i = 0; i < m_inputCount - 1; ++i) {
      builder.addTestInput(m_currency.defaultDustThreshold());
    }
  }

  AccountPublicAddress address = generateAddress();
  std::vector<uint64_t> outputAmounts;
  assert(m_amount >= m_firstOutput + m_fee);
  decomposeAmount(m_amount - m_firstOutput - m_fee, m_currency.defaultDustThreshold(), outputAmounts);
  std::sort(outputAmounts.begin(), outputAmounts.end());

  if (m_firstOutput != 0) {
    builder.addOutput(m_firstOutput, address);
  }

  for (auto outAmount : outputAmounts) {
    builder.addOutput(outAmount, address);
  }

  return builder.build();
}

Transaction FusionTransactionBuilder::buildTx() const {
  return convertTx(*buildReader());
}

Transaction FusionTransactionBuilder::createFusionTransactionBySize(size_t targetSize) {
  auto tx = buildReader();

  size_t realSize = tx->getTransactionData().size();
  if (realSize < targetSize) {
    setExtraSize(targetSize - realSize);
    tx = buildReader();

    realSize = tx->getTransactionData().size();
    if (realSize > targetSize) {
      setExtraSize(getExtraSize() - 1);
      tx = buildReader();
    }
  }

  return convertTx(*tx);
}
