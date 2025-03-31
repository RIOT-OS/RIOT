/*
 * Copyright (C) 2021 Inria
 *               2021 Freie Universität Berlin
 *               2021 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef ZTIMER64_H
#define ZTIMER64_H

/**
 * @ingroup     sys_ztimer
 * @defgroup    sys_ztimer64 ztimer 64bit version
 * @brief       ztimer 64bit version
 *
 * # Introduction
 *
 * This module implements a 64bit version of the ztimer API.
 * All ztimer_* functions have a ztimer64_* equivalent that behaves the same,
 * but allowing offsets >= 2^32.
 *
 * In addition to that, ztimer64 offers functions taking absolute target times
 * as parameter.
 * If the absolute target time is in the past, a timer would trigger right away.
 *
 * Here's a table for absolute variants:
 *
 * | relative version               | absolute version               |
 * |--------------------------------|--------------------------------|
 * | ztimer64_set()                 | ztimer64_set_at()              |
 * | ztimer64_sleep()               | ztimer64_sleep_until()         |
 * | ztimer64_set_msg()             | ztimer64_set_msg_at()          |
 * | ztimer64_set_wakeup()          | ztimer64_set_wakeup_at()       |
 * | ztimer64_set_timeout_flag()    | ztimer64_set_timeout_flag_at() |
 * | ztimer64_msg_receive_timeout() | ztimer64_msg_receive_until()   |
 * | ztimer64_mutex_lock_timeout()  | ztimer64_mutex_lock_until()    |
 * | ztimer64_rmutex_lock_timeout() | ztimer64_rmutex_lock_until()   |
 *
 * ## ztimer64 clocks
 *
 * ztimer64 provides automatic setup of ZTIMER64_SEC, ZTIMER64_MSEC and
 * ZTIMER64_USEC, using the ztimer(32) clocks as base clocks.
 * Enable them by depending on ztimer64_sec, ztimer64_msec and / or ztimer64_usec.
 *
 * @warning ztimer64 always keeps a timer running on the base clock. Depending
 *          on the base clock, this blocks low-power sleep modes. If the hardware
 *          supports it, the msec and sec timers should be using a low-power capable
 *          timer (e.g., periph_rtt). ztimer64_usec will almost certainly block
 *          low-power sleep.
 *
 * TODO:
 *  - some explicit power management
 *  - implement adjust_set and adjust_sleep API
 *  - implement setting a 64bit reference time (epoch)
 *
 * @{
 *
 * @file
 * @brief       ztimer 64bit API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdint.h>

#include "irq.h"
#include "mutex.h"
#include "msg.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ztimer64_t forward declaration
 */
typedef struct ztimer64_base ztimer64_base_t;
/**
 * @brief ztimer64_clock_t forward declaration
 */
typedef struct ztimer64_clock ztimer64_clock_t;

/**
 * @brief   Minimum information for each timer
 */
struct ztimer64_base {
    ztimer64_base_t *next;          /**< next timer in list */
    uint64_t target;                /**< absolute target time */
};

/**
 * @brief   ztimer64 structure
 *
 * This type represents an instance of a timer, which is set on an
 * underlying clock object
 */
typedef struct {
    ztimer64_base_t base;               /**< clock list entry */
    void (*callback)(void *arg);        /**< timer callback function pointer */
    void *arg;                          /**< timer callback argument */
} ztimer64_t;

/**
 * @brief   ztimer64 clock structure
 */
struct ztimer64_clock {
    ztimer64_base_t *first;         /**< list of active timers              */
    ztimer_clock_t *base_clock;     /**< 32bit clock backend                */
    ztimer_t base_timer;            /**< 32bit backend timer                */
    uint64_t checkpoint;            /**< lower timer checkpoint offset      */
    uint16_t adjust_set;            /**< will be subtracted on every set()  */
    uint16_t adjust_sleep;          /**< will be subtracted on every sleep(),
                                         in addition to adjust_set          */
#if MODULE_PM_LAYERED || DOXYGEN
    uint8_t block_pm_mode;          /**< min. pm mode to block for the clock to run */
#endif
};

/**
 * @brief   Get the current time from a clock
 *
 * @param[in]   clock          ztimer clock to operate on
 *
 * @return  Current count on @p clock
 */
uint64_t ztimer64_now(ztimer64_clock_t *clock);

/**
 * @brief   Get absolute target time for a clock given offset
 *
 * @param[in]   clock       ztimer64 clock to operate on
 * @param[in]   offset      relative target time
 *
 * @returns absolute target time
 */
