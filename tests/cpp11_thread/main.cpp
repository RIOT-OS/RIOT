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
 * @brief test thread replacement header
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

/* http://en.cppreference.com/w/cpp/thread/thread */
int main() {
  puts("\n************ C++ thread test ***********");

  const int initial_num_threads = sched_num_threads;

  puts("Creating one thread and passing an argument ...");
  {
    const int i = initial_num_threads + 1;
    thread t([=](const int j) { expect(j == i); }, i);
    try {
      t.join();
    }
    catch (const std::system_error& e) {
      expect(false);
    }
  }
  puts("Done\n");

  expect(sched_num_threads == initial_num_threads);

  puts("Creating detached thread ...");
  {
    thread t([] {
      // nop
    });
    expect(t.joinable() == 1);
    t.detach();
    expect(t.joinable() == 0);
  }
  puts("Done\n");

  expect(sched_num_threads == initial_num_threads);

  puts("Join on 'finished' thread ...");
  {
    thread t;
    expect(t.joinable() == 0);
    t = thread([] {
      // nop
    });
    expect(t.joinable() == 1);
    try {
      t.join();
    }
    catch (const std::system_error& e) {
      expect(false);
    }
  }
  puts("Done\n");

  expect(sched_num_threads == initial_num_threads);

  puts("Join on 'running' thread ...");
  {
    mutex m;
    thread t1, t2;
    condition_variable cv;
    expect(t1.joinable() == 0);
    expect(t2.joinable() == 0);
    t1 = thread([&m, &cv] {
      unique_lock<mutex> lk(m);
      cv.wait(lk);
    });
    expect(t1.joinable() == 1);
    t2 = thread([&t1] {
      try {
        t1.join();
      }
      catch (const std::system_error& e) {
        expect(false);
      }
    });
    expect(t2.joinable() == 1);
    cv.notify_one();
    try {
      t2.join();
    }
    catch (const std::system_error& e) {
      expect(false);
    }
    expect(t1.joinable() == 0);
    expect(t2.joinable() == 0);
  }
  puts("Done\n");

  expect(sched_num_threads == initial_num_threads);

  puts("Testing sleep_for ...");
  {
    timex_t before, after;
    xtimer_now_timex(&before);
    this_thread::sleep_for(chrono::seconds(1));
    xtimer_now_timex(&after);
    auto diff = timex_sub(after, before);
    expect(diff.seconds >= 1);
  }
  puts("Done\n");

  expect(sched_num_threads == initial_num_threads);

  puts("Testing sleep_until ...");
  {
    timex_t before, after;
    xtimer_now_timex(&before);
    this_thread::sleep_until(riot::now() += chrono::seconds(1));
    xtimer_now_timex(&after);
    auto diff = timex_sub(after, before);
    expect(diff.seconds >= 1);
  }
  puts("Done\n");

  expect(sched_num_threads == initial_num_threads);

  puts("Swapping two threads ...");
  {
    thread t1([] {
      // nop
    });
    thread t2([] {
      // nop
    });
    auto t1_old = t1.get_id();
    auto t2_old = t2.get_id();
    t1.swap(t2);
    expect(t1_old == t2.get_id());
    expect(t2_old == t1.get_id());
    t1.join();
    t2.join();
  }
  puts("Done\n");

  expect(sched_num_threads == initial_num_threads);

  puts("Move constructor ...");
  {
    thread t1([] {
      // nop
    });
    thread t2(move(t1));
    expect(t1.joinable() == 0);
    expect(t2.joinable() == 1);
    t2.join();
  }
  puts("Done\n");

  expect(sched_num_threads == initial_num_threads);

  puts("Bye, bye.");
  puts("******************************************");

  return 0;
}
