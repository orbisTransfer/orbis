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
#include "TcpConnector.h"
#include <cassert>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <mswsock.h>
#include <System/InterruptedException.h>
#include <System/Ipv4Address.h>
#include "Dispatcher.h"
#include "ErrorMessage.h"
#include "TcpConnection.h"

namespace System {

namespace {

struct TcpConnectorContext : public OVERLAPPED {
  NativeContext* context;
  size_t connection;
  bool interrupted;
};

LPFN_CONNECTEX connectEx = nullptr;

}

TcpConnector::TcpConnector() : dispatcher(nullptr) {
}

TcpConnector::TcpConnector(Dispatcher& dispatcher) : dispatcher(&dispatcher), context(nullptr) {
}

TcpConnector::TcpConnector(TcpConnector&& other) : dispatcher(other.dispatcher) {
  if (dispatcher != nullptr) {
    assert(other.context == nullptr);
    context = nullptr;
    other.dispatcher = nullptr;
  }
}

TcpConnector::~TcpConnector() {
  assert(dispatcher == nullptr || context == nullptr);
}

TcpConnector& TcpConnector::operator=(TcpConnector&& other) {
  assert(dispatcher == nullptr || context == nullptr);
  dispatcher = other.dispatcher;
  if (dispatcher != nullptr) {
    assert(other.context == nullptr);
    context = nullptr;
    other.dispatcher = nullptr;
  }

  return *this;
}

TcpConnection TcpConnector::connect(const Ipv4Address& address, uint16_t port) {
  assert(dispatcher != nullptr);
  assert(context == nullptr);
  if (dispatcher->interrupted()) {
    throw InterruptedException();
  }

  std::string message;
  SOCKET connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (connection == INVALID_SOCKET) {
    message = "socket failed, " + errorMessage(WSAGetLastError());
  } else {
    sockaddr_in bindAddress;
    bindAddress.sin_family = AF_INET;
    bindAddress.sin_port = 0;
    bindAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(connection, reinterpret_cast<sockaddr*>(&bindAddress), sizeof bindAddress) != 0) {
      message = "bind failed, " + errorMessage(WSAGetLastError());
    } else {
      GUID guidConnectEx = WSAID_CONNECTEX;
      DWORD read = sizeof connectEx;
      if (connectEx == nullptr && WSAIoctl(connection, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEx, sizeof guidConnectEx, &connectEx, sizeof connectEx, &read, NULL, NULL) != 0) {
        message = "WSAIoctl failed, " + errorMessage(WSAGetLastError());
      } else {
        assert(read == sizeof connectEx);
        if (CreateIoCompletionPort(reinterpret_cast<HANDLE>(connection), dispatcher->getCompletionPort(), 0, 0) != dispatcher->getCompletionPort()) {
          message = "CreateIoCompletionPort failed, " + lastErrorMessage();
        } else {
          sockaddr_in addressData;
          addressData.sin_family = AF_INET;
          addressData.sin_port = htons(port);
          addressData.sin_addr.S_un.S_addr = htonl(address.getValue());
          TcpConnectorContext context2;
          context2.hEvent = NULL;
          if (connectEx(connection, reinterpret_cast<sockaddr*>(&addressData), sizeof addressData, NULL, 0, NULL, &context2) == TRUE) {
            message = "ConnectEx returned immediately, which is not supported.";
          } else {
            int lastError = WSAGetLastError();
            if (lastError != WSA_IO_PENDING) {
              message = "ConnectEx failed, " + errorMessage(lastError);
            } else {
              context2.context = dispatcher->getCurrentContext();
              context2.connection = connection;
              context2.interrupted = false;
              context = &context2;
              dispatcher->getCurrentContext()->interruptProcedure = [&]() {
                assert(dispatcher != nullptr);
                assert(context != nullptr);
                TcpConnectorContext* context2 = static_cast<TcpConnectorContext*>(context);
                if (!context2->interrupted) {
                  if (CancelIoEx(reinterpret_cast<HANDLE>(context2->connection), context2) != TRUE) {
                    DWORD lastError = GetLastError();
                    if (lastError != ERROR_NOT_FOUND) {
                      throw std::runtime_error("TcpConnector::stop, CancelIoEx failed, " + lastErrorMessage());
                    }

                    context2->context->interrupted = true;
                  }

                  context2->interrupted = true;
                }
              };

              dispatcher->dispatch();
              dispatcher->getCurrentContext()->interruptProcedure = nullptr;
              assert(context2.context == dispatcher->getCurrentContext());
              assert(context2.connection == connection);
              assert(dispatcher != nullptr);
              assert(context == &context2);
              context = nullptr;
              DWORD transferred;
              DWORD flags;
              if (WSAGetOverlappedResult(connection, &context2, &transferred, FALSE, &flags) != TRUE) {
                lastError = WSAGetLastError();
                if (lastError != ERROR_OPERATION_ABORTED) {
                  message = "ConnectEx failed, " + errorMessage(lastError);
                } else {
                  assert(context2.interrupted);
                  if (closesocket(connection) != 0) {
                    throw std::runtime_error("TcpConnector::connect, closesocket failed, " + errorMessage(WSAGetLastError()));
                  } else {
                    throw InterruptedException();
                  }
                }
              } else {
                assert(transferred == 0);
                assert(flags == 0);
                DWORD value = 1;
                if (setsockopt(connection, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, reinterpret_cast<char*>(&value), sizeof(value)) != 0) {
                  message = "setsockopt failed, " + errorMessage(WSAGetLastError());
                } else {
                  return TcpConnection(*dispatcher, connection);
                }
              }
            }
          }
        }
      }
    }

    int result = closesocket(connection);
    assert(result == 0);
  }

  throw std::runtime_error("TcpConnector::connect, " + message);
}

}
