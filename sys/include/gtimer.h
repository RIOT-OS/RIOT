/**
 * Declarations for the global virtual clock
 *
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 *
 * If compiled with the GTIMER_TEST flag, the time doesn't progress on it's on,
 * but must be set by gtimer_set_local. This is for testing purpose.
 *
 * @ingroup sys
 * @{
 * @file    gtimer.h
 * @brief   Declarations for the virtual clock
 * @author  Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author  Daniel Jentsch <d.jentsch@fu-berlin.de>
 * @}
 */
#ifndef __GTIMER_H
#define __GTIMER_H

#include <stdint.h>
#include "timex.h"
#include "vtimer.h"

typedef struct gtimer_timeval {
    uint64_t local;
    uint64_t global;
    float rate;
} gtimer_timeval_t;

#ifdef GTIMER_TEST
/**
 * Sets the time and clock correction to 0.
 */
void gtimer_test_reset(void);
/**
 * @brief Simulates the behaviour of waiting.
 *
 * The function will return immediately but all function of gtimer (and may
 * other depending on them) will act like the time was actually passed.
 */
void gtimer_test_wait(timex_t time);
#endif

/**
 * @brief Returns the current global time.
 */
void gtimer_now(timex_t *out);

/**
 * @brief Starts a new global clock based on a real hw clock.
 */
void gtimer_init(void);

/**
 * @brief Returns the current global and local time.
 *
 * Should only be used by a time synchronization protocol.
 *
 * @param[out] current local, global time and relative clock rate.
 */
void gtimer_sync_now(gtimer_timeval_t *out);

/**
 * @brief Sets to relative rate between the local and the global clock.
 *
 * Should only be used by a time synchronization protocol.
 */
void gtimer_sync_set_relative_rate(float rate);

/**
 * @brief Adds a offset.
 *
 * Should only be used by a time synchronization protocol.
 */
void gtimer_sync_set_global_offset(int64_t global_offset);

/**
 * @brief Returns the relative rate between the local and the global clock.
 *
 * Should only be used by a time synchronization protocol.
 */
float gtimer_sync_get_relative_rate(void);

#ifdef GTIMER_TEST
/**
 * @brief For testing purpose only. "Sets" the local hardware time.
 */
void gtimer_sync_set_local(timex_t new_now);
#endif

#endif /* __GTIMER_H */