static inline uint64_t ztimer64_offset2absolute(ztimer64_clock_t *clock,
                                                uint64_t offset)
{
    unsigned state = irq_disable();
    uint64_t result = ztimer64_now(clock) + offset;

    irq_restore(state);
    return result;
}

/**
 * @brief   Set a timer on a clock (absolute version)
 *
 * This will place @p timer in the timer targets queue of @p clock.
 *
 * @note The memory pointed to by @p timer is not copied and must
 *       remain in scope until the callback is fired or the timer
 *       is removed via @ref ztimer64_remove
 *
 * @param[in]   clock       ztimer64 clock to operate on
 * @param[in]   timer       timer entry to set
 * @param[in]   target      absolute target time
 */
void ztimer64_set_at(ztimer64_clock_t *clock, ztimer64_t *timer,
                     uint64_t target);

/**
 * @brief   Set a timer on a clock (relative version)
 *
 * This will place @p timer in the timer targets queue of @p clock.
 *
 * @note The memory pointed to by @p timer is not copied and must
 *       remain in scope until the callback is fired or the timer
 *       is removed via @ref ztimer64_remove
 *
 * @param[in]   clock       ztimer64 clock to operate on
 * @param[in]   timer       timer entry to set
 * @param[in]   offset      relative target time
 */
static inline void ztimer64_set(ztimer64_clock_t *clock, ztimer64_t *timer,
                                uint64_t offset)
{
    ztimer64_set_at(clock, timer,
                    ztimer64_offset2absolute(clock, offset));
}

/**
 * @brief   Check if a timer is currently active
 *
 * @param[in]   timer       timer to check
 *
 * @return 1 if timer is active
 * @return 0 if timer is not active
 */
unsigned ztimer64_is_set(const ztimer64_t *timer);

/**
 * @brief   Remove a timer from a clock
 *
 * This will place @p timer in the timer targets queue for @p clock.
 *
 * This function does nothing if @p timer is not found in the timer queue of
 * @p clock.
 *
 * @param[in]   clock       ztimer64 clock to operate on
 * @param[in]   timer       timer entry to remove
 */
void ztimer64_remove(ztimer64_clock_t *clock, ztimer64_t *timer);

/**
 * @brief   Post a message at a specified time
 *
 * This function sets a timer that will send a message at time @p target.
 *
 * @note The memory pointed to by @p timer and @p msg will not be copied, i.e.
 *       `*timer` and `*msg` needs to remain valid until the timer has triggered.
 *
 * @param[in]   clock           ztimer64 clock to operate on
 * @param[in]   timer           ztimer64 timer struct to use
 * @param[in]   target          absolute target time
 * @param[in]   msg             pointer to msg that will be sent
 * @param[in]   target_pid      pid the message will be sent to
 */
void ztimer64_set_msg_at(ztimer64_clock_t *clock, ztimer64_t *timer,
                         uint64_t target,
                         msg_t *msg, kernel_pid_t target_pid);

/**
 * @brief   Post a message after a delay (relative version)
 *
 * This function sets a timer that will send a message @p offset ticks
 * from now.
 *
 * @note The memory pointed to by @p timer and @p msg will not be copied, i.e.
 *       `*timer` and `*msg` needs to remain valid until the timer has triggered.
 *
 * @param[in]   clock           ztimer64 clock to operate on
 * @param[in]   timer           ztimer64 timer struct to use
 * @param[in]   offset          relative target time
 * @param[in]   msg             pointer to msg that will be sent
 * @param[in]   target_pid      pid the message will be sent to
 */
static inline void ztimer64_set_msg(ztimer64_clock_t *clock, ztimer64_t *timer,
                                    uint64_t offset,
                                    msg_t *msg, kernel_pid_t target_pid)
{
    ztimer64_set_msg_at(clock, timer,
                        ztimer64_offset2absolute(clock, offset),
                        msg, target_pid);
}

/**
 * @brief receive a message (blocking, with absolute timeout)
 *
 * Similar to msg_receive(), but with a timeout parameter.
 * The function will return after waiting at most until @p target.
 *
 * @note: This might function might leave a message with type MSG_ZTIMER64 in the
 *        thread's message queue, which must be handled (ignored).
 *
 * @param[in]   clock           ztimer64 clock to operate on
 * @param[out]  msg             pointer to buffer which will be filled if a
 *                              message is received
 * @param[in]   target          absolute target, in @p clock time
 *
 * @return  >=0 if a message was received
 * @return  -ETIME on timeout
 */
int ztimer64_msg_receive_until(ztimer64_clock_t *clock, msg_t *msg,
                               uint64_t target);

