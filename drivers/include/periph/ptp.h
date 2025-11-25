/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_ptp  PTP-Clock
 * @ingroup     drivers_periph
 * @brief       Low-level PTP clock peripheral driver
 *
 * # Introduction
 *
 * The [Precision Time Protocol (PTP)](https://standards.ieee.org/content/ieee-standards/en/standard/1588-2019.html)
 * can be used to synchronize clocks with hardware support in an Ethernet PHY,
 * that allows to precisely estimate (and, thus, compensate) network delay
 * between time server and client. This allows PTP synchronization to be highly
 * accurate (<< 1 µs offset between server and client), but requires hardware
 * support on each synchronized node and the PTP server, all nodes to be
 * connected to the same* Ethernet network, and (for best results) hardware
 * support in all intermediate switches.
 *
 * # (No) Synchronization Using This API
 *
 * This API is intended to provide basic access to a PTP hardware clock. This
 * does not cover any synchronization. Some PTP hardware clocks (e.g. on the
 * STM32) can be used without synchronization by manually setting the time.
 * Thus, the PTP clock can be useful even without synchronization implemented.
 *
 * It is intended that the actual synchronization is implemented externally
 * later on. The functions @ref ptp_clock_adjust and @ref ptp_clock_adjust_speed
 * are specified with the needs of a synchronization solution in mind.
 *
 * # Clock vs Timer
 *
 * This API provides both `ptp_clock_*()` and `ptp_timer_*()` functions, to
 * distinguish between the feature set being used. A PTP peripheral might only
 * support the feature `periph_ptp`, but not `periph_ptp_timer`.
 *
 * # (Lack of) Power Considerations
 *
 * It is assumed that a board connected to a wired network is also powered
 * from mains. Additionally, a high-resolution high-accuracy clock that is
 * periodically synced over network is just not going to play well with
 * low-power application scenarios.
 *
 * @{
 * @file
 * @brief       Low-level PTP clock peripheral driver interface definitions
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef PERIPH_PTP_H
#define PERIPH_PTP_H

#include <stdint.h>

#include "periph_cpu.h"
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/* verify settings from periph_cpu.h */
#if !defined(HAVE_PTP_CLOCK_READ) && !defined(HAVE_PTP_CLOCK_READ_U64)
#error "Neither ptp_clock_read() nor ptp_clock_read_u64() implemented"
#endif

#if !defined(HAVE_PTP_CLOCK_SET) && !defined(HAVE_PTP_CLOCK_SET_U64)
#error "Neither ptp_clock_set() nor ptp_clock_set_u64() implemented"
#endif

#if \
    !defined(HAVE_PTP_TIMER_SET_ABSOLUTE) && \
    !defined(HAVE_PTP_TIMER_SET_ABSOLUTE_U64) && \
    IS_USED(MODULE_PERIPH_PTP_TIMER)
#error "Neither ptp_timer_set_absolute() nor ptp_timer_set_absolute_u64() implemented"
#endif

/**
 * @brief   Unsigned integer type to store seconds since epoch for use in PTP
 *
 * The PTP protocol defines the seconds part of PTP timestamps as an 48 bit
 * unsigned integer. We go for 32 bit for now (works until year 2106) and will
 * later extend this type to 64 bit. Users are highly encouraged to use this
 * type instead of `uint32_t`, if they intent that their software still works
 * in a couple of decades.
 */
typedef uint32_t ptp_seconds_t;

/**
 * @brief   A PTP timestamp in seconds + nanoseconds since UNIX epoch
 *
 * According to IEEE 1588-2019 specification in section "5.3.3 Timestamp",
 * timestamps are represented as seconds and nanoseconds since epoch. For the
 * seconds parts an 48 bit unsigned integer is used in the protocol and a 32 bit
 * unsigned integer for the nanoseconds.
 */
typedef struct {
    ptp_seconds_t seconds;  /**< Seconds since UNIX epoch */
    uint32_t nanoseconds;   /**< Nanoseconds part */
} ptp_timestamp_t;

/**
 * @brief   Compare two PTP timestamps
 *
 * @param   a       First timestamp
 * @param   b       Second timestamp
 *
 * @retval  -1      @p a < @p b
 * @retval  0       @p a == @p b
 * @retval  1       @p a > @p b
 */
static inline int ptp_cmp(const ptp_timestamp_t *a, const ptp_timestamp_t *b)
{
    if (a->seconds < b->seconds) {
        return -1;
    }

    if (a->seconds > b->seconds) {
        return 1;
    }

    if (a->nanoseconds < b->nanoseconds) {
        return -1;
    }

    if (a->nanoseconds > b->nanoseconds) {
        return 1;
    }

    return 0;
}

/**
 * @brief   Add a given offset onto the given timestamp
 *
 * @param[in,out]   t       Timestamp to add offset to
 * @param[in]       offset  Offset to apply
 */
static inline void ptp_add(ptp_timestamp_t *t, int64_t offset)
{
    /* Modulo for negative numbers should be avoided */
    if (offset >= 0) {
        uint64_t abs_offset = offset;
        t->seconds += abs_offset / NS_PER_SEC;
        t->nanoseconds += abs_offset % NS_PER_SEC;
        /* correct overflow of nanosecond part */
        if (t->nanoseconds >= NS_PER_SEC) {
            t->nanoseconds -= NS_PER_SEC;
            t->seconds++;
        }
    }
    else {
        uint64_t abs_offset = -offset;
        t->seconds -= abs_offset / NS_PER_SEC;
        t->nanoseconds -= abs_offset % NS_PER_SEC;
        /* correct underflow of nanosecond part */
        if (t->nanoseconds > NS_PER_SEC) {
            t->nanoseconds += NS_PER_SEC;
            t->seconds--;
        }
    }
}

/**
 * @brief   Convert time from nanoseconds since epoch to ptp_timestamp_t format
 *
 * @param[out]  dest            The timestamp will be written here
 * @param[in]   ns_since_epoch  Time in nanoseconds since epoch to convert
 */
static inline void ptp_ns2ts(ptp_timestamp_t *dest, uint64_t ns_since_epoch)
{
    dest->seconds = ns_since_epoch / NS_PER_SEC;
    dest->nanoseconds = ns_since_epoch % NS_PER_SEC;
}

/**
 * @brief   Convert time from nanoseconds since epoch to ptp_timestamp_t format
 *
 * @param[in]   t               Time to convert to nanoseconds since epoch
 *
 * @return  The time specified by @p t in nanoseconds since epoch
 */
static inline uint64_t ptp_ts2ns(const ptp_timestamp_t *t)
{
    return t->seconds * NS_PER_SEC + t->nanoseconds;
}

/**
 * @brief   Initialize the given PTP peripheral
 *
 * @note    Implementations of this functions have to use `assert()` to make
 *          the configuration is valid.
 *
 * @pre     This function must be called at most once
 *
 * After calling this, the PTP clock (and the PTP timer, if the feature
 * `periph_ptp_timer` is used in addition to `periph_ptp_clock`) can be used.
 */
void ptp_init(void);

/**
 * @brief   Adjust the PTP clock speed as given
 *
 * @param   correction  Specification of the clock speed adjustment
 *
 * @note    This implies feature `periph_ptp_speed_adjustment`
 *
 * The clock speed is adjusted in regard to its nominal clock speed. This means
 * that calls with the same value in @p correction are idempotent.
 *
 * 1. A call with @p correction set to `0` restores the nominal clock speed.
 * 2. A call with a positive value for @p correction speeds the clock up
 *    by `correction / (1 << 32)` (so up to ~50% for `INT32_MAX`).
 * 3. A call with a negative value for @p correction slows the clock down by
 *    `-correction / (1 << 32)` (so up to 50% for `INT32_MIN`).
 *
 * This allows the clock speed to be adjusted in steps ± 2.3284E-08 % in relation
 * to its nominal clock speed, thus, allowing to counter systematic clock drift.
 * In addition, this is intended to "smoothly" synchronize the clock over time
 * to avoid jumps in the system time. (Especially calling @ref ptp_clock_adjust
 * with negative values might be something to avoid, when applications are not
 * prepared with clocks going backwards.)
 */
void ptp_clock_adjust_speed(int32_t correction);

/**
 * @brief   Adjust the PTP clock as given
 *
 * @param   offset  Offset to add onto current system time
 *
 * Same as `ptp_clock_set_u64(ptp_clock_read_u64() + offset)`, but implemented
 * to introduce as little error as possible while setting the offset. This
 * is intended to be used by clock synchronization implementations.
 */
void ptp_clock_adjust(int64_t offset);

#if defined(HAVE_PTP_CLOCK_READ) || defined(DOXYGEN)
/**
 * @brief   Get the current system time as PTP timestamp
 *
 * @param[out]  timestamp   The timestamp will be written here
 * @pre     The PTP clock is initialized
 */
void ptp_clock_read(ptp_timestamp_t *timestamp);
#endif /* HAVE_PTP_CLOCK_READ */

#if defined(HAVE_PTP_CLOCK_READ_U64) || defined(DOXYGEN)
/**
 * @brief   Get the current system time in nanosecond since UNIX epoch
 *
 * @return  Nanoseconds since 1. 1. 1970 0:00:00 UTC
 *
 * @pre     The PTP clock is initialized
 *
 * @note    An `uint64_t` allows nanosecond timestamps within 1970-01-01 and
 *          2554-07-21 to be represented.
 */
uint64_t ptp_clock_read_u64(void);
#endif /* HAVE_PTP_CLOCK_READ_U64 */

#if defined(HAVE_PTP_CLOCK_SET) || defined(DOXYGEN)
/**
 * @brief   Set the current system time
 *
 * @param   time    The new system time
 *
 * @pre     The PTP clock is initialized
 */
void ptp_clock_set(const ptp_timestamp_t *time);
#endif /* HAVE_PTP_CLOCK_SET */

#if defined(HAVE_PTP_CLOCK_SET_U64) || defined(DOXYGEN)
/**
 * @brief   Set the current system time in nanosecond since UNIX epoch
 *
 * @param   ns_since_epoch  New time to set
 *
 * @pre     The PTP clock is initialized
 */
void ptp_clock_set_u64(uint64_t ns_since_epoch);
#endif /* HAVE_PTP_CLOCK_SET_U64 */

/**
 * @brief   External function to call when the PTP clock timer fired
 *
 * @note    This function needs to be implemented by the *user* of this API
 * @note    This function implies feature `periph_ptp_timer`
 *
 * Since at most one PTP clock is expected on each board, we can avoid the
 * overhead of indirect function calls here and let users of this API just
 * implement this function.
 */
void ptp_timer_cb(void);

#if defined(HAVE_PTP_TIMER_SET_ABSOLUTE) || defined(DOXYGEN)
/**
 * @brief   Set an absolute timeout value, possibly overwriting an existing
 *          timeout
 *
 * @param   target  Timestamp when the timeout should fire
 *
 * @note    Only a single timeout is supported by the PTP timer
 * @note    This function implies feature `periph_ptp_timer`
 *
 * @details If the target time is in the past or equal to the current time, the
 *          IRQ should trigger right away.
 */
void ptp_timer_set_absolute(const ptp_timestamp_t *target);
#endif /* HAVE_PTP_TIMER_SET_ABSOLUTE */

#if defined(HAVE_PTP_TIMER_SET_ABSOLUTE_U64) || defined(DOXYGEN)
/**
 * @brief   Set an absolute timeout value, possibly overwriting an existing
 *          timeout
 *
 * @param   target  Timestamp when the timeout should fire (ns since epoch)
 *
 * @note    Only a single timeout is supported by the PTP timer
 * @note    This function implies feature `periph_ptp_timer`
 *
 * @details If the target time is in the past or equal to the current time, the
 *          IRQ should trigger right away.
 */
void ptp_timer_set_absolute_u64(uint64_t target);
#endif /* HAVE_PTP_TIMER_SET_ABSOLUTE_U64 */

/**
 * @brief   Set an relative timeout value, possibly overwriting an existing
 *          timeout
 *
 * @param   target  Number of nanoseconds after which the timeout should fire
 *
 * @note    Only a single timeout is supported by the PTP timer
 * @note    This function implies feature `periph_ptp_timer`
 */
void ptp_timer_set_u64(uint64_t target);

/**
 * @brief   Clears any pending timeout on the PTP timer
 *
 * @note    This function implies feature `periph_ptp_timer`
 */
void ptp_timer_clear(void);

/* Fallback implementations (the driver can implement either the
 * functions using `ptp_timestamp_t` or `uint64_t`, the other flavor will
 * be provided on top here): */

#ifndef HAVE_PTP_CLOCK_READ
static inline void ptp_clock_read(struct ptp_timestamp_t *timestamp)
{
    ptp_ns2ts(timestamp, ptp_clock_read_u64());
}
#endif /* !HAVE_PTP_CLOCK_READ */

#ifndef HAVE_PTP_CLOCK_READ_U64
static inline uint64_t ptp_clock_read_u64(void)
{
    ptp_timestamp_t ts;
    ptp_clock_read(&ts);
    return ptp_ts2ns(&ts);
}
#endif /* !HAVE_PTP_CLOCK_READ_U64 */

#ifndef HAVE_PTP_CLOCK_SET
static inline void ptp_clock_set(const ptp_timestamp_t *time)
{
    ptp_clock_set_u64(ptp_ts2ns(time));
}
#endif /* !HAVE_PTP_CLOCK_SET */

#ifndef HAVE_PTP_CLOCK_SET_U64
static inline void ptp_clock_set_u64(uint64_t ns_since_epoch)
{
    ptp_timestamp_t time;
    ptp_ns2ts(&time, ns_since_epoch);
    ptp_clock_set(&time);
}
#endif /* !HAVE_PTP_CLOCK_SET_U64 */

#ifndef HAVE_PTP_TIMER_SET_ABSOLUTE
static inline void ptp_timer_set_absolute(const ptp_timestamp_t *target)
{
    ptp_timer_set_absolute_u64(ptp_ts2ns(target));
}
#endif /* !HAVE_PTP_TIMER_SET_ABSOLUTE */

#ifndef HAVE_PTP_TIMER_SET_ABSOLUTE_U64
static inline void ptp_timer_set_absolute_u64(uint64_t target)
{
    ptp_timestamp_t ts;
    ptp_ns2ts(&ts, target);
    ptp_timer_set_absolute(&ts);
}
#endif /* !HAVE_PTP_TIMER_SET_ABSOLUTE_U64 */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_PTP_H */
/** @} */
