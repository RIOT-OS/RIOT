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
 * @see     <a href="http://en.cppreference.com/w/cpp/thread/thread">
 *            std::thread, std::this_thread
 *          </a>
 *
 * @author  Raphael Hiesgen <raphael.hiesgen (at) haw-hamburg.de>
 *
 * @}
 */

#ifndef RIOT_THREAD_HPP
#define RIOT_THREAD_HPP

#include "time.h"
#include "thread.h"

#include <array>
#include <tuple>
#include <atomic>
#include <memory>
#include <utility>
#include <exception>
#include <stdexcept>
#include <functional>
#include <type_traits>

#include "riot/mutex.hpp"
#include "riot/chrono.hpp"
#include "riot/condition_variable.hpp"

#include "riot/detail/thread_util.hpp"

namespace riot {

namespace {
/**
 * @brief Identify uninitialized threads.
 */
constexpr kernel_pid_t thread_uninitialized = -1;
/**
 * @brief The stack size for new threads.
 */
constexpr size_t stack_size = THREAD_STACKSIZE_MAIN;
}

/**
 * @brief Holds context data for the thread.
 */
struct thread_data {
  thread_data() : ref_count{2}, joining_thread{thread_uninitialized} {
    // nop
  }
  /** @cond INTERNAL */
  std::atomic<unsigned> ref_count;
  kernel_pid_t joining_thread;
  std::array<char, stack_size> stack;
  /** @endcond */
};

/**
 * @brief This deleter prevents our thread data from being destroyed if the
 * thread object is destroyed before the thread had a chance to run.
 */
struct thread_data_deleter {
  /**
   * @brief Called by the deleter of a thread object to manage the lifetime of
   *        the thread internal management data.
   */
  void operator()(thread_data* ptr) {
    if (--ptr->ref_count == 0) {
      delete ptr;
    }
  }
};

/**
 * @brief implementation of thread::id
 * @see   <a href="http://en.cppreference.com/w/cpp/thread/thread/id">
 *          thread::id
 *        </a>
 */
class thread_id {
  template <class T, class Traits>
  friend std::basic_ostream<T, Traits>& operator<<(std::basic_ostream
                                                   <T, Traits>& out,
                                                   thread_id id);
  friend class thread;

public:
  /**
   * @brief Creates a uninitialized thread id.
   */
  inline thread_id() noexcept : m_handle{thread_uninitialized} {}
  /**
   * @brief Create a thread id from a native handle.
   */
  explicit inline thread_id(kernel_pid_t handle) : m_handle{handle} {}

  /**
   * @brief Comparison operator for thread ids.
   */
  inline bool operator==(thread_id other) noexcept {
    return m_handle == other.m_handle;
  }
  /**
   * @brief Comparison operator for thread ids.
   */
  inline bool operator!=(thread_id other) noexcept {
    return !(m_handle == other.m_handle);
  }
  /**
   * @brief Comparison operator for thread ids.
   */
  inline bool operator<(thread_id other) noexcept {
    return m_handle < other.m_handle;
  }
  /**
   * @brief Comparison operator for thread ids.
   */
  inline bool operator<=(thread_id other) noexcept {
    return !(m_handle > other.m_handle);
  }
  /**
   * @brief Comparison operator for thread ids.
   */
  inline bool operator>(thread_id other) noexcept {
    return m_handle > other.m_handle;
  }
  /**
   * @brief Comparison operator for thread ids.
   */
  inline bool operator>=(thread_id other) noexcept {
    return !(m_handle < other.m_handle);
  }

private:
  kernel_pid_t m_handle;
};

/**
 * @brief Enable printing of thread ids using output streams.
 */
template <class T, class Traits>
inline std::basic_ostream<T, Traits>& operator<<(std::basic_ostream
                                                 <T, Traits>& out,
                                                 thread_id id) {
  return out << id.m_handle;
}

namespace this_thread {

/**
 * @brief Access the id of the currently running thread.
 */
inline thread_id get_id() noexcept { return thread_id{thread_getpid()}; }
/**
 * @brief Yield the currently running thread.
 */
inline void yield() noexcept { thread_yield(); }
/**
 * @brief Puts the current thread to sleep.
 * @param[in] ns    Duration to sleep in nanoseconds.
 */
void sleep_for(const std::chrono::nanoseconds& ns);
/**
 * @brief Puts the current thread to sleep.
 * @param[in] sleep_duration    The duration to sleep.
 */
template <class Rep, class Period>
void sleep_for(const std::chrono::duration<Rep, Period>& sleep_duration) {
  using namespace std::chrono;
  if (sleep_duration > std::chrono::duration<Rep, Period>::zero()) {
    constexpr std::chrono::duration<long double> max = nanoseconds::max();
    nanoseconds ns;
    if (sleep_duration < max) {
      ns = duration_cast<nanoseconds>(sleep_duration);
      if (ns < sleep_duration) {
        ++ns;
      }
    } else {
      ns = nanoseconds::max();
    }
    sleep_for(ns);
  }
}
/**
 * @brief Puts the current thread to sleep.
 * @param[in] sleep_time    A point in time that specifies when the thread
 *                          should wake up.
 */
inline void sleep_until(const riot::time_point& sleep_time) {
  mutex mtx;
  condition_variable cv;
  unique_lock<mutex> lk(mtx);
  while (riot::now() < sleep_time) {
    cv.wait_until(lk, sleep_time);
  }
}
} // namespace this_thread

/**
 * @brief   C++11 compliant implementation of thread, however uses the time
 *          point from out chrono header instead of the specified one
 * @see     <a href="http://en.cppreference.com/w/cpp/thread/thread">
 *            std::thread
 *          </a>
 */
class thread {
public:
  /**
   * @brief The id is of type `thread_id`-
   */
  using id = thread_id;
  /**
   * @brief The native handle type is the `kernel_pid_t` of RIOT.
   */
  using native_handle_type = kernel_pid_t;

