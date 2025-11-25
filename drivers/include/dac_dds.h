/*
 * Copyright (C) 2020 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_dac_dds DAC Direct Digital Synthesis
 * @ingroup     drivers_periph_dac
 *
 * @{
 * @file
 * @brief       Use a DAC to play a buffer of samples
 *
 *              A buffer of (audio) samples can be played on a DAC with
 *              a given sample rate.
 *              To supply a steady stream of samples, a double buffer is
 *              used.
 *              While buffer A is played on the DAC the user can fill buffer
 *              B with the next batch of samples by calling @ref dac_dds_play
 *              again. Once buffer A has finished playing, buffer B will
 *              automatically be used instead.
 *
 *              A callback can be registered that signals when the next buffer
 *              is ready to be filled.
 *              Do not do the actual buffer filling inside the callback as this
 *              is called from the timer context that is used to play the samples.
 *              Instead, use it to wake a thread that then provides the samples
 *              and calls @ref dac_dds_play.
 *              If the next sample buffer is already prepared, you can also call
 *              `dac_dds_play` within the callback, just don't do any I/O or
 *              heavy processing.
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 */

#ifndef DAC_DDS_H
#define DAC_DDS_H

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include "periph/dac.h"
#include "periph/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The callback that will be called when the end of the current sample buffer
 *          has been reached.
 *          Should be used to start filling the next sample buffer with @ref dac_dds_play.
 *
 * @note    Will be called in interrupt context. Only use the callback to signal a
 *          thread. Don't directly fill the sample buffer in the callback.
 */
typedef void (*dac_dds_cb_t)(void *arg);

/**
 * @brief   Configuration struct for a DAC DDS channel
 * @{
 */
typedef struct {
    dac_t dac;          /**< The DAC used for playing the sample              */
    tim_t timer;        /**< Timer used for periodic DAC events               */
    uint32_t timer_hz;  /**< Timer frequency, must be at least 2x sample rate */
} dac_dds_params_t;
/** @} */

/**
 * @brief   Index of the DAC DDS channel
 */
typedef uint8_t dac_dds_t;

/**
 * @brief   A sample has a resolution of 8 bit
 */
#ifndef DAC_FLAG_8BIT
#define DAC_FLAG_8BIT   (0x0)
#endif

/**
 * @brief   A sample has a resolution of 16 bit
 */
#ifndef DAC_FLAG_16BIT
#define DAC_FLAG_16BIT  (0x1)
#endif

/**
 * @brief   Initialize a DAC for playing audio samples
 *          A user defined callback can be provided that will be called when
 *          the next buffer can be queued.
 * @experimental
 *
 * @param[in] dac           The DAC to initialize
 * @param[in] sample_rate   The sample rate in Hz
 * @param[in] flags         Optional flags (@ref DAC_FLAG_16BIT)
 * @param[in] cb            Will be called when the next buffer can be queued
 * @param[in] cb_arg        Callback argument
 */
void dac_dds_init(dac_dds_t dac, uint16_t sample_rate, uint8_t flags,
                  dac_dds_cb_t cb, void *cb_arg);

/**
 * @brief   Change the 'buffer done' callback.
 *          A user defined callback can be provided that will be called when
 *          the next buffer can be queued.
 *          This function can be used to change the callback on the fly.
 *
 *          Passing in a @p cb of `NULL` can be used to only update the arg
 *          without updating the @p cb. Conversely, to clear the callback, both
 *          @p cb and @p cb_arg need to be passed in as NULL.
 * @experimental
 *
 * @param[in] dac           The DAC to configure
 * @param[in] cb            Called when the played buffer is done
 * @param[in] cb_arg        Callback argument
 */
void dac_dds_set_cb(dac_dds_t dac, dac_dds_cb_t cb, void *cb_arg);

/**
 * @brief   Play a buffer of (audio) samples on a DAC
 *
 *          If this function is called while another buffer is already
 *          being played, the new `buf` will be played when the current
 *          buffer has finished playing.
 *
 *          The DAC implementations allows one buffer to be queued
 *          (double buffering).
 *
 *          Whenever a new buffer can be queued, the @ref dac_dds_cb_t
 *          callback function will be executed.
 *
 * @experimental
 *
 * @param[in] dac           The DAC to play the sample on
 * @param[in] buf           A buffer with (audio) samples
 * @param[in] len           Number of bytes to be played
 *
 * @return                  `true` if the buffer was queued while another
 *                          buffer is currently playing.
 *                          `false` if the new buffer is played immediately.
 *                          That means playing was just started or an underrun
 *                          occurred.
 */
bool dac_dds_play(dac_dds_t dac, const void *buf, size_t len);

/**
 * @brief   Stop playback of the current sample buffer
 *
 * @param[in] dac           The DAC to stop
 */
void dac_dds_stop(dac_dds_t dac);

#ifdef __cplusplus
}
#endif

#endif /* DAC_DDS_H */
/** @} */
