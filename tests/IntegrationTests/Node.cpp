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
#include <gtest/gtest.h>

#include <fstream>

#include <Common/StringTools.h>
#include <System/Dispatcher.h>
#include <System/Timer.h>

#include <Serialization/JsonOutputStreamSerializer.h>
#include <Serialization/JsonInputStreamSerializer.h>
#include <Serialization/SerializationOverloads.h>

#include "OrbisCore/Currency.h"
#include "OrbisCore/OrbisTools.h"
#include "Wallet/WalletGreen.h"

#include "../IntegrationTestLib/TestNetwork.h"
#include "../IntegrationTestLib/NodeObserver.h"
#include "../IntegrationTestLib/NodeCallback.h"

#include "BaseTests.h"

using namespace Tests;
using namespace Orbis;

namespace Orbis {
  void serialize(BlockShortEntry& v, ISerializer& s) {
    s(v.blockHash, "hash");
    
    if (s.type() == ISerializer::INPUT) {
      std::string blockBinary;
      if (s.binary(blockBinary, "block")) {
        fromBinaryArray(v.block, Common::asBinaryArray(blockBinary));
        v.hasBlock = true;
      }
    } else {
      if (v.hasBlock) {
        std::string blockBinary(Common::asString(toBinaryArray(v.block)));
        s.binary(blockBinary, "block");
      }
    }

    s(v.txsShortInfo, "transactions");
  }

  void serialize(TransactionShortInfo& v, ISerializer& s) {
    s(v.txId, "hash");
    s(v.txPrefix, "prefix");
  }

  bool operator == (const BlockShortEntry& a, const BlockShortEntry& b) {
    return
      a.blockHash == b.blockHash &&
      a.hasBlock == b.hasBlock &&
      // a.block == b.block &&
      a.txsShortInfo == b.txsShortInfo;
  }

  bool operator == (const TransactionShortInfo& a, const TransactionShortInfo& b) {
    return a.txId == b.txId;
  }
}

struct BlockchainInfo {
  std::list<BlockShortEntry> blocks;
  std::unordered_map<Crypto::Hash, std::vector<uint32_t>> globalOutputs;

  bool operator == (const BlockchainInfo& other) const {
    return blocks == other.blocks && globalOutputs == other.globalOutputs;
  }

  void serialize(ISerializer& s) {
    s(blocks, "blocks");
    s(globalOutputs, "outputs");
  }
};

void storeBlockchainInfo(const std::string& filename, BlockchainInfo& bc) {
  JsonOutputStreamSerializer s;
  serialize(bc, s);

  std::ofstream jsonBlocks(filename, std::ios::trunc);
  jsonBlocks << s.getValue();
}

void loadBlockchainInfo(const std::string& filename, BlockchainInfo& bc) {
  std::ifstream jsonBlocks(filename);
  JsonInputStreamSerializer s(jsonBlocks);
  serialize(bc, s);
}


class NodeTest: public BaseTest {

protected:

  void startNetworkWithBlockchain(const std::string& sourcePath);
  void readBlockchainInfo(INode& node, BlockchainInfo& bc);
  void dumpBlockchainInfo(INode& node);
};

void NodeTest::startNetworkWithBlockchain(const std::string& sourcePath) {
  auto networkCfg = TestNetworkBuilder(2, Topology::Ring).build();

  for (auto& node : networkCfg) {
    node.blockchainLocation = sourcePath;
  }

  network.addNodes(networkCfg);
  network.waitNodesReady();
}

