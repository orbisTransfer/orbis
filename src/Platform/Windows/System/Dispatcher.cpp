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
#include "Dispatcher.h"
#include <cassert>
#include <string>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include "ErrorMessage.h"

namespace System {

namespace {

struct DispatcherContext : public OVERLAPPED {
  NativeContext* context;
};

const size_t STACK_SIZE = 16384;
const size_t RESERVE_STACK_SIZE = 2097152;
}

Dispatcher::Dispatcher() {
  static_assert(sizeof(CRITICAL_SECTION) == sizeof(Dispatcher::criticalSection), "CRITICAL_SECTION size doesn't fit sizeof(Dispatcher::criticalSection)");
  BOOL result = InitializeCriticalSectionAndSpinCount(reinterpret_cast<LPCRITICAL_SECTION>(criticalSection), 4000);
  assert(result != FALSE);
  std::string message;
  if (ConvertThreadToFiberEx(NULL, 0) == NULL) {
    message = "ConvertThreadToFiberEx failed, " + lastErrorMessage();
  } else {
    completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (completionPort == NULL) {
      message = "CreateIoCompletionPort failed, " + lastErrorMessage();
    } else {
      WSADATA wsaData;
      int wsaResult = WSAStartup(0x0202, &wsaData);
      if (wsaResult != 0) {
        message = "WSAStartup failed, " + errorMessage(wsaResult);
      } else {
        remoteNotificationSent = false;
        reinterpret_cast<LPOVERLAPPED>(remoteSpawnOverlapped)->hEvent = NULL;
        threadId = GetCurrentThreadId();

        mainContext.fiber = GetCurrentFiber();
        mainContext.interrupted = false;
        mainContext.group = &contextGroup;
        mainContext.groupPrev = nullptr;
        mainContext.groupNext = nullptr;
        contextGroup.firstContext = nullptr;
        contextGroup.lastContext = nullptr;
        contextGroup.firstWaiter = nullptr;
        contextGroup.lastWaiter = nullptr;
        currentContext = &mainContext;
        firstResumingContext = nullptr;
        firstReusableContext = nullptr;
        runningContextCount = 0;
        return;
      }

      BOOL result2 = CloseHandle(completionPort);
      assert(result2 == TRUE);
    }

    BOOL result2 = ConvertFiberToThread();
    assert(result == TRUE);
  }
  
  DeleteCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(criticalSection));
  throw std::runtime_error("Dispatcher::Dispatcher, " + message);
}

Dispatcher::~Dispatcher() {
  assert(GetCurrentThreadId() == threadId);
  for (NativeContext* context = contextGroup.firstContext; context != nullptr; context = context->groupNext) {
    interrupt(context);
  }

  yield();
  assert(timers.empty());
  assert(contextGroup.firstContext == nullptr);
  assert(contextGroup.firstWaiter == nullptr);
  assert(firstResumingContext == nullptr);
  assert(runningContextCount == 0);
  while (firstReusableContext != nullptr) {
    void* fiber = firstReusableContext->fiber;
    firstReusableContext = firstReusableContext->next;
    DeleteFiber(fiber);
  }

  int wsaResult = WSACleanup();
  assert(wsaResult == 0);
  BOOL result = CloseHandle(completionPort);
  assert(result == TRUE);
  result = ConvertFiberToThread();
  assert(result == TRUE);
  DeleteCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(criticalSection));
}

void Dispatcher::clear() {
  assert(GetCurrentThreadId() == threadId);
  while (firstReusableContext != nullptr) {
    void* fiber = firstReusableContext->fiber;
    firstReusableContext = firstReusableContext->next;
    DeleteFiber(fiber);
  }
}

