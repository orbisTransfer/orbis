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

#include <arpa/inet.h>
#include <cassert>
#include <sys/epoll.h>
#include <unistd.h>

#include <System/ErrorMessage.h>
#include <System/InterruptedException.h>
#include <System/Ipv4Address.h>

namespace System {

TcpConnection::TcpConnection() : dispatcher(nullptr) {
}

TcpConnection::TcpConnection(TcpConnection&& other) : dispatcher(other.dispatcher) {
  if (other.dispatcher != nullptr) {
    assert(other.contextPair.writeContext == nullptr);
    assert(other.contextPair.readContext == nullptr);
    connection = other.connection;
    contextPair = other.contextPair;
    other.dispatcher = nullptr;
  }
}

TcpConnection::~TcpConnection() {
  if (dispatcher != nullptr) {
    assert(contextPair.readContext == nullptr);
    assert(contextPair.writeContext == nullptr);
    int result = close(connection);
    assert(result != -1);
  }
}

TcpConnection& TcpConnection::operator=(TcpConnection&& other) {
  if (dispatcher != nullptr) {
    assert(contextPair.readContext == nullptr);
    assert(contextPair.writeContext == nullptr);
    if (close(connection) == -1) {
      throw std::runtime_error("TcpConnection::operator=, close failed, " + lastErrorMessage());
    }
  }

  dispatcher = other.dispatcher;
  if (other.dispatcher != nullptr) {
    assert(other.contextPair.readContext == nullptr);
    assert(other.contextPair.writeContext == nullptr);
    connection = other.connection;
    contextPair = other.contextPair;
    other.dispatcher = nullptr;
  }

  return *this;
}

size_t TcpConnection::read(uint8_t* data, size_t size) {
  assert(dispatcher != nullptr);
  assert(contextPair.readContext == nullptr);
  if (dispatcher->interrupted()) {
    throw InterruptedException();
  }

  std::string message;
  ssize_t transferred = ::recv(connection, (void *)data, size, 0);
  if (transferred == -1) {
    if (errno != EAGAIN  && errno != EWOULDBLOCK) {
      message = "recv failed, " + lastErrorMessage();
    } else {
      epoll_event connectionEvent;
      OperationContext operationContext;
      operationContext.interrupted = false;
      operationContext.context = dispatcher->getCurrentContext();
      contextPair.readContext = &operationContext;
      connectionEvent.data.ptr = &contextPair;

      if(contextPair.writeContext != nullptr) {
        connectionEvent.events = EPOLLIN | EPOLLOUT | EPOLLONESHOT;
      } else {
        connectionEvent.events = EPOLLIN | EPOLLONESHOT;
      }

      if (epoll_ctl(dispatcher->getEpoll(), EPOLL_CTL_MOD, connection, &connectionEvent) == -1) {
        message = "epoll_ctl failed, " + lastErrorMessage();
      } else {
        dispatcher->getCurrentContext()->interruptProcedure = [&]() {
            assert(dispatcher != nullptr);
            assert(contextPair.readContext != nullptr);
            epoll_event connectionEvent;
            connectionEvent.events = 0;
            connectionEvent.data.ptr = nullptr;

            if (epoll_ctl(dispatcher->getEpoll(), EPOLL_CTL_MOD, connection, &connectionEvent) == -1) {
              throw std::runtime_error("TcpConnection::stop, epoll_ctl failed, " + lastErrorMessage());
            }

            contextPair.readContext->interrupted = true;
            dispatcher->pushContext(contextPair.readContext->context);
        };

        dispatcher->dispatch();
        dispatcher->getCurrentContext()->interruptProcedure = nullptr;
        assert(dispatcher != nullptr);
        assert(operationContext.context == dispatcher->getCurrentContext());
        assert(contextPair.readContext == &operationContext);

        if (operationContext.interrupted) {
          contextPair.readContext = nullptr;
          throw InterruptedException();
        }

        contextPair.readContext = nullptr;
        if(contextPair.writeContext != nullptr) { //write is presented, rearm
          epoll_event connectionEvent;
          connectionEvent.events = EPOLLOUT | EPOLLONESHOT;
          connectionEvent.data.ptr = &contextPair;

          if (epoll_ctl(dispatcher->getEpoll(), EPOLL_CTL_MOD, connection, &connectionEvent) == -1) {
            message = "epoll_ctl failed, " + lastErrorMessage();
            throw std::runtime_error("TcpConnection::read");
          }
        }

        if((operationContext.events & (EPOLLERR | EPOLLHUP)) != 0) {
          throw std::runtime_error("TcpConnection::read");
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

    throw std::runtime_error("TcpConnection::read, "+ message);
  }

  assert(transferred <= static_cast<ssize_t>(size));
  return transferred;
}

std::size_t TcpConnection::write(const uint8_t* data, size_t size) {
  assert(dispatcher != nullptr);
  assert(contextPair.writeContext == nullptr);
  if (dispatcher->interrupted()) {
    throw InterruptedException();
  }

  std::string message;
  if(size == 0) {
    if(shutdown(connection, SHUT_WR) == -1) {
      throw std::runtime_error("TcpConnection::write, shutdown failed, " + lastErrorMessage());
    }

    return 0;
  }

  ssize_t transferred = ::send(connection, (void *)data, size, MSG_NOSIGNAL);
  if (transferred == -1) {
    if (errno != EAGAIN  && errno != EWOULDBLOCK) {
      message = "send failed, " + lastErrorMessage();
    } else {
      epoll_event connectionEvent;
      OperationContext operationContext;
      operationContext.interrupted = false;
      operationContext.context = dispatcher->getCurrentContext();
      contextPair.writeContext = &operationContext;
      connectionEvent.data.ptr = &contextPair;

      if(contextPair.readContext != nullptr) {
        connectionEvent.events = EPOLLIN | EPOLLOUT | EPOLLONESHOT;
      } else {
        connectionEvent.events = EPOLLOUT | EPOLLONESHOT;
      }

      if (epoll_ctl(dispatcher->getEpoll(), EPOLL_CTL_MOD, connection, &connectionEvent) == -1) {
        message = "epoll_ctl failed, " + lastErrorMessage();
      } else {
        dispatcher->getCurrentContext()->interruptProcedure = [&]() {
            assert(dispatcher != nullptr);
            assert(contextPair.writeContext != nullptr);
            epoll_event connectionEvent;
            connectionEvent.events = 0;
            connectionEvent.data.ptr = nullptr;

            if (epoll_ctl(dispatcher->getEpoll(), EPOLL_CTL_MOD, connection, &connectionEvent) == -1) {
              throw std::runtime_error("TcpConnection::stop, epoll_ctl failed, " + lastErrorMessage());
            }

            contextPair.writeContext->interrupted = true;
            dispatcher->pushContext(contextPair.writeContext->context);
        };

        dispatcher->dispatch();
        dispatcher->getCurrentContext()->interruptProcedure = nullptr;
        assert(dispatcher != nullptr);
        assert(operationContext.context == dispatcher->getCurrentContext());
        assert(contextPair.writeContext == &operationContext);

        if (operationContext.interrupted) {
          contextPair.writeContext = nullptr;
          throw InterruptedException();
        }

        contextPair.writeContext = nullptr;
        if(contextPair.readContext != nullptr) { //read is presented, rearm
          epoll_event connectionEvent;
          connectionEvent.events = EPOLLIN | EPOLLONESHOT;
          connectionEvent.data.ptr = &contextPair;

          if (epoll_ctl(dispatcher->getEpoll(), EPOLL_CTL_MOD, connection, &connectionEvent) == -1) {
            message = "epoll_ctl failed, " + lastErrorMessage();
            throw std::runtime_error("TcpConnection::write, " + message);
          }
        }

        if((operationContext.events & (EPOLLERR | EPOLLHUP)) != 0) {
          throw std::runtime_error("TcpConnection::write, events & (EPOLLERR | EPOLLHUP) != 0");
        }

        ssize_t transferred = ::send(connection, (void *)data, size, 0);
        if (transferred == -1) {
          message = "send failed, "  + lastErrorMessage();
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

TcpConnection::TcpConnection(Dispatcher& dispatcher, int socket) : dispatcher(&dispatcher), connection(socket) {
  contextPair.readContext = nullptr;
  contextPair.writeContext = nullptr;
  epoll_event connectionEvent;
  connectionEvent.events = EPOLLONESHOT;
  connectionEvent.data.ptr = nullptr;

  if (epoll_ctl(dispatcher.getEpoll(), EPOLL_CTL_ADD, socket, &connectionEvent) == -1) {
    throw std::runtime_error("TcpConnection::TcpConnection, epoll_ctl failed, " + lastErrorMessage());
  }
}

}
