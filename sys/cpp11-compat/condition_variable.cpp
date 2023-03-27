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
 * @brief   C++11 condition variable drop in replacement
 *
 * @author  Raphael Hiesgen <raphael.hiesgen (at) haw-hamburg.de>
 *
 * @}
 */

#include <stdexcept>
#include <system_error>

#include "irq.h"
#include "sched.h"
#include "thread.h"
#include "time_units.h"
#include "ztimer64.h"
#include "priority_queue.h"

#include "riot/condition_variable.hpp"

using namespace std::chrono;

namespace riot {

condition_variable::~condition_variable() { m_queue.first = NULL; }

void condition_variable::notify_one() noexcept {
  unsigned old_state = irq_disable();
  priority_queue_node_t* head = priority_queue_remove_head(&m_queue);
  int other_prio = -1;
  if (head != NULL) {
    thread_t* other_thread = (thread_t*)sched_threads[head->data];
    if (other_thread) {
      other_prio = other_thread->priority;
      sched_set_status(other_thread, STATUS_PENDING);
    }
    head->data = -1u;
  }
  irq_restore(old_state);
  if (other_prio >= 0) {
    sched_switch(other_prio);
  }
}

void condition_variable::notify_all() noexcept {
  unsigned old_state = irq_disable();
  int other_prio = -1;
  while (true) {
    priority_queue_node_t* head = priority_queue_remove_head(&m_queue);
    if (head == NULL) {
      break;
    }
    thread_t* other_thread = (thread_t*)sched_threads[head->data];
    if (other_thread) {
      auto max_prio
        = [](int a, int b) { return (a < 0) ? b : ((a < b) ? a : b); };
      other_prio = max_prio(other_prio, other_thread->priority);
      sched_set_status(other_thread, STATUS_PENDING);
    }
    head->data = -1u;
  }
  irq_restore(old_state);
  if (other_prio >= 0) {
    sched_switch(other_prio);
  }
}

void condition_variable::wait(unique_lock<mutex>& lock) noexcept {
  priority_queue_node_t n;
  n.priority = thread_get_active()->priority;
  n.data = thread_getpid();
  n.next = NULL;
  // the signaling thread may not hold the mutex, the queue is not thread safe
  unsigned old_state = irq_disable();
  priority_queue_add(&m_queue, &n);
  irq_restore(old_state);
  mutex_unlock_and_sleep(lock.mutex()->native_handle());
  if (n.data != -1u) {
    // on signaling n.data is set to -1u
    // if it isn't set, then the wakeup is either spurious or a timer wakeup
    old_state = irq_disable();
    priority_queue_remove(&m_queue, &n);
    irq_restore(old_state);
  }
  mutex_lock(lock.mutex()->native_handle());
}

cv_status condition_variable::wait_until(unique_lock<mutex>& lock,
                                         const time_point& timeout_time) {
  ztimer64_t timer;
  uint64_t total_timeout_time_us = timeout_time.microseconds();
  total_timeout_time_us += timeout_time.seconds() * US_PER_SEC;

  ztimer64_set_wakeup_at(ZTIMER64_USEC, &timer, total_timeout_time_us,
                         thread_getpid());
  wait(lock);
  if (ztimer64_now(ZTIMER64_USEC) >= total_timeout_time_us) {
    ztimer64_remove(ZTIMER64_USEC, &timer);
    return cv_status::timeout;
  }
  ztimer64_remove(ZTIMER64_USEC, &timer);
  return cv_status::no_timeout;
}

} // namespace riot
