/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_pdm Pulse Density Modulation (PDM) driver
 * @ingroup     drivers_periph
 * @brief       Low-level Pulse Density Modulation (PDM) driver
 *
 * @{
 * @file
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#ifndef PERIPH_PDM_H
#define PERIPH_PDM_H

#include <stdint.h>

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default PDM mode values
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
 * @brief   Default PDM sampling rate values
 * @{
 */
#ifndef HAVE_PDM_SAMPLE_RATE_T
typedef enum {
    PDM_SAMPLE_RATE_16KHZ = 0,      /**< 16kHz */
    PDM_SAMPLE_RATE_20KHZ,          /**< 20kHz */
    PDM_SAMPLE_RATE_41KHZ,          /**< 41.6kHz */
    PDM_SAMPLE_RATE_50KHZ,          /**< 50kHz */
    PDM_SAMPLE_RATE_60KHZ,          /**< 60kHz */
} pdm_sample_rate_t;
#endif
/** @} */

/**
 * @brief   Default PDM min gain values (in dB)
 */
#ifndef PDM_GAIN_MIN
#define PDM_GAIN_MIN        (-20)
#endif

/**
 * @brief   Default PDM max gain values (in dB)
 */
#ifndef PDM_GAIN_MAX
#define PDM_GAIN_MAX        (20)
#endif

/**
 * @brief   Default PDM samples frame buffer size
 */
#ifndef PDM_BUF_SIZE
#define PDM_BUF_SIZE        (128U)
#endif

// Define the new buffer size
#define NEW_BUF_SIZE    (PDM_BUF_SIZE * 5 * 16 * 16)

/**
 * @brief   Signature for data received interrupt callback
 *
 * @param[in] arg           context to the callback (optional)
 * @param[in] buf           the buffer containing the current samples frame
 */
typedef void(*pdm_data_cb_t)(void *arg, int16_t *buf);

/**
 * @brief   Interrupt context for a PDM device
 */
#ifndef HAVE_PDM_ISR_CTX_T
typedef struct {
    pdm_data_cb_t cb;       /**< data received interrupt callback */
    void *arg;              /**< argument to both callback routines */
} pdm_isr_ctx_t;
#endif

/**
 * @brief   Initialize the PDM peripheral
 * @param[in] mode      mode (Mono or Stereo)
 * @param[in] rate      sample rate
 * @param[in] gain      gain
 * @param[in] cb        data received callback function
 * @param[in] arg       context passed to the callback function
 *
 * @return  0 on successful initialization
 * @return <0 on error
 */
int pdm_init(pdm_mode_t mode, pdm_sample_rate_t rate, int8_t gain, pdm_data_cb_t cb, void *arg);

/**
 * @brief   Start the PDM peripheral
 */
void pdm_start(void);

/**
 * @brief   Stop the PDM peripheral
 */
void pdm_stop(void);

#ifdef __cplusplus

#endif

#endif /* PERIPH_PDM_H */
/** @} */
