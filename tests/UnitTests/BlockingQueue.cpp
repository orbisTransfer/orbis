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
#include <gtest/gtest.h>
#include "Common/BlockingQueue.h"

#include <future>
#include <vector>
#include <numeric>
#include <memory>

class ParallelProcessor {
public:

  ParallelProcessor(size_t threads)
    : m_threads(threads) {}

  template <typename F>
  void spawn(F f) {
    for (auto& t : m_threads) {
      t = std::thread(f);
    }
  }

  void join() {
    for (auto& t : m_threads) {
      t.join();
    }
  }

private:

  std::vector<std::thread> m_threads;
  
};

// single producer, many consumers
void TestQueue_SPMC(unsigned iterations, unsigned threadCount, unsigned queueSize) {

  BlockingQueue<int> bq(queueSize);

  ParallelProcessor processor(threadCount);
  std::atomic<int64_t> result(0);

  processor.spawn([&bq, &result]{
      int v = 0;
      int64_t sum = 0;

      while (bq.pop(v)) {
        sum += v;
      }
      
      result += sum;
      // std::cout << "Sum: " << sum << std::endl;
    });

  int64_t expectedSum = 0;

  for (unsigned i = 0; i < iterations; ++i) {
    expectedSum += i;
    ASSERT_TRUE(bq.push(i));
  }

  bq.close();
  processor.join();

  ASSERT_EQ(expectedSum, result.load());
}

void TestQueue_MPSC(unsigned iterations, unsigned threadCount, unsigned queueSize) {

  BlockingQueue<int> bq(queueSize);

  ParallelProcessor processor(threadCount);
  std::atomic<unsigned> counter(0);
  std::atomic<int64_t> pushed(0);

  processor.spawn([&]{
    int64_t sum = 0;

    for(;;) {
      unsigned value = counter.fetch_add(1);
      if (value >= iterations)
        break;

      bq.push(value);
      sum += value;
    }

    pushed += sum;
    // std::cout << "Sum: " << sum << std::endl;
  });

  int64_t expectedSum = 0;

  for (unsigned i = 0; i < iterations; ++i) {
    int value;
    ASSERT_TRUE(bq.pop(value));
    expectedSum += i;
  }

  ASSERT_EQ(0, bq.size());

  processor.join();

  ASSERT_EQ(expectedSum, pushed);
}


TEST(BlockingQueue, SPMC)
{
  TestQueue_SPMC(10000, 1, 1);
  TestQueue_SPMC(10000, 4, 1);
  TestQueue_SPMC(10000, 16, 16);
  TestQueue_SPMC(10000, 16, 100);
}

TEST(BlockingQueue, MPSC)
{
  TestQueue_MPSC(10000, 1, 1);
  TestQueue_MPSC(10000, 4, 1);
  TestQueue_MPSC(10000, 16, 16);
  TestQueue_MPSC(10000, 16, 100);
}


TEST(BlockingQueue, PerfTest)
{
  // TestQueue_SPMC(1000000, 32, 1);
}

TEST(BlockingQueue, Close)
{
  BlockingQueue<int> bq(4);
  ParallelProcessor p(4);

  p.spawn([&bq] {
    int v;
    while (bq.pop(v))
      ;
  });

  bq.push(10); // enqueue 1 item

  bq.close(); // all threads should unblock and finish
  p.join(); 
}

TEST(BlockingQueue, CloseAndWait)
{
  size_t queueSize = 100;
  BlockingQueue<int> bq(queueSize);
  ParallelProcessor p(4);

  std::atomic<size_t> itemsPopped(0);

  // fill the queue
  for (int i = 0; i < queueSize; ++i)
    bq.push(i); 

  p.spawn([&bq, &itemsPopped] {
    int v;
    while (bq.pop(v)) {
      itemsPopped += 1;
      // some delay to make close() really wait
      std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
    }
  });


  // check with multiple closing
  auto f1 = std::async(std::launch::async, [&] { bq.close(true); });
  auto f2 = std::async(std::launch::async, [&] { bq.close(true); });

  bq.close(true);

  f1.get();
  f2.get();

  p.join();

  ASSERT_EQ(queueSize, itemsPopped.load());
}

TEST(BlockingQueue, AllowsMoveOnly)
{
  BlockingQueue<std::unique_ptr<int>> bq(1);

  std::unique_ptr<int> v(new int(100));
  ASSERT_TRUE(bq.push(std::move(v)));

  std::unique_ptr<int> popval;
  bq.pop(popval);

  ASSERT_EQ(*popval, 100);
}
