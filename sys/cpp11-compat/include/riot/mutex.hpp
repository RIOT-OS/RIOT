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
 * @brief C++11 compliant implementation of mutex, uses the time point
 *              implemented in our chrono replacement instead of the specified
 *              one
 * @see   <a href="http://en.cppreference.com/w/cpp/thread/mutex">
 *          std::mutex
 *        </a>
 */
class mutex {
public:
  /**
   * The native handle type used by the mutex.
   */
  using native_handle_type = mutex_t*;

  inline constexpr mutex() noexcept : m_mtx{{0}} {}
  ~mutex();

  /**
   * @brief Lock the mutex.
   */
  void lock();
  /**
   * @brief Try to lock the mutex.
   * @return `true` if the mutex was locked, `false` otherwise.
   */
  bool try_lock() noexcept;
  /**
   * @brief Unlock the mutex.
   */
  void unlock() noexcept;

  /**
   * @brief Provides access to the native handle.
   * @return The native handle of the mutex.
   */
  inline native_handle_type native_handle() { return &m_mtx; }

private:
  mutex(const mutex&);
  mutex& operator=(const mutex&);

  mutex_t m_mtx;
};

/**
 * @brief Tag type for defer lock strategy.
 */
struct defer_lock_t {};
/**
 * @brief Tag type for try lock strategy.
 */
struct try_to_lock_t {};
/**
 * @brief Tag type for adopt lock strategy.
 */
struct adopt_lock_t {};

/**
 * @brief Tag constant for defer lock strategy.
 */
constexpr defer_lock_t defer_lock = defer_lock_t();
/**
 * @brief Tag constant for try lock strategy.
 */
constexpr try_to_lock_t try_to_lock = try_to_lock_t();
/**
 * @brief Tag constant for adopt lock strategy.
 */
constexpr adopt_lock_t adopt_lock = adopt_lock_t();

/**
 * @brief C++11 compliant implementation of unique lock
 * @see   <a href="http://en.cppreference.com/w/cpp/thread/lock_guard">
 *          std::lock_guard
 *        </a>
 */
template <class Mutex>
class lock_guard {
public:
  /**
   * The type of Mutex used by the lock_guard.
   */
  using mutex_type = Mutex;

  /**
   * @brief Constructs a lock_gurad from a Mutex and locks it.
   */
  inline explicit lock_guard(mutex_type& mtx) : m_mtx(mtx) { m_mtx.lock(); }
  /**
   * @brief Constructs a lock_guard from a Mutex, acquireing ownership without
   *        locking it.
   */
  inline lock_guard(mutex_type& mtx, adopt_lock_t) : m_mtx{mtx} {}
  inline ~lock_guard() { m_mtx.unlock(); }

private:
  mutex_type& m_mtx;
};

/**
 * @brief C++11 compliant implementation of unique lock
 * @see   <a href="http://en.cppreference.com/w/cpp/thread/unique_lock">
 *          std::unique_lock
 *        </a>
 */
template <class Mutex>
class unique_lock {
public:
  /**
   * The type of Mutex used by the lock.
   */
  using mutex_type = Mutex;

  inline unique_lock() noexcept : m_mtx{nullptr}, m_owns{false} {}
  /**
   * @brief Constructs a unique_lock from a Mutex and locks it.
   */
  inline explicit unique_lock(mutex_type& mtx) : m_mtx{&mtx}, m_owns{true} {
    m_mtx->lock();
  }
  /**
   * @brief Constructs a unique_lock from a Mutex but does not lock it.
   */
  inline unique_lock(mutex_type& mtx, defer_lock_t) noexcept : m_mtx{&mtx},
                                                               m_owns{false} {}
  /**
   * @brief Constructs a unique_lock from a Mutex and tries to lock it.
   */
  inline unique_lock(mutex_type& mtx, try_to_lock_t)
      : m_mtx{&mtx}, m_owns{mtx.try_lock()} {}
  /**
   * @brief Constructs a unique_lock from a Mutex that is already owned by the
   *        thread.
   */
  inline unique_lock(mutex_type& mtx, adopt_lock_t)
      : m_mtx{&mtx}, m_owns{true} {}
  inline ~unique_lock() {
    if (m_owns) {
      m_mtx->unlock();
    }
  }
  /**
   * @brief Move constructor.
   */
  inline unique_lock(unique_lock&& lock) noexcept : m_mtx{lock.m_mtx},
                                                    m_owns{lock.m_owns} {
    lock.m_mtx = nullptr;
    lock.m_owns = false;
  }
  /**
   * @brief Move assignment operator.
   */
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

  /**
   * @brief Locks the associated mutex.
   */
  void lock();
  /**
   * @brief Tries to lock the associated mutex.
   * @return `true` if the mutex has been locked successfully,
   *         `false` otherwise.
   */
  bool try_lock();
  /**
   * @brief Unlocks the associated mutex.
   */
  void unlock();

  /**
   * @brief Swap this unique_lock with another unique_lock.
   */
  inline void swap(unique_lock& lock) noexcept {
    std::swap(m_mtx, lock.m_mtx);
    std::swap(m_owns, lock.m_owns);
  }

  /**
   * @brief Disassociate this lock from its mutex. The caller is responsible to
   *        unlock the mutex if it was locked before.
   * @return A pointer to the associated mutex or `nullptr` if there was none.
   */
  inline mutex_type* release() noexcept {
    mutex_type* mtx = m_mtx;
    m_mtx = nullptr;
    m_owns = false;
    return mtx;
  }

  /**
   * @brief Query ownership of the associate mutex.
   * @return `true` if an associated mutex exists and the lock owns it,
   *         `false` otherwise.
   */
  inline bool owns_lock() const noexcept { return m_owns; }
  /**
   * @brief Operator to query the ownership of the associated mutex.
   * @return `true` if an associated mutex exists and the lock owns it,
   *         `false` otherwise.
   */
  inline explicit operator bool() const noexcept { return m_owns; }
  /**
   * @brief Provides access to the associated mutex.
   * @return A pointer to the associated mutex or nullptr it there was none.
   */
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

/**
 * @brief Swaps two mutexes.
 * @param[inout] lhs    Reference to one mutex.
 * @param[inout] rhs    Reference to the other mutex.
 */
template <class Mutex>
inline void swap(unique_lock<Mutex>& lhs, unique_lock<Mutex>& rhs) noexcept {
  lhs.swap(rhs);
}

} // namespace riot

#endif // RIOT_MUTEX_HPP
