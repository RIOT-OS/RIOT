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
#include <cassert>
#include <system_error>

#include "riot/mutex.hpp"
#include "riot/chrono.hpp"
#include "riot/thread.hpp"
#include "riot/condition_variable.hpp"

using namespace std;
using namespace riot;

/* http://en.cppreference.com/w/cpp/thread/thread */
int main() {
  puts("\n************ C++ thread test ***********");

  assert(sched_num_threads == 2); // main + idle

  puts("Creating one thread and passing an argument ...");
  {
    constexpr int i = 3;
    thread t([=](const int j) { assert(j == i); }, i);
    try {
      t.join();
    }
    catch (const std::system_error& e) {
      assert(false);
    }
  }
  puts("Done\n");

  assert(sched_num_threads == 2);

  puts("Creating detached thread ...");
  {
    thread t([] {
      // nop
    });
    assert(t.joinable() == 1);
    t.detach();
    assert(t.joinable() == 0);
  }
  puts("Done\n");

  assert(sched_num_threads == 2);

  puts("Join on 'finished' thread ...");
  {
    thread t;
    assert(t.joinable() == 0);
    t = thread([] {
      // nop
    });
    assert(t.joinable() == 1);
    try {
      t.join();
    }
    catch (const std::system_error& e) {
      assert(false);
    }
  }
  puts("Done\n");

  assert(sched_num_threads == 2);

  puts("Join on 'running' thread ...");
  {
    mutex m;
    thread t1, t2;
    condition_variable cv;
    assert(t1.joinable() == 0);
    assert(t2.joinable() == 0);
    t1 = thread([&m, &cv] {
      unique_lock<mutex> lk(m);
      cv.wait(lk);
    });
    assert(t1.joinable() == 1);
    t2 = thread([&t1] {
      try {
        t1.join();
      }
      catch (const std::system_error& e) {
        assert(false);
      }
    });
    assert(t2.joinable() == 1);
    cv.notify_one();
    try {
      t2.join();
    }
    catch (const std::system_error& e) {
      assert(false);
    }
    assert(t1.joinable() == 0);
    assert(t2.joinable() == 0);
  }
  puts("Done\n");

  assert(sched_num_threads == 2);

  puts("Testing sleep_for ...");
  {
    timex_t before, after;
    vtimer_now(&before);
    this_thread::sleep_for(chrono::seconds(1));
    vtimer_now(&after);
    auto diff = timex_sub(after, before);
    assert(diff.seconds >= 1);
  }
  puts("Done\n");

  assert(sched_num_threads == 2);

  puts("Testing sleep_until ...");
  {
    timex_t before, after;
    vtimer_now(&before);
    this_thread::sleep_until(riot::now() += chrono::seconds(1));
    vtimer_now(&after);
    auto diff = timex_sub(after, before);
    assert(diff.seconds >= 1);
  }
  puts("Done\n");

  assert(sched_num_threads == 2);

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
    assert(t1_old == t2.get_id());
    assert(t2_old == t1.get_id());
    t1.join();
    t2.join();
  }
  puts("Done\n");

  assert(sched_num_threads == 2);

  puts("Move constructor ...");
  {
    thread t1([] {
      // nop
    });
    thread t2(move(t1));
    assert(t1.joinable() == 0);
    assert(t2.joinable() == 1);
    t2.join();
  }
  puts("Done\n");

  assert(sched_num_threads == 2);

  puts("Bye, bye.");
  puts("******************************************");

  return 0;
}
