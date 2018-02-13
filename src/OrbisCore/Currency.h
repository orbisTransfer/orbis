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
#include <string>
#include <vector>
#include <boost/utility.hpp>
#include "../OrbisConfig.h"
#include "../orbis/hash.h"
#include "../Logging/LoggerRef.h"
#include "OrbisBasic.h"
#include "Difficulty.h"

namespace Orbis {

class AccountBase;

class Currency {
public:
  uint64_t maxBlockHeight() const { return m_maxBlockHeight; }
  size_t maxBlockBlobSize() const { return m_maxBlockBlobSize; }
  size_t maxTxSize() const { return m_maxTxSize; }
  uint64_t publicAddressBase58Prefix() const { return m_publicAddressBase58Prefix; }
  size_t minedMoneyUnlockWindow() const { return m_minedMoneyUnlockWindow; }

  size_t timestampCheckWindow() const { return m_timestampCheckWindow; }
  uint64_t blockFutureTimeLimit() const { return m_blockFutureTimeLimit; }

  uint64_t moneySupply() const { return m_moneySupply; }
  unsigned int emissionSpeedFactor() const { return m_emissionSpeedFactor; }

  size_t rewardBlocksWindow() const { return m_rewardBlocksWindow; }
  size_t blockGrantedFullRewardZone() const { return m_blockGrantedFullRewardZone; }
  size_t minerTxBlobReservedSize() const { return m_minerTxBlobReservedSize; }

  size_t numberOfDecimalPlaces() const { return m_numberOfDecimalPlaces; }
  uint64_t coin() const { return m_coin; }

  uint64_t minimumFee() const { return m_mininumFee; }
  uint64_t defaultDustThreshold() const { return m_defaultDustThreshold; }

  uint64_t difficultyTarget() const { return m_difficultyTarget; }
  size_t difficultyWindow() const { return m_difficultyWindow; }
  size_t difficultyLag() const { return m_difficultyLag; }
  size_t difficultyCut() const { return m_difficultyCut; }
  size_t difficultyBlocksCount() const { return m_difficultyWindow + m_difficultyLag; }

  size_t maxBlockSizeInitial() const { return m_maxBlockSizeInitial; }
  uint64_t maxBlockSizeGrowthSpeedNumerator() const { return m_maxBlockSizeGrowthSpeedNumerator; }
  uint64_t maxBlockSizeGrowthSpeedDenominator() const { return m_maxBlockSizeGrowthSpeedDenominator; }

  uint64_t lockedTxAllowedDeltaSeconds() const { return m_lockedTxAllowedDeltaSeconds; }
  size_t lockedTxAllowedDeltaBlocks() const { return m_lockedTxAllowedDeltaBlocks; }

  uint64_t mempoolTxLiveTime() const { return m_mempoolTxLiveTime; }
  uint64_t mempoolTxFromAltBlockLiveTime() const { return m_mempoolTxFromAltBlockLiveTime; }
  uint64_t numberOfPeriodsToForgetTxDeletedFromPool() const { return m_numberOfPeriodsToForgetTxDeletedFromPool; }

  size_t fusionTxMaxSize() const { return m_fusionTxMaxSize; }
  size_t fusionTxMinInputCount() const { return m_fusionTxMinInputCount; }
  size_t fusionTxMinInOutCountRatio() const { return m_fusionTxMinInOutCountRatio; }

  const std::string& blocksFileName() const { return m_blocksFileName; }
  const std::string& blocksCacheFileName() const { return m_blocksCacheFileName; }
  const std::string& blockIndexesFileName() const { return m_blockIndexesFileName; }
  const std::string& txPoolFileName() const { return m_txPoolFileName; }
  const std::string& blockchinIndicesFileName() const { return m_blockchinIndicesFileName; }

  bool isTestnet() const { return m_testnet; }

  const Block& genesisBlock() const { return m_genesisBlock; }
  const Crypto::Hash& genesisBlockHash() const { return m_genesisBlockHash; }

  bool getBlockReward(size_t medianSize, size_t currentBlockSize, uint64_t alreadyGeneratedCoins, uint64_t fee,
    uint64_t& reward, int64_t& emissionChange) const;
  size_t maxBlockCumulativeSize(uint64_t height) const;

  bool constructMinerTx(uint32_t height, size_t medianSize, uint64_t alreadyGeneratedCoins, size_t currentBlockSize,
    uint64_t fee, const AccountPublicAddress& minerAddress, Transaction& tx,
    const BinaryArray& extraNonce = BinaryArray(), size_t maxOuts = 1) const;

