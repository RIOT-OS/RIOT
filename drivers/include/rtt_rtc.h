/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @defgroup    drivers_rtt_rtc     RTC emulation on top of a RTT
 * @ingroup     drivers_periph_rtc
 *
 * @{
 *
 * @file
 * @brief       Additional functions provided in addition to the normal RTC API.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the time as epoch (elapsed seconds since @ref RIOT_EPOCH)
 *        with sub-second precision
 *        This feature is an extension provided by the `rtt_rtc` module.
 *
 * @note The actual µs precision depends on the underlying hardware.
 *       The smallest time step will be 1 / @ref RTT_FREQUENCY.
 *
 * @param[in] s     The new epoch timestamp
 * @param[in] us    Sub-Seconds
 */
void rtt_rtc_settimeofday(uint32_t s, uint32_t us);

/**
 * @brief Get the current epoch (elapsed seconds since @ref RIOT_EPOCH)
 *        with sub-second precision
 *        This feature is an extension provided by the `rtt_rtc` module.
 *
 * @note The actual µs precision depends on the underlying hardware.
 *       The smallest time step will be 1 / @ref RTT_FREQUENCY.
 *
 * @param[out] s    The current epoch timestamp
 * @param[out] us   Sub-Seconds
 */
void rtt_rtc_gettimeofday(uint32_t *s, uint32_t *us);

#ifdef __cplusplus
}
#endif

/** @} */
