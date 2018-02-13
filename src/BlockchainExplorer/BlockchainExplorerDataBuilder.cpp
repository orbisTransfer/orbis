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

#include "BlockchainExplorerDataBuilder.h"

#include <boost/utility/value_init.hpp>
#include <boost/range/combine.hpp>

#include "Common/StringTools.h"
#include "OrbisCore/OrbisFormatUtils.h"
#include "OrbisCore/OrbisTools.h"
#include "OrbisCore/TransactionExtra.h"
#include "OrbisConfig.h"

namespace Orbis {

BlockchainExplorerDataBuilder::BlockchainExplorerDataBuilder(Orbis::ICore& core, Orbis::IOrbisProtocolQuery& protocol) :
core(core),
protocol(protocol) {
}

bool BlockchainExplorerDataBuilder::getMixin(const Transaction& transaction, uint64_t& mixin) {
  mixin = 0;
  for (const TransactionInput& txin : transaction.inputs) {
    if (txin.type() != typeid(KeyInput)) {
      continue;
    }
    uint64_t currentMixin = boost::get<KeyInput>(txin).outputIndexes.size();
    if (currentMixin > mixin) {
      mixin = currentMixin;
    }
  }
  return true;
}

bool BlockchainExplorerDataBuilder::getPaymentId(const Transaction& transaction, Crypto::Hash& paymentId) {
  std::vector<TransactionExtraField> txExtraFields;
  parseTransactionExtra(transaction.extra, txExtraFields);
  TransactionExtraNonce extraNonce;
  if (!findTransactionExtraFieldByType(txExtraFields, extraNonce)) {
    return false;
  }
  return getPaymentIdFromTransactionExtraNonce(extraNonce.nonce, paymentId);
}

bool BlockchainExplorerDataBuilder::fillTxExtra(const std::vector<uint8_t>& rawExtra, TransactionExtraDetails& extraDetails) {
  extraDetails.raw = rawExtra;
  std::vector<TransactionExtraField> txExtraFields;
  parseTransactionExtra(rawExtra, txExtraFields);
  for (const TransactionExtraField& field : txExtraFields) {
    if (typeid(TransactionExtraPadding) == field.type()) {
      extraDetails.padding.push_back(std::move(boost::get<TransactionExtraPadding>(field).size));
    } else if (typeid(TransactionExtraPublicKey) == field.type()) {
      extraDetails.publicKey.push_back(std::move(boost::get<TransactionExtraPublicKey>(field).publicKey));
    } else if (typeid(TransactionExtraNonce) == field.type()) {
      extraDetails.nonce.push_back(Common::toHex(boost::get<TransactionExtraNonce>(field).nonce.data(), boost::get<TransactionExtraNonce>(field).nonce.size()));
    }
  }
  return true;
}

size_t BlockchainExplorerDataBuilder::median(std::vector<size_t>& v) {
  if (v.empty())
    return boost::value_initialized<size_t>();
  if (v.size() == 1)
    return v[0];

  size_t n = (v.size()) / 2;
  std::sort(v.begin(), v.end());
  //nth_element(v.begin(), v.begin()+n-1, v.end());
  if (v.size() % 2) {//1, 3, 5...
    return v[n];
  } else {//2, 4, 6...
    return (v[n - 1] + v[n]) / 2;
  }

}

bool BlockchainExplorerDataBuilder::fillBlockDetails(const Block &block, BlockDetails& blockDetails) {
  Crypto::Hash hash = get_block_hash(block);

  blockDetails.majorVersion = block.majorVersion;
  blockDetails.minorVersion = block.minorVersion;
  blockDetails.timestamp = block.timestamp;
  blockDetails.prevBlockHash = block.previousBlockHash;
  blockDetails.nonce = block.nonce;
  blockDetails.hash = hash;

  blockDetails.reward = 0;
  for (const TransactionOutput& out : block.baseTransaction.outputs) {
    blockDetails.reward += out.amount;
  }

  if (block.baseTransaction.inputs.front().type() != typeid(BaseInput))
    return false;
  blockDetails.height = boost::get<BaseInput>(block.baseTransaction.inputs.front()).blockIndex;

  Crypto::Hash tmpHash = core.getBlockIdByHeight(blockDetails.height);
  blockDetails.isOrphaned = hash != tmpHash;

  if (!core.getBlockDifficulty(blockDetails.height, blockDetails.difficulty)) {
    return false;
  }

  std::vector<size_t> blocksSizes;
  if (!core.getBackwardBlocksSizes(blockDetails.height, blocksSizes, parameters::ORBIS_REWARD_BLOCKS_WINDOW)) {
    return false;
  }
  blockDetails.sizeMedian = median(blocksSizes);

  size_t blockSize = 0;
  if (!core.getBlockSize(hash, blockSize)) {
    return false;
  }
  blockDetails.transactionsCumulativeSize = blockSize;

  size_t blokBlobSize = getObjectBinarySize(block);
  size_t minerTxBlobSize = getObjectBinarySize(block.baseTransaction);
  blockDetails.blockSize = blokBlobSize + blockDetails.transactionsCumulativeSize - minerTxBlobSize;

  if (!core.getAlreadyGeneratedCoins(hash, blockDetails.alreadyGeneratedCoins)) {
    return false;
  }

  if (!core.getGeneratedTransactionsNumber(blockDetails.height, blockDetails.alreadyGeneratedTransactions)) {
    return false;
  }

  uint64_t prevBlockGeneratedCoins = 0;
  if (blockDetails.height > 0) {
    if (!core.getAlreadyGeneratedCoins(block.previousBlockHash, prevBlockGeneratedCoins)) {
      return false;
    }
  }
  uint64_t maxReward = 0;
  uint64_t currentReward = 0;
  int64_t emissionChange = 0;
  if (!core.getBlockReward(blockDetails.sizeMedian, 0, prevBlockGeneratedCoins, 0, maxReward, emissionChange)) {
    return false;
  }
  if (!core.getBlockReward(blockDetails.sizeMedian, blockDetails.transactionsCumulativeSize, prevBlockGeneratedCoins, 0, currentReward, emissionChange)) {
    return false;
  }

  blockDetails.baseReward = maxReward;
  if (maxReward == 0 && currentReward == 0) {
    blockDetails.penalty = static_cast<double>(0);
  } else {
    if (maxReward < currentReward) {
      return false;
    }
    blockDetails.penalty = static_cast<double>(maxReward - currentReward) / static_cast<double>(maxReward);
  }


  blockDetails.transactions.reserve(block.transactionHashes.size() + 1);
  TransactionDetails transactionDetails;
  if (!fillTransactionDetails(block.baseTransaction, transactionDetails, block.timestamp)) {
    return false;
  }
  blockDetails.transactions.push_back(std::move(transactionDetails));

  std::list<Transaction> found;
  std::list<Crypto::Hash> missed;
  core.getTransactions(block.transactionHashes, found, missed, blockDetails.isOrphaned);
  if (found.size() != block.transactionHashes.size()) {
    return false;
  }

  blockDetails.totalFeeAmount = 0;

  for (const Transaction& tx : found) {
    TransactionDetails transactionDetails;
    if (!fillTransactionDetails(tx, transactionDetails, block.timestamp)) {
      return false;
    }
    blockDetails.transactions.push_back(std::move(transactionDetails));
    blockDetails.totalFeeAmount += transactionDetails.fee;
  }
  return true;
}

bool BlockchainExplorerDataBuilder::fillTransactionDetails(const Transaction& transaction, TransactionDetails& transactionDetails, uint64_t timestamp) {
  Crypto::Hash hash = getObjectHash(transaction);
  transactionDetails.hash = hash;

  transactionDetails.timestamp = timestamp;

  Crypto::Hash blockHash;
  uint32_t blockHeight;
  if (!core.getBlockContainingTx(hash, blockHash, blockHeight)) {
    transactionDetails.inBlockchain = false;
    transactionDetails.blockHeight = boost::value_initialized<uint32_t>();
    transactionDetails.blockHash = boost::value_initialized<Crypto::Hash>();
  } else {
    transactionDetails.inBlockchain = true;
    transactionDetails.blockHeight = blockHeight;
    transactionDetails.blockHash = blockHash;
    if (timestamp == 0) {
      Block block;
      if (!core.getBlockByHash(blockHash, block)) {
        return false;
      }
      transactionDetails.timestamp = block.timestamp;
    }
  }

  transactionDetails.size = getObjectBinarySize(transaction);
  transactionDetails.unlockTime = transaction.unlockTime;
  transactionDetails.totalOutputsAmount = get_outs_money_amount(transaction);

  uint64_t inputsAmount;
  if (!get_inputs_money_amount(transaction, inputsAmount)) {
    return false;
  }
  transactionDetails.totalInputsAmount = inputsAmount;

  if (transaction.inputs.size() > 0 && transaction.inputs.front().type() == typeid(BaseInput)) {
    //It's gen transaction
    transactionDetails.fee = 0;
    transactionDetails.mixin = 0;
  } else {
    uint64_t fee;
    if (!get_tx_fee(transaction, fee)) {
      return false;
    }
    transactionDetails.fee = fee;
    uint64_t mixin;
    if (!getMixin(transaction, mixin)) {
      return false;
    }
    transactionDetails.mixin = mixin;
  }

  Crypto::Hash paymentId;
  if (getPaymentId(transaction, paymentId)) {
    transactionDetails.paymentId = paymentId;
  } else {
    transactionDetails.paymentId = boost::value_initialized<Crypto::Hash>();
  }

  fillTxExtra(transaction.extra, transactionDetails.extra);

  transactionDetails.signatures.reserve(transaction.signatures.size());
  for (const std::vector<Crypto::Signature>& signatures : transaction.signatures) {
    std::vector<Crypto::Signature> signaturesDetails;
    signaturesDetails.reserve(signatures.size());
    for (const Crypto::Signature& signature : signatures) {
      signaturesDetails.push_back(std::move(signature));
    }
    transactionDetails.signatures.push_back(std::move(signaturesDetails));
  }

  transactionDetails.inputs.reserve(transaction.inputs.size());
  for (const TransactionInput& txIn : transaction.inputs) {
    TransactionInputDetails txInDetails;

    if (txIn.type() == typeid(BaseInput)) {
      TransactionInputGenerateDetails txInGenDetails;
      txInGenDetails.height = boost::get<BaseInput>(txIn).blockIndex;
      txInDetails.amount = 0;
      for (const TransactionOutput& out : transaction.outputs) {
        txInDetails.amount += out.amount;
      }
      txInDetails.input = txInGenDetails;
    } else if (txIn.type() == typeid(KeyInput)) {
      TransactionInputToKeyDetails txInToKeyDetails;
      const KeyInput& txInToKey = boost::get<KeyInput>(txIn);
      std::list<std::pair<Crypto::Hash, size_t>> outputReferences;
      if (!core.scanOutputkeysForIndices(txInToKey, outputReferences)) {
        return false;
      }
      txInDetails.amount = txInToKey.amount;
      txInToKeyDetails.outputIndexes = txInToKey.outputIndexes;
      txInToKeyDetails.keyImage = txInToKey.keyImage;
      txInToKeyDetails.mixin = txInToKey.outputIndexes.size();
      txInToKeyDetails.output.number = outputReferences.back().second;
      txInToKeyDetails.output.transactionHash = outputReferences.back().first;
      txInDetails.input = txInToKeyDetails;
    } else if (txIn.type() == typeid(MultisignatureInput)) {
      TransactionInputMultisignatureDetails txInMultisigDetails;
      const MultisignatureInput& txInMultisig = boost::get<MultisignatureInput>(txIn);
      txInDetails.amount = txInMultisig.amount;
      txInMultisigDetails.signatures = txInMultisig.signatureCount;
      std::pair<Crypto::Hash, size_t> outputReference;
      if (!core.getMultisigOutputReference(txInMultisig, outputReference)) {
        return false;
      }
      txInMultisigDetails.output.number = outputReference.second;
      txInMultisigDetails.output.transactionHash = outputReference.first;
      txInDetails.input = txInMultisigDetails;
    } else {
      return false;
    }
    transactionDetails.inputs.push_back(std::move(txInDetails));
  }

  transactionDetails.outputs.reserve(transaction.outputs.size());
  std::vector<uint32_t> globalIndices;
  globalIndices.reserve(transaction.outputs.size());
  if (!transactionDetails.inBlockchain || !core.get_tx_outputs_gindexs(hash, globalIndices)) {
    for (size_t i = 0; i < transaction.outputs.size(); ++i) {
      globalIndices.push_back(0);
    }
  }

  typedef boost::tuple<TransactionOutput, uint32_t> outputWithIndex;
  auto range = boost::combine(transaction.outputs, globalIndices);
  for (const outputWithIndex& txOutput : range) {
    TransactionOutputDetails txOutDetails;
    txOutDetails.amount = txOutput.get<0>().amount;
    txOutDetails.globalIndex = txOutput.get<1>();

    if (txOutput.get<0>().target.type() == typeid(KeyOutput)) {
      TransactionOutputToKeyDetails txOutToKeyDetails;
      txOutToKeyDetails.txOutKey = boost::get<KeyOutput>(txOutput.get<0>().target).key;
      txOutDetails.output = txOutToKeyDetails;
    } else if (txOutput.get<0>().target.type() == typeid(MultisignatureOutput)) {
      TransactionOutputMultisignatureDetails txOutMultisigDetails;
      MultisignatureOutput txOutMultisig = boost::get<MultisignatureOutput>(txOutput.get<0>().target);
      txOutMultisigDetails.keys.reserve(txOutMultisig.keys.size());
      for (const Crypto::PublicKey& key : txOutMultisig.keys) {
        txOutMultisigDetails.keys.push_back(std::move(key));
      }
      txOutMultisigDetails.requiredSignatures = txOutMultisig.requiredSignatureCount;
      txOutDetails.output = txOutMultisigDetails;
    } else {
      return false;
    }
    transactionDetails.outputs.push_back(std::move(txOutDetails));
  }

  return true;
}

}