  /**
   * @brief Per default, an uninitialized thread is created.
   */
  inline thread() noexcept : m_handle{thread_uninitialized} {}
  /**
   * @brief Create a thread from a functor and arguments for it.
   * @param[in] f     Functor to run as a thread.
   * @param[in] args  Arguments passed to the functor.
   */
  template <class F, class... Args>
  explicit thread(F&& f, Args&&... args);

  /**
   * @brief Disallow copy constructor.
   */
  thread(const thread&) = delete;

  /**
   * @brief Move constructor.
   */
  inline thread(thread&& t) noexcept : m_handle{t.m_handle} {
    t.m_handle = thread_uninitialized;
    std::swap(m_data, t.m_data);
  }

  ~thread();

  /**
   * @brief Disallow copy assignment operator.
   */
  thread& operator=(const thread&) = delete;

  /**
   * @brief Move assignment operator.
   */
  thread& operator=(thread&&) noexcept;

  /**
   * @brief Swap threads.
   * @param[inout] t    Thread to swap data with.
   */
  void swap(thread& t) noexcept {
    std::swap(m_data, t.m_data);
    std::swap(m_handle, t.m_handle);
  }

  /**
   * @brief Query if the thread is joinable.
   * @return  `true` if the thread is joinable, `false` otherwise.
   */
  inline bool joinable() const noexcept {
    return m_handle != thread_uninitialized;
  }
  /**
   * @brief Block until the thread finishes. Leads to an error if the thread is
   * not joinable or a thread joins itself.
   */
  void join();
  /**
   * @brief Detaches a thread from its handle and allows it to execute
   *        independently. The thread cleans up its resources when it
   *        finishes.
   */
  void detach();
  /**
   * @brief Returns the id of a thread.
   */
  inline id get_id() const noexcept { return thread_id{m_handle}; }
  /**
   * @brief Returns the native handle to a thread.
   */
  inline native_handle_type native_handle() noexcept { return m_handle; }

  /**
   *  @brief Returns the number of concurrent threads supported by the
   *         underlying hardware. Since there is no RIOT API to query this
   *         information, the function always returns 1;
   */
  static unsigned hardware_concurrency() noexcept;

private:
  kernel_pid_t m_handle;
  std::unique_ptr<thread_data, thread_data_deleter> m_data;
};

/**
 * @brief Swaps two threads.
 * @param[inout] lhs    Reference to one thread.
 * @param[inout] rhs    Reference to the other thread.
 */
void swap(thread& lhs, thread& rhs) noexcept;

/** @cond INTERNAL */
template <class Tuple>
void* thread_proxy(void* vp) {
  { // without this scope, the objects here are not cleaned up corrctly
    std::unique_ptr<Tuple> p(static_cast<Tuple*>(vp));
    auto tmp = std::get<0>(*p);
    std::unique_ptr<thread_data, thread_data_deleter> data{tmp};
    // create indices for the arguments, 0 is thread_data and 1 is the function
    auto indices = detail::get_indices<std::tuple_size<Tuple>::value, 2>();
    try {
      detail::apply_args(std::get<1>(*p), indices, *p);
    }
    catch (...) {
      // nop
    }
    if (data->joining_thread != thread_uninitialized) {
      thread_wakeup(data->joining_thread);
    }
  }
  // some riot cleanup code
  sched_task_exit();
  return nullptr;
}
/** @endcond */

template <class F, class... Args>
thread::thread(F&& f, Args&&... args)
    : m_data{new thread_data} {
  using namespace std;
  using func_and_args = tuple
    <thread_data*, typename decay<F>::type, typename decay<Args>::type...>;
  unique_ptr<func_and_args> p(
    new func_and_args(m_data.get(), forward<F>(f), forward<Args>(args)...));
  m_handle = thread_create(
    m_data->stack.data(), stack_size, THREAD_PRIORITY_MAIN - 1, 0,
    &thread_proxy<func_and_args>, p.get(), "riot_cpp_thread");
  if (m_handle >= 0) {
    p.release();
  } else {
    throw std::system_error(
      std::make_error_code(std::errc::resource_unavailable_try_again),
        "Failed to create thread.");
  }
}

inline thread& thread::operator=(thread&& other) noexcept {
  if (m_handle != thread_uninitialized) {
    std::terminate();
  }
  m_handle = other.m_handle;
  other.m_handle = thread_uninitialized;
  std::swap(m_data, other.m_data);
  return *this;
}

inline void swap(thread& lhs, thread& rhs) noexcept { lhs.swap(rhs); }

} // namespace riot

#endif // RIOT_THREAD_HPP