void NodeTest::readBlockchainInfo(INode& node, BlockchainInfo& bc) {
  std::vector<Crypto::Hash> history = { currency.genesisBlockHash() };
  uint64_t timestamp = 0;
  uint32_t startHeight = 0;
  size_t itemsAdded = 0;
  NodeCallback cb;

  bc.blocks = {
    BlockShortEntry{ currency.genesisBlockHash(), true, currency.genesisBlock() }
  };

  do {
    itemsAdded = 0;
    std::vector<BlockShortEntry> blocks;
    node.queryBlocks(std::vector<Crypto::Hash>(history.rbegin(), history.rend()), timestamp, blocks, startHeight, cb.callback());

    ASSERT_TRUE(cb.get() == std::error_code());

    uint64_t currentHeight = startHeight;

    for (auto& entry : blocks) {

      if (currentHeight < history.size()) {
        // detach no expected
        ASSERT_EQ(entry.blockHash, history[currentHeight]);
      } else {
        auto txHash = getObjectHash(entry.block.baseTransaction);

        std::vector<uint32_t> globalIndices;
        node.getTransactionOutsGlobalIndices(txHash, globalIndices, cb.callback());

        ASSERT_TRUE(!cb.get());

        bc.globalOutputs.insert(std::make_pair(txHash, std::move(globalIndices)));

        bc.blocks.push_back(entry);
        history.push_back(entry.blockHash);
        ++itemsAdded;
      }

      ++currentHeight;
    }
  } while (itemsAdded > 0);
}

void NodeTest::dumpBlockchainInfo(INode& node) {
  BlockchainInfo bc;
  ASSERT_NO_FATAL_FAILURE(readBlockchainInfo(node, bc));
  storeBlockchainInfo("blocks.js", bc);
}


TEST_F(NodeTest, generateBlockchain) {
  
  auto networkCfg = TestNetworkBuilder(2, Topology::Ring).build();
  networkCfg[0].cleanupDataDir = false;
  network.addNodes(networkCfg);
  network.waitNodesReady();

  auto& daemon = network.getNode(0);

  {
    std::unique_ptr<INode> mainNode;
    ASSERT_TRUE(daemon.makeINode(mainNode));

    std::string password = "pass";
    Orbis::WalletGreen wallet(dispatcher, currency, *mainNode);

    wallet.initialize(password);

    std::string minerAddress = wallet.createAddress();
    daemon.startMining(1, minerAddress);

    System::Timer timer(dispatcher);

    while (daemon.getLocalHeight() < 300) {
      std::cout << "Waiting for block..." << std::endl;
      timer.sleep(std::chrono::seconds(10));
    }

    daemon.stopMining();

    std::ofstream walletFile("wallet.bin", std::ios::binary | std::ios::trunc);
    wallet.save(walletFile);
    wallet.shutdown();

    dumpBlockchainInfo(*mainNode);
 }
}

TEST_F(NodeTest, dumpBlockchain) {
  startNetworkWithBlockchain("testnet_300");
  auto& daemon = network.getNode(0);
  auto mainNode = network.getNode(0).makeINode();
  dumpBlockchainInfo(*mainNode);
}

TEST_F(NodeTest, addMoreBlocks) {
  auto networkCfg = TestNetworkBuilder(2, Topology::Ring).build();
  networkCfg[0].cleanupDataDir = false;
  networkCfg[0].blockchainLocation = "testnet_300";
  networkCfg[1].blockchainLocation = "testnet_300";
  network.addNodes(networkCfg);
  network.waitNodesReady();

  auto& daemon = network.getNode(0);

  {
    std::unique_ptr<INode> mainNode;
    ASSERT_TRUE(daemon.makeINode(mainNode));

    auto startHeight = daemon.getLocalHeight();

    std::string password = "pass";
    Orbis::WalletGreen wallet(dispatcher, currency, *mainNode);

    {
      std::ifstream walletFile("wallet.bin", std::ios::binary);
      wallet.load(walletFile, password);
    }

    std::string minerAddress = wallet.getAddress(0);
    daemon.startMining(1, minerAddress);

    System::Timer timer(dispatcher);

    while (daemon.getLocalHeight() <= startHeight + 3) {
      std::cout << "Waiting for block..." << std::endl;
      timer.sleep(std::chrono::seconds(1));
    }

    daemon.stopMining();

    std::ofstream walletFile("wallet.bin", std::ios::binary | std::ios::trunc);
    wallet.save(walletFile);
    wallet.shutdown();

    dumpBlockchainInfo(*mainNode);
  }
}


