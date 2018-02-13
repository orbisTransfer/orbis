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

#include <numeric>
#include <random>

#include "OrbisCore/TransactionApi.h"
#include "OrbisCore/OrbisFormatUtils.h" // TODO: delete
#include "OrbisCore/Account.h"
#include "orbis/orbis.h"
#include "TransactionApiHelpers.h"

using namespace Orbis;

namespace {
 
  template <size_t size> 
  void fillRandomBytes(std::array<uint8_t, size>& data) {
    for (size_t i = 0; i < size; ++i) {
      data[i] = std::rand() % std::numeric_limits<uint8_t>::max();
    }
  }
  
  template <typename Array>
  Array randomArray() {
    Array a;
    fillRandomBytes(a);
    return a;
  }

  void derivePublicKey(const AccountKeys& reciever, const Crypto::PublicKey& srcTxKey, size_t outputIndex, PublicKey& ephemeralKey) {
    Crypto::KeyDerivation derivation;
    Crypto::generate_key_derivation(srcTxKey, reinterpret_cast<const Crypto::SecretKey&>(reciever.viewSecretKey), derivation);
    Crypto::derive_public_key(derivation, outputIndex, 
      reinterpret_cast<const Crypto::PublicKey&>(reciever.address.spendPublicKey), 
      reinterpret_cast<Crypto::PublicKey&>(ephemeralKey));
  }


  std::unique_ptr<ITransaction> reloadedTx(const std::unique_ptr<ITransaction>& tx) {
    auto txBlob = tx->getTransactionData();
    return createTransaction(txBlob);
  }

  void checkTxReload(const std::unique_ptr<ITransaction>& tx) {
    auto txBlob = tx->getTransactionData();
    auto tx2 = createTransaction(txBlob);
    ASSERT_EQ(tx2->getTransactionData(), txBlob);
  }


  class TransactionApi : public testing::Test {
  protected:
    
    virtual void SetUp() override {
      sender = generateAccountKeys();
      tx = createTransaction();
      txHash = tx->getTransactionHash();
    }

    TransactionTypes::InputKeyInfo createInputInfo(uint64_t amount) {
      TransactionTypes::InputKeyInfo info;

      Orbis::KeyPair srcTxKeys = Orbis::generateKeyPair();

      PublicKey targetKey;

      derivePublicKey(sender, srcTxKeys.publicKey, 5, targetKey);

      TransactionTypes::GlobalOutput gout = { targetKey, 0 };

      info.amount = 1000;
      info.outputs.push_back(gout);

      info.realOutput.transactionIndex = 0;
      info.realOutput.outputInTransaction = 5;
      info.realOutput.transactionPublicKey = reinterpret_cast<const PublicKey&>(srcTxKeys.publicKey);

      return info;
    }

    void checkHashChanged() {
      auto txNewHash = tx->getTransactionHash();
      EXPECT_NE(txHash, txNewHash);
      txHash = txNewHash;
    }

    void checkHashUnchanged() {
      EXPECT_EQ(txHash, tx->getTransactionHash());
    }


    AccountKeys sender;
    std::unique_ptr<ITransaction> tx;
    Hash txHash;
  };

}

TEST_F(TransactionApi, createEmptyReload) {
  auto hash = tx->getTransactionHash();
  auto pk = tx->getTransactionPublicKey();
  checkTxReload(tx);
  // transaction key should not change on reload
  auto reloaded = reloadedTx(tx);
  ASSERT_EQ(pk, reloaded->getTransactionPublicKey());
  ASSERT_EQ(hash, reloaded->getTransactionHash());
}

TEST_F(TransactionApi, addAndSignInput) {
  ASSERT_EQ(0, tx->getInputCount());
  ASSERT_EQ(0, tx->getInputTotalAmount());

  TransactionTypes::InputKeyInfo info = createInputInfo(1000);
  KeyPair ephKeys;
  size_t index = tx->addInput(sender, info, ephKeys);

  ASSERT_EQ(0, index);
  ASSERT_EQ(1, tx->getInputCount());
  ASSERT_EQ(1000, tx->getInputTotalAmount());
  ASSERT_EQ(TransactionTypes::InputType::Key, tx->getInputType(index));
  ASSERT_EQ(1, tx->getRequiredSignaturesCount(index));

  ASSERT_TRUE(tx->validateInputs());
  ASSERT_FALSE(tx->validateSignatures()); // signature not present

  tx->signInputKey(index, info, ephKeys);

  ASSERT_TRUE(tx->validateSignatures()); // now it's ok

  auto txBlob = tx->getTransactionData();
  ASSERT_FALSE(txBlob.empty());

  EXPECT_NO_FATAL_FAILURE(checkHashChanged());
}

