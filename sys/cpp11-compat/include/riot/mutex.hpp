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
 * @brief   C++11 mutex drop in replacement
 * @see     <a href="http://en.cppreference.com/w/cpp/thread/mutex">
 *            std::mutex, std::lock_guard and std::unique_lock
 *          </a>
 *
 * @author  Raphael Hiesgen <raphael.hiesgen (at) haw-hamburg.de>
 *
 * @}
 */

#ifndef RIOT_MUTEX_HPP
#define RIOT_MUTEX_HPP

#include "mutex.h"

#include <utility>
#include <stdexcept>
#include <system_error>

namespace riot {

/**
 * @brief C++11 complient implementation of mutex, uses the time point
 *              implemented in our chrono replacement instead of the specified
 *              one
 * @see   <a href="http://en.cppreference.com/w/cpp/thread/mutex">
 *          std::mutex
 *        </a>
 */
class mutex {
 public:
  using native_handle_type = mutex_t*;

  inline constexpr mutex() noexcept : m_mtx{0, PRIORITY_QUEUE_INIT} {}
  ~mutex();

  void lock();
  bool try_lock() noexcept;
  void unlock() noexcept;

  inline native_handle_type native_handle() { return &m_mtx; }

 private:
  mutex(const mutex&);
  mutex& operator=(const mutex&);

  mutex_t m_mtx;
};

struct defer_lock_t {};
struct try_to_lock_t {};
struct adopt_lock_t {};

constexpr defer_lock_t defer_lock = defer_lock_t();
constexpr try_to_lock_t try_to_lock = try_to_lock_t();
constexpr adopt_lock_t adopt_lock = adopt_lock_t();

/**
 * @brief C++11 complient implementation of unique lock
 * @see   <a href="http://en.cppreference.com/w/cpp/thread/lock_guard">
 *          std::lock_guard
 *        </a>
 */
template <class Mutex>
class lock_guard {

 public:
  using mutex_type = Mutex;

  inline explicit lock_guard(mutex_type& mtx) : m_mtx(mtx) { m_mtx.lock(); }
  inline lock_guard(mutex_type& mtx, adopt_lock_t) : m_mtx{mtx} {}
  inline ~lock_guard() { m_mtx.unlock(); }

 private:
  mutex_type& m_mtx;
};

/**
 * @brief C++11 complient implementation of unique lock
 * @see   <a href="http://en.cppreference.com/w/cpp/thread/unique_lock">
 *          std::unique_lock
 *        </a>
 */
template <class Mutex>
class unique_lock {

 public:
  using mutex_type = Mutex;

  inline unique_lock() noexcept : m_mtx{nullptr}, m_owns{false} {}
  inline explicit unique_lock(mutex_type& mtx) : m_mtx{&mtx}, m_owns{true} {
    m_mtx->lock();
  }
  inline unique_lock(mutex_type& mtx, defer_lock_t) noexcept : m_mtx{&mtx},
                                                               m_owns{false} {}
  inline unique_lock(mutex_type& mtx, try_to_lock_t)
      : m_mtx{&mtx}, m_owns{mtx.try_lock()} {}
  inline unique_lock(mutex_type& mtx, adopt_lock_t)
      : m_mtx{&mtx}, m_owns{true} {}
  inline ~unique_lock() {
    if (m_owns) {
      m_mtx->unlock();
    }
  }
  inline unique_lock(unique_lock&& lock) noexcept : m_mtx{lock.m_mtx},
                                                    m_owns{lock.m_owns} {
    lock.m_mtx = nullptr;
    lock.m_owns = false;
  }
  inline unique_lock& operator=(unique_lock&& lock) noexcept {
    if (m_owns) {
      m_mtx->unlock();
    }
    m_mtx = lock.m_mtx;
    m_owns = lock.m_owns;
    lock.m_mtx = nullptr;
    lock.m_owns = false;
    return *this;
  }

  void lock();
  bool try_lock();

  void unlock();

  inline void swap(unique_lock& lock) noexcept {
    std::swap(m_mtx, lock.m_mtx);
    std::swap(m_owns, lock.m_owns);
  }

  inline mutex_type* release() noexcept {
    mutex_type* mtx = m_mtx;
    m_mtx = nullptr;
    m_owns = false;
    return mtx;
  }

  inline bool owns_lock() const noexcept { return m_owns; }
  inline explicit operator bool() const noexcept { return m_owns; }
  inline mutex_type* mutex() const noexcept { return m_mtx; }

 private:
  unique_lock(unique_lock const&);
  unique_lock& operator=(unique_lock const&);

  mutex_type* m_mtx;
  bool m_owns;
};

template <class Mutex>
void unique_lock<Mutex>::lock() {
  if (m_mtx == nullptr) {
    throw std::system_error(
      std::make_error_code(std::errc::operation_not_permitted),
      "References null mutex.");
  }
  if (m_owns) {
    throw std::system_error(
      std::make_error_code(std::errc::resource_deadlock_would_occur),
      "Already locked.");
  }
  m_mtx->lock();
  m_owns = true;
}

template <class Mutex>
bool unique_lock<Mutex>::try_lock() {
  if (m_mtx == nullptr) {
    throw std::system_error(
      std::make_error_code(std::errc::operation_not_permitted),
      "References null mutex.");
  }
  if (m_owns) {
    throw std::system_error(
      std::make_error_code(std::errc::resource_deadlock_would_occur),
      "Already locked.");
  }
  m_owns = m_mtx->try_lock();
  return m_owns;
}

template <class Mutex>
void unique_lock<Mutex>::unlock() {
  if (!m_owns) {
    throw std::system_error(
      std::make_error_code(std::errc::operation_not_permitted),
      "Mutex not locked.");
  }
  m_mtx->unlock();
  m_owns = false;
}

template <class Mutex>
inline void swap(unique_lock<Mutex>& lhs, unique_lock<Mutex>& rhs) noexcept {
  lhs.swap(rhs);
}

} // namespace riot

#endif // RIOT_MUTEX_HPP
