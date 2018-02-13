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
#include "gtest/gtest.h"

#include <thread>
#include <iostream>

#include "System/ContextGroup.h"
#include "System/Dispatcher.h"
#include "System/Event.h"

#include "OrbisCore/MessageQueue.h"
#include "OrbisCore/BlockchainMessages.h"
#include "OrbisCore/IntrusiveLinkedList.h"
#include "OrbisCore/OrbisTools.h"

using namespace Orbis;

class MessageQueueTest : public testing::Test {
public:
  MessageQueueTest() : contextGroup(dispatcher) {}

  bool addMessageQueue(MessageQueue<BlockchainMessage>&  messageQueue);
  bool removeMessageQueue(MessageQueue<BlockchainMessage>& messageQueue);

  void sendBlockchainMessage(const BlockchainMessage& message);
  void interruptBlockchainMessageWaiting();

  void SetUp() override;
  void TearDown() override;

protected:
  System::Dispatcher dispatcher;
  System::ContextGroup contextGroup;
  IntrusiveLinkedList<MessageQueue<BlockchainMessage>> blockchainMessageQueueList;
};

bool MessageQueueTest::addMessageQueue(MessageQueue<BlockchainMessage>& messageQueue) {
  return blockchainMessageQueueList.insert(messageQueue);
}

bool MessageQueueTest::removeMessageQueue(MessageQueue<BlockchainMessage>& messageQueue) {
  return blockchainMessageQueueList.remove(messageQueue);
}

void MessageQueueTest::sendBlockchainMessage(const BlockchainMessage& message) {
  for (IntrusiveLinkedList<MessageQueue<BlockchainMessage>>::iterator iter = blockchainMessageQueueList.begin(); iter != blockchainMessageQueueList.end(); ++iter) {
    iter->push(message);
  }
}

void MessageQueueTest::interruptBlockchainMessageWaiting() {
  for (IntrusiveLinkedList<MessageQueue<BlockchainMessage>>::iterator iter = blockchainMessageQueueList.begin(); iter != blockchainMessageQueueList.end(); ++iter) {
    iter->stop();
  }
}

void MessageQueueTest::SetUp() {
  ASSERT_TRUE(blockchainMessageQueueList.empty());
}

void MessageQueueTest::TearDown() {
  ASSERT_TRUE(blockchainMessageQueueList.empty());
}

TEST_F(MessageQueueTest, singleNewBlockMessage) {
  MessageQueue<BlockchainMessage> queue(dispatcher);
  MesageQueueGuard<MessageQueueTest, BlockchainMessage> guard(*this, queue);

  Crypto::Hash randomHash;
  for (uint8_t& i : randomHash.data) {
    i = rand();
  }


  contextGroup.spawn([&]() {
    const BlockchainMessage& m = queue.front();
    ASSERT_EQ(m.getType(), BlockchainMessage::MessageType::NEW_BLOCK_MESSAGE);
    Crypto::Hash h;
    ASSERT_TRUE(m.getNewBlockHash(h));
    ASSERT_EQ(h, randomHash);
    ASSERT_NO_THROW(queue.pop());
  });

  ASSERT_NO_THROW(sendBlockchainMessage(BlockchainMessage(NewBlockMessage(randomHash))));

  contextGroup.wait();
}

TEST_F(MessageQueueTest, singleNewAlternativeBlockMessage) {
  MessageQueue<BlockchainMessage> queue(dispatcher);
  MesageQueueGuard<MessageQueueTest, BlockchainMessage> guard(*this, queue);

  Crypto::Hash randomHash;
  for (uint8_t& i : randomHash.data) {
    i = rand();
  }

  contextGroup.spawn([&]() {
    const BlockchainMessage& m = queue.front();
    ASSERT_EQ(m.getType(), BlockchainMessage::MessageType::NEW_ALTERNATIVE_BLOCK_MESSAGE);
    Crypto::Hash h;
    ASSERT_TRUE(m.getNewAlternativeBlockHash(h));
    ASSERT_EQ(h, randomHash);
    ASSERT_NO_THROW(queue.pop());
  });

  ASSERT_NO_THROW(sendBlockchainMessage(BlockchainMessage(NewAlternativeBlockMessage(randomHash))));

  contextGroup.wait();
}

