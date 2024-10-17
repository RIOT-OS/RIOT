/*
 * Copyright (C) 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ws281x
 *
 * @{
 * @file
 * @brief       Default configuration for WS2812/SK6812 RGB LEDs
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef WS281X_PARAMS_H
#define WS281X_PARAMS_H

#include "board.h"
#include "saul_reg.h"

#include "ws281x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration parameters for WS281x RGB LEDs
 * @{
 */
#ifndef WS281X_PARAM_PIN
#define WS281X_PARAM_PIN                (GPIO_PIN(0, 0)) /**< GPIO pin connected to the data pin of the first LED */
#endif
#ifndef WS281X_PARAM_NUMOF
#define WS281X_PARAM_NUMOF              (8U)            /**< Number of LEDs chained */
#endif
#ifndef WS281X_PARAM_BUF
/**
 * @brief   Data buffer holding the LED states
 */
extern uint8_t ws281x_buf[WS281X_PARAM_NUMOF * WS281X_BYTES_PER_DEVICE];
#define WS281X_PARAM_BUF                (ws281x_buf)  /**< Data buffer holding LED states */
#endif

#ifndef WS281X_PARAMS
/**
 * @brief   WS281x initialization parameters
 */
#define WS281X_PARAMS                   { \
                                            .pin = WS281X_PARAM_PIN,  \
                                            .numof = WS281X_PARAM_NUMOF, \
                                            .buf = WS281X_PARAM_BUF, \
                                        }
#endif
/**@}*/

/**
 * @brief   Initialization parameters for WS281x devices
 */
static const ws281x_params_t ws281x_params[] =
{
    WS281X_PARAMS
};

/** @brief Timer used for WS281x (by the timer_gpio_ll implementation)
 *
 * A single timer is configured for any number of WS281x strands, so this does
 * not need to be part of params.
 *
 * It is required that the timer has at least 2 channels. (Future versions may
 * require a 3rd channel).
 *
 * It is required that the timer's MAX_VALUE is 2^n-1, which is a trivial but
 * not explicitly stated case.
 *
 * This timer is configured at WS281x initialization time, and kept stopped
 * outside of transmissions.
 *
 * The default value of 2 is chosen because the only platform on which the
 * module is usable is nRF5x, where TIMER_DEV(1) is in use by the radio module.
 * It is strongly advised to explicitly set this timer to a known free timer,
 * as the default may change without notice.
 * */
#if !defined(WS281X_TIMER_DEV) || defined(DOXYGEN)
#define WS281X_TIMER_DEV TIMER_DEV(2)
#endif

/** @brief Maximum value of the timer used for WS281x (by the timer_gpio_ll implementation)
 *
 * This macro needs to be defined to the `TIMER_x_MAX_VALUE` corresponding to
 * the `TIMER_DEV(x)` in @ref WS281X_TIMER_DEV.
 * */
#ifndef WS281X_TIMER_MAX_VALUE
#define WS281X_TIMER_MAX_VALUE TIMER_2_MAX_VALUE
#endif

/** @brief Frequency for the timer used for WS281x (by the timer_gpio_ll implementation)
 *
 * This should be set to a frequency that is a close multiple of 3MHz,
 * depending on the precise low and high times. A value of 16MHz works well.
 * */
#ifndef WS281X_TIMER_FREQ
#define WS281X_TIMER_FREQ 16000000
#endif

/**
 * @brief   SAUL info
 */
#ifndef WS281X_SAUL_INFO
#define WS281X_SAUL_INFO  { .name = "WS281X RGB LED" }
#endif

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t ws281x_saul_info[] =
{
    WS281X_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* WS281X_PARAMS_H */
