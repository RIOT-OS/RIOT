/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_freqm FREQM
 * @ingroup     drivers_periph
 * @brief       FREQM peripheral driver interface
 *
 * This interface allows to configure and use the Frequency Meter (FREQM)
 * peripheral.
 *
 * The Frequency Meter uses the frequency of a known reference clock to
 * determine the frequency of a signal connected via GPIO.
 *
 * @{
 *
 * @file
 * @brief       FREQM peripheral driver interface definitions
 *
 * @author      Urs Gompper <urs.gompper@ml-pa.com>
 */

#ifndef PERIPH_FREQM_H
#define PERIPH_FREQM_H

#include <errno.h>
#include <mutex.h>

#include "periph_cpu.h"
#include "periph/gpio.h"
#include "time_units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Frequency meter callback function.
 *        When a measurement is done the callbackfunction is called.
 *
 * @param result    measured frequency in hz
 * @param overflow  overflow in sticky counter
 * @param context   pointer to user defined context data
 */
typedef void (*freqm_cb_t)(uint32_t result, bool overflow, void *context);

/**
 * @brief Define default Frequency meter type identifier
 */
#ifndef HAVE_FREQM_T
typedef uint_fast8_t freqm_t;
#endif

/**
 * @brief   Initialize the frequency meter
 *
 * @param[in] idx           index of the configuration
 */
void freqm_init(freqm_t idx);

/**
 * @brief   Read number of periods of measured clock and calculate its frequency
 *
 * This function returns after triggering the measurement and calls
 * @p freqm_callback , with the calculated result and @p context , when the
 * measurement is done.
 *
 * @param[in]   idx             index of the configuration
 * @param[in]   freqm_cb        callback function when measurement is ready
 * @param[in]   context         context for the callback function
 * @param[in]   period_us       measurement duration in microseconds
 */
void freqm_frequency_get_async(freqm_t idx, freqm_cb_t freqm_cb, void *context,
                               uint32_t period_us);

/**
 * @brief   Read number of periods of measured clock and calculate its frequency
 *
 * This function uses a blocking mutex to wait for the measurement to finish.
 *
 * @param[in]   idx         index of the configuration
 * @param[out]  result      calculated frequency
 * @param[in]   period_us   measurement duration in microseconds
 *
 * @return      -EOVERFLOW if FREQM sticky counter has an overflow
 * @return      0 on success
 */
int freqm_frequency_get(freqm_t idx, uint32_t *result, uint32_t period_us);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_FREQM_H */
