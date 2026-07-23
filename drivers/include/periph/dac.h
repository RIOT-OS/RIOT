/*
 * Copyright (C) 2014 Simon Brummer
 *               2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_periph_dac DAC
 * @ingroup     drivers_periph
 * @brief       DAC peripheral driver interface
 *
 * Similar to the ADC driver interface (@ref drivers_periph_adc), the DAC
 * interface uses the concept of lines, corresponds to a tuple of a DAC device
 * and a DAC output channel.
 *
 * The DAC interface expects data to be served as a 16-bit unsigned integer,
 * independent of the actual resolution of the hardware device. It is up to the
 * DAC driver, to scale the given value to the maximal width that can be
 * handled. The device driver should, on the other hand, implement the DAC in a
 * way, that it will use the bit width, that comes closest to 16-bit.
 *
 * This kind of 'auto-scaling' is quite sufficient for this interface, as
 * standard DAC peripherals use a fixed conversion resolution internally anyway,
 * so that any particular bit-width configuration on this driver level would not
 * have much effect...
 *
 * # (Low-) Power Implications
 *
 * The configured DAC peripherals are active (and consume power) from the point
 * of initialization. When calling dac_poweroff(), the implementation **should**
 * disable the given DAC line and put the DAC peripheral to sleep (e.g. through
 * peripheral clock gating). When woken up again through dac_poweron(), the
 * given DAC line **should** transparently continue it's previous operation.
 *
 * The DAC driver implementation may need to block (and free) certain power
 * modes in the driver's dac_init(), dac_poweron(), and the dac_poweroff()
 * functions.
 *
 * @{
 * @file
 * @brief       DAC peripheral driver interface definition
 *
 * @author      Simon Brummer <simon.brummer@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define default DAC type identifier
 */
#ifndef HAVE_DAC_T
typedef uint_fast8_t dac_t;
#endif

/**
 * @brief   Return codes used by the DAC driver interface
 */
enum {
    DAC_OK     = 0,
    DAC_NOLINE = -1
};

/**
 * @brief   Default DAC undefined value
 */
#ifndef DAC_UNDEF
#define DAC_UNDEF           (UINT_FAST8_MAX)
#endif

/**
 * @brief   Default DAC access macro
 */
#ifndef DAC_LINE
#define DAC_LINE(x)         (x)
#endif

/**
 * @brief   Initialize the given DAC line
 *
 * After initialization, the corresponding DAC line is active and its output is
 * set to 0.
 *
 * @param[in] line         DAC line to initialize
 *
 * @return  DAC_OK on success
 * @return  DAC_NOLINE on invalid DAC line
 */
int8_t dac_init(dac_t line);

/**
 * @brief   Write a value onto DAC Device on a given Channel
 *
 * The value is always given as 16-bit value and is internally scaled to the
 * actual resolution that the DAC unit provides (e.g. 12-bit). So to get the
 * maximum output voltage, this function has to be called with @p value set to
 * 65535 (UINT16_MAX).
 *
 * @param[in] line         DAC line to set
 * @param[in] value        value to set @p line to
 */
void dac_set(dac_t line, uint16_t value);

/**
 * @brief   Enable the given DAC line
 *
 * @param[in] line          DAC line to power on
 */
void dac_poweron(dac_t line);

/**
 * @brief   Disable the given DAC line
 *
 * @param[in] line          DAC line to power off
 */
void dac_poweroff(dac_t line);

/**
 * @brief   Get the sample rate of the DAC in Hz
 *
 * @return  Sampling Rate in Hz
 */
uint32_t dac_get_freq(void);

/**
 * @brief   Options for @ref dac_play
 * @{
 */
#define DAC_PLAY_LOOPED     (1 << 0)    /**< keep playing the sample buffer in a loop */
/** @} */

/**
 * @brief   Signature of event callback functions triggered from interrupts
 *
 * @param[in] arg       optional context for the callback
 */
typedef void (*dma_cb_t)(void *arg);

/**
 * @brief   Configure DAC line for playing sample buffer
 *
 * @param[in]   line        DAC line to configure
 * @param[in]   cb          Callback to be executed when the current sample buffer
 *                          is done. Will be executed in interrupt context.
 *                          May be NULL.
 * @param[in]   arg         Callback argument
 *
 * @retval      0           Success
 * @retval      -EEXIST     Line has already been configured
 * @retval      -ENOMEM     Can't allocate resources for playback
 */
int dac_play_setup(dac_t line, dma_cb_t cb, void *arg);

/**
 * @brief   Start playing sample buffer
 *
 * @param[in]   line        DAC line to user for playing
 * @param[in]   buf         Sample buffer to play
 * @param[in]   len         Number of samples to play
 * @param[in]   flags       Playback options
 */
void dac_play(dac_t line, const uint16_t *buf, size_t len, uint8_t flags);

/**
 * @brief   Release DAC line
 *
 * @param[in]   line        DAC line to release
 */
void dac_play_teardown(dac_t line);

#ifdef __cplusplus
}
#endif

/** @} */
