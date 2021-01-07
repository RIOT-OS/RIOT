/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    sys_ztimer ztimer high level timer abstraction layer
 * @ingroup     sys
 * @brief       High level timer abstraction layer
 *
 * # Introduction
 *
 * ztimer provides a high level abstraction of hardware timers for application
 * timing needs.
 *
 * The basic functions of the ztimer module are ztimer_now(), ztimer_sleep(),
 * ztimer_set() and ztimer_remove().
 *
 * They all take a pointer to a clock device (or virtual timer device) as first
 * parameter.
 *
 * RIOT provides ZTIMER_USEC, ZTIMER_MSEC, ZTIMER_SEC by default, which can be
 * used in an application by depending on the modules ztimer_usec, ztimer_msec
 * or ztimer_sec. They will then automatically get configured.
 *
 * Every ztimer clock allows multiple timeouts to be scheduled. They all
 * provide unsigned 32bit range. In this documentation, a timeout or its
 * corresponding struct will be called `timer`, and when the time out has
 * passed, it has `triggered`.
 *
 * As ztimer can use arbitrarily configurable backends, a ztimer clock instance
 * can run at configurable frequencies. Throughout this documentation, one
 * clock step is called `tick`.  For the pre-defined clocks ZTIMER_USEC,
 * ZTIMER_MSEC and ZTIMER_SEC, one clock tick corresponds to one microsecond,
 * one millisecond or one second, respectively.
 *
 * ztimer_now() returns the current clock tick count as uint32_t.
 *
 * ztimer_sleep() pauses the current thread for the passed amount of clock
 * ticks. E.g., ```ztimer_sleep(ZTIMER_SEC, 5);``` will suspend the currently
 * running thread for five seconds.
 *
 * ztimer_set() takes a ztimer_t object (containing a function pointer and
 * void * argument) and an interval as arguments. After at least the interval
 * (in number of ticks for the corresponding clock) has passed, the callback
 * will be called in interrupt context.
 * A timer can be cancelled using ztimer_remove().
 *
 * Example:
 *
 * ```
 * #include "ztimer.h"
 *
 * static void callback(void *arg)
 * {
 *    puts(arg);
 * }
 *
 * int main()
 * {
 *     ztimer_t timeout = { .callback=callback, .arg="Hello ztimer!" };
 *     ztimer_set(ZTIMER_SEC, &timeout, 2);
 *
 *     ztimer_sleep(ZTIMER_SEC, 5);
 * }
 * ```
 *
 *
 * # Design
 *
 * ## clocks, virtual timers, chaining
 *
 * The system is composed of clocks (virtual ztimer devices) which can be
 * chained to create an abstract view of a hardware timer/counter device. Each
 * ztimer clock acts as a operation on the next clock in the chain. At the end of
 * each ztimer chain there is always some kind of counter device object.
 *
 * Each clock device handles multiplexing (allowing multiple timers to be set)
 * and extension to full 32bit.
 *
 * Hardware interface submodules:
 *
 * - @ref ztimer_periph_rtt_init "ztimer_periph_rtt" interface for periph_rtt
 * - @ref ztimer_periph_rtc_init "ztimer_periph_rtc" interface for periph_rtc
 * - @ref ztimer_periph_timer_init "ztimer_periph_timer" interface for periph_timer
 *
 * Filter submodules:
 *
 * - @ref ztimer_convert_frac_init "ztimer_convert_frac" for fast frequency
 *   conversion using the frac library
 * - @ref ztimer_convert_muldiv64_init "ztimer_convert_muldiv64" for accurate
 *   but slow frequency conversion using 64bit division
 *
 *
 * A common chain could be:
 *
 * 1. ztimer_periph_timer (e.g., on top of an 1024Hz 16bit hardware timer)
 * 2. ztimer_convert_frac (to convert 1024 to 1000Hz)
 *
 * This is how e.g., the clock ZTIMER_MSEC might be configured on a specific
 * system.
 *
 * Every clock in the chain can always be used on its own. E.g. in the example
 * above, the ztimer_periph object can be used as ztimer clock with 1024Hz
 * ticks in addition to the ztimer_convert_frac with 1000Hz.
 *
 *
 * ## Timer handling
 *
 * Timers in ztimer are stored in a clock using a linked list for which each
 * entry stores the difference to the previous entry in the timer (T[n]). The
 * clock also stores the absolute time on which the relative offsets are based
 * (B), effectively storing the absolute target time for each entry (as B +
 * sum(T[0-n])).  Storing the entries in this way allows all entries to use the
 * full width of the used uint32_t, compared to storing the absolute time.
 *
 * In order to prevent timer processing offset to add up, whenever a timer
 * triggers, the list's absolute base time is set to the *expected* trigger
 * time (B + T[0]). The underlying clock is then set to alarm at (now() +
 * (now() - B) + T[1]). Thus even though the list is keeping relative offsets,
 * the time keeping is done by keeping track of the absolute times.
 *
 * Currently, a sorted singly linked list is used for storing the timers.
 * This choice has some implications:
 *
 * - only one pointer needed per timer object (for "next" element)
 * - simple implementation
 * - acceptable runtime for expected number of active timers (<50)
 * - constant get_min() (important for timer triggering)
 * - O(n) insertion / removal of timer objects
 *
 * By making the list doubly-linked, removal of timer objects could be easily
 * made a constant operation, at the price of another pointer per timer object
 * (for "previous" element).
 *
 * If deemed necessary, the linked list can be exchanged our augmented with
 * another data structure providing better algorithmic guarantees. It remains
 * to be shown whether the increased complexity would lead to better
 * performance for any reasonable amount of active timers.
 *
 *
 * ## Clock extension
 *
 * The API always allows setting full 32bit relative offsets for every clock.
 *
 * In some cases (e.g., a hardware timer only allowing getting/setting smaller
 * values or a conversion which would overflow uint32_t for large intervals),
 * ztimer takes care of extending timers.
 * This is enabled automatically for every ztimer clock that has a "max_value"
 * setting smaller than 2**32-1. If a ztimer_set() would overflow that value,
 * intermediate intervals of length (max_value / 2) are set until the remaining
 * interval fits into max_value.
 * If extension is enabled for a clock, ztimer_now() uses interval
 * checkpointing, storing the current time and corresponding clock tick value on
 * each call and using that information to calculate the current time.
 * This ensures correct ztimer_now() values if ztimer_now() is called at least
 * once every "max_value" ticks. This is ensured by scheduling intermediate
 * callbacks every (max_value / 2) ticks (even if no timeout is configured).
 *
 *
 * ## Reliability
 *
 * Care has been taken to avoid any unexpected behaviour of ztimer. In
 * particular, ztimer tries hard to avoid underflows (setting a backend timer
 * to a value at or behind the current time, causing the timer interrupt to
 * trigger one whole timer period too late).
 * This is done by always setting relative timeouts to backend timers, with
 * interrupts disabled and ensuring that very small values don't cause
 * underflows.
 *
 *
 * ## Configuration and convention
 *
 * As timer hardware and capabilities is diverse and ztimer allows configuring
 * and using arbitrary clock backends and conversions, it is envisioned to
 * provide default configurations that application developers can assume to be
 * available.
 *
 * These are implemented by using pointers to ztimer clocks using default names.
 *
 * For now, there are:
 *
 * ZTIMER_USEC: clock providing microsecond ticks
 *
 * ZTIMER_MSEC: clock providing millisecond ticks, using a low power timer if
 *              available on the platform
 *
 * ZTIMER_SEC:  clock providing second time, possibly using epoch semantics
 *
 * These pointers are defined in `ztimer.h` and can be used like this:
 *
 *     ztimer_now(ZTIMER_USEC);
 *
 * They also need to be added to USEMODULE using the names `ztimer_usec`,
 * `ztimer_msec` and `ztimer_sec`.
 *
 *
 * ## Some notes on ztimer's accuracy
 *
 * 1. ztimer *should* wait "at least" the specified timeout
 *
 * 2. due to its implementation details, expect +-1 clock tick systemic
 *    inaccuracy for all clocks.
 *
 * 3. for the predefined clocks (ZTIMER_USEC, ZTIMER_MSEC, ZTIMER_SEC), tick
 *    conversion might be applied using ztimer_convert_*, causing errors due to
 *    integer conversion and rounding. In particular, most RTT's closest match
 *    for milliseconds are 1024Hz, which will be converted using convert_frac to
 *    provide the 1ms clock.
 *
 * 4. Some platforms don't have any timer that can be configured to 1us. E.g.,
 *    the fe310 (hifive1/b) only supports a 32kHz timer, and most atmegas only
 *    support 250kHz. In order to not completely break all applications using
 *    ZTIMER_USEC, that clock will only provide ~30.5ms respectively 4us maximum
 *    accuracy on those boards. With DEVELHELP=1, a warning will be printed at
 *    boot time.
 *
 * 5. Due to +-1 systemic inaccuracies, it is advisable to use ZTIMER_MSEC for
 *    second timers up to 49 days (instead of ZTIMER_SEC).
 *
 * @{
 *
 * @file
 * @brief       ztimer API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef ZTIMER_H
#define ZTIMER_H

#include <stdint.h>

#include "sched.h"
#include "msg.h"
#include "mutex.h"
#include "rmutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Disables interaction with pm_layered for a clock
 */
#define ZTIMER_CLOCK_NO_REQUIRED_PM_MODE (UINT8_MAX)

/**
 * @brief ztimer_base_t forward declaration
 */
typedef struct ztimer_base ztimer_base_t;

/**
 * @brief ztimer_clock_t forward declaration
 */
typedef struct ztimer_clock ztimer_clock_t;

/**
 * @brief   Minimum information for each timer
 */
struct ztimer_base {
    ztimer_base_t *next;        /**< next timer in list */
    uint32_t offset;            /**< offset from last timer in list */
};

#if MODULE_ZTIMER_NOW64
typedef uint64_t ztimer_now_t;  /**< type for ztimer_now() result */
#else
typedef uint32_t ztimer_now_t;  /**< type for ztimer_now() result */
#endif

/**
 * @brief   ztimer structure
 *
 * This type represents an instance of a timer, which is set on an
 * underlying clock object
 */
typedef struct {
    ztimer_base_t base;             /**< clock list entry */
    void (*callback)(void *arg);    /**< timer callback function pointer */
    void *arg;                      /**< timer callback argument */
} ztimer_t;

/**
 * @brief   ztimer backend method structure
 *
 * This table contains pointers to the virtual methods for a ztimer clock.
 *
 * These functions used by ztimer core to interact with the underlying clock.
 */
typedef struct {
    /**
     * @brief   Set a new timer target
     * @param   clock       ztimer clock to set the new target
     * @param   val         Relative target (e.g. fire at value `now() + val`)
     */
    void (*set)(ztimer_clock_t *clock, uint32_t val);

    /**
     * @brief   Get the current count of the timer
     * @param   clock       ztimer clock to get the current time from
     */
    uint32_t (*now)(ztimer_clock_t *clock);

    /**
     * @brief   Cancel any set target
     * @param   clock       ztimer clock to cancel a pending alarm, if any
     */
    void (*cancel)(ztimer_clock_t *clock);
} ztimer_ops_t;

/**
 * @brief   ztimer device structure
 */
struct ztimer_clock {
    ztimer_base_t list;             /**< list of active timers              */
    const ztimer_ops_t *ops;        /**< pointer to methods structure       */
    ztimer_base_t *last;            /**< last timer in queue, for _is_set() */
    uint16_t adjust_set;            /**< will be subtracted on every set()  */
    uint16_t adjust_sleep;          /**< will be subtracted on every sleep(),
                                         in addition to adjust_set          */
#if MODULE_ZTIMER_EXTEND || MODULE_ZTIMER_NOW64 || DOXYGEN
    /* values used for checkpointed intervals and 32bit extension */
    uint32_t max_value;             /**< maximum relative timer value       */
    uint32_t lower_last;            /**< timer value at last now() call     */
    ztimer_now_t checkpoint;        /**< cumulated time at last now() call  */
#endif
#if MODULE_PM_LAYERED || DOXYGEN
    uint8_t required_pm_mode;       /**< min. pm mode required for the clock to run */
#endif
};

/**
 * @brief   main ztimer callback handler
 */
void ztimer_handler(ztimer_clock_t *clock);

/* User API */
/**
 * @brief   Set a timer on a clock
 *
 * This will place @p timer in the timer targets queue of @p clock.
 *
 * @note The memory pointed to by @p timer is not copied and must
 *       remain in scope until the callback is fired or the timer
 *       is removed via @ref ztimer_remove
 *
 * @param[in]   clock       ztimer clock to operate on
 * @param[in]   timer       timer entry to set
 * @param[in]   val         timer target (relative ticks from now)
 */
void ztimer_set(ztimer_clock_t *clock, ztimer_t *timer, uint32_t val);

/**
 * @brief   Check if a timer is currently active
 *
 * @param[in]   clock       ztimer clock to operate on
 * @param[in]   timer       timer to check
 *
 * @return  > 0 if timer is active
 * @return 0 if timer is not active
 */
unsigned ztimer_is_set(const ztimer_clock_t *clock, const ztimer_t *timer);

/**
 * @brief   Remove a timer from a clock
 *
 * This will place @p timer in the timer targets queue for @p clock.
 *
 * This function does nothing if @p timer is not found in the timer queue of
 * @p clock.
 *
 * @param[in]   clock       ztimer clock to operate on
 * @param[in]   timer       timer entry to remove
 */
void ztimer_remove(ztimer_clock_t *clock, ztimer_t *timer);

/**
 * @brief   Post a message after a delay
 *
 * This function sets a timer that will send a message @p offset ticks
 * from now.
 *
 * @note The memory pointed to by @p timer and @p msg will not be copied, i.e.
 *       `*timer` and `*msg` needs to remain valid until the timer has triggered.
 *
 * @param[in]   clock           ztimer clock to operate on
 * @param[in]   timer           ztimer timer struct to use
 * @param[in]   offset          ticks from now
 * @param[in]   msg             pointer to msg that will be sent
 * @param[in]   target_pid      pid the message will be sent to
 */
void ztimer_set_msg(ztimer_clock_t *clock, ztimer_t *timer, uint32_t offset,
                    msg_t *msg, kernel_pid_t target_pid);

/**
 * @brief receive a message (blocking, with timeout)
 *
 * Similar to msg_receive(), but with a timeout parameter.
 * The function will return after waiting at most @p timeout ticks.
 *
 * @note: This might function might leave a message with type MSG_ZTIMER in the
 *        thread's message queue, which must be handled (ignored).
 *
 * @param[in]   clock           ztimer clock to operate on
 * @param[out]  msg             pointer to buffer which will be filled if a
 *                              message is received
 * @param[in]   timeout         relative timeout, in @p clock time units
 *
 * @return  >=0 if a message was received
 * @return  -ETIME on timeout
 */
int ztimer_msg_receive_timeout(ztimer_clock_t *clock, msg_t *msg,
                               uint32_t timeout);

/* created with dist/tools/define2u16.py */
#define MSG_ZTIMER 0xc83e   /**< msg type used by ztimer_msg_receive_timeout */

/**
 * @brief ztimer_now() for extending timers
 *
 * @internal
 *
 * @param[in]   clock          ztimer clock to operate on
 * @return  Current count on the clock @p clock
 */
ztimer_now_t _ztimer_now_extend(ztimer_clock_t *clock);

/**
 * @brief   Get the current time from a clock
 *
 * @param[in]   clock          ztimer clock to operate on
 *
 * @return  Current count on @p clock
 */
static inline ztimer_now_t ztimer_now(ztimer_clock_t *clock)
{
#if MODULE_ZTIMER_NOW64
    if (1) {
#elif MODULE_ZTIMER_EXTEND
    if (clock->max_value < UINT32_MAX) {
#else
    if (0) {
#endif
        return _ztimer_now_extend(clock);
    }
    else {
        return clock->ops->now(clock);
    }
}

/**
 * @brief Suspend the calling thread until the time (@p last_wakeup + @p period)
 *
 * This function can be used to create periodic wakeups.
 *
 * When the function returns, @p last_wakeup is set to
 * (@p last_wakeup + @p period).
 *
 * @c last_wakeup should be set to ztimer_now(@p clock) before first call of the
 * function.
 *
 * If the time (@p last_wakeup + @p period) has already passed, the function
 * sets @p last_wakeup to @p last_wakeup + @p period and returns immediately.
 *
 * @param[in]   clock           ztimer clock to operate on
 * @param[in]   last_wakeup     base time stamp for the wakeup
 * @param[in]   period          time in ticks that will be added to @p last_wakeup
 */
void ztimer_periodic_wakeup(ztimer_clock_t *clock, uint32_t *last_wakeup,
                            uint32_t period);

/**
 * @brief   Put the calling thread to sleep for the specified number of ticks
 *
 * @param[in]   clock           ztimer clock to use
 * @param[in]   duration        duration of sleep, in @p ztimer time units
 */
void ztimer_sleep(ztimer_clock_t *clock, uint32_t duration);

/**
 * @brief   Busy-wait specified duration
 *
 * @note: This blocks lower priority threads. Use only for *very* short delays.
 *
 * @param[in]   clock           ztimer clock to use
 * @param[in]   duration        duration to spin, in @p clock time units
 */
static inline void ztimer_spin(ztimer_clock_t *clock, uint32_t duration)
{
    uint32_t end = ztimer_now(clock) + duration;

    /* Rely on integer overflow. `end - now` will be smaller than `duration`,
     * counting down, until it underflows to UINT32_MAX. Loop ends then. */
    while ((end - ztimer_now(clock)) <= duration) {}
}

/**
 * @brief Set a timer that wakes up a thread
 *
 * This function sets a timer that will wake up a thread when the timer has
 * expired.
 *
 * @param[in] clock         ztimer clock to operate on
 * @param[in] timer         timer struct to work with.
 * @param[in] offset        clock ticks from now
 * @param[in] pid           pid of the thread that will be woken up
 */
void ztimer_set_wakeup(ztimer_clock_t *clock, ztimer_t *timer, uint32_t offset,
                       kernel_pid_t pid);

/**
 * @brief    Set timeout thread flag after @p timeout
 *
 * This function will set THREAD_FLAG_TIMEOUT on the current thread after @p
 * timeout usec have passed.
 *
 * @param[in]   clock           ztimer clock to operate on
 * @param[in]   timer           timer struct to use
 * @param[in]   timeout         timeout in ztimer_clock's ticks
 */
void ztimer_set_timeout_flag(ztimer_clock_t *clock, ztimer_t *timer,
                             uint32_t timeout);

/**
 * @brief   Try to lock the given mutex, but give up after @p timeout
 *
 * @param[in]       clock       ztimer clock to operate on
 * @param[in,out]   mutex       Mutex object to lock
 * @param[in]       timeout     timeout after which to give up
 *
 * @retval  0               Success, caller has the mutex
 * @retval  -ECANCELED      Failed to obtain mutex within @p timeout
 */
int ztimer_mutex_lock_timeout(ztimer_clock_t *clock, mutex_t *mutex,
                              uint32_t timeout);

/**
 * @brief   Try to lock the given rmutex, but give up after @p timeout
 *
 * @param[in]       clock       ztimer clock to operate on
 * @param[in,out]   rmutex      rmutex object to lock
 * @param[in]       timeout     timeout after which to give up
 *
 * @retval  0               Success, caller has the rmutex
 * @retval  -ECANCELED      Failed to obtain rmutex within @p timeout
 */
int ztimer_rmutex_lock_timeout(ztimer_clock_t *clock, rmutex_t *rmutex,
                               uint32_t timeout);

/**
 * @brief   Update ztimer clock head list offset
 *
 * @internal
 *
 * @param[in]   clock  ztimer clock to work on
 */
void ztimer_update_head_offset(ztimer_clock_t *clock);

/**
 * @brief   Initialize the board-specific default ztimer configuration
 */
void ztimer_init(void);

#if defined(MODULE_ZTIMER_EXTEND) || defined(DOXYGEN)
/**
 * @brief   Initialize possible ztimer extension intermediate timer
 *
 * This will basically just set a timer to (clock->max_value >> 1), *if*
 * max_value is not UINT32_MAX.
 *
 * This is called automatically by all ztimer backends and extension modules.
 *
 * @internal
 */
static inline void ztimer_init_extend(ztimer_clock_t *clock)
{
    if (clock->max_value < UINT32_MAX) {
        clock->ops->set(clock, clock->max_value >> 1);
    }
}
#endif /* MODULE_ZTIMER_EXTEND */

/* default ztimer virtual devices */
/**
 * @brief   Default ztimer microsecond clock
 */
extern ztimer_clock_t *const ZTIMER_USEC;

/**
 * @brief   Default ztimer millisecond clock
 */
extern ztimer_clock_t *const ZTIMER_MSEC;

/**
 * @brief   Default ztimer second clock
 */
extern ztimer_clock_t *const ZTIMER_SEC;

/**
 * @brief   Base ztimer for the microsecond clock (ZTIMER_USEC)
 *
 * This ztimer will reference the counter device object at the end of the
 * chain of ztimer_clock_t for ZTIMER_USEC.
 *
 * If the base counter device object's frequency (CONFIG_ZTIMER_USEC_BASE_FREQ)
 * is not 1MHz then ZTIMER_USEC will be converted on top of this one. Otherwise
 * they will reference the same ztimer_clock.
 *
 * To avoid chained conversions its better to base new ztimer_clock on top of
 * ZTIMER_USEC_BASE running at CONFIG_ZTIMER_USEC_BASE_FREQ.
 *
 */
extern ztimer_clock_t *const ZTIMER_USEC_BASE;

/**
 * @brief   Base ztimer for the millisecond clock (ZTIMER_MSEC)
 *
 * This ztimer will reference the counter device object at the end of the
 * chain of ztimer_clock_t for ZTIMER_MSEC.
 *
 * If ztimer_periph_rtt is not used then ZTIMER_MSEC_BASE will reference the
 * same base as ZTIMER_USEC_BASE.
 *
 * If the base counter device object's frequency (CONFIG_ZTIMER_MSEC_BASE_FREQ)
 * is not 1KHz then ZTIMER_MSEC will be converted on top of this one. Otherwise
 * they will reference the same ztimer_clock.
 *
 * To avoid chained conversions its better to base new ztimer_clock on top of
 * ZTIMER_MSEC_BASE running at CONFIG_ZTIMER_MSEC_BASE_FREQ.
 *
 */
extern ztimer_clock_t *const ZTIMER_MSEC_BASE;

#ifdef __cplusplus
}
#endif

#endif /* ZTIMER_H */
/** @} */
