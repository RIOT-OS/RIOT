/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mkrwan
 *
 * @{
 * @file
 * @brief       Default configuration for MKRWAN
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef MKRWAN_PARAMS_H
#define MKRWAN_PARAMS_H

#include "board.h"
#include "mkrwan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the MKRWAN
 * @{
 */
#ifndef MKRWAN_PARAM_UART_DEV
#define MKRWAN_PARAM_UART_DEV           UART_DEV(1)
#endif
#ifndef MKRWAN_PARAM_UART_BAUDRATE
#define MKRWAN_PARAM_UART_BAUDRATE      (19200U)
#endif
#ifndef MKRWAN_PARAM_RESET_PIN
#define MKRWAN_PARAM_RESET_PIN          GPIO_PIN(0,27) /* PA 27 */
#endif
#ifndef MKRWAN_PARAM_BOOT0_PIN
#define MKRWAN_PARAM_BOOT0_PIN          GPIO_PIN(1,9) /* PB 9 */
#endif

#ifndef MKRWAN_PARAMS
#define MKRWAN_PARAMS                   { .uart      = MKRWAN_PARAM_UART_DEV,      \
                                          .baudrate  = MKRWAN_PARAM_UART_BAUDRATE, \
                                          .reset_pin = MKRWAN_PARAM_RESET_PIN,     \
                                          .boot0_pin = MKRWAN_PARAM_BOOT0_PIN }
#endif
/**@}*/

/**
 * @brief   Configure MKRWAN
 */
static const mkrwan_params_t mkrwan_params[] =
{
    MKRWAN_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* MKRWAN_PARAMS_H */
/** @} */