  bool isFusionTransaction(const Transaction& transaction) const;
  bool isFusionTransaction(const Transaction& transaction, size_t size) const;
  bool isFusionTransaction(const std::vector<uint64_t>& inputsAmounts, const std::vector<uint64_t>& outputsAmounts, size_t size) const;
  bool isAmountApplicableInFusionTransactionInput(uint64_t amount, uint64_t threshold) const;
  bool isAmountApplicableInFusionTransactionInput(uint64_t amount, uint64_t threshold, uint8_t& amountPowerOfTen) const;

  std::string accountAddressAsString(const AccountBase& account) const;
  std::string accountAddressAsString(const AccountPublicAddress& accountPublicAddress) const;
  bool parseAccountAddressString(const std::string& str, AccountPublicAddress& addr) const;

  std::string formatAmount(uint64_t amount) const;
  std::string formatAmount(int64_t amount) const;
  bool parseAmount(const std::string& str, uint64_t& amount) const;

  difficulty_type nextDifficulty(std::vector<uint64_t> timestamps, std::vector<difficulty_type> cumulativeDifficulties) const;
  bool checkProofOfWork(Crypto::cn_context& context, const Block& block, difficulty_type currentDiffic, Crypto::Hash& proofOfWork) const;

  size_t getApproximateMaximumInputCount(size_t transactionSize, size_t outputCount, size_t mixinCount) const;

private:
  Currency(Logging::ILogger& log) : logger(log, "currency") {
  }

  bool init();

  bool generateGenesisBlock();

private:
  uint64_t m_maxBlockHeight;
  size_t m_maxBlockBlobSize;
  size_t m_maxTxSize;
  uint64_t m_publicAddressBase58Prefix;
  size_t m_minedMoneyUnlockWindow;

  size_t m_timestampCheckWindow;
  uint64_t m_blockFutureTimeLimit;

  uint64_t m_moneySupply;
  unsigned int m_emissionSpeedFactor;

  size_t m_rewardBlocksWindow;
  size_t m_blockGrantedFullRewardZone;
  size_t m_minerTxBlobReservedSize;

  size_t m_numberOfDecimalPlaces;
  uint64_t m_coin;

  uint64_t m_mininumFee;
  uint64_t m_defaultDustThreshold;

  uint64_t m_difficultyTarget;
  size_t m_difficultyWindow;
  size_t m_difficultyLag;
  size_t m_difficultyCut;

  size_t m_maxBlockSizeInitial;
  uint64_t m_maxBlockSizeGrowthSpeedNumerator;
  uint64_t m_maxBlockSizeGrowthSpeedDenominator;

  uint64_t m_lockedTxAllowedDeltaSeconds;
  size_t m_lockedTxAllowedDeltaBlocks;

  uint64_t m_mempoolTxLiveTime;
  uint64_t m_mempoolTxFromAltBlockLiveTime;
  uint64_t m_numberOfPeriodsToForgetTxDeletedFromPool;

  size_t m_fusionTxMaxSize;
  size_t m_fusionTxMinInputCount;
  size_t m_fusionTxMinInOutCountRatio;

  std::string m_blocksFileName;
  std::string m_blocksCacheFileName;
  std::string m_blockIndexesFileName;
  std::string m_txPoolFileName;
  std::string m_blockchinIndicesFileName;

  static const std::vector<uint64_t> PRETTY_AMOUNTS;

  bool m_testnet;

  Block m_genesisBlock;
  Crypto::Hash m_genesisBlockHash;

  Logging::LoggerRef logger;

  friend class CurrencyBuilder;
};

class CurrencyBuilder : boost::noncopyable {
public:
  CurrencyBuilder(Logging::ILogger& log);

  Currency currency() {
    if (!m_currency.init()) {
      throw std::runtime_error("Failed to initialize currency object");
    }
    return m_currency;
  }

  Transaction generateGenesisTransaction();

  CurrencyBuilder& maxBlockNumber(uint64_t val) { m_currency.m_maxBlockHeight = val; return *this; }
  CurrencyBuilder& maxBlockBlobSize(size_t val) { m_currency.m_maxBlockBlobSize = val; return *this; }
  CurrencyBuilder& maxTxSize(size_t val) { m_currency.m_maxTxSize = val; return *this; }
  CurrencyBuilder& publicAddressBase58Prefix(uint64_t val) { m_currency.m_publicAddressBase58Prefix = val; return *this; }
  CurrencyBuilder& minedMoneyUnlockWindow(size_t val) { m_currency.m_minedMoneyUnlockWindow = val; return *this; }