TEST_F(TransactionApi, addAndSignInputMsig) {

  MultisignatureInput inputMsig;

  inputMsig.amount = 1000;
  inputMsig.outputIndex = 0;
  inputMsig.signatureCount = 3;

  auto index = tx->addInput(inputMsig);

  ASSERT_EQ(0, index);
  ASSERT_EQ(1, tx->getInputCount());
  ASSERT_EQ(1000, tx->getInputTotalAmount());
  ASSERT_EQ(TransactionTypes::InputType::Multisignature, tx->getInputType(index));
  ASSERT_EQ(3, tx->getRequiredSignaturesCount(index));

  KeyPair kp1;
  Crypto::generate_keys(kp1.publicKey, kp1.secretKey );

  auto srcTxKey = kp1.publicKey;
  AccountKeys accounts[] = { generateAccountKeys(), generateAccountKeys(), generateAccountKeys() };

  tx->signInputMultisignature(index, srcTxKey, 0, accounts[0]);

  ASSERT_FALSE(tx->validateSignatures());

  tx->signInputMultisignature(index, srcTxKey, 0, accounts[1]);
  tx->signInputMultisignature(index, srcTxKey, 0, accounts[2]);

  ASSERT_TRUE(tx->validateSignatures());

  auto txBlob = tx->getTransactionData();
  ASSERT_FALSE(txBlob.empty());
  EXPECT_NO_FATAL_FAILURE(checkHashChanged());
}

TEST_F(TransactionApi, addOutputKey) {
  ASSERT_EQ(0, tx->getOutputCount());
  ASSERT_EQ(0, tx->getOutputTotalAmount());

  size_t index = tx->addOutput(1000, sender.address);

  ASSERT_EQ(0, index);
  ASSERT_EQ(1, tx->getOutputCount());
  ASSERT_EQ(1000, tx->getOutputTotalAmount());
  ASSERT_EQ(TransactionTypes::OutputType::Key, tx->getOutputType(index));
  EXPECT_NO_FATAL_FAILURE(checkHashChanged());
}

TEST_F(TransactionApi, addOutputMsig) {
  ASSERT_EQ(0, tx->getOutputCount());
  ASSERT_EQ(0, tx->getOutputTotalAmount());

  AccountKeys accounts[] = { generateAccountKeys(), generateAccountKeys(), generateAccountKeys() };
  std::vector<AccountPublicAddress> targets;

  for (size_t i = 0; i < sizeof(accounts)/sizeof(accounts[0]); ++i)
    targets.push_back(accounts[i].address);

  size_t index = tx->addOutput(1000, targets, 2);

  ASSERT_EQ(0, index);
  ASSERT_EQ(1, tx->getOutputCount());
  ASSERT_EQ(1000, tx->getOutputTotalAmount());
  ASSERT_EQ(TransactionTypes::OutputType::Multisignature, tx->getOutputType(index));
  EXPECT_NO_FATAL_FAILURE(checkHashChanged());
}

TEST_F(TransactionApi, secretKey) {
  tx->addOutput(1000, sender.address);
  ASSERT_EQ(1000, tx->getOutputTotalAmount()); 
  // reloaded transaction does not have secret key, cannot add outputs
  auto tx2 = reloadedTx(tx);
  ASSERT_ANY_THROW(tx2->addOutput(1000, sender.address));
  // take secret key from first transaction and add to second (reloaded)
  SecretKey txSecretKey;
  ASSERT_TRUE(tx->getTransactionSecretKey(txSecretKey));
  
  KeyPair kp1;
  Crypto::generate_keys(kp1.publicKey, kp1.secretKey);
  SecretKey sk = kp1.secretKey;
  ASSERT_ANY_THROW(tx2->setTransactionSecretKey(sk)); // unrelated secret key should not be accepted
  
  tx2->setTransactionSecretKey(txSecretKey);
  // adding output should succeed
  tx2->addOutput(500, sender.address);
  ASSERT_EQ(1500, tx2->getOutputTotalAmount());
}

TEST_F(TransactionApi, prefixHash) {
  auto hash = tx->getTransactionPrefixHash();
  tx->addOutput(1000, sender.address);
  // transaction hash should change
  ASSERT_NE(hash, tx->getTransactionPrefixHash());
  hash = tx->getTransactionPrefixHash();
  // prefix hash should not change on reload
  ASSERT_EQ(hash, reloadedTx(tx)->getTransactionPrefixHash());
}

