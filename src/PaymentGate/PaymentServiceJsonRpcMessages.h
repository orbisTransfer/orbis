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

#include <exception>
#include <limits>
#include <vector>

#include "Serialization/ISerializer.h"

namespace PaymentService {

const uint32_t DEFAULT_ANONYMITY_LEVEL = 6;

class RequestSerializationError: public std::exception {
public:
  virtual const char* what() const throw() override { return "Request error"; }
};

struct Reset {
  struct Request {
    std::string viewSecretKey;

    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    void serialize(Orbis::ISerializer& serializer);
  };
};

struct GetViewKey {
  struct Request {
    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    std::string viewSecretKey;

    void serialize(Orbis::ISerializer& serializer);
  };
};

struct GetStatus {
  struct Request {
    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    uint32_t blockCount;
    uint32_t knownBlockCount;
    std::string lastBlockHash;
    uint32_t peerCount;

    void serialize(Orbis::ISerializer& serializer);
  };
};

struct GetAddresses {
  struct Request {
    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    std::vector<std::string> addresses;

    void serialize(Orbis::ISerializer& serializer);
  };
};

struct CreateAddress {
  struct Request {
    std::string spendSecretKey;
    std::string spendPublicKey;

    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    std::string address;

    void serialize(Orbis::ISerializer& serializer);
  };
};

struct DeleteAddress {
  struct Request {
    std::string address;

    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    void serialize(Orbis::ISerializer& serializer);
  };
};

struct GetSpendKeys {
  struct Request {
    std::string address;

    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    std::string spendSecretKey;
    std::string spendPublicKey;

    void serialize(Orbis::ISerializer& serializer);
  };
};

struct GetBalance {
  struct Request {
    std::string address;

    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    uint64_t availableBalance;
    uint64_t lockedAmount;

    void serialize(Orbis::ISerializer& serializer);
  };
};

struct GetBlockHashes {
  struct Request {
    uint32_t firstBlockIndex;
    uint32_t blockCount;

    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    std::vector<std::string> blockHashes;

    void serialize(Orbis::ISerializer& serializer);
  };
};

struct TransactionHashesInBlockRpcInfo {
  std::string blockHash;
  std::vector<std::string> transactionHashes;

  void serialize(Orbis::ISerializer& serializer);
};

struct GetTransactionHashes {
  struct Request {
    std::vector<std::string> addresses;
    std::string blockHash;
    uint32_t firstBlockIndex = std::numeric_limits<uint32_t>::max();
    uint32_t blockCount;
    std::string paymentId;

    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    std::vector<TransactionHashesInBlockRpcInfo> items;

    void serialize(Orbis::ISerializer& serializer);
  };
};

struct TransferRpcInfo {
  uint8_t type;
  std::string address;
  int64_t amount;

  void serialize(Orbis::ISerializer& serializer);
};

struct TransactionRpcInfo {
  uint8_t state;
  std::string transactionHash;
  uint32_t blockIndex;
  uint64_t timestamp;
  bool isBase;
  uint64_t unlockTime;
  int64_t amount;
  uint64_t fee;
  std::vector<TransferRpcInfo> transfers;
  std::string extra;
  std::string paymentId;

  void serialize(Orbis::ISerializer& serializer);
};

struct GetTransaction {
  struct Request {
    std::string transactionHash;

    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    TransactionRpcInfo transaction;

    void serialize(Orbis::ISerializer& serializer);
  };
};

struct TransactionsInBlockRpcInfo {
  std::string blockHash;
  std::vector<TransactionRpcInfo> transactions;

  void serialize(Orbis::ISerializer& serializer);
};

struct GetTransactions {
  struct Request {
    std::vector<std::string> addresses;
    std::string blockHash;
    uint32_t firstBlockIndex = std::numeric_limits<uint32_t>::max();
    uint32_t blockCount;
    std::string paymentId;

    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    std::vector<TransactionsInBlockRpcInfo> items;

    void serialize(Orbis::ISerializer& serializer);
  };
};

struct GetUnconfirmedTransactionHashes {
  struct Request {
    std::vector<std::string> addresses;

    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    std::vector<std::string> transactionHashes;

    void serialize(Orbis::ISerializer& serializer);
  };
};

struct WalletRpcOrder {
  std::string address;
  uint64_t amount;

  void serialize(Orbis::ISerializer& serializer);
};

struct SendTransaction {
  struct Request {
    std::vector<std::string> sourceAddresses;
    std::vector<WalletRpcOrder> transfers;
    std::string changeAddress;
    uint64_t fee = 0;
    uint32_t anonymity = DEFAULT_ANONYMITY_LEVEL;
    std::string extra;
    std::string paymentId;
    uint64_t unlockTime = 0;

    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    std::string transactionHash;

    void serialize(Orbis::ISerializer& serializer);
  };
};

struct CreateDelayedTransaction {
  struct Request {
    std::vector<std::string> addresses;
    std::vector<WalletRpcOrder> transfers;
    std::string changeAddress;
    uint64_t fee = 0;
    uint32_t anonymity = DEFAULT_ANONYMITY_LEVEL;
    std::string extra;
    std::string paymentId;
    uint64_t unlockTime = 0;

    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    std::string transactionHash;

    void serialize(Orbis::ISerializer& serializer);
  };
};

struct GetDelayedTransactionHashes {
  struct Request {
    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    std::vector<std::string> transactionHashes;

    void serialize(Orbis::ISerializer& serializer);
  };
};

struct DeleteDelayedTransaction {
  struct Request {
    std::string transactionHash;

    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    void serialize(Orbis::ISerializer& serializer);
  };
};

struct SendDelayedTransaction {
  struct Request {
    std::string transactionHash;

    void serialize(Orbis::ISerializer& serializer);
  };

  struct Response {
    void serialize(Orbis::ISerializer& serializer);
  };
};

} //namespace PaymentService
