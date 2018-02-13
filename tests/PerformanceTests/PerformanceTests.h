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

#include <iostream>
#include <stdint.h>

#include <boost/chrono.hpp>

class performance_timer
{
public:
  typedef boost::chrono::high_resolution_clock clock;

  performance_timer()
  {
    m_base = clock::now();
  }

  void start()
  {
    m_start = clock::now();
  }

  int elapsed_ms()
  {
    clock::duration elapsed = clock::now() - m_start;
    return static_cast<int>(boost::chrono::duration_cast<boost::chrono::milliseconds>(elapsed).count());
  }

private:
  clock::time_point m_base;
  clock::time_point m_start;
};


template <typename T>
class test_runner
{
public:
  test_runner()
    : m_elapsed(0)
  {
  }

  bool run()
  {
    T test;
    if (!test.init())
      return false;

    performance_timer timer;
    timer.start();
    warm_up();
    std::cout << "Warm up: " << timer.elapsed_ms() << " ms" << std::endl;

    timer.start();
    for (size_t i = 0; i < T::loop_count; ++i)
    {
      if (!test.test())
        return false;
    }
    m_elapsed = timer.elapsed_ms();

    return true;
  }

  int elapsed_time() const { return m_elapsed; }

  int time_per_call() const
  {
    static_assert(0 < T::loop_count, "T::loop_count must be greater than 0");
    return m_elapsed / T::loop_count;
  }

private:
  /**
   * Warm up processor core, enabling turbo boost, etc.
   */
  uint64_t warm_up()
  {
    const size_t warm_up_rounds = 1000 * 1000 * 1000;
    m_warm_up = 0;
    for (size_t i = 0; i < warm_up_rounds; ++i)
    {
      ++m_warm_up;
    }
    return m_warm_up;
  }

private:
  volatile uint64_t m_warm_up;  ///<! This field is intended for preclude compiler optimizations
  int m_elapsed;
};

template <typename T>
void run_test(const char* test_name)
{
  test_runner<T> runner;
  if (runner.run())
  {
    std::cout << test_name << " - OK:\n";
    std::cout << "  loop count:    " << T::loop_count << '\n';
    std::cout << "  elapsed:       " << runner.elapsed_time() << " ms\n";
    std::cout << "  time per call: " << runner.time_per_call() << " ms/call\n" << std::endl;
  }
  else
  {
    std::cout << test_name << " - FAILED" << std::endl;
  }
}

#define QUOTEME(x) #x
#define TEST_PERFORMANCE0(test_class)         run_test< test_class >(QUOTEME(test_class))
#define TEST_PERFORMANCE1(test_class, a0)     run_test< test_class<a0> >(QUOTEME(test_class<a0>))
#define TEST_PERFORMANCE2(test_class, a0, a1) run_test< test_class<a0, a1> >(QUOTEME(test_class) "<" QUOTEME(a0) ", " QUOTEME(a1) ">")
