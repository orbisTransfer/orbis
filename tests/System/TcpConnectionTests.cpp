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
#include <System/Dispatcher.h>
#include <System/ContextGroup.h>
#include <System/Event.h>
#include <System/InterruptedException.h>
#include <System/Ipv4Address.h>
#include <System/TcpConnection.h>
#include <System/TcpConnector.h>
#include <System/TcpListener.h>
#include <System/TcpStream.h>
#include <System/Timer.h>
#include <gtest/gtest.h>

using namespace System;

namespace {

const Ipv4Address LISTEN_ADDRESS("127.0.0.1");
const uint16_t LISTEN_PORT = 6666;

void fillRandomBuf(std::vector<uint8_t>& buf) {
  for (size_t i = 0; i < buf.size(); ++i) {
    buf[i] = static_cast<uint8_t>(rand() & 0xff);
  }
}

void fillRandomString(std::string& buf) {
  for (size_t i = 0; i < buf.size(); ++i) {
    buf[i] = static_cast<uint8_t>(rand() & 0xff);
  }
}

std::string removePort(const std::string& address) {
  size_t colonPosition = address.rfind(':');
  if (colonPosition == std::string::npos) {
    throw std::runtime_error("removePort");
  }

  return address.substr(0, colonPosition);
}

}

class TcpConnectionTests : public testing::Test {
public:
  TcpConnectionTests() : listener(dispatcher, LISTEN_ADDRESS, LISTEN_PORT), contextGroup(dispatcher) {
  }

  void connect() {
    connection1 = TcpConnector(dispatcher).connect(LISTEN_ADDRESS, LISTEN_PORT);
    connection2 = listener.accept();
  }

protected:
  Dispatcher dispatcher;
  TcpListener listener;
  TcpConnection connection1;
  TcpConnection connection2;
  ContextGroup contextGroup;
};

TEST_F(TcpConnectionTests, sendAndClose) {
  connect();
  ASSERT_EQ(LISTEN_ADDRESS, connection1.getPeerAddressAndPort().first);
  ASSERT_EQ(LISTEN_ADDRESS, connection2.getPeerAddressAndPort().first);
  connection1.write(reinterpret_cast<const uint8_t*>("Test"), 4);
  uint8_t data[1024];
  size_t size = connection2.read(data, 1024);
  ASSERT_EQ(4, size);
  ASSERT_EQ(0, memcmp(data, "Test", 4));
  connection1 = TcpConnection();
  size = connection2.read(data, 1024);
  ASSERT_EQ(0, size);
}

TEST_F(TcpConnectionTests, stoppedState) {
  connect();
  bool stopped = false;
  contextGroup.spawn([&] {
    try {
      uint8_t data[1024];
      connection1.read(data, 1024);
    } catch (InterruptedException&) {
      stopped = true;
    }

    ASSERT_TRUE(stopped);
    contextGroup.interrupt();
    stopped = false;
    try {
      connection1.write(reinterpret_cast<const uint8_t*>("Test"), 4);
    } catch (InterruptedException&) {
      stopped = true;
    }
  });
  contextGroup.interrupt();
  contextGroup.wait();

  ASSERT_TRUE(stopped);
}

TEST_F(TcpConnectionTests, interruptRead) {
  connect();
  contextGroup.spawn([&]() {
    Timer(dispatcher).sleep(std::chrono::milliseconds(10));
    contextGroup.interrupt();
  });

  bool stopped = false;
  contextGroup.spawn([&]() {
    try {
      uint8_t data[1024];
      connection1.read(data, 1024);
    } catch (InterruptedException &) {
      stopped = true;
    }
  });

  contextGroup.wait();
  ASSERT_TRUE(stopped);
}

TEST_F(TcpConnectionTests, reuseWriteAfterInterrupt) {
  connect();
  contextGroup.spawn([&]() {
    Timer(dispatcher).sleep(std::chrono::milliseconds(10));
    contextGroup.interrupt();
  });

  bool stopped = false;
  contextGroup.spawn([&]() {
    try {
      uint8_t data[1024];
      connection1.read(data, 1024);
    } catch (InterruptedException &) {
      stopped = true;
    }
  });

  contextGroup.wait();
  ASSERT_TRUE(stopped);
  stopped = false;

  contextGroup.spawn([&]() {
    Timer(dispatcher).sleep(std::chrono::milliseconds(10));
    contextGroup.interrupt();
  });

  contextGroup.spawn([&] {
    try {
      uint8_t buff[1024];
      std::fill(std::begin(buff), std::end(buff), 0xff);
      connection1.write(buff, sizeof(buff)); // write smth
      connection1 = TcpConnection();         // close connection
    } catch (InterruptedException&) {
      stopped = true;
    }
  });

  contextGroup.spawn([&]() {
    try {
      uint8_t data[1024];
      connection2.read(data, 1024);
    } catch (InterruptedException &) {
      stopped = true;
    }
  });

  contextGroup.wait();
  ASSERT_TRUE(!stopped);
}