/**
 * @brief receive a message (blocking, with relative timeout)
 *
 * Similar to msg_receive(), but with a timeout parameter.
 * The function will return after waiting at most @p timeout ticks.
 *
 * @note: This might function might leave a message with type MSG_ZTIMER64 in the
 *        thread's message queue, which must be handled (ignored).
 *
 * @param[in]   clock           ztimer64 clock to operate on
 * @param[out]  msg             pointer to buffer which will be filled if a
 *                              message is received
 * @param[in]   timeout         relative timeout, in @p clock time units
 *
 * @return  >=0 if a message was received
 * @return  -ETIME on timeout
 */
static inline int ztimer64_msg_receive_timeout(ztimer64_clock_t *clock,
                                               msg_t *msg,
                                               uint64_t timeout)
{
    return ztimer64_msg_receive_until(clock, msg,
                                      ztimer64_offset2absolute(clock, timeout));
}

#define MSG_ZTIMER64 0xc83f   /**< msg type used by ztimer64_msg_receive_timeout */

/**
 * @brief Suspend the calling thread until the time (@p last_wakeup + @p period)
 *
 * This function can be used to create periodic wakeups.
 *
 * When the function returns, @p last_wakeup is set to
 * (@p last_wakeup + @p period).
 *
 * @c last_wakeup should be set to ztimer64_now(@p clock) before first call of the
 * function.
 *
 * If the time (@p last_wakeup + @p period) has already passed, the function
 * sets @p last_wakeup to @p last_wakeup + @p period and returns immediately.
 *
 * @note if you period is smaller than 2^32 ticks, consider using the 32bit version.
 *
 * @param[in]   clock           ztimer64 clock to operate on
 * @param[in]   last_wakeup     base time stamp for the wakeup
 * @param[in]   period          time in ticks that will be added to @p last_wakeup
 */
void ztimer64_periodic_wakeup(ztimer64_clock_t *clock, uint64_t *last_wakeup,
                              uint64_t period);

/**
 * @brief   Put the calling thread to sleep until the specified time
 *
 * @param[in]   clock           ztimer64 clock to use
 * @param[in]   target          wakeup time, in @p clock time
 */
void ztimer64_sleep_until(ztimer64_clock_t *clock, uint64_t target);

/**
 * @brief   Put the calling thread to sleep for the specified number of ticks
 *
 * @param[in]   clock           ztimer64 clock to use
 * @param[in]   duration        duration of sleep, in @p ztimer time units
 */
static inline void ztimer64_sleep(ztimer64_clock_t *clock, uint64_t duration)
{
    ztimer64_sleep_until(clock, ztimer64_offset2absolute(clock, duration));
}

/**
 * @brief   Busy-wait until specified target time
 *
 * @note: This blocks lower priority threads. Use only for *very* short delays.
 *
 * @param[in]   clock           ztimer64 clock to use
 * @param[in]   target          time when spinning should end, in @p clock time
 */
static inline void ztimer64_spin_until(ztimer64_clock_t *clock, uint64_t target)
{
    while (ztimer64_now(clock) <= target) {}
}

/**
 * @brief Set a timer that wakes up a thread (absolute version)
 *
 * This function sets a timer that will wake up a thread when the timer has
 * expired.
 *
 * @param[in] clock         ztimer64 clock to operate on
 * @param[in] timer         timer struct to work with.
 * @param[in] target        wakeup time
 * @param[in] pid           pid of the thread that will be woken up
 */
void ztimer64_set_wakeup_at(ztimer64_clock_t *clock, ztimer64_t *timer,
                            uint64_t target,
                            kernel_pid_t pid);

/**
 * @brief Set a timer that wakes up a thread (relative version)
 *
 * This function sets a timer that will wake up a thread when the timer has
 * expired.
 *
 * @param[in] clock         ztimer64 clock to operate on
 * @param[in] timer         timer struct to work with.
 * @param[in] offset        clock ticks from now
 * @param[in] pid           pid of the thread that will be woken up
 */
static inline void ztimer64_set_wakeup(ztimer64_clock_t *clock,
                                       ztimer64_t *timer, uint64_t offset,
                                       kernel_pid_t pid)
{
    ztimer64_set_wakeup_at(clock, timer,
                           ztimer64_offset2absolute(clock, offset), pid);
}

/**
 * @brief    Set timeout thread flag at @p target time
 *
 * This function will set THREAD_FLAG_TIMEOUT on the current thread at time
 * @p target.
 *
 * @param[in]   clock           ztimer64 clock to operate on
 * @param[in]   timer           timer struct to use
 * @param[in]   target          target in @p clock time
 */
void ztimer64_set_timeout_flag_at(ztimer64_clock_t *clock, ztimer64_t *timer,
                                  uint64_t target);

