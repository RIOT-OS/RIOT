/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_rtt64           64 bit RTT extension
 * @ingroup     drivers_periph_rtt
 *
 * @{
 *
 * @file
 * @brief       64 bit RTT extension
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef RTT64_H
#define RTT64_H

#include <stdint.h>

#include "periph/rtt.h"
#include "time_units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RTT64 time format
 *
 * The upper 48 bits of the counter are the seconds since 1st January 1970
 * The lower 16 bits are the fractions of the second with 1/64k prec
 */
typedef uint64_t rtt64_t;

/**
 * @brief Get seconds from RTT64 time format
 *
 * @param[in] time  timestamp in RTT64 format
 *
 * @return Second part of the timestamp
 */
static inline uint64_t rtt64_sec(rtt64_t time)
{
    return time >> 16;
}

/**
 * @brief Get microseconds from RTT64 time format
 *
 * @param[in] time  timestamp in RTT64 format
 *
 * @return Microsecond part of the timestamp
 */
static inline uint32_t rtt64_usec(rtt64_t time)
{
    return ((uint64_t)US_PER_SEC * (time & 0xFFFF)) >> 16;
}

/**
 * @brief Convert time value to RTT64 time format
 *
 * @param[in] secs  Seconds since 01.01.1970
 * @param[in] us    Microseconds of the current second
 *
 * @return Timestamp in RTT64 format
 */
static inline rtt64_t rtt64_counter(uint64_t secs, uint32_t us)
{
    uint64_t now = secs << 16;
    now += ((uint64_t)us * 0xFFFF) / US_PER_SEC;

    return now;
}

/**
 * @brief Initialize RTT 64 bit extension
 */
void rtt64_init(void);

/**
 * @brief Set RTT 64 bit counter
 *
 * The upper 48 bits of the counter are the seconds since 1st January 1970
 * The lower 16 bits are the fractions of the second with 1/64k precision
 *
 * @param now timestamp in the 48:16 format
 */
void rtt64_set_counter(rtt64_t now);

/**
 * @brief Get RTT 64 bit counter
 *
 * The upper 48 bits of the counter are the seconds since 1st January 1970
 * The lower 16 bits are the fractions of the second with 1/64k precision
 *
 * @return uint64_t current time in the 48:16 format
 */
rtt64_t rtt64_get_counter(void);

/**
 * @brief Set RTT 64 bit alarm counter
 *
 * The upper 48 bits of the alarm are the seconds since 1st January 1970
 * The lower 16 bits are the fractions of the second with 1/64k precision
 *
 * @param alarm alarm in the 48:16 format
 * @param cb    alarm callback
 * @param arg   alarm callback argument
 */
void rtt64_set_alarm_counter(rtt64_t alarm, rtt_cb_t cb, void *arg);

/**
 * @brief Get RTT 64 bit alarm counter
 *
 * The upper 48 bits of the alarm are the seconds since 1st January 1970
 * The lower 16 bits are the fractions of the second with 1/64k precision
 *
 * @return uint64_t alarm in the 48:16 format
 */
rtt64_t rtt64_get_alarm_counter(void);

/**
 * @brief Clear any pending alarm
 */
void rtt64_clear_alarm(void);

/**
 * @brief Set RTT 64 time in seconds, microseconds
 *
 * @param secs  Current seconds since 1st January 1970
 * @param us    Microseconds of the current second
 */
static inline void rtt64_set_time(uint64_t secs, uint32_t us)
{
    rtt64_t now = rtt64_counter(secs, us);
    rtt64_set_counter(now);
}

/**
 * @brief Get RTT 64 time in seconds, microseconds
 *
 * @param secs  Current seconds since 1st January 1970
 * @param us    Microseconds of the current second
 */
static inline void rtt64_get_time(uint64_t *secs, uint32_t *us)
{
    rtt64_t now = rtt64_get_counter();

    if (secs) {
        *secs = rtt64_sec(now);
    }
    if (us) {
        *us = rtt64_usec(now);
    }
}

/**
 * @brief Set RTT 64 bit alarm in seconds, microseconds
 *
 * @param secs  Second of when the alarm should happen
 * @param us    Microsecond of the alarm
 * @param cb    Alarm callback
 * @param arg   Alarm callback argument
 */
static inline void rtt64_set_alarm_time(uint64_t secs, uint32_t us, rtt_cb_t cb, void *arg)
{
    rtt64_t alarm = rtt64_counter(secs, us);
    rtt64_set_alarm_counter(alarm, cb, arg);
}

/**
 * @brief Get RTT 64 bit alarm in seconds, microseconds
 *
 * @param secs  Second of the alarm
 * @param us    Microsecond of the alarm
 */
static inline void rtt64_get_alarm_time(uint64_t *secs, uint32_t *us)
{
    rtt64_t alarm = rtt64_get_alarm_counter();

    if (secs) {
        *secs = rtt64_sec(alarm);
    }
    if (us) {
        *us = rtt64_usec(alarm);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* RTT64_H */
/** @} */
