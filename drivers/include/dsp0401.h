/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_dsp0401 DSP0401
 * @ingroup     drivers_display
 * @brief       Device driver interface for the DSP0401 alphanumeric display
 * @{
 *
 * @file
 * @brief       Device driver interface for the DSP0401 alphanumeric display
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <inttypes.h>
#include <stdint.h>
#include "periph/gpio.h"
#include "periph/pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Return codes
 */
enum {
    DSP0401_OK = 0,       /**< All ok */
    DSP0401_ERR_CLK_GPIO, /**< Something went wrong with CLK GPIO */
    DSP0401_ERR_SDI_GPIO, /**< Something went wrong with SDI GPIO */
    DSP0401_ERR_LAT_GPIO, /**< Something went wrong with LAT GPIO */
    DSP0401_ERR_PWM,      /**< Something went wrong with PWM */
};

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    gpio_t sdi;              /**< Data input pin */
    gpio_t clk;              /**< Clock pin */
    gpio_t lat;              /**< Latch pin */
    pwm_t pwm;               /**< PWM device */
    uint8_t pwm_channel;     /**< PWM device channel */
    uint8_t brightness;      /**< LED brightness */
    uint8_t module_count;    /**< Number of connected modules */
} dsp0401_params_t;

/**
 * @brief   Device descriptor for the DSP0401
 */
typedef struct {
    dsp0401_params_t params; /**< Device parameters */
} dsp0401_t;

/**
 * @brief   Initialize the given DSP0401
 *
 * @param[out] dev          Initialized device descriptor of DSP0401 device
 * @param[in]  params       Device parameters to use
 *
 * @return                  DSP0401_OK if everything is good
 * @return                  -DSP0401_ERR_CLK_GPIO if an error occurred during CLK GPIO initialization
 * @return                  -DSP0401_ERR_SDI_GPIO if an error occurred during SDI GPIO initialization
 * @return                  -DSP0401_ERR_LAT_GPIO if an error occurred during LAT GPIO initialization
 * @return                  -DSP0401_ERR_PWM if an error occurred during PWM initialization
 */
int dsp0401_init(dsp0401_t *dev, const dsp0401_params_t *params);

/**
 * @brief   Display the given text on the DSP0401
 *
 * @param[in] dev           Device descriptor of the DSP0401 device
 * @param[in] text          The text to display
 */
void dsp0401_display_text(const dsp0401_t *dev, char *text);

/**
 * @brief   Clear the text displayed on the DSP0401
 *
 * @param[in] dev           Device descriptor of the DSP0401 device
 */
void dsp0401_clear_text(const dsp0401_t *dev);

/**
 * @brief   Scroll the given text on the DSP0401
 *
 * @param[in] dev           Device descriptor of the DSP0401 device
 * @param[in] text          The text to scroll on the display
 * @param[in] delay         Delay in ms between each horizontal move
 */
void dsp0401_scroll_text(const dsp0401_t *dev, char *text, uint16_t delay);

#ifdef __cplusplus
}
#endif

/** @} */