TEST_F(MessageQueueTest, singleChainSwitchMessage) {
  MessageQueue<BlockchainMessage> queue(dispatcher);
  MesageQueueGuard<MessageQueueTest, BlockchainMessage> guard(*this, queue);

  const size_t NUMBER_OF_BLOCKS = 10;
  std::vector<Crypto::Hash> randomHashes;
  for (size_t i = 0; i < NUMBER_OF_BLOCKS; ++i) {
    Crypto::Hash randomHash;
    for (uint8_t& j : randomHash.data) {
      j = rand();
    }
    randomHashes.push_back(randomHash);
  }

  contextGroup.spawn([&]() {
    const BlockchainMessage& m = queue.front();
    ASSERT_EQ(m.getType(), BlockchainMessage::MessageType::CHAIN_SWITCH_MESSAGE);
    std::vector<Crypto::Hash> res;
    ASSERT_TRUE(m.getChainSwitch(res));
    ASSERT_EQ(res, randomHashes);
    ASSERT_NO_THROW(queue.pop());
  });


  std::vector<Crypto::Hash> copy = randomHashes;
  ASSERT_NO_THROW(sendBlockchainMessage(BlockchainMessage(ChainSwitchMessage(std::move(copy)))));

  contextGroup.wait();
}

TEST_F(MessageQueueTest, manyMessagesOneListener) {
  MessageQueue<BlockchainMessage> queue(dispatcher);
  MesageQueueGuard<MessageQueueTest, BlockchainMessage> guard(*this, queue);

  const size_t NUMBER_OF_BLOCKS = 10;
  std::vector<Crypto::Hash> randomHashes;
  for (size_t i = 0; i < NUMBER_OF_BLOCKS; ++i) {
    Crypto::Hash randomHash;
    for (uint8_t& j : randomHash.data) {
      j = rand();
    }
    randomHashes.push_back(randomHash);
  }

  contextGroup.spawn([&]() {
    for (size_t i = 0; i < NUMBER_OF_BLOCKS; ++i) {
      const BlockchainMessage& m = queue.front();
      ASSERT_EQ(m.getType(), BlockchainMessage::MessageType::NEW_BLOCK_MESSAGE);
      Crypto::Hash h;
      ASSERT_TRUE(m.getNewBlockHash(h));
      ASSERT_EQ(h, randomHashes[i]);
      ASSERT_NO_THROW(queue.pop());
    }
  });

  for (auto h : randomHashes) {
    ASSERT_NO_THROW(sendBlockchainMessage(BlockchainMessage(NewBlockMessage(h))));
  }

  contextGroup.wait();
}

TEST_F(MessageQueueTest, manyMessagesManyListeners) {
  const size_t NUMBER_OF_LISTENERS = 5;
  std::array<std::unique_ptr<MessageQueue<BlockchainMessage>>, NUMBER_OF_LISTENERS> queues;
  std::array<std::unique_ptr<MesageQueueGuard<MessageQueueTest, BlockchainMessage>>, NUMBER_OF_LISTENERS> quards;

  for (size_t i = 0; i < NUMBER_OF_LISTENERS; ++i) {
    queues[i] = std::unique_ptr<MessageQueue<BlockchainMessage>>(new MessageQueue<BlockchainMessage>(dispatcher));
    quards[i] = std::unique_ptr<MesageQueueGuard<MessageQueueTest, BlockchainMessage>>(new MesageQueueGuard<MessageQueueTest, BlockchainMessage>(*this, *queues[i]));
  }

  const size_t NUMBER_OF_BLOCKS = 10;
  std::vector<Crypto::Hash> randomHashes;
  for (size_t i = 0; i < NUMBER_OF_BLOCKS; ++i) {
    Crypto::Hash randomHash;
    for (uint8_t& j : randomHash.data) {
      j = rand();
    }
    randomHashes.push_back(randomHash);
  }

  contextGroup.spawn([&]() {
    for (size_t i = 0; i < NUMBER_OF_LISTENERS; ++i) {
      for (size_t j = 0; j < NUMBER_OF_BLOCKS; ++j) {
        const BlockchainMessage& m = queues[i]->front();
        ASSERT_EQ(m.getType(), BlockchainMessage::MessageType::NEW_BLOCK_MESSAGE);
        Crypto::Hash h;
        ASSERT_TRUE(m.getNewBlockHash(h));
        ASSERT_EQ(h, randomHashes[j]);
        ASSERT_NO_THROW(queues[i]->pop());
      }
    }
  });


  for (auto h : randomHashes) {
    ASSERT_NO_THROW(sendBlockchainMessage(BlockchainMessage(NewBlockMessage(h))));
  }

  contextGroup.wait();
}

