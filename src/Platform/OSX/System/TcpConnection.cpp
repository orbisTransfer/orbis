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

#include <netinet/in.h>
#include <sys/event.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Dispatcher.h"
#include <System/ErrorMessage.h>
#include <System/InterruptedException.h>
#include <System/Ipv4Address.h>

namespace System {

TcpConnection::TcpConnection() : dispatcher(nullptr) {
}

TcpConnection::TcpConnection(TcpConnection&& other) : dispatcher(other.dispatcher) {
  if (other.dispatcher != nullptr) {
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
    int result = close(connection);
    assert(result != -1);
  }
}

TcpConnection& TcpConnection::operator=(TcpConnection&& other) {
  if (dispatcher != nullptr) {
    assert(readContext == nullptr);
    assert(writeContext == nullptr);
    if (close(connection) == -1) {
      throw std::runtime_error("TcpConnection::operator=, close failed, " + lastErrorMessage());
    }
  }

  dispatcher = other.dispatcher;
  if (other.dispatcher != nullptr) {
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

  std::string message;
  ssize_t transferred = ::recv(connection, (void *)data, size, 0);
  if (transferred == -1) {
    if (errno != EAGAIN  && errno != EWOULDBLOCK) {
      message = "recv failed, " + lastErrorMessage();
    } else {
      OperationContext context;
      context.context = dispatcher->getCurrentContext();
      context.interrupted = false;
      struct kevent event;
      EV_SET(&event, connection, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR | EV_ONESHOT, 0, 0, &context);
      if (kevent(dispatcher->getKqueue(), &event, 1, NULL, 0, NULL) == -1) {
        message = "kevent failed, " + lastErrorMessage();
      } else {
        readContext = &context;
        dispatcher->getCurrentContext()->interruptProcedure = [&] {
          assert(dispatcher != nullptr);
          assert(readContext != nullptr);
          OperationContext* context = static_cast<OperationContext*>(readContext);
          if (!context->interrupted) {
            struct kevent event;
            EV_SET(&event, connection, EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, NULL);
            
            if (kevent(dispatcher->getKqueue(), &event, 1, NULL, 0, NULL) == -1) {
              throw std::runtime_error("TcpListener::interruptionProcedure, kevent failed, " + lastErrorMessage());
            }
            
            context->interrupted = true;
            dispatcher->pushContext(context->context);
          }
        };
        
        dispatcher->dispatch();
        dispatcher->getCurrentContext()->interruptProcedure = nullptr;
        assert(dispatcher != nullptr);
        assert(context.context == dispatcher->getCurrentContext());
        assert(readContext == &context);
        readContext = nullptr;
        context.context = nullptr;
        if (context.interrupted) {
          throw InterruptedException();
        }

        ssize_t transferred = ::recv(connection, (void *)data, size, 0);
        if (transferred == -1) {
          message = "recv failed, " + lastErrorMessage();
        } else {
          assert(transferred <= static_cast<ssize_t>(size));
          return transferred;
        }
      }
    }

    throw std::runtime_error("TcpConnection::read, " + message);
  }

  assert(transferred <= static_cast<ssize_t>(size));
  return transferred;
}

size_t TcpConnection::write(const uint8_t* data, size_t size) {
  assert(dispatcher != nullptr);
  assert(writeContext == nullptr);
  if (dispatcher->interrupted()) {
    throw InterruptedException();
  }

  std::string message;
  if (size == 0) {
    if (shutdown(connection, SHUT_WR) == -1) {
      throw std::runtime_error("TcpConnection::write, shutdown failed, " + lastErrorMessage());
    }

    return 0;
  }

  ssize_t transferred = ::send(connection, (void *)data, size, 0);
  if (transferred == -1) {
    if (errno != EAGAIN  && errno != EWOULDBLOCK) {
      message = "send failed, " + lastErrorMessage();
    } else {
      OperationContext context;
      context.context = dispatcher->getCurrentContext();
      context.interrupted = false;
      struct kevent event;
      EV_SET(&event, connection, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, &context);
      if (kevent(dispatcher->getKqueue(), &event, 1, NULL, 0, NULL) == -1) {
        message = "kevent failed, " + lastErrorMessage();
      } else {
        writeContext = &context;
        dispatcher->getCurrentContext()->interruptProcedure = [&] {
          assert(dispatcher != nullptr);
          assert(writeContext != nullptr);
          OperationContext* context = static_cast<OperationContext*>(writeContext);
          if (!context->interrupted) {
            struct kevent event;
            EV_SET(&event, connection, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, NULL);
            
            if (kevent(dispatcher->getKqueue(), &event, 1, NULL, 0, NULL) == -1) {
              throw std::runtime_error("TcpListener::stop, kevent failed, " + lastErrorMessage());
            }
            
            context->interrupted = true;
            dispatcher->pushContext(context->context);            
          }
        };
        
        dispatcher->dispatch();
        dispatcher->getCurrentContext()->interruptProcedure = nullptr;
        assert(dispatcher != nullptr);
        assert(context.context == dispatcher->getCurrentContext());
        assert(writeContext == &context);
        writeContext = nullptr;
        context.context = nullptr;
        if (context.interrupted) {
          throw InterruptedException();
        }

        ssize_t transferred = ::send(connection, (void *)data, size, 0);
        if (transferred == -1) {
          message = "send failed, " + lastErrorMessage();
        } else {
          assert(transferred <= static_cast<ssize_t>(size));
          return transferred;
        }
      }
    }

    throw std::runtime_error("TcpConnection::write, " + message);
  }

  assert(transferred <= static_cast<ssize_t>(size));
  return transferred;
}

std::pair<Ipv4Address, uint16_t> TcpConnection::getPeerAddressAndPort() const {
  sockaddr_in addr;
  socklen_t size = sizeof(addr);
  if (getpeername(connection, reinterpret_cast<sockaddr*>(&addr), &size) != 0) {
    throw std::runtime_error("TcpConnection::getPeerAddress, getpeername failed, " + lastErrorMessage());
  }

  assert(size == sizeof(sockaddr_in));
  return std::make_pair(Ipv4Address(htonl(addr.sin_addr.s_addr)), htons(addr.sin_port));
}

TcpConnection::TcpConnection(Dispatcher& dispatcher, int socket) : dispatcher(&dispatcher), connection(socket), readContext(nullptr), writeContext(nullptr) {
  int val = 1;
  if (setsockopt(connection, SOL_SOCKET, SO_NOSIGPIPE, (void*)&val, sizeof val) == -1) {
    throw std::runtime_error("TcpConnection::TcpConnection, setsockopt failed, " + lastErrorMessage());
  }
}

}