/**
 * @brief    Set timeout thread flag after @p timeout
 *
 * This function will set THREAD_FLAG_TIMEOUT on the current thread after @p
 * timeout usec have passed.
 *
 * @param[in]   clock           ztimer64 clock to operate on
 * @param[in]   timer           timer struct to use
 * @param[in]   timeout         timeout in ztimer64_clock's ticks
 */
static inline void ztimer64_set_timeout_flag(ztimer64_clock_t *clock,
                                             ztimer64_t *timer,
                                             uint64_t timeout)
{
    ztimer64_set_timeout_flag_at(clock, timer,
                                 ztimer64_offset2absolute(clock, timeout));
}

/**
 * @brief   Try to lock the given mutex, but give up at @p target time
 *
 * @param[in]       clock       ztimer64 clock to operate on
 * @param[in,out]   mutex       Mutex object to lock
 * @param[in]       target      time after which to give up
 *
 * @retval  0               Success, caller has the mutex
 * @retval  -ECANCELED      Failed to obtain mutex within @p timeout
 */
int ztimer64_mutex_lock_until(ztimer64_clock_t *clock, mutex_t *mutex,
                              uint64_t target);

/**
 * @brief   Try to lock the given mutex, but give up after @p timeout
 *
 * @param[in]       clock       ztimer64 clock to operate on
 * @param[in,out]   mutex       Mutex object to lock
 * @param[in]       timeout     timeout after which to give up
 *
 * @retval  0               Success, caller has the mutex
 * @retval  -ECANCELED      Failed to obtain mutex within @p timeout
 */
static inline int ztimer64_mutex_lock_timeout(ztimer64_clock_t *clock,
                                              mutex_t *mutex,
                                              uint64_t timeout)
{
    return ztimer64_mutex_lock_until(clock, mutex,
                                     ztimer64_offset2absolute(clock, timeout));
}

/**
 * @brief   Try to lock the given rmutex, but give up at @p time
 *
 * @param[in]       clock       ztimer64 clock to operate on
 * @param[in,out]   rmutex      rmutex object to lock
 * @param[in]       target      time after which to give up
 *
 * @retval  0               Success, caller has the rmutex
 * @retval  -ECANCELED      Failed to obtain rmutex before @p time
 */
int ztimer64_rmutex_lock_until(ztimer64_clock_t *clock, rmutex_t *rmutex,
                               uint64_t target);

/**
 * @brief   Try to lock the given rmutex, but give up after @p timeout
 *
 * @param[in]       clock       ztimer64 clock to operate on
 * @param[in,out]   rmutex      rmutex object to lock
 * @param[in]       timeout     timeout after which to give up
 *
 * @retval  0               Success, caller has the rmutex
 * @retval  -ECANCELED      Failed to obtain rmutex within @p timeout
 */
static inline int ztimer64_rmutex_lock_timeout(ztimer64_clock_t *clock,
                                               rmutex_t *rmutex,
                                               uint64_t timeout)
{
    return ztimer64_rmutex_lock_until(clock, rmutex,
                                      ztimer64_offset2absolute(clock, timeout));
}

/**
 * @brief   Update ztimer clock head list offset
 *
 * @internal
 *
 * @param[in]   clock  ztimer clock to work on
 */
void ztimer64_update_head_offset(ztimer64_clock_t *clock);

/**
 * @brief   Initialize the board-specific default ztimer configuration
 */
void ztimer64_init(void);

/**
 * @brief           Initialize @p clock to be run from @p base_clock
 * @param[in,out]   clock       Clock to initialize
 * @param[in]       base_clock  Base clock to use
 */
void ztimer64_clock_init(ztimer64_clock_t *clock, ztimer_clock_t *base_clock);

/* default ztimer virtual devices */
/**
 * @brief   Default ztimer microsecond clock
 */
extern ztimer64_clock_t *const ZTIMER64_USEC;

/**
 * @brief   Default ztimer millisecond clock
 */
extern ztimer64_clock_t *const ZTIMER64_MSEC;

/**
 * @brief   Default ztimer second clock
 */
extern ztimer64_clock_t *const ZTIMER64_SEC;

/**
 * @brief   Measure ztimer64 overhead
 *
 * This function can be used to measure the overhead incurred by ztimer64.
 * It will configure a callback to trigger after @p base ticks, then return the
 * number of ticks that have passed, minus @p base.
 *
 * @param[in]   clock   ztimer64 clock to operate on
 * @param[in]   base    base interval to use
 * @return  (time from ztimer64_set() until callback) - base
 */
int64_t ztimer64_overhead(ztimer64_clock_t *clock, uint64_t base);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ZTIMER64_H */