  CurrencyBuilder& timestampCheckWindow(size_t val) { m_currency.m_timestampCheckWindow = val; return *this; }
  CurrencyBuilder& blockFutureTimeLimit(uint64_t val) { m_currency.m_blockFutureTimeLimit = val; return *this; }

  CurrencyBuilder& moneySupply(uint64_t val) { m_currency.m_moneySupply = val; return *this; }
  CurrencyBuilder& emissionSpeedFactor(unsigned int val);

  CurrencyBuilder& rewardBlocksWindow(size_t val) { m_currency.m_rewardBlocksWindow = val; return *this; }
  CurrencyBuilder& blockGrantedFullRewardZone(size_t val) { m_currency.m_blockGrantedFullRewardZone = val; return *this; }
  CurrencyBuilder& minerTxBlobReservedSize(size_t val) { m_currency.m_minerTxBlobReservedSize = val; return *this; }

  CurrencyBuilder& numberOfDecimalPlaces(size_t val);

  CurrencyBuilder& mininumFee(uint64_t val) { m_currency.m_mininumFee = val; return *this; }
  CurrencyBuilder& defaultDustThreshold(uint64_t val) { m_currency.m_defaultDustThreshold = val; return *this; }

  CurrencyBuilder& difficultyTarget(uint64_t val) { m_currency.m_difficultyTarget = val; return *this; }
  CurrencyBuilder& difficultyWindow(size_t val);
  CurrencyBuilder& difficultyLag(size_t val) { m_currency.m_difficultyLag = val; return *this; }
  CurrencyBuilder& difficultyCut(size_t val) { m_currency.m_difficultyCut = val; return *this; }

  CurrencyBuilder& maxBlockSizeInitial(size_t val) { m_currency.m_maxBlockSizeInitial = val; return *this; }
  CurrencyBuilder& maxBlockSizeGrowthSpeedNumerator(uint64_t val) { m_currency.m_maxBlockSizeGrowthSpeedNumerator = val; return *this; }
  CurrencyBuilder& maxBlockSizeGrowthSpeedDenominator(uint64_t val) { m_currency.m_maxBlockSizeGrowthSpeedDenominator = val; return *this; }

  CurrencyBuilder& lockedTxAllowedDeltaSeconds(uint64_t val) { m_currency.m_lockedTxAllowedDeltaSeconds = val; return *this; }
  CurrencyBuilder& lockedTxAllowedDeltaBlocks(size_t val) { m_currency.m_lockedTxAllowedDeltaBlocks = val; return *this; }

  CurrencyBuilder& mempoolTxLiveTime(uint64_t val) { m_currency.m_mempoolTxLiveTime = val; return *this; }
  CurrencyBuilder& mempoolTxFromAltBlockLiveTime(uint64_t val) { m_currency.m_mempoolTxFromAltBlockLiveTime = val; return *this; }
  CurrencyBuilder& numberOfPeriodsToForgetTxDeletedFromPool(uint64_t val) { m_currency.m_numberOfPeriodsToForgetTxDeletedFromPool = val; return *this; }

  CurrencyBuilder& fusionTxMaxSize(size_t val) { m_currency.m_fusionTxMaxSize = val; return *this; }
  CurrencyBuilder& fusionTxMinInputCount(size_t val) { m_currency.m_fusionTxMinInputCount = val; return *this; }
  CurrencyBuilder& fusionTxMinInOutCountRatio(size_t val) { m_currency.m_fusionTxMinInOutCountRatio = val; return *this; }

  CurrencyBuilder& blocksFileName(const std::string& val) { m_currency.m_blocksFileName = val; return *this; }
  CurrencyBuilder& blocksCacheFileName(const std::string& val) { m_currency.m_blocksCacheFileName = val; return *this; }
  CurrencyBuilder& blockIndexesFileName(const std::string& val) { m_currency.m_blockIndexesFileName = val; return *this; }
  CurrencyBuilder& txPoolFileName(const std::string& val) { m_currency.m_txPoolFileName = val; return *this; }
  CurrencyBuilder& blockchinIndicesFileName(const std::string& val) { m_currency.m_blockchinIndicesFileName = val; return *this; }
  
  CurrencyBuilder& testnet(bool val) { m_currency.m_testnet = val; return *this; }

private:
  Currency m_currency;
};

}
