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
#include "ws281x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration parameters for WS281x RGB LEDs
 * @{
 */
#ifndef WS281X_PARAM_PIN
#define WS281X_PARAM_PIN                (GPIO_PIN(0,0)) /**< GPIO pin connected to the data pin of the first LED */
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

#ifdef __cplusplus
}
#endif

#endif /* WS281X_PARAMS_H */
/** @} */
