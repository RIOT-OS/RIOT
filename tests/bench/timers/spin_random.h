/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_spin_random Spin_random - random CPU delays
 * @ingroup     sys
 * @brief       CPU spinning routines for random runtime delays
 *
 * @{
 * @file
 * @brief       spin_random declarations
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef SPIN_RANDOM_H
#define SPIN_RANDOM_H

#include <stdint.h>
#include "periph/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Spin for a short random delay to increase fuzziness of a test
 *
 * The amount of time spent in this function will be between 0 and up to at
 * least the amount of time that the spin_random_calibrate function was called
 * with.
 * The spin duration is < 2 * spin_max_target, in timer ticks of the timer that
 * was used by spin_random_calibrate.
 *
 * @pre spin_random_calibrate must have been run beforehand to calibrate the
 * maximum spin count
 */
void spin_random_delay(void);

/**
 * @brief   Calibrate the maximum spin count
 *
 * The calibration function will ensure that @ref spin_random_delay will give a
 * random delay up to at least @p spin_max_target counts on the timer device
 * given by @p timer_dev.
 *
 * The new maximum spin count will ensure that the maximum random spin in
 * @ref spin_random_delay will be at least @p spin_max_target ticks long, and
 * no more than 2 * @p spin_max_target ticks.
 *
 * @pre The periph_timer @p timer_dev must be initialized and running (counting).
 *
 * @param[in]   timer_dev   Timer device to use as reference
 * @param[in]   spin_max_target Spin duration target time in @p timer_dev ticks.
 *
 * @return the new maximum spin count
 */
uint32_t spin_random_calibrate(tim_t timer_dev, uint32_t spin_max_target);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SPIN_RANDOM_H */
