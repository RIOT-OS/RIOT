/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-FileCopyrightText: 2024-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_periph_pdm Pulse Density Modulation (PDM)
 * @ingroup     drivers_periph
 * @brief       Low-level Pulse Density Modulation (PDM) driver
 *
 * @{
 * @file
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Ben Wehrberger <ben.wehrberger@haw-hamburg.de>
 *
 */

#include <stdint.h>

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   PDM operation modes
 * @{
 */
#ifndef HAVE_PDM_MODE_T
typedef enum {
    PDM_MODE_MONO = 0,      /**< Mono mode */
    PDM_MODE_STEREO,        /**< Stereo mode */
} pdm_mode_t;
#endif
/** @} */

/**
 * @brief   Minimum PDM gain (in dB)
 */
#ifndef PDM_GAIN_MIN
#  define PDM_GAIN_MIN        (-20)
#endif

/**
 * @brief   Maximum PDM gain (in dB)
 */
#ifndef PDM_GAIN_MAX
#  define PDM_GAIN_MAX        (20)
#endif

/**
 * @brief   Default PDM sample buffer size
 */
#ifndef PDM_BUF_SIZE
#  define PDM_BUF_SIZE        (64U)
#endif

/**
 * @brief   Startup backoff in ms during which samples are discarded while the
 *          peripheral settles
 *
 * @note    Depending on the hardware a microphone settling/startup artifact
 *          may appear that is way longer (up to several 100 ms). Its duration
 *          and shape depends on the config. Increase PDM_BACKOFF_MS accordingly
 *          if the application needs clean samples from the start.
 */
#ifndef PDM_BACKOFF_MS
#  define PDM_BACKOFF_MS      (5U)
#endif

/**
 * @brief   Signature for data received interrupt callback
 *
 * @param[in] arg       context passed to the callback (optional)
 * @param[in] buf       the buffer containing the current sample frame
 */
typedef void (*pdm_data_cb_t)(void *arg, int16_t *buf);

/**
 * @brief   Interrupt context for a PDM device
 */
#ifndef HAVE_PDM_ISR_CTX_T
typedef struct {
    pdm_data_cb_t cb;       /**< data received interrupt callback */
    void *arg;              /**< context passed to the callback */
} pdm_isr_ctx_t;
#endif

/**
 * @brief   Initialize the PDM peripheral
 *
 * The requested rate is approximated to the closest rate the hardware can
 * produce. The actually configured rate is returned so the caller can use it
 * for further processing.
 *
 * @param[in] mode      mode (Mono or Stereo)
 * @param[in] rate      sample rate in Hz
 * @param[in] gain      gain in dB [PDM_GAIN_MIN...PDM_GAIN_MAX]
 * @param[in] cb        data received callback function
 * @param[in] arg       context passed to the callback function
 *
 * @retval  >0   actual configured PDM sample rate in Hz on success
 * @retval  <0   on error
 */
int32_t pdm_init(pdm_mode_t mode, uint32_t rate, int8_t gain,
                 pdm_data_cb_t cb, void *arg);

/**
 * @brief   Start the PDM peripheral
 */
void pdm_start(void);

/**
 * @brief   Stop the PDM peripheral
 */
void pdm_stop(void);

#ifdef __cplusplus
}
#endif

/** @} */
