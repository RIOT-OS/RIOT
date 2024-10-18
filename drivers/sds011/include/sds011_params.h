/*
 * Copyright (C) 2018 HAW-Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sds011
 * @{
 *
 * @file
 * @brief       SDS011 sensor specific configuration
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */

#ifndef SDS011_PARAMS_H
#define SDS011_PARAMS_H

#include "board.h"
#include "periph/uart.h"
#include "saul_reg.h"
#include "sds011.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Set default configuration parameters for the SDS011 driver
 * @{
 */
#ifndef SDS011_PARAM_UART_DEV
#define SDS011_PARAM_UART_DEV       (UART_DEV(1))
#endif
#ifndef SDS011_PARAM_PWR_PIN
#define SDS011_PARAM_PWR_PIN        (GPIO_PIN(0, 0))
#endif
#ifndef SDS011_PARAM_PWR_PIN_AH
#define SDS011_PARAM_PWR_PIN_AH     (true)
#endif

#ifndef SDS011_PARAMS
#define SDS011_PARAMS               { .uart    = SDS011_PARAM_UART_DEV, \
                                      .pwr_pin = SDS011_PARAM_PWR_PIN, \
                                      .pwr_ah  = SDS011_PARAM_PWR_PIN_AH, \
                                      .dev_id  = SDS011_DEVID_WILDCARD }
#endif

#ifndef SDS011_SAUL_INFO
#define SDS011_SAUL_INFO            { .name = "SDS011" }
#endif
/** @} */

/**
 * @brief   SDS011 configuration
 */
static const  sds011_params_t sds011_params[] =
{
    SDS011_PARAMS
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
saul_reg_info_t sds011_saul_info[] =
{
    SDS011_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SDS011_PARAMS_H */