void Dispatcher::dispatch() {
  assert(GetCurrentThreadId() == threadId);
  NativeContext* context;
  for (;;) {
    if (firstResumingContext != nullptr) {
      context = firstResumingContext;
      firstResumingContext = context->next;
      break;
    }

    LARGE_INTEGER frequency;
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    QueryPerformanceFrequency(&frequency);
    uint64_t currentTime = ticks.QuadPart / (frequency.QuadPart / 1000);
    auto timerContextPair = timers.begin();
    auto end = timers.end();
    while (timerContextPair != end && timerContextPair->first <= currentTime) {
      pushContext(timerContextPair->second);
      timerContextPair = timers.erase(timerContextPair);
    }

    if (firstResumingContext != nullptr) {
      context = firstResumingContext;
      firstResumingContext = context->next;
      break;
    }

    DWORD timeout = timers.empty() ? INFINITE : static_cast<DWORD>(std::min(timers.begin()->first - currentTime, static_cast<uint64_t>(INFINITE - 1)));
    OVERLAPPED_ENTRY entry;
    ULONG actual = 0;
    if (GetQueuedCompletionStatusEx(completionPort, &entry, 1, &actual, timeout, TRUE) == TRUE) {
      if (entry.lpOverlapped == reinterpret_cast<LPOVERLAPPED>(remoteSpawnOverlapped)) {
        EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(criticalSection));
        assert(remoteNotificationSent);
        assert(!remoteSpawningProcedures.empty());
        do {
          spawn(std::move(remoteSpawningProcedures.front()));
          remoteSpawningProcedures.pop();
        } while (!remoteSpawningProcedures.empty());

        remoteNotificationSent = false;
        LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(criticalSection));
        continue;
      }

      context = reinterpret_cast<DispatcherContext*>(entry.lpOverlapped)->context;
      break;
    }

    DWORD lastError = GetLastError();
    if (lastError == WAIT_TIMEOUT) {
      continue;
    }

    if (lastError != WAIT_IO_COMPLETION) {
      throw std::runtime_error("Dispatcher::dispatch, GetQueuedCompletionStatusEx failed, " + errorMessage(lastError));
    }
  }

  if (context != currentContext) {
    currentContext = context;
    SwitchToFiber(context->fiber);
  }
}

NativeContext* Dispatcher::getCurrentContext() const {
  assert(GetCurrentThreadId() == threadId);
  return currentContext;
}

void Dispatcher::interrupt() {
  interrupt(currentContext);
}

void Dispatcher::interrupt(NativeContext* context) {
  assert(GetCurrentThreadId() == threadId);
  assert(context != nullptr);
  if (!context->interrupted) {
    if (context->interruptProcedure != nullptr) {
      context->interruptProcedure();
      context->interruptProcedure = nullptr;
    } else {
      context->interrupted = true;
    }
  }
}

bool Dispatcher::interrupted() {
  if (currentContext->interrupted) {
    currentContext->interrupted = false;
    return true;
  }

  return false;
}

void Dispatcher::pushContext(NativeContext* context) {
  assert(GetCurrentThreadId() == threadId);
  assert(context != nullptr);
  context->next = nullptr;
  if (firstResumingContext != nullptr) {
    assert(lastResumingContext->next == nullptr);
    lastResumingContext->next = context;
  } else {
    firstResumingContext = context;
  }

  lastResumingContext = context;
}

void Dispatcher::remoteSpawn(std::function<void()>&& procedure) {
  EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(criticalSection));
  remoteSpawningProcedures.push(std::move(procedure));
  if (!remoteNotificationSent) {
    remoteNotificationSent = true;
    if (PostQueuedCompletionStatus(completionPort, 0, 0, reinterpret_cast<LPOVERLAPPED>(remoteSpawnOverlapped)) == NULL) {
      LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(criticalSection));
      throw std::runtime_error("Dispatcher::remoteSpawn, PostQueuedCompletionStatus failed, " + lastErrorMessage());
    };
  }

  LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(criticalSection));
}

void Dispatcher::spawn(std::function<void()>&& procedure) {
  assert(GetCurrentThreadId() == threadId);
  NativeContext* context = &getReusableContext();
  if (contextGroup.firstContext != nullptr) {
    context->groupPrev = contextGroup.lastContext;
    assert(contextGroup.lastContext->groupNext == nullptr);
    contextGroup.lastContext->groupNext = context;
  } else {
    context->groupPrev = nullptr;
    contextGroup.firstContext = context;
    contextGroup.firstWaiter = nullptr;
  }

  context->interrupted = false;
  context->group = &contextGroup;
  context->groupNext = nullptr;
  context->procedure = std::move(procedure);
  contextGroup.lastContext = context;
  pushContext(context);
}

