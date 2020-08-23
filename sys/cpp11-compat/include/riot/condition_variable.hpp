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
 * @brief  C++11 condition variable drop in replacement
 * @see    <a href="http://en.cppreference.com/w/cpp/thread/condition_variable">
 *           std::condition_variable
 *         </a>
 *
 * @author Raphael Hiesgen <raphael.hiesgen (at) haw-hamburg.de>
 *
 * @}
 */

#ifndef RIOT_CONDITION_VARIABLE_HPP
#define RIOT_CONDITION_VARIABLE_HPP

#include "sched.h"
#include "xtimer.h"
#include "priority_queue.h"

#include "riot/mutex.hpp"
#include "riot/chrono.hpp"

namespace riot {

/**
 * @brief Status for timeout-based calls of the condition variable.
 */
enum class cv_status {
  no_timeout,
  timeout
};

/**
 * @brief C++11 compliant implementation of condition variable, uses the time
 *        point implemented in our chrono replacement instead of the
 *        specified one
 * @see   <a href="http://en.cppreference.com/w/cpp/thread/condition_variable">
 *          std::condition_variable
 *        </a>
 */
class condition_variable {
public:
  /**
   * @brief The native handle type used by the condition variable.
   */
  using native_handle_type = priority_queue_t*;
  inline condition_variable() { m_queue.first = NULL; }
  ~condition_variable();

  /**
   * @brief Notify one thread waiting on this condition.
   */
  void notify_one() noexcept;
  /**
   * @brief Notify all threads waiting on this condition variable.
   */
  void notify_all() noexcept;

  /**
   * @brief Block until woken up through the condition variable.
   * @param lock  A lock that is locked by the current thread.
   */
  void wait(unique_lock<mutex>& lock) noexcept;
  /**
   * @brief Block until woken up through the condition variable and a predicate
   *        is fulfilled.
   * @param lock  A lock that is locked by the current thread.
   * @param pred  A predicate that returns a bool to signify if the thread
   *              should continue to wait when woken up through the cv.
   */
  template <class Predicate>
  void wait(unique_lock<mutex>& lock, Predicate pred);
  /**
   * @brief Block until woken up through the condition variable or a specified
   *        point in time is reached. The lock is reacquired either way.
   * @param lock          A lock that is locked by the current thread.
   * @param timeout_time  Point in time when the thread is woken up
   *                      independently of the condition variable.
   * @return A status to signify if woken up due to a timeout or the cv.
   */
  cv_status wait_until(unique_lock<mutex>& lock,
                       const time_point& timeout_time);
  /**
   * @brief Block until woken up through the condition variable and a predicate
   *        is fulfilled or a specified point in time is reached. The lock is
   *        reacquired either way.
   * @param lock          A lock that is locked by the current thread.
   * @param timeout_time  Point in time when the thread is woken up
   *                      independently of the condition variable.
   * @param pred          A predicate that returns a bool to signify if the
   *                      thread should continue to wait when woken up through
   *                      the cv.
   * @return Result of the pred when the function returns.
   */
  template <class Predicate>
  bool wait_until(unique_lock<mutex>& lock, const time_point& timeout_time,
                  Predicate pred);

  /**
   * @brief Blocks until woken up through the condition variable or when the
   *        thread has been blocked for a certain time.
   * @param lock      A lock that is locked by the current thread.
   * @param rel_time  The maximum time spent blocking.
   * @return A status to signify if woken up due to a timeout or the cv.
   */
  template <class Rep, class Period>
  cv_status wait_for(unique_lock<mutex>& lock,
                     const std::chrono::duration<Rep, Period>& rel_time);
  /**
   * @brief Blocks until woken up through the condition variable and a predicate
   *        is fulfilled or when the thread has been blocked for a certain time.
   * @param lock      A lock that is locked by the current thread.
   * @param rel_time  The maximum time spent blocking.
   * @param pred      A predicate that returns a bool to signify if the thread
   *                  should continue to wait when woken up through the cv.
   * @return Result of the pred when the function returns.
   */
  template <class Rep, class Period, class Predicate>
  bool wait_for(unique_lock<mutex>& lock,
                const std::chrono::duration<Rep, Period>& rel_time,
                Predicate pred);

  /**
   * @brief Returns the native handle of the condition variable.
   */
  inline native_handle_type native_handle() { return &m_queue; }

private:
  condition_variable(const condition_variable&);
  condition_variable& operator=(const condition_variable&);

  priority_queue_t m_queue;
};

template <class Predicate>
void condition_variable::wait(unique_lock<mutex>& lock, Predicate pred) {
  while (!pred()) {
    wait(lock);
  }
}

template <class Predicate>
bool condition_variable::wait_until(unique_lock<mutex>& lock,
                                    const time_point& timeout_time,
                                    Predicate pred) {
  while (!pred()) {
    if (wait_until(lock, timeout_time) == cv_status::timeout) {
      return pred();
    }
  }
  return true;
}

template <class Rep, class Period>
cv_status condition_variable::wait_for(unique_lock<mutex>& lock,
                                       const std::chrono::duration
                                       <Rep, Period>& timeout_duration) {
  using namespace std::chrono;
  using std::chrono::duration;
  if (timeout_duration <= timeout_duration.zero()) {
    return cv_status::timeout;
  }
  timex_t timeout, before, after;
  auto s = duration_cast<seconds>(timeout_duration);
  timeout.seconds = s.count();
  timeout.microseconds
    = (duration_cast<microseconds>(timeout_duration - s)).count();
  xtimer_now_timex(&before);
  xtimer_t timer;
  xtimer_set_wakeup(&timer, timex_uint64(timeout), thread_getpid());
  wait(lock);
  xtimer_now_timex(&after);
  xtimer_remove(&timer);
  auto passed = timex_sub(after, before);
  auto cmp = timex_cmp(passed, timeout);
  return cmp < 1 ? cv_status::no_timeout : cv_status::timeout;
}

template <class Rep, class Period, class Predicate>
inline bool condition_variable::wait_for(unique_lock<mutex>& lock,
                                         const std::chrono::duration
                                         <Rep, Period>& timeout_duration,
                                         Predicate pred) {
  return wait_until(lock, std::chrono::steady_clock::now() + timeout_duration,
                    std::move(pred));
}

} // namespace riot

#endif // RIOT_CONDITION_VARIABLE_HPP
