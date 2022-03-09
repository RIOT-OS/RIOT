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
 * @brief test condition variable replacement header
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

/* http://en.cppreference.com/w/cpp/thread/condition_variable */
int main() {
  puts("\n************ C++ condition_variable test ***********");

  puts("Wait with predicate and notify one ... ");
  {
    mutex m;
    condition_variable cv;
    string data;
    bool ready = false;
    bool processed = false;
    thread worker([&] {
      unique_lock<mutex> lk(m);
      cv.wait(lk, [&ready] { return ready; });
      data += " after processing";
      processed = true;
      cv.notify_one();
    });
    data = "Example data";
    {
      lock_guard<mutex> lk(m);
      /* cppcheck-suppress unreadVariable
       * (reason: variable is read in the thread created above) */
      ready = true;
      cv.notify_one();
    }
    {
      unique_lock<mutex> lk(m);
      cv.wait(lk, [&processed] { return processed; });
    }
    string expected = "Example data after processing";
    expect(data == expected);
    worker.join();
  }
  puts("Done\n");

  puts("Wait and notify all ...");
  {
    mutex m;
    condition_variable cv;
    auto waits = [&m, &cv] {
      unique_lock<mutex> lk(m);
      cv.wait(lk);
    };
    thread t1(waits);
    thread t2(waits);
    thread t3(waits);
    thread([&m, &cv] {
             unique_lock<mutex> lk(m);
             cv.notify_all();
           }).detach();
    t1.join();
    t2.join();
    t3.join();
  }
  puts("Done\n");

  puts("Wait for ...");
  {
    using chrono::system_clock;
    constexpr unsigned timeout = 1;
    mutex m;
    condition_variable cv;
    timex_t before, after;
    unique_lock<mutex> lk(m);
    xtimer_now_timex(&before);
    cv.wait_for(lk, chrono::seconds(timeout));
    xtimer_now_timex(&after);
    auto diff = timex_sub(after, before);
    expect(diff.seconds >= timeout);
  }
  puts("Done\n");

  puts("Wait until ...");
  {
    using chrono::system_clock;
    constexpr unsigned timeout = 1;
    mutex m;
    condition_variable cv;
    timex_t before, after;
    unique_lock<mutex> lk(m);
    xtimer_now_timex(&before);
    auto time = riot::now() += chrono::seconds(timeout);
    cv.wait_until(lk, time);
    xtimer_now_timex(&after);
    auto diff = timex_sub(after, before);
    expect(diff.seconds >= timeout);
  }
  puts("Done\n");

  puts("Bye, bye. ");
  puts("******************************************************\n");

  return 0;
}
