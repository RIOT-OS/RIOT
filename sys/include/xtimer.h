/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup  sys_xtimer Timers
 * @ingroup   sys
 * @brief     Provides a high level timer module to register
 *            timers, get current system time, and let a thread sleep for
 *            a certain amount of time.
 *
 * The implementation takes one low-level timer and multiplexes it.
 *
 * Insertion and removal of timers has O(n) complexity with (n) being the
 * number of active timers.  The reason for this is that multiplexing is
 * realized by next-first singly linked lists.
 *
 * @{
 * @file
 * @brief   xtimer interface definitions
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */
#ifndef XTIMER_H
#define XTIMER_H

#include <stdint.h>
#include "timex.h"
#include "msg.h"
#include "mutex.h"

#include "board.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief xtimer timestamp (64 bit)
 *
 * @note This is a struct in order to make the xtimer API type strict
 */
typedef struct {
    uint64_t ticks64;       /**< Tick count */
} xtimer_ticks64_t;

/**
 * @brief xtimer timestamp (32 bit)
 *
 * @note This is a struct in order to make the xtimer API type strict
 */
typedef struct {
    uint32_t ticks32;       /**< Tick count */
} xtimer_ticks32_t;

/**
 * @brief xtimer callback type
 */
typedef void (*xtimer_callback_t)(void*);

/**
 * @brief xtimer timer structure
 */
typedef struct xtimer {
    struct xtimer *next;         /**< reference to next timer in timer lists */
    uint32_t target;             /**< lower 32bit absolute target time */
    uint32_t long_target;        /**< upper 32bit absolute target time */
    xtimer_callback_t callback;  /**< callback function to call when timer
                                     expires */
    void *arg;                   /**< argument to pass to callback function */
} xtimer_t;

/**
 * @brief get the current system time as 32bit time stamp value
 *
 * @note    Overflows 2**32 ticks, thus returns xtimer_now64() % 32,
 *          but is cheaper.
 *
 * @return  current time as 32bit time stamp
 */
static inline xtimer_ticks32_t xtimer_now(void);

/**
 * @brief get the current system time as 64bit time stamp
 *
 * @return  current time as 64bit time stamp
 */
static inline xtimer_ticks64_t xtimer_now64(void);

/**
 * @brief get the current system time into a timex_t
 *
 * @param[out] out  pointer to timex_t the time will be written to
 */
void xtimer_now_timex(timex_t *out);

/**
 * @brief get the current system time in microseconds since start
 *
 * This is a convenience function for @c xtimer_usec_from_ticks(xtimer_now())
 */
static inline uint32_t xtimer_now_usec(void);

/**
 * @brief get the current system time in microseconds since start
 *
 * This is a convenience function for @c xtimer_usec_from_ticks64(xtimer_now64())
 */
static inline uint64_t xtimer_now_usec64(void);

/**
 * @brief xtimer initialization function
 *
 * This sets up xtimer. Has to be called once at system boot.
 * If @ref auto_init is enabled, it will call this for you.
 */
void xtimer_init(void);

/**
 * @brief Pause the execution of a thread for some seconds
 *
 * When called from an ISR, this function will spin and thus block the MCU in
 * interrupt context for the specified amount in *seconds*, so don't *ever* use
 * it there.
 *
 * @param[in] seconds   the amount of seconds the thread should sleep
 */
static inline void xtimer_sleep(uint32_t seconds);

/**
 * @brief Pause the execution of a thread for some microseconds
 *
 * When called from an ISR, this function will spin and thus block the MCU for
 * the specified amount in microseconds, so only use it there for *very* short
 * periods, e.g., less than XTIMER_BACKOFF.
 *
 * @param[in] microseconds  the amount of microseconds the thread should sleep
 */
static inline void xtimer_usleep(uint32_t microseconds);

/**
 * @brief Stop execution of a thread for some time
 *
 * Don't expect nanosecond accuracy. As of now, this function just calls
 * xtimer_usleep(nanoseconds/1000).
 *
 * When called from an ISR, this function will spin-block, so only use it there
 * for *very* short periods.
 *
 * @param[in] nanoseconds   the amount of nanoseconds the thread should sleep
 */
static inline void xtimer_nanosleep(uint32_t nanoseconds);

/**
 * @brief Stop execution of a thread for some time, 32bit version
 *
 * When called from an ISR, this function will spin and thus block the MCU for
 * the specified amount, so only use it there for *very* short periods,
 * e.g. less than XTIMER_BACKOFF.
 *
 * @param[in] ticks  number of ticks the thread should sleep
 */
static inline void xtimer_tsleep32(xtimer_ticks32_t ticks);

/**
 * @brief Stop execution of a thread for some time, 64bit version
 *
 * When called from an ISR, this function will spin and thus block the MCU for
 * the specified amount, so only use it there for *very* short periods,
 * e.g. less than XTIMER_BACKOFF.
 *
 * @param[in] ticks  number of ticks the thread should sleep
 */
static inline void xtimer_tsleep64(xtimer_ticks64_t ticks);

/**
 * @brief Stop execution of a thread for some time, blocking
 *
 * This function will spin-block, so only use it *very* short periods.
 *
 * @param[in] ticks  the number of xtimer ticks the thread should spin for
 */
static inline void xtimer_spin(xtimer_ticks32_t ticks);

/**
 * @brief will cause the calling thread to be suspended until the absolute
 * time (@p last_wakeup + @p period).
 *
 * When the function returns, @p last_wakeup is set to
 * (@p last_wakeup + @p period).
 *
 * This function can be used to create periodic wakeups.
 * @c last_wakeup should be set to xtimer_now() before first call of the
 * function.
 *
 * If the result of (@p last_wakeup + @p period) would be in the past, the function
 * sets @p last_wakeup to @p last_wakeup + @p period and returns immediately.
 *
 * @param[in] last_wakeup   base time stamp for the wakeup
 * @param[in] period        time in microseconds that will be added to last_wakeup
 */
static inline void xtimer_periodic_wakeup(xtimer_ticks32_t *last_wakeup, uint32_t period);

/**
 * @brief Set a timer that sends a message
 *
 * This function sets a timer that will send a message @p offset ticks
 * from now.
 *
 * The mesage struct specified by msg parameter will not be copied, e.g., it
 * needs to point to valid memory until the message has been delivered.
 *
 * @param[in] timer         timer struct to work with.
 *                          Its xtimer_t::target and xtimer_t::long_target
 *                          fields need to be initialized with 0 on first use.
 * @param[in] offset        microseconds from now
 * @param[in] msg           ptr to msg that will be sent
 * @param[in] target_pid    pid the message will be sent to
 */
static inline void xtimer_set_msg(xtimer_t *timer, uint32_t offset, msg_t *msg, kernel_pid_t target_pid);

/**
 * @brief Set a timer that sends a message, 64bit version
 *
 * This function sets a timer that will send a message @p offset microseconds
 * from now.
 *
 * The mesage struct specified by msg parameter will not be copied, e.g., it
 * needs to point to valid memory until the message has been delivered.
 *
 * @param[in] timer         timer struct to work with.
 *                          Its xtimer_t::target and xtimer_t::long_target
 *                          fields need to be initialized with 0 on first use.
 * @param[in] offset        microseconds from now
 * @param[in] msg           ptr to msg that will be sent
 * @param[in] target_pid    pid the message will be sent to
 */
static inline void xtimer_set_msg64(xtimer_t *timer, uint64_t offset, msg_t *msg, kernel_pid_t target_pid);

/**
 * @brief Set a timer that wakes up a thread
 *
 * This function sets a timer that will wake up a thread when the timer has
 * expired.
 *
 * @param[in] timer         timer struct to work with.
 *                          Its xtimer_t::target and xtimer_t::long_target
 *                          fields need to be initialized with 0 on first use
 * @param[in] offset        microseconds from now
 * @param[in] pid           pid of the thread that will be woken up
 */
static inline void xtimer_set_wakeup(xtimer_t *timer, uint32_t offset, kernel_pid_t pid);

/**
 * @brief Set a timer that wakes up a thread, 64bit version
 *
 * This function sets a timer that will wake up a thread when the timer has
 * expired.
 *
 * @param[in] timer         timer struct to work with.
 *                          Its xtimer_t::target and xtimer_t::long_target
 *                          fields need to be initialized with 0 on first use
 * @param[in] offset        microseconds from now
 * @param[in] pid           pid of the thread that will be woken up
 */
static inline void xtimer_set_wakeup64(xtimer_t *timer, uint64_t offset, kernel_pid_t pid);

/**
 * @brief Set a timer to execute a callback at some time in the future
 *
 * Expects timer->callback to be set.
 *
 * The callback specified in the timer struct will be executed @p offset
 * ticks in the future.
 *
 * @warning BEWARE! Callbacks from xtimer_set() are being executed in interrupt
 * context (unless offset < XTIMER_BACKOFF). DON'T USE THIS FUNCTION unless you
 * know *exactly* what that means.
 *
 * @param[in] timer     the timer structure to use.
 *                      Its xtimer_t::target and xtimer_t::long_target
 *                      fields need to be initialized with 0 on first use
 * @param[in] offset    time in microseconds from now specifying that timer's
 *                      callback's execution time
 */
static inline void xtimer_set(xtimer_t *timer, uint32_t offset);

/**
 * @brief Set a timer to execute a callback at some time in the future, 64bit
 * version
 *
 * Expects timer->callback to be set.
 *
 * The callback specified in the timer struct will be executed @p offset_usec
 * microseconds in the future.
 *
 * @warning BEWARE! Callbacks from xtimer_set() are being executed in interrupt
 * context (unless offset < XTIMER_BACKOFF). DON'T USE THIS FUNCTION unless you
 * know *exactly* what that means.
 *
 * @param[in] timer       the timer structure to use.
 *                        Its xtimer_t::target and xtimer_t::long_target
 *                        fields need to be initialized with 0 on first use
 * @param[in] offset_us   time in microseconds from now specifying that timer's
 *                        callback's execution time
 */
static inline void xtimer_set64(xtimer_t *timer, uint64_t offset_us);

/**
 * @brief remove a timer
 *
 * @note this function runs in O(n) with n being the number of active timers
 *
 * @param[in] timer ptr to timer structure that will be removed
 */
void xtimer_remove(xtimer_t *timer);

/**
 * @brief receive a message blocking but with timeout
 *
 * @param[out] msg      pointer to a msg_t which will be filled in case of
 *                      no timeout
 * @param[in]  timeout  timeout in microseconds relative
 *
 * @return     < 0 on error, other value otherwise
 */
static inline int xtimer_msg_receive_timeout(msg_t *msg, uint32_t timeout);

/**
 * @brief receive a message blocking but with timeout, 64bit version
 *
 * @param[out] msg      pointer to a msg_t which will be filled in case of no
 *                      timeout
 * @param[in]  timeout  timeout in microseconds relative
 *
 * @return     < 0 on error, other value otherwise
 */
static inline int xtimer_msg_receive_timeout64(msg_t *msg, uint64_t timeout);

/**
 * @brief Convert microseconds to xtimer ticks
 *
 * @param[in] usec  microseconds
 *
 * @return xtimer time stamp
 */
static inline xtimer_ticks32_t xtimer_ticks_from_usec(uint32_t usec);

/**
 * @brief Convert microseconds to xtimer ticks, 64 bit version
 *
 * @param[in] usec  microseconds
 *
 * @return xtimer time stamp
 */
static inline xtimer_ticks64_t xtimer_ticks_from_usec64(uint64_t usec);

/**
 * @brief Convert xtimer ticks to microseconds
 *
 * @param[in] ticks  xtimer time stamp
 *
 * @return microseconds
 */
static inline uint32_t xtimer_usec_from_ticks(xtimer_ticks32_t ticks);

/**
 * @brief Convert xtimer ticks to microseconds, 64 bit version
 *
 * @param[in] ticks  xtimer time stamp
 *
 * @return microseconds
 */
static inline uint64_t xtimer_usec_from_ticks64(xtimer_ticks64_t ticks);

/**
 * @brief Create an xtimer time stamp
 *
 * @param[in] ticks  number of xtimer ticks
 *
 * @return xtimer time stamp
 */
static inline xtimer_ticks32_t xtimer_ticks(uint32_t ticks);

/**
 * @brief Create an xtimer time stamp, 64 bit version
 *
 * @param[in] ticks  number of xtimer ticks
 *
 * @return xtimer time stamp
 */
static inline xtimer_ticks64_t xtimer_ticks64(uint64_t ticks);

/**
 * @brief Compute difference between two xtimer time stamps
 *
 * @param[in] a  left operand
 * @param[in] b  right operand
 *
 * @return @p a - @p b
 */
static inline xtimer_ticks32_t xtimer_diff(xtimer_ticks32_t a, xtimer_ticks32_t b);

/**
 * @brief Compute difference between two xtimer time stamps, 64 bit version
 *
 * @param[in] a  left operand
 * @param[in] b  right operand
 *
 * @return @p a - @p b
 */
static inline xtimer_ticks64_t xtimer_diff64(xtimer_ticks64_t a, xtimer_ticks64_t b);

/**
 * @brief Compute 32 bit difference between two 64 bit xtimer time stamps
 *
 * @param[in] a  left operand
 * @param[in] b  right operand
 *
 * @return @p a - @p b cast truncated to 32 bit
 */
static inline xtimer_ticks32_t xtimer_diff32_64(xtimer_ticks64_t a, xtimer_ticks64_t b);

/**
 * @brief Compare two xtimer time stamps
 *
 * @param[in] a  left operand
 * @param[in] b  right operand
 *
 * @return @p a < @p b
 */
static inline bool xtimer_less(xtimer_ticks32_t a, xtimer_ticks32_t b);

/**
 * @brief Compare two xtimer time stamps, 64 bit version
 *
 * @param[in] a  left operand
 * @param[in] b  right operand
 *
 * @return @p a < @p b
 */
static inline bool xtimer_less64(xtimer_ticks64_t a, xtimer_ticks64_t b);

/**
 * @brief lock a mutex but with timeout
 *
 * @note this requires core_thread_flags to be enabled
 *
 * @param[in]    mutex  mutex to lock
 * @param[in]    us     timeout in microseconds relative
 *
 * @return       0, when returned after mutex was locked
 * @return       -1, when the timeout occcured
 */
int xtimer_mutex_lock_timeout(mutex_t *mutex, uint64_t us);

/**
 * @brief    Set timeout thread flag after @p timeout
 *
 * This function will set THREAD_FLAG_TIMEOUT on the current thread after @p
 * timeout usec have passed.
 *
 * @param[in]   t       timer struct to use
 * @param[in]   timeout timeout in usec
 */
void xtimer_set_timeout_flag(xtimer_t *t, uint32_t timeout);

/**
 * @brief xtimer backoff value
 *
 * All timers that are less than XTIMER_BACKOFF microseconds in the future will
 * just spin.
 *
 * This is supposed to be defined per-device in e.g., periph_conf.h.
 */
#ifndef XTIMER_BACKOFF
#define XTIMER_BACKOFF 30
#endif

/**
 * @brief xtimer overhead value, in hardware ticks
 *
 * This value specifies the time a timer will be late if uncorrected, e.g.,
 * the system-specific xtimer execution time from timer ISR to executing
 * a timer's callback's first instruction.
 *
 * E.g., with XTIMER_OVERHEAD == 0
 * start=xtimer_now();
 * xtimer_set(&timer, X);
 * (in callback:)
 * overhead=xtimer_now()-start-X;
 *
 * xtimer automatically substracts XTIMER_OVERHEAD from a timer's target time,
 * but when the timer triggers, xtimer will spin-lock until a timer's target
 * time is reached, so timers will never trigger early.
 *
 * This is supposed to be defined per-device in e.g., periph_conf.h.
 */
#ifndef XTIMER_OVERHEAD
#define XTIMER_OVERHEAD 20
#endif

#ifndef XTIMER_ISR_BACKOFF
/**
 * @brief   xtimer IRQ backoff time, in hardware ticks
 *
 * When scheduling the next IRQ, if it is less than the backoff time
 * in the future, just spin.
 *
 * This is supposed to be defined per-device in e.g., periph_conf.h.
 */
#define XTIMER_ISR_BACKOFF 20
#endif

#ifndef XTIMER_PERIODIC_SPIN
/**
 * @brief   xtimer_periodic_wakeup spin cutoff
 *
 * If the difference between target time and now is less than this value, then
 * xtimer_periodic_wakeup will use xtimer_spin instead of setting a timer.
 */
#define XTIMER_PERIODIC_SPIN (XTIMER_BACKOFF * 2)
#endif

#ifndef XTIMER_PERIODIC_RELATIVE
/**
 * @brief   xtimer_periodic_wakeup relative target cutoff
 *
 * If the difference between target time and now is less than this value, then
 * xtimer_periodic_wakeup will set a relative target time in the future instead
 * of the true target.
 *
 * This is done to prevent target time underflows.
 */
#define XTIMER_PERIODIC_RELATIVE (512)
#endif

/*
 * Default xtimer configuration
 */
#ifndef XTIMER_DEV
/**
 * @brief Underlying hardware timer device to assign to xtimer
 */
#define XTIMER_DEV TIMER_DEV(0)
/**
 * @brief Underlying hardware timer channel to assign to xtimer
 */
#define XTIMER_CHAN (0)

#if (TIMER_0_MAX_VALUE) == 0xfffffful
#define XTIMER_WIDTH (24)
#elif (TIMER_0_MAX_VALUE) == 0xffff
#define XTIMER_WIDTH (16)
#endif

#endif

#ifndef XTIMER_WIDTH
/**
 * @brief xtimer timer width
 *
 * This value specifies the width (in bits) of the hardware timer used by xtimer.
 * Default is 32.
 */
#define XTIMER_WIDTH (32)
#endif

#if (XTIMER_WIDTH != 32) || DOXYGEN
/**
 * @brief xtimer timer mask
 *
 * This value specifies the mask relative to 0xffffffff that the used timer
 * counts to, e.g., 0xffffffff & ~TIMER_MAXVALUE.
 *
 * For a 16bit timer, the mask would be 0xFFFF0000, for a 24bit timer, the mask
 * would be 0xFF000000.
 */
#define XTIMER_MASK ((0xffffffff >> XTIMER_WIDTH) << XTIMER_WIDTH)
#else
#define XTIMER_MASK (0)
#endif

/**
 * @brief  Base frequency of xtimer is 1 MHz
 */
#define XTIMER_HZ_BASE (1000000ul)

#ifndef XTIMER_HZ
/**
 * @brief  Frequency of the underlying hardware timer
 */
#define XTIMER_HZ XTIMER_HZ_BASE
#endif

#ifndef XTIMER_SHIFT
#if (XTIMER_HZ == 32768ul)
/* No shift necessary, the conversion is not a power of two and is handled by
 * functions in tick_conversion.h */
#define XTIMER_SHIFT (0)
#elif (XTIMER_HZ == XTIMER_HZ_BASE)
/**
 * @brief   xtimer prescaler value
 *
 * If the underlying hardware timer is running at a power of two multiple of
 * 15625, XTIMER_SHIFT can be used to adjust the difference.
 *
 * For a 1 MHz hardware timer, set XTIMER_SHIFT to 0.
 * For a 2 MHz or 500 kHz, set XTIMER_SHIFT to 1.
 * For a 4 MHz or 250 kHz, set XTIMER_SHIFT to 2.
 * For a 8 MHz or 125 kHz, set XTIMER_SHIFT to 3.
 * For a 16 MHz or 62.5 kHz, set XTIMER_SHIFT to 4.
 * and for 32 MHz, set XTIMER_SHIFT to 5.
 *
 * The direction of the shift is handled by the macros in tick_conversion.h
 */
#define XTIMER_SHIFT (0)
#elif (XTIMER_HZ >> 1 == XTIMER_HZ_BASE) || (XTIMER_HZ << 1 == XTIMER_HZ_BASE)
#define XTIMER_SHIFT (1)
#elif (XTIMER_HZ >> 2 == XTIMER_HZ_BASE) || (XTIMER_HZ << 2 == XTIMER_HZ_BASE)
#define XTIMER_SHIFT (2)
#elif (XTIMER_HZ >> 3 == XTIMER_HZ_BASE) || (XTIMER_HZ << 3 == XTIMER_HZ_BASE)
#define XTIMER_SHIFT (3)
#elif (XTIMER_HZ >> 4 == XTIMER_HZ_BASE) || (XTIMER_HZ << 4 == XTIMER_HZ_BASE)
#define XTIMER_SHIFT (4)
#elif (XTIMER_HZ >> 5 == XTIMER_HZ_BASE) || (XTIMER_HZ << 5 == XTIMER_HZ_BASE)
#define XTIMER_SHIFT (5)
#elif (XTIMER_HZ >> 6 == XTIMER_HZ_BASE) || (XTIMER_HZ << 6 == XTIMER_HZ_BASE)
#define XTIMER_SHIFT (6)
#else
#error "XTIMER_SHIFT cannot be derived for given XTIMER_HZ, verify settings!"
#endif
#else
#error "XTIMER_SHIFT is set relative to XTIMER_HZ, no manual define required!"
#endif

#include "xtimer/tick_conversion.h"

#include "xtimer/implementation.h"

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* XTIMER_H */
