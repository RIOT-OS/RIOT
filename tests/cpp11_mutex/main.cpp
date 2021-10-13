/*
 * Copyright (C) 2015 Hamburg University of Applied Sciences (HAW)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief test mutex replacement header
 *
 * @author Raphael Hiesgen <raphael.hiesgen@haw-hamburg.de>
 *
 * @}
 */
#include <string>
#include <cstdio>
#include <system_error>

#include "riot/mutex.hpp"
#include "riot/chrono.hpp"
#include "riot/thread.hpp"
#include "riot/condition_variable.hpp"

#include "test_utils/expect.h"

using namespace std;
using namespace riot;

/* http://en.cppreference.com/w/cpp/thread/mutex */
int main() {
  puts("\n************ C++ mutex test ***********");

  puts("Lock and unlock ... ");
  {
    mutex m;
    int resource = 0;
    auto f = [&m, &resource] {
      for (int i = 0; i < 3; ++i) {
        m.lock();
        ++resource;
        this_thread::sleep_for(chrono::milliseconds(100));
        m.unlock();
      }
    };
#ifndef NDEBUG
    /* We can't use expect here, otherwise cppcheck will produce errors */
    assert(resource == 0);
#endif
    auto start = std::chrono::system_clock::now();
    thread t1(f);
    thread t2(f);
    t1.join();
    t2.join();
#ifndef NDEBUG
    /* We can't use expect here, otherwise cppcheck will produce errors */
    assert(resource == 6);
#endif
    auto duration = std::chrono::duration_cast
      <chrono::milliseconds>(std::chrono::system_clock::now() - start);
    expect(duration.count() >= 600);
  }
  puts("Done\n");

  puts("Try_lock ...");
  {
    mutex m;
    m.lock();
    thread([&m] {
             auto res = m.try_lock();
             expect(res == false);
           }).detach();
    m.unlock();
  }

  {
    mutex m;
    thread([&m] {
             auto res = m.try_lock();
             expect(res == true);
             m.unlock();
           }).detach();
  }
  puts("Done\n");

  puts("Bye, bye.");
  puts("*****************************************\n");

  return 0;
}