TEST_F(TransactionApi, findOutputs) {
  AccountKeys accounts[] = { generateAccountKeys(), generateAccountKeys(), generateAccountKeys() };

  tx->addOutput(1111, accounts[0].address);
  tx->addOutput(2222, accounts[1].address);
  tx->addOutput(3333, accounts[2].address);

  std::vector<uint32_t> outs;
  uint64_t amount = 0;

  tx->findOutputsToAccount(accounts[2].address, accounts[2].viewSecretKey, outs, amount);

  ASSERT_EQ(1, outs.size());
  ASSERT_EQ(2, outs[0]);
  ASSERT_EQ(3333, amount);
}

TEST_F(TransactionApi, setGetPaymentId) {
  Hash paymentId = Crypto::rand<Hash>();

  ASSERT_FALSE(tx->getPaymentId(paymentId));

  tx->setPaymentId(paymentId);

  EXPECT_NO_FATAL_FAILURE(checkHashChanged());

  Hash paymentId2;
  ASSERT_TRUE(tx->getPaymentId(paymentId2));
  ASSERT_EQ(paymentId, paymentId2);

  auto tx2 = reloadedTx(tx);

  Hash paymentId3;
  ASSERT_TRUE(tx->getPaymentId(paymentId3));
  ASSERT_EQ(paymentId, paymentId3);
}

TEST_F(TransactionApi, setExtraNonce) {
  BinaryArray extraNonce = Common::asBinaryArray("Hello, world"); // just a sequence of bytes
  BinaryArray s;

  ASSERT_FALSE(tx->getExtraNonce(s));
  tx->setExtraNonce(extraNonce);

  ASSERT_TRUE(tx->getExtraNonce(s));
  ASSERT_EQ(extraNonce, s);

  s.clear();

  ASSERT_TRUE(reloadedTx(tx)->getExtraNonce(s));
  ASSERT_EQ(extraNonce, s);
}

TEST_F(TransactionApi, appendExtra) {
  BinaryArray ba;

  ba.resize(100);
  std::iota(ba.begin(), ba.end(), 0);

  auto tx = createTransaction();

  auto extra = tx->getExtra();

  ASSERT_FALSE(extra.empty());

  tx->appendExtra(ba);

  auto newExtra = tx->getExtra();

  ASSERT_EQ(ba.size() + extra.size(), newExtra.size());
  ASSERT_EQ(0, memcmp(newExtra.data() + extra.size(), ba.data(), ba.size()));
}


TEST_F(TransactionApi, doubleSpendInTransactionKey) {
  TransactionTypes::InputKeyInfo info = createInputInfo(1000);

  KeyPair ephKeys;
  tx->addInput(sender, info, ephKeys);
  ASSERT_TRUE(tx->validateInputs());
  // now, add the same output again
  tx->addInput(sender, info, ephKeys);
  ASSERT_FALSE(tx->validateInputs());
}

TEST_F(TransactionApi, doubleSpendInTransactionMultisignature) {
  MultisignatureInput inputMsig = { 1000, 0, 2 };

  tx->addInput(inputMsig);
  ASSERT_TRUE(tx->validateInputs());
  tx->addInput(inputMsig);
  ASSERT_FALSE(tx->validateInputs());
}


TEST_F(TransactionApi, unableToModifySignedTransaction) {

  MultisignatureInput inputMsig;

  inputMsig.amount = 1000;
  inputMsig.outputIndex = 0;
  inputMsig.signatureCount = 2;
  auto index = tx->addInput(inputMsig);

  KeyPair kp1;
  Crypto::generate_keys(kp1.publicKey, kp1.secretKey);

  auto srcTxKey = kp1.publicKey;

  tx->signInputMultisignature(index, srcTxKey, 0, generateAccountKeys());

  // from now on, we cannot modify transaction prefix
  ASSERT_ANY_THROW(tx->addInput(inputMsig));
  ASSERT_ANY_THROW(tx->addOutput(500, sender.address));

  Hash paymentId;
  ASSERT_ANY_THROW(tx->setPaymentId(paymentId));
  ASSERT_ANY_THROW(tx->setExtraNonce(Common::asBinaryArray("smth")));

  // but can add more signatures
  tx->signInputMultisignature(index, srcTxKey, 0, generateAccountKeys());

  EXPECT_NO_FATAL_FAILURE(checkHashChanged());
}