TEST_F(NodeTest, queryBlocks) {
  BlockchainInfo knownBc, nodeBc;

  loadBlockchainInfo("blocks.js", knownBc);

  startNetworkWithBlockchain("testnet_300");
  auto& daemon = network.getNode(0);
  std::unique_ptr<INode> mainNode;

  // check full sync

  ASSERT_TRUE(daemon.makeINode(mainNode));
  ASSERT_NO_FATAL_FAILURE(readBlockchainInfo(*mainNode, nodeBc));
  ASSERT_EQ(knownBc, nodeBc);

  // check query with timestamp

  size_t pivotBlockIndex = knownBc.blocks.size() / 3 * 2;

  auto iter = knownBc.blocks.begin();
  std::advance(iter, pivotBlockIndex);

  ASSERT_TRUE(iter->hasBlock);
  auto timestamp = iter->block.timestamp - 1;
  uint32_t startHeight = 0;
  std::vector<BlockShortEntry> blocks;

  std::cout << "Requesting timestamp: " << timestamp << std::endl;
  
  NodeCallback cb;
  mainNode->queryBlocks({ currency.genesisBlockHash() }, timestamp, blocks, startHeight, cb.callback());
  ASSERT_TRUE(!cb.get());

  EXPECT_EQ(0, startHeight);
  EXPECT_EQ(knownBc.blocks.begin()->blockHash, blocks.begin()->blockHash);
  EXPECT_EQ(knownBc.blocks.size(), blocks.size());

  auto startBlockIter = std::find_if(blocks.begin(), blocks.end(), [](const BlockShortEntry& e) { return e.hasBlock; });
  ASSERT_TRUE(startBlockIter != blocks.end());

  const Block& startBlock = startBlockIter->block;

  std::cout << "Starting block timestamp: " << startBlock.timestamp << std::endl;
  auto startFullIndex = std::distance(blocks.begin(), startBlockIter);
  EXPECT_EQ(pivotBlockIndex, startFullIndex);

  auto it = blocks.begin();
  for (const auto& item : knownBc.blocks) {
    ASSERT_EQ(item.blockHash, it->blockHash);
    ++it;
  }
}


TEST_F(NodeTest, observerHeightNotifications) {
  BlockchainInfo extraBlocks;
  loadBlockchainInfo("blocks_extra.js", extraBlocks);

  startNetworkWithBlockchain("testnet_300");

  auto& daemon = network.getNode(0);

  {
    std::unique_ptr<INode> mainNode;
    daemon.makeINode(mainNode);

    NodeObserver observer(*mainNode);

    std::chrono::seconds timeout(10);
    uint32_t knownHeight = 0;
    uint32_t localHeight = 0;
    size_t peerCount = 0;

    EXPECT_TRUE(observer.m_localHeight.waitFor(timeout, localHeight));
    EXPECT_TRUE(observer.m_knownHeight.waitFor(timeout, knownHeight));
    EXPECT_TRUE(observer.m_peerCount.waitFor(timeout, peerCount));

    EXPECT_GT(localHeight, 0U);
    EXPECT_GT(knownHeight, 0U);
    EXPECT_GT(peerCount, 0U);

    std::cout << "Local height = " << localHeight << std::endl;
    std::cout << "Known height = " << knownHeight << std::endl;
    std::cout << "Peer count = " << peerCount << std::endl;

    EXPECT_EQ(localHeight, mainNode->getLastLocalBlockHeight());
    EXPECT_EQ(knownHeight, mainNode->getLastKnownBlockHeight());

    // submit 1 block and check observer

    uint32_t newKnownHeight = 0;
    uint32_t newLocalHeight = 0;

    auto blockData = toBinaryArray(extraBlocks.blocks.begin()->block);
    ASSERT_TRUE(daemon.submitBlock(Common::toHex(blockData.data(), blockData.size())));

    ASSERT_TRUE(observer.m_localHeight.waitFor(timeout, newLocalHeight));
    ASSERT_TRUE(observer.m_knownHeight.waitFor(timeout, newKnownHeight));

    size_t blocksSubmitted = 1;

    EXPECT_EQ(localHeight + blocksSubmitted, newLocalHeight);
    EXPECT_EQ(knownHeight + blocksSubmitted, newKnownHeight);

    EXPECT_EQ(newLocalHeight, mainNode->getLastLocalBlockHeight());
    EXPECT_EQ(newKnownHeight, mainNode->getLastKnownBlockHeight());

  }
}