void Dispatcher::yield() {
  assert(GetCurrentThreadId() == threadId);
  for (;;) {
    LARGE_INTEGER frequency;
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    QueryPerformanceFrequency(&frequency);
    uint64_t currentTime = ticks.QuadPart / (frequency.QuadPart / 1000);
    auto timerContextPair = timers.begin();
    auto end = timers.end();
    while (timerContextPair != end && timerContextPair->first <= currentTime) {
      timerContextPair->second->interruptProcedure = nullptr;
      pushContext(timerContextPair->second);
      timerContextPair = timers.erase(timerContextPair);
    }

    OVERLAPPED_ENTRY entries[16];
    ULONG actual = 0;
    if (GetQueuedCompletionStatusEx(completionPort, entries, 16, &actual, 0, TRUE) == TRUE) {
      assert(actual > 0);
      for (ULONG i = 0; i < actual; ++i) {
        if (entries[i].lpOverlapped == reinterpret_cast<LPOVERLAPPED>(remoteSpawnOverlapped)) {
          EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(criticalSection));
          assert(remoteNotificationSent);
          assert(!remoteSpawningProcedures.empty());
          do {
            spawn(std::move(remoteSpawningProcedures.front()));
            remoteSpawningProcedures.pop();
          } while (!remoteSpawningProcedures.empty());

          remoteNotificationSent = false;
          LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(criticalSection));
          continue;
        }

        NativeContext* context = reinterpret_cast<DispatcherContext*>(entries[i].lpOverlapped)->context;
        context->interruptProcedure = nullptr;
        pushContext(context);
      }
    } else {
      DWORD lastError = GetLastError();
      if (lastError == WAIT_TIMEOUT) {
        break;
      } else if (lastError != WAIT_IO_COMPLETION) {
        throw std::runtime_error("Dispatcher::yield, GetQueuedCompletionStatusEx failed, " + errorMessage(lastError));
      }
    }
  }

  if (firstResumingContext != nullptr) {
    pushContext(currentContext);
    dispatch();
  }
}

void Dispatcher::addTimer(uint64_t time, NativeContext* context) {
  assert(GetCurrentThreadId() == threadId);
  timers.insert(std::make_pair(time, context));
}

void* Dispatcher::getCompletionPort() const {
  return completionPort;
}

NativeContext& Dispatcher::getReusableContext() {
  if (firstReusableContext == nullptr) {
    void* fiber = CreateFiberEx(STACK_SIZE, RESERVE_STACK_SIZE, 0, contextProcedureStatic, this);
    if (fiber == NULL) {
      throw std::runtime_error("Dispatcher::getReusableContext, CreateFiberEx failed, " + lastErrorMessage());
    }

    SwitchToFiber(fiber);
    assert(firstReusableContext != nullptr);
    firstReusableContext->fiber = fiber;
  }

  NativeContext* context = firstReusableContext;
  firstReusableContext = context->next;
  return *context;
}

void Dispatcher::pushReusableContext(NativeContext& context) {
  context.next = firstReusableContext;
  firstReusableContext = &context;
  --runningContextCount;
}

void Dispatcher::interruptTimer(uint64_t time, NativeContext* context) {
  assert(GetCurrentThreadId() == threadId);
  auto range = timers.equal_range(time);
  for (auto it = range.first; ; ++it) {
    assert(it != range.second);
    if (it->second == context) {
      pushContext(context);
      timers.erase(it);
      break;
    }
  }
}

void Dispatcher::contextProcedure() {
  assert(GetCurrentThreadId() == threadId);
  assert(firstReusableContext == nullptr);
  NativeContext context;
  context.interrupted = false;
  context.next = nullptr;
  firstReusableContext = &context;
  SwitchToFiber(currentContext->fiber);
  for (;;) {
    ++runningContextCount;
    try {
      context.procedure();
    } catch (std::exception&) {
    }

    if (context.group != nullptr) {
      if (context.groupPrev != nullptr) {
        assert(context.groupPrev->groupNext == &context);
        context.groupPrev->groupNext = context.groupNext;
        if (context.groupNext != nullptr) {
          assert(context.groupNext->groupPrev == &context);
          context.groupNext->groupPrev = context.groupPrev;
        } else {
          assert(context.group->lastContext == &context);
          context.group->lastContext = context.groupPrev;
        }
      } else {
        assert(context.group->firstContext == &context);
        context.group->firstContext = context.groupNext;
        if (context.groupNext != nullptr) {
          assert(context.groupNext->groupPrev == &context);
          context.groupNext->groupPrev = nullptr;
        } else {
          assert(context.group->lastContext == &context);
          if (context.group->firstWaiter != nullptr) {
            if (firstResumingContext != nullptr) {
              assert(lastResumingContext->next == nullptr);
              lastResumingContext->next = context.group->firstWaiter;
            } else {
              firstResumingContext = context.group->firstWaiter;
            }

            lastResumingContext = context.group->lastWaiter;
            context.group->firstWaiter = nullptr;
          }
        }
      }

      pushReusableContext(context);
    }

    dispatch();
  }
}

void __stdcall Dispatcher::contextProcedureStatic(void* context) {
  static_cast<Dispatcher*>(context)->contextProcedure();
}

}
