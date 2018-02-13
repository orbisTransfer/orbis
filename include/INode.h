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
#include <functional>
#include <system_error>
#include <vector>

#include "orbis/orbis.h"
#include "OrbisCore/OrbisBasic.h"
#include "OrbisProtocol/OrbisProtocolDefinitions.h"
#include "Rpc/CoreRpcServerCommandsDefinitions.h"

#include "BlockchainExplorerData.h"
#include "ITransaction.h"

namespace Orbis {

class INodeObserver {
public:
  virtual ~INodeObserver() {}
  virtual void peerCountUpdated(size_t count) {}
  virtual void localBlockchainUpdated(uint32_t height) {}
  virtual void lastKnownBlockHeightUpdated(uint32_t height) {}
  virtual void poolChanged() {}
  virtual void blockchainSynchronized(uint32_t topHeight) {}
};

struct OutEntry {
  uint32_t outGlobalIndex;
  Crypto::PublicKey outKey;
};

struct OutsForAmount {
  uint64_t amount;
  std::vector<OutEntry> outs;
};

struct TransactionShortInfo {
  Crypto::Hash txId;
  TransactionPrefix txPrefix;
};

struct BlockShortEntry {
  Crypto::Hash blockHash;
  bool hasBlock;
  Orbis::Block block;
  std::vector<TransactionShortInfo> txsShortInfo;
};

class INode {
public:
  typedef std::function<void(std::error_code)> Callback;

  virtual ~INode() {}
  virtual bool addObserver(INodeObserver* observer) = 0;
  virtual bool removeObserver(INodeObserver* observer) = 0;

  virtual void init(const Callback& callback) = 0;
  virtual bool shutdown() = 0;

  virtual size_t getPeerCount() const = 0;
  virtual uint32_t getLastLocalBlockHeight() const = 0;
  virtual uint32_t getLastKnownBlockHeight() const = 0;
  virtual uint32_t getLocalBlockCount() const = 0;
  virtual uint32_t getKnownBlockCount() const = 0;
  virtual uint64_t getLastLocalBlockTimestamp() const = 0;

  virtual void relayTransaction(const Transaction& transaction, const Callback& callback) = 0;
  virtual void getRandomOutsByAmounts(std::vector<uint64_t>&& amounts, uint64_t outsCount, std::vector<Orbis::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& result, const Callback& callback) = 0;
  virtual void getNewBlocks(std::vector<Crypto::Hash>&& knownBlockIds, std::vector<Orbis::block_complete_entry>& newBlocks, uint32_t& startHeight, const Callback& callback) = 0;
  virtual void getTransactionOutsGlobalIndices(const Crypto::Hash& transactionHash, std::vector<uint32_t>& outsGlobalIndices, const Callback& callback) = 0;
  virtual void queryBlocks(std::vector<Crypto::Hash>&& knownBlockIds, uint64_t timestamp, std::vector<BlockShortEntry>& newBlocks, uint32_t& startHeight, const Callback& callback) = 0;
  virtual void getPoolSymmetricDifference(std::vector<Crypto::Hash>&& knownPoolTxIds, Crypto::Hash knownBlockId, bool& isBcActual, std::vector<std::unique_ptr<ITransactionReader>>& newTxs, std::vector<Crypto::Hash>& deletedTxIds, const Callback& callback) = 0;
  virtual void getMultisignatureOutputByGlobalIndex(uint64_t amount, uint32_t gindex, MultisignatureOutput& out, const Callback& callback) = 0;

  virtual void getBlocks(const std::vector<uint32_t>& blockHeights, std::vector<std::vector<BlockDetails>>& blocks, const Callback& callback) = 0;
  virtual void getBlocks(const std::vector<Crypto::Hash>& blockHashes, std::vector<BlockDetails>& blocks, const Callback& callback) = 0;
  virtual void getBlocks(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t blocksNumberLimit, std::vector<BlockDetails>& blocks, uint32_t& blocksNumberWithinTimestamps, const Callback& callback) = 0;
  virtual void getTransactions(const std::vector<Crypto::Hash>& transactionHashes, std::vector<TransactionDetails>& transactions, const Callback& callback) = 0;
  virtual void getTransactionsByPaymentId(const Crypto::Hash& paymentId, std::vector<TransactionDetails>& transactions, const Callback& callback) = 0;
  virtual void getPoolTransactions(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t transactionsNumberLimit, std::vector<TransactionDetails>& transactions, uint64_t& transactionsNumberWithinTimestamps, const Callback& callback) = 0;
  virtual void isSynchronized(bool& syncStatus, const Callback& callback) = 0;
};

}