TEST_F(MessageQueueTest, interruptWaiting) {
  const size_t NUMBER_OF_LISTENERS = 5;
  std::array<std::unique_ptr<MessageQueue<BlockchainMessage>>, NUMBER_OF_LISTENERS> queues;
  std::array<std::unique_ptr<MesageQueueGuard<MessageQueueTest, BlockchainMessage>>, NUMBER_OF_LISTENERS> quards;

  for (size_t i = 0; i < NUMBER_OF_LISTENERS; ++i) {
    queues[i] = std::unique_ptr<MessageQueue<BlockchainMessage>>(new MessageQueue<BlockchainMessage>(dispatcher));
    quards[i] = std::unique_ptr<MesageQueueGuard<MessageQueueTest, BlockchainMessage>>(new MesageQueueGuard<MessageQueueTest, BlockchainMessage>(*this, *queues[i]));
  }

  const size_t NUMBER_OF_BLOCKS = 10;
  std::vector<Crypto::Hash> randomHashes;
  for (size_t i = 0; i < NUMBER_OF_BLOCKS; ++i) {
    Crypto::Hash randomHash;
    for (uint8_t& j : randomHash.data) {
      j = rand();
    }
    randomHashes.push_back(randomHash);
  }

  System::Event shutdownEvent(dispatcher);
  contextGroup.spawn([&]() {
    shutdownEvent.wait();
    for (size_t i = 0; i < NUMBER_OF_LISTENERS; ++i) {
      for (size_t j = 0; j < NUMBER_OF_BLOCKS; ++j) {
        const BlockchainMessage& m = queues[i]->front();
        ASSERT_EQ(m.getType(), BlockchainMessage::MessageType::NEW_BLOCK_MESSAGE);
        Crypto::Hash h;
        ASSERT_TRUE(m.getNewBlockHash(h));
        ASSERT_EQ(h, randomHashes[j]);
        ASSERT_NO_THROW(queues[i]->pop());
      }
    }

    for (size_t i = 0; i < NUMBER_OF_LISTENERS; ++i) {
      for (size_t j = 0; j < NUMBER_OF_BLOCKS; ++j) {
        ASSERT_ANY_THROW(queues[i]->front());
        ASSERT_ANY_THROW(queues[i]->pop());
      }
    }

  });

  for (auto h : randomHashes) {
    ASSERT_NO_THROW(sendBlockchainMessage(BlockchainMessage(NewBlockMessage(h))));
  }

  interruptBlockchainMessageWaiting();

  shutdownEvent.set();

  contextGroup.wait();
}

TEST_F(MessageQueueTest, doubleAddQueueToList) {
  MessageQueue<BlockchainMessage> queue(dispatcher);
  ASSERT_TRUE(blockchainMessageQueueList.insert(queue));
  ASSERT_FALSE(blockchainMessageQueueList.insert(queue));

  ASSERT_TRUE(blockchainMessageQueueList.remove(queue));
  ASSERT_FALSE(blockchainMessageQueueList.remove(queue));
}