TEST_F(TcpConnectionTests, reuseReadAfterInterrupt) {
  connect();
  contextGroup.spawn([&]() {
    Timer(dispatcher).sleep(std::chrono::milliseconds(10));
    contextGroup.interrupt();
  });

  bool stopped = false;
  contextGroup.spawn([&]() {
    try {
      uint8_t data[1024];
      connection1.read(data, 1024);
    } catch (InterruptedException &) {
      stopped = true;
    }
  });

  contextGroup.wait();
  ASSERT_TRUE(stopped);
  stopped = false;

  contextGroup.spawn([&]() {
    Timer(dispatcher).sleep(std::chrono::milliseconds(10));
    contextGroup.interrupt();
  });

  contextGroup.spawn([&] {
    try {
      uint8_t buff[1024];
      std::fill(std::begin(buff), std::end(buff), 0xff);
      connection2.write(buff, sizeof(buff)); // write smth
      connection2 = TcpConnection();         // close connection
    } catch (InterruptedException&) {
      stopped = true;
    }
  });

  contextGroup.spawn([&]() {
    try {
      uint8_t data[1024];
      connection1.read(data, 1024);
    } catch (InterruptedException &) {
      stopped = true;
    }
  });

  contextGroup.wait();
  ASSERT_TRUE(!stopped);
}

TEST_F(TcpConnectionTests, sendBigChunk) {
  connect();
  
  const size_t bufsize =  15* 1024 * 1024; // 15MB
  std::vector<uint8_t> buf;
  buf.resize(bufsize);
  fillRandomBuf(buf);

  std::vector<uint8_t> incoming;
  Event readComplete(dispatcher);

  contextGroup.spawn([&]{
    uint8_t readBuf[1024];
    size_t readSize;
    while ((readSize = connection2.read(readBuf, sizeof(readBuf))) > 0) {
      incoming.insert(incoming.end(), readBuf, readBuf + readSize);
    }

    readComplete.set();
  });

  contextGroup.spawn([&]{
    uint8_t* bufPtr = &buf[0];
    size_t left = bufsize;
    while(left > 0) {
      auto transferred =  connection1.write(bufPtr, std::min(left, size_t(666)));
      left -= transferred;
      bufPtr += transferred;
    }

    connection1 = TcpConnection(); // close connection
  });

  readComplete.wait();

  ASSERT_EQ(bufsize, incoming.size());
  ASSERT_EQ(buf, incoming);
}

TEST_F(TcpConnectionTests, writeWhenReadWaiting) {
  connect();

  Event readStarted(dispatcher);
  Event readCompleted(dispatcher);
  Event writeCompleted(dispatcher);

  size_t writeSize = 0;
  bool readStopped = false;

  contextGroup.spawn([&]{
    try {
      uint8_t readBuf[1024];
      size_t readSize;
      readStarted.set();
      while ((readSize = connection2.read(readBuf, sizeof(readBuf))) > 0) {
      }
    } catch (InterruptedException&) {
      readStopped = true;
    }
    connection2 = TcpConnection();
    readCompleted.set();
  });

  readStarted.wait();

  contextGroup.spawn([&]{
    uint8_t writeBuf[1024];
    for (int i = 0; i < 100; ++i) {
      writeSize += connection2.write(writeBuf, sizeof(writeBuf));
    }
    //connection2.stop();
    contextGroup.interrupt();
    writeCompleted.set();
  });

  uint8_t readBuf[100];
  size_t readSize;
  size_t totalRead = 0;
  while ((readSize = connection1.read(readBuf, sizeof(readBuf))) > 0) {
    totalRead += readSize;
  }

  ASSERT_EQ(writeSize, totalRead);
  readCompleted.wait();
  ASSERT_TRUE(readStopped);
  writeCompleted.wait();
}

TEST_F(TcpConnectionTests, sendBigChunkThruTcpStream) {
  connect();
  const size_t bufsize = 15 * 1024 * 1024; // 15MB
  std::string buf;
  buf.resize(bufsize);
  fillRandomString(buf);

  std::string incoming;
  Event readComplete(dispatcher);

  contextGroup.spawn([&]{
    uint8_t readBuf[1024];
    size_t readSize;
    while ((readSize = connection2.read(readBuf, sizeof(readBuf))) > 0) {
      incoming.insert(incoming.end(), readBuf, readBuf + readSize);
    }

    readComplete.set();
  });


  contextGroup.spawn([&]{
    TcpStreambuf streambuf(connection1);
    std::iostream stream(&streambuf);

    stream << buf;
    stream.flush();

    connection1 = TcpConnection(); // close connection
  });

  readComplete.wait();

  ASSERT_EQ(bufsize, incoming.size());

  //ASSERT_EQ(buf, incoming); 
  for (size_t i = 0; i < bufsize; ++i) {
    ASSERT_EQ(buf[i], incoming[i]); //for better output.
  }
}
