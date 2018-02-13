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

#include <fcntl.h>
#include <netdb.h>
#include <sys/errno.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <System/InterruptedException.h>
#include <System/Ipv4Address.h>
#include "Dispatcher.h"
#include "ErrorMessage.h"
#include "TcpConnection.h"

namespace System {

namespace {

struct ConnectorContext : public OperationContext {
  int connection;
};

}

TcpConnector::TcpConnector() : dispatcher(nullptr) {
}

TcpConnector::TcpConnector(Dispatcher& dispatcher) : dispatcher(&dispatcher), context(nullptr) {
}

TcpConnector::TcpConnector(TcpConnector&& other) : dispatcher(other.dispatcher) {
  if (other.dispatcher != nullptr) {
    assert(other.context == nullptr);
    context = nullptr;
    other.dispatcher = nullptr;
  }
}

TcpConnector::~TcpConnector() {
  assert(dispatcher == nullptr || context == nullptr);
}

TcpConnector& TcpConnector::operator=(TcpConnector&& other) {
  dispatcher = other.dispatcher;
  if (other.dispatcher != nullptr) {
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
  int connection = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (connection == -1) {
    message = "socket failed, " + lastErrorMessage();
  } else {
    sockaddr_in bindAddress;
    bindAddress.sin_family = AF_INET;
    bindAddress.sin_port = 0;
    bindAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(connection, reinterpret_cast<sockaddr*>(&bindAddress), sizeof bindAddress) != 0) {
      message = "bind failed, " + lastErrorMessage();
    } else {
      int flags = fcntl(connection, F_GETFL, 0);
      if (flags == -1 || fcntl(connection, F_SETFL, flags | O_NONBLOCK) == -1) {
        message = "fcntl failed, " + lastErrorMessage();
      } else {
        sockaddr_in addressData;
        addressData.sin_family = AF_INET;
        addressData.sin_port = htons(port);
        addressData.sin_addr.s_addr = htonl(address.getValue());
        int result = ::connect(connection, reinterpret_cast<sockaddr *>(&addressData), sizeof addressData);
        if (result == -1) {
          if (errno == EINPROGRESS) {
            ConnectorContext connectorContext;
            connectorContext.context = dispatcher->getCurrentContext();
            connectorContext.interrupted = false;
            connectorContext.connection = connection;

            struct kevent event;
            EV_SET(&event, connection, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, &connectorContext);
            if (kevent(dispatcher->getKqueue(), &event, 1, NULL, 0, NULL) == -1) {
              message = "kevent failed, " + lastErrorMessage();
            } else {
              context = &connectorContext;
              dispatcher->getCurrentContext()->interruptProcedure = [&] {
                assert(dispatcher != nullptr);
                assert(context != nullptr);
                ConnectorContext* connectorContext = static_cast<ConnectorContext*>(context);
                if (!connectorContext->interrupted) {
                  if (close(connectorContext->connection) == -1) {
                    throw std::runtime_error("TcpListener::stop, close failed, " + lastErrorMessage());
                  }
                  
                  dispatcher->pushContext(connectorContext->context);
                  connectorContext->interrupted = true;
                }                
              };
              
              dispatcher->dispatch();
              dispatcher->getCurrentContext()->interruptProcedure = nullptr;
              assert(dispatcher != nullptr);
              assert(connectorContext.context == dispatcher->getCurrentContext());
              assert(context == &connectorContext);
              context = nullptr;
              connectorContext.context = nullptr;
              if (connectorContext.interrupted) {
                throw InterruptedException();
              }

              struct kevent event;
              EV_SET(&event, connection, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);

              if (kevent(dispatcher->getKqueue(), &event, 1, NULL, 0, NULL) == -1) {
                message = "kevent failed, " + lastErrorMessage();
              } else {
                int retval = -1;
                socklen_t retValLen = sizeof(retval);
                int s = getsockopt(connection, SOL_SOCKET, SO_ERROR, &retval, &retValLen);
                if (s == -1) {
                  message = "getsockopt failed, " + lastErrorMessage();
                } else {
                  if (retval != 0) {
                    message = "getsockopt failed, " + lastErrorMessage();
                  } else {
                    return TcpConnection(*dispatcher, connection);
                  }
                }
              }
            }
          }
        } else {
          return TcpConnection(*dispatcher, connection);
        }
      }
    }

    int result = close(connection);
    assert(result != -1);;
  }

  throw std::runtime_error("TcpConnector::connect, " + message);
}

}
