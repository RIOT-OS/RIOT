/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * The implementation takes one low-level timer that is supposed to run at 1MHz
 * speed and multiplexes it.
 *
 * Insertion and removal of timers has O(n) complexity with (n) being the
 * number of active timers.  The reason for this is that multiplexing is
 * realized by next-first singly linked lists.
 *
 * @{
 * @file
 * @brief   xtimer interface definitions
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef XTIMER_H
#define XTIMER_H

#include <stdint.h>
#include "msg.h"
#include "periph/timer.h"
#include "timex.h"

#include "board.h"
#include "periph_conf.h"

/**
 * @brief internal define to allow using variables instead of defines
 */
#ifdef XTIMER_TRACE
#include "xtimer_trace.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief xtimer callback type
 */
typedef void (*timer_callback_t)(void*);

/**
 * @brief xtimer timer structure
 */
typedef struct xtimer {
    struct xtimer *next;        /**< reference to next timer in timer lists */
    uint32_t target;            /**< lower 32bit absolute target time */
    uint32_t long_target;       /**< upper 32bit absolute target time */
    timer_callback_t callback;  /**< callback function to call when timer
                                     expires */
    void *arg;                  /**< argument to pass to callback function */
} xtimer_t;

/**
 * @brief get the current system time as 32bit microsecond value
 *
 * @note    Overflows every ~71minutes, thus returns xtimer_now64() % 32,
 *          but is more efficient.
 *
 * @return  current time as 32bit microsecond value
 */
static inline uint32_t xtimer_now(void);

/**
 * @brief get the current system time as 64bit microsecond value
 *
 * @return  current time as 64bit microsecond value
 */
uint64_t xtimer_now64(void);

/**
 * @brief get the current system time into a timex_t
 *
 * @param[out] out  pointer to timex_t the time will be written to
 */
void xtimer_now_timex(timex_t *out);

/**
 * @brief xtimer initialization function
 *
 * This sets up xtimer. Has to be called once at system boot.
 * If @ref auto_init is enabled, it will call this for you.
 */
void xtimer_init(void);

/**
 * @brief Stop execution of a thread for some time
 *
 * When called from an ISR, this function will spin and thus block the MCU in
 * interrupt context for the specified amount in *seconds*, so don't *ever* use
 * it there.
 *
 * @param[in] seconds   the amount of seconds the thread should sleep
 */
static void xtimer_sleep(uint32_t seconds);

/**
 * @brief Stop execution of a thread for some time
 *
 * When called from an ISR, this function will spin and thus block the MCU for
 * the specified amount in microseconds, so only use it there for *very* short
 * periods, e.g., less than XTIMER_BACKOFF.
 *
 * @param[in] microseconds  the amount of microseconds the thread should sleep
 */
static void xtimer_usleep(uint32_t microseconds);

/**
 * @brief Stop execution of a thread for some time, 64bit version
 *
 * When called from an ISR, this function will spin and thus block the MCU for
 * the specified amount in microseconds, so only use it there for *very* short
 * periods, e.g., less then XTIMER_BACKOFF.
 *
 * @param[in] microseconds  the amount of microseconds the thread should sleep
 */
static inline void xtimer_usleep64(uint64_t microseconds);

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
static void xtimer_nanosleep(uint32_t nanoseconds);

/**
 * @brief Stop execution of a thread for some time, blocking
 *
 * This function will spin-block, so only use it *very* short periods.
 *
 * @param[in] microseconds  the amount of microseconds the thread should spin
 */
static inline void xtimer_spin(uint32_t microseconds);

 /**
 * @brief will cause the calling thread to be suspended until the absolute
 * time (@p last_wakeup + @p interval).
 *
 * When the function returns, @p last_wakeup is set to
 * (@p last_wakeup + @p interval).
 *
 * This function can be used to create periodic wakeups.
 * @c last_wakeup should be set to xtimer_now() before first call of the
 * function.
 *
 * If the result of (@p last_wakeup + usecs) would be in the past, the function
 * sets @p last_wakeup to @p last_wakeup + @p interval and returns immediately.
 *
 * @param[in] last_wakeup   base time for the wakeup
 * @param[in] usecs         time in microseconds that will be added to
 *                          last_wakeup
 */
void xtimer_usleep_until(uint32_t *last_wakeup, uint32_t usecs);

/**
 * @brief Set a timer that sends a message
 *
 * This function sets a timer that will send a message @p offset microseconds
 * from now.
 *
 * The mesage struct specified by msg parameter will not be copied, e.g., it
 * needs to point to valid memory until the message has been delivered.
 *
 * @param[in] timer         timer struct to work with
 * @param[in] offset        microseconds from now
 * @param[in] msg           ptr to msg that will be sent
 * @param[in] target_pid    pid the message will be sent to
 */
void xtimer_set_msg(xtimer_t *timer, uint32_t offset, msg_t *msg, kernel_pid_t target_pid);

/**
 * @brief Set a timer that sends a message, 64bit version
 *
 * This function sets a timer that will send a message @p offset microseconds
 * from now.
 *
 * The mesage struct specified by msg parameter will not be copied, e.g., it
 * needs to point to valid memory until the message has been delivered.
 *
 * @param[in] timer         timer struct to work with
 * @param[in] offset        microseconds from now
 * @param[in] msg           ptr to msg that will be sent
 * @param[in] target_pid    pid the message will be sent to
 */
void xtimer_set_msg64(xtimer_t *timer, uint64_t offset, msg_t *msg, kernel_pid_t target_pid);

/**
 * @brief Set a timer that wakes up a thread
 *
 * This function sets a timer that will wake up a thread when the timer has
 * expired.
 *
 * @param[in] timer         timer struct to work with
 * @param[in] offset        microseconds from now
 * @param[in] pid           pid of the thread that will be woken up
 */
void xtimer_set_wakeup(xtimer_t *timer, uint32_t offset, kernel_pid_t pid);

/**
 * @brief Set a timer that wakes up a thread, 64bit version
 *
 * This function sets a timer that will wake up a thread when the timer has
 * expired.
 *
 * @param[in] timer         timer struct to work with
 * @param[in] offset        microseconds from now
 * @param[in] pid           pid of the thread that will be woken up
 */
void xtimer_set_wakeup64(xtimer_t *timer, uint64_t offset, kernel_pid_t pid);

/**
 * @brief Set a timer to execute a callback at some time in the future
 *
 * Expects timer->callback to be set.
 *
 * The callback specified in the timer struct will be executed @p offset
 * microseconds in the future.
 *
 * @warning BEWARE! Callbacks from xtimer_set() are being executed in interrupt
 * context (unless offset < XTIMER_BACKOFF). DON'T USE THIS FUNCTION unless you
 * know *exactly* what that means.
 *
 * @param[in] timer     the timer structure to use
 * @param[in] offset    time in microseconds from now specifying that timer's
 *                      callback's execution time
 */
void xtimer_set(xtimer_t *timer, uint32_t offset);

/**
 * @brief remove a timer
 *
 * @note this function runs in O(n) with n being the number of active timers
 *
 * @param[in] timer ptr to timer structure that will be removed
 *
 * @return  1 on success
 * @return  0 when timer was not active
 */
int xtimer_remove(xtimer_t *timer);

/**
 * @brief receive a message blocking but with timeout
 *
 * @param[out]  msg     pointer to a msg_t which will be filled in case of
 *                      no timeout
 * @param[in]   us      timeout in microseconds relative
 *
 * @return       < 0 on error, other value otherwise
 */
int xtimer_msg_receive_timeout(msg_t *msg, uint32_t us);

/**
 * @brief receive a message blocking but with timeout, 64bit version
 *
 * @param[out]   msg    pointer to a msg_t which will be filled in case of no
 *                      timeout
 * @param[in]    us     timeout in microseconds relative
 *
 * @return       < 0 on error, other value otherwise
 */
int xtimer_msg_receive_timeout64(msg_t *msg, uint64_t us);

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
 * @brief xtimer overhead value
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
 * @brief   xtimer isr backoff time
 *
 * When scheduling the next isr, if it is less than the backoff time
 * in the future, just spin.
 *
 * This is supposed to be defined per-device in e.g., periph_conf.h.
 */
#define XTIMER_ISR_BACKOFF 20
#endif

/*
 * @brief   xtimer prescaler value
 *
 * xtimer assumes it is running with an underlying 1MHz timer.
 * If the timer is slower by a power of two, XTIMER_SHIFT can be used to
 * adjust the difference.
 *
 * This will also initialize the underlying periph timer with
 * us_per_tick == (1<<XTIMER_SHIFT).
 *
 * For example, if the timer is running with 250khz, set XTIMER_SHIFT to 2.
 */
#ifndef XTIMER_SHIFT
#define XTIMER_SHIFT (0)
#endif

/**
 * @brief set xtimer default timer configuration
 * @{
 */
#ifndef XTIMER
#define XTIMER (0)
#define XTIMER_CHAN (0)

#if TIMER_0_MAX_VALUE == 0xffffff
#define XTIMER_MASK 0xff000000
#elif TIMER_0_MAX_VALUE == 0xffff
#define XTIMER_MASK 0xffff0000
#endif

#endif
/**
 * @}
 */

#ifndef XTIMER_MASK
/**
 * @brief xtimer timer mask
 *
 * This value specifies the mask relative to 0xffffffff that the used timer
 * counts to, e.g., 0xffffffff & ~TIMER_MAXVALUE.
 *
 * For a 16bit timer, the mask would be 0xFFFF0000, for a 24bit timer, the mask
 * would be 0xFF000000. Don't set this for 32bit timers.
 *
 * This is supposed to be defined per-device in e.g., periph_conf.h.
 */
#define XTIMER_MASK (0)
#endif
#define XTIMER_MASK_SHIFTED (XTIMER_MASK << XTIMER_SHIFT)

#ifndef XTIMER_USLEEP_UNTIL_OVERHEAD
/**
 * @brief xtimer_usleep_until overhead value
 *
 * This value specifies the time a xtimer_usleep_until will be late
 * if uncorrected.
 *
 * This is supposed to be defined per-device in e.g., periph_conf.h.
 */
#define XTIMER_USLEEP_UNTIL_OVERHEAD 10
#endif

#if XTIMER_MASK
extern volatile uint32_t _high_cnt;
#endif

/**
 * @brief IPC message type for xtimer msg callback
 */
#define MSG_XTIMER 12345

/**
 * @brief returns the (masked) low-level timer counter value.
 */
static inline uint32_t _xtimer_now(void)
{
#if XTIMER_SHIFT
    return ((uint32_t)timer_read(XTIMER)) << XTIMER_SHIFT;
#else
    return timer_read(XTIMER);
#endif
}

/**
 * @brief drop bits of a value that don't fit into the low-level timer.
 */
static inline uint32_t _mask(uint32_t val)
{
    return val & ~XTIMER_MASK_SHIFTED;
}

/**
 * @{
 * @brief xtimer internal stuff
 * @internal
 */
int _xtimer_set_absolute(xtimer_t *timer, uint32_t target);
void _xtimer_set64(xtimer_t *timer, uint32_t offset, uint32_t long_offset);
void _xtimer_sleep(uint32_t offset, uint32_t long_offset);
static inline void xtimer_spin_until(uint32_t value);
/** @} */

#if XTIMER_MASK
#ifndef XTIMER_SHIFT_ON_COMPARE
/**
 * @brief ignore some bits when comparing timer values
 *
 * (only relevant when XTIMER_MASK != 0, e.g., timers < 32bit.)
 *
 * When combining _xtimer_now() and _high_cnt, we have to get the same value in
 * order to work around a race between overflowing _xtimer_now() and OR'ing the
 * resulting values.
 * But some platforms are too slow to get the same timer
 * value twice, so we use this define to ignore some of the bits.
 *
 * Use tests/xtimer_shift_on_compare to find the correct value for your MCU.
 */
#define XTIMER_SHIFT_ON_COMPARE     (0)
#endif
#endif

#ifndef XTIMER_MIN_SPIN
/**
 * @brief Minimal value xtimer_spin() can spin
 */
#define XTIMER_MIN_SPIN (1<<XTIMER_SHIFT)
#endif

static inline uint32_t xtimer_now(void)
{
#if XTIMER_MASK
    uint32_t a, b;
    do {
        a = _xtimer_now() | _high_cnt;
        b = _xtimer_now() | _high_cnt;
    } while ((a >> XTIMER_SHIFT_ON_COMPARE) != (b >> XTIMER_SHIFT_ON_COMPARE));
    return b;
#else
    return _xtimer_now();
#endif
}

static inline void xtimer_spin_until(uint32_t value) {
#if XTIMER_MASK
    value = _mask(value);
#endif
    while (_xtimer_now() > value);
    while (_xtimer_now() < value);
}

static inline void xtimer_spin(uint32_t offset) {
    uint32_t start = _xtimer_now();
    while ((_xtimer_now() - start) < offset);
}

static inline void xtimer_usleep(uint32_t microseconds)
{
    _xtimer_sleep(microseconds, 0);
}

static inline void xtimer_usleep64(uint64_t microseconds)
{
    _xtimer_sleep((uint32_t) microseconds, (uint32_t) (microseconds >> 32));
}

static inline void xtimer_sleep(uint32_t seconds)
{
    xtimer_usleep64((uint64_t)seconds * SEC_IN_USEC);
}

static inline void xtimer_nanosleep(uint32_t nanoseconds)
{
    _xtimer_sleep(nanoseconds/1000, 0);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* XTIMER_H */
