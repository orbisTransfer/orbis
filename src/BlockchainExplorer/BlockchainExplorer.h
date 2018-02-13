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

#include <mutex>
#include <atomic>
#include <unordered_set>

#include "IBlockchainExplorer.h"
#include "INode.h"

#include "Common/ObserverManager.h"
#include "BlockchainExplorerErrors.h"

#include "Wallet/WalletAsyncContextCounter.h"

#include "Logging/LoggerRef.h"

namespace Orbis {

class BlockchainExplorer : public IBlockchainExplorer, public INodeObserver {
public:
  BlockchainExplorer(INode& node, Logging::ILogger& logger);

  BlockchainExplorer(const BlockchainExplorer&) = delete;
  BlockchainExplorer(BlockchainExplorer&&) = delete;

  BlockchainExplorer& operator=(const BlockchainExplorer&) = delete;
  BlockchainExplorer& operator=(BlockchainExplorer&&) = delete;

  virtual ~BlockchainExplorer();
    
  virtual bool addObserver(IBlockchainObserver* observer) override;
  virtual bool removeObserver(IBlockchainObserver* observer) override;

  virtual bool getBlocks(const std::vector<uint32_t>& blockHeights, std::vector<std::vector<BlockDetails>>& blocks) override;
  virtual bool getBlocks(const std::vector<Crypto::Hash>& blockHashes, std::vector<BlockDetails>& blocks) override;
  virtual bool getBlocks(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t blocksNumberLimit, std::vector<BlockDetails>& blocks, uint32_t& blocksNumberWithinTimestamps) override;

  virtual bool getBlockchainTop(BlockDetails& topBlock) override;

  virtual bool getTransactions(const std::vector<Crypto::Hash>& transactionHashes, std::vector<TransactionDetails>& transactions) override;
  virtual bool getTransactionsByPaymentId(const Crypto::Hash& paymentId, std::vector<TransactionDetails>& transactions) override;
  virtual bool getPoolTransactions(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t transactionsNumberLimit, std::vector<TransactionDetails>& transactions, uint64_t& transactionsNumberWithinTimestamps) override;
  virtual bool getPoolState(const std::vector<Crypto::Hash>& knownPoolTransactionHashes, Crypto::Hash knownBlockchainTop, bool& isBlockchainActual, std::vector<TransactionDetails>& newTransactions, std::vector<Crypto::Hash>& removedTransactions) override;

  virtual uint64_t getRewardBlocksWindow() override;
  virtual uint64_t getFullRewardMaxBlockSize(uint8_t majorVersion) override;

  virtual bool isSynchronized() override;

  virtual void init() override;
  virtual void shutdown() override;

  virtual void poolChanged() override;
  virtual void blockchainSynchronized(uint32_t topHeight) override;
  virtual void localBlockchainUpdated(uint32_t height) override;

  typedef WalletAsyncContextCounter AsyncContextCounter;

private:
  void poolUpdateEndHandler();

  class PoolUpdateGuard {
  public:
    PoolUpdateGuard();

    bool beginUpdate();
    bool endUpdate();

  private:
    enum class State {
      NONE,
      UPDATING,
      UPDATE_REQUIRED
    };

    std::atomic<State> m_state;
  };

  enum State {
    NOT_INITIALIZED,
    INITIALIZED
  };

  BlockDetails knownBlockchainTop;
  uint32_t knownBlockchainTopHeight;
  std::unordered_set<Crypto::Hash> knownPoolState;

  std::atomic<State> state;
  std::atomic<bool> synchronized;
  std::atomic<uint32_t> observersCounter;
  Tools::ObserverManager<IBlockchainObserver> observerManager;

  std::mutex mutex;

  INode& node;
  Logging::LoggerRef logger;

  AsyncContextCounter asyncContextCounter;
  PoolUpdateGuard poolUpdateGuard;
};
}
