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
#include "TcpConnection.h"
#include <cassert>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2ipdef.h>
#include <System/InterruptedException.h>
#include <System/Ipv4Address.h>
#include "Dispatcher.h"
#include "ErrorMessage.h"

namespace System {

namespace {

struct TcpConnectionContext : public OVERLAPPED {
  NativeContext* context;
  bool interrupted;
};

}

TcpConnection::TcpConnection() : dispatcher(nullptr) {
}

TcpConnection::TcpConnection(TcpConnection&& other) : dispatcher(other.dispatcher) {
  if (dispatcher != nullptr) {
    assert(other.readContext == nullptr);
    assert(other.writeContext == nullptr);
    connection = other.connection;
    readContext = nullptr;
    writeContext = nullptr;
    other.dispatcher = nullptr;
  }
}

TcpConnection::~TcpConnection() {
  if (dispatcher != nullptr) {
    assert(readContext == nullptr);
    assert(writeContext == nullptr);
    int result = closesocket(connection);
    assert(result == 0);
  }
}

TcpConnection& TcpConnection::operator=(TcpConnection&& other) {
  if (dispatcher != nullptr) {
    assert(readContext == nullptr);
    assert(writeContext == nullptr);
    if (closesocket(connection) != 0) {
      throw std::runtime_error("TcpConnection::operator=, closesocket failed, " + errorMessage(WSAGetLastError()));
    }
  }

  dispatcher = other.dispatcher;
  if (dispatcher != nullptr) {
    assert(other.readContext == nullptr);
    assert(other.writeContext == nullptr);
    connection = other.connection;
    readContext = nullptr;
    writeContext = nullptr;
    other.dispatcher = nullptr;
  }

  return *this;
}

size_t TcpConnection::read(uint8_t* data, size_t size) {
  assert(dispatcher != nullptr);
  assert(readContext == nullptr);
  if (dispatcher->interrupted()) {
    throw InterruptedException();
  }

  WSABUF buf{static_cast<ULONG>(size), reinterpret_cast<char*>(data)};
  DWORD flags = 0;
  TcpConnectionContext context;
  context.hEvent = NULL;
  if (WSARecv(connection, &buf, 1, NULL, &flags, &context, NULL) != 0) {
    int lastError = WSAGetLastError();
    if (lastError != WSA_IO_PENDING) {
      throw std::runtime_error("TcpConnection::read, WSARecv failed, " + errorMessage(lastError));
    }
  }

  assert(flags == 0);
  context.context = dispatcher->getCurrentContext();
  context.interrupted = false;
  readContext = &context;
  dispatcher->getCurrentContext()->interruptProcedure = [&]() {
    assert(dispatcher != nullptr);
    assert(readContext != nullptr);
    TcpConnectionContext* context = static_cast<TcpConnectionContext*>(readContext);
    if (!context->interrupted) {
      if (CancelIoEx(reinterpret_cast<HANDLE>(connection), context) != TRUE) {
        DWORD lastError = GetLastError();
        if (lastError != ERROR_NOT_FOUND) {
          throw std::runtime_error("TcpConnection::stop, CancelIoEx failed, " + lastErrorMessage());
        }

        context->context->interrupted = true;
      }

      context->interrupted = true;
    }
  };

  dispatcher->dispatch();
  dispatcher->getCurrentContext()->interruptProcedure = nullptr;
  assert(context.context == dispatcher->getCurrentContext());
  assert(dispatcher != nullptr);
  assert(readContext == &context);
  readContext = nullptr;
  DWORD transferred;
  if (WSAGetOverlappedResult(connection, &context, &transferred, FALSE, &flags) != TRUE) {
    int lastError = WSAGetLastError();
    if (lastError != ERROR_OPERATION_ABORTED) {
      throw std::runtime_error("TcpConnection::read, WSAGetOverlappedResult failed, " + errorMessage(lastError));
    }

    assert(context.interrupted);
    throw InterruptedException();
  }

  assert(transferred <= size);
  assert(flags == 0);
  return transferred;
}

size_t TcpConnection::write(const uint8_t* data, size_t size) {
  assert(dispatcher != nullptr);
  assert(writeContext == nullptr);
  if (dispatcher->interrupted()) {
    throw InterruptedException();
  }

  if (size == 0) {
    if (shutdown(connection, SD_SEND) != 0) {
      throw std::runtime_error("TcpConnection::write, shutdown failed, " + errorMessage(WSAGetLastError()));
    }

    return 0;
  }

  WSABUF buf{static_cast<ULONG>(size), reinterpret_cast<char*>(const_cast<uint8_t*>(data))};
  TcpConnectionContext context;
  context.hEvent = NULL;
  if (WSASend(connection, &buf, 1, NULL, 0, &context, NULL) != 0) {
    int lastError = WSAGetLastError();
    if (lastError != WSA_IO_PENDING) {
      throw std::runtime_error("TcpConnection::write, WSASend failed, " + errorMessage(lastError));
    }
  }

  context.context = dispatcher->getCurrentContext();
  context.interrupted = false;
  writeContext = &context;
  dispatcher->getCurrentContext()->interruptProcedure = [&]() {
    assert(dispatcher != nullptr);
    assert(writeContext != nullptr);
    TcpConnectionContext* context = static_cast<TcpConnectionContext*>(writeContext);
    if (!context->interrupted) {
      if (CancelIoEx(reinterpret_cast<HANDLE>(connection), context) != TRUE) {
        DWORD lastError = GetLastError();
        if (lastError != ERROR_NOT_FOUND) {
          throw std::runtime_error("TcpConnection::stop, CancelIoEx failed, " + lastErrorMessage());
        }

        context->context->interrupted = true;
      }

      context->interrupted = true;
    }
  };

  dispatcher->dispatch();
  dispatcher->getCurrentContext()->interruptProcedure = nullptr;
  assert(context.context == dispatcher->getCurrentContext());
  assert(dispatcher != nullptr);
  assert(writeContext == &context);
  writeContext = nullptr;
  DWORD transferred;
  DWORD flags;
  if (WSAGetOverlappedResult(connection, &context, &transferred, FALSE, &flags) != TRUE) {
    int lastError = WSAGetLastError();
    if (lastError != ERROR_OPERATION_ABORTED) {
      throw std::runtime_error("TcpConnection::write, WSAGetOverlappedResult failed, " + errorMessage(lastError));
    }

    assert(context.interrupted);
    throw InterruptedException();
  }

  assert(transferred == size);
  assert(flags == 0);
  return transferred;
}

std::pair<Ipv4Address, uint16_t> TcpConnection::getPeerAddressAndPort() const {
  sockaddr_in address;
  int size = sizeof(address);
  if (getpeername(connection, reinterpret_cast<sockaddr*>(&address), &size) != 0) {
    throw std::runtime_error("TcpConnection::getPeerAddress, getpeername failed, " + errorMessage(WSAGetLastError()));
  }

  assert(size == sizeof(sockaddr_in));
  return std::make_pair(Ipv4Address(htonl(address.sin_addr.S_un.S_addr)), htons(address.sin_port));
}

TcpConnection::TcpConnection(Dispatcher& dispatcher, size_t connection) : dispatcher(&dispatcher), connection(connection), readContext(nullptr), writeContext(nullptr) {
}

}
