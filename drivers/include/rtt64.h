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
#include "time_q48.h"
#include "time_units.h"

#ifdef __cplusplus
extern "C" {
#endif

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
void rtt64_set(time_q48_t now);

/**
 * @brief Get RTT 64 bit counter
 *
 * The upper 48 bits of the counter are the seconds since 1st January 1970
 * The lower 16 bits are the fractions of the second with 1/64k precision
 *
 * @return uint64_t current time in the 48:16 format
 */
time_q48_t rtt64_get(void);

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
void rtt64_set_alarm(time_q48_t alarm, rtt_cb_t cb, void *arg);

/**
 * @brief Get RTT 64 bit alarm counter
 *
 * The upper 48 bits of the alarm are the seconds since 1st January 1970
 * The lower 16 bits are the fractions of the second with 1/64k precision
 *
 * @return uint64_t alarm in the 48:16 format
 */
time_q48_t rtt64_get_alarm(void);

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
    time_q48_t now = time_to_q48(secs, us);
    rtt64_set(now);
}

/**
 * @brief Get RTT 64 time in seconds, microseconds
 *
 * @param secs  Current seconds since 1st January 1970
 * @param us    Microseconds of the current second
 */
static inline void rtt64_get_time(uint64_t *secs, uint32_t *us)
{
    time_q48_t now = rtt64_get();

    if (secs) {
        *secs = time_q48_get_sec(now);
    }
    if (us) {
        *us = time_q48_get_usec(now);
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
    time_q48_t alarm = time_to_q48(secs, us);
    rtt64_set_alarm(alarm, cb, arg);
}

/**
 * @brief Get RTT 64 bit alarm in seconds, microseconds
 *
 * @param secs  Second of the alarm
 * @param us    Microsecond of the alarm
 */
static inline void rtt64_get_alarm_time(uint64_t *secs, uint32_t *us)
{
    time_q48_t alarm = rtt64_get_alarm();

    if (secs) {
        *secs = time_q48_get_sec(alarm);
    }
    if (us) {
        *us = time_q48_get_usec(alarm);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* RTT64_H */
/** @} */
