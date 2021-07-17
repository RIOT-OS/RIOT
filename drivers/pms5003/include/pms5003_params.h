/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pms5003
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef PMS5003_PARAMS_H
#define PMS5003_PARAMS_H

#include "board.h"
#include "pms5003.h"

#include "saul_reg.h"
#include "periph/gpio.h"
#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef PMS5003_PARAM_UART_DEV
#define PMS5003_PARAM_UART_DEV          (UART_DEV(0))
#endif
#ifndef PMS5003_PARAM_RESET_PIN_
#define PMS5003_PARAM_RESET_PIN         (GPIO_UNDEF)
#endif
/* SET in the datasheet */
#ifndef PMS5003_PARAM_ENABLE_PIN
#define PMS5003_PARAM_ENABLE_PIN        (GPIO_UNDEF)
#endif
#ifndef PMS5003_SAUL_INFO
#define PMS5003_SAUL_INFO               { .name = "pms5003" }
#endif
#ifndef PMS5003_PARAMS
#define PMS5003_PARAMS         { .uart = PMS5003_PARAM_UART_DEV, \
                                 .reset_pin = PMS5003_PARAM_RESET_PIN, \
                                 .enable_pin = PMS5003_PARAM_ENABLE_PIN }
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const pms5003_params_t pms5003_params[] =
{
    PMS5003_PARAMS
};

/**
 * @brief   Define the number of configured sensors
 */
#define PMS5003_NUMOF     ARRAY_SIZE(pms5003_params)

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t pms5003_saul_info[] =
{
    PMS5003_SAUL_INFO
};

/**
 * @brief   Number of saul info structs
 */
#define PMS5003_INFO_NUM ARRAY_SIZE(pms5003_saul_info)

#ifdef __cplusplus
}
#endif

#endif /* PMS5003_PARAMS_H */
/** @} */
