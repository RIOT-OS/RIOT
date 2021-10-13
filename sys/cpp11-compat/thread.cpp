/*
 * Copyright (C) 2015 Hamburg University of Applied Sciences (HAW)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpp11-compat
 * @{
 *
 * @file
 * @brief   C++11 thread drop in replacement
 *
 * @author  Raphael Hiesgen <raphael.hiesgen (at) haw-hamburg.de>
 *
 * @}
 */

#include "xtimer.h"

#include <cerrno>
#include <system_error>

#include "riot/thread.hpp"

using namespace std;

namespace riot {

thread::~thread() {
  if (joinable()) {
    terminate();
  }
}

void thread::join() {
  if (this->get_id() == this_thread::get_id()) {
    throw system_error(make_error_code(errc::resource_deadlock_would_occur),
                       "Joining this leads to a deadlock.");
  }
  if (joinable()) {
    auto status = thread_getstatus(m_handle);
    if (status != STATUS_NOT_FOUND && status != STATUS_STOPPED) {
      m_data->joining_thread = thread_getpid();
      thread_sleep();
    }
    m_handle = thread_uninitialized;
  } else {
    throw system_error(make_error_code(errc::invalid_argument),
                       "Can not join an unjoinable thread.");
  }
  // missing: no_such_process system error
}

void thread::detach() {
  if (joinable()) {
    m_handle = thread_uninitialized;
  } else {
    throw system_error(make_error_code(errc::invalid_argument),
                       "Can not detach an unjoinable thread.");
  }
}

unsigned thread::hardware_concurrency() noexcept {
  // there is currently no API for this
  return 1;
}

namespace this_thread {

void sleep_for(const chrono::nanoseconds& ns) {
  using namespace chrono;
  if (ns > nanoseconds::zero()) {
    xtimer_usleep64(static_cast<uint64_t>(duration_cast<microseconds>(ns).count()));
  }
}

} // namespace this_thread

} // namespace riot
