/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_xbee
 * @{
 *
 * @file
 * @brief       Default configuration for XBee devices
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the XBee driver
 * @{
 */
#ifndef XBEE_PARAM_UART
#define XBEE_PARAM_UART         (UART_DEV(1))
#endif
#ifndef XBEE_PARAM_BR
#define XBEE_PARAM_BR           (9600U)
#endif
#ifndef XBEE_PARAM_PIN_SLEEP
#define XBEE_PARAM_PIN_SLEEP    (GPIO_UNDEF)
#endif
#ifndef XBEE_PARAM_PIN_RESET
#define XBEE_PARAM_PIN_RESET    (GPIO_UNDEF)
#endif

#ifndef XBEE_PARAMS
#define XBEE_PARAMS             { .uart = XBEE_PARAM_UART, \
                                  .br = XBEE_PARAM_BR, \
                                  .pin_sleep = XBEE_PARAM_PIN_SLEEP, \
                                  .pin_reset = XBEE_PARAM_PIN_RESET }
#endif
/**@}*/

/**
 * @brief   XBee configuration
 */
static const xbee_params_t xbee_params[] =
{
    XBEE_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
