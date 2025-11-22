/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_rn2xx3
 * @{
 *
 * @file
 * @brief       Default configuration for RN2483/RN2903 devices
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "board.h"
#include "rn2xx3.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the RN2483/RN2903 driver
 * @{
 */
#ifndef RN2XX3_PARAM_UART
#define RN2XX3_PARAM_UART         UART_DEV(1)
#endif
#ifndef RN2XX3_PARAM_BAUDRATE
#define RN2XX3_PARAM_BAUDRATE     (57600U)
#endif
#ifndef RN2XX3_PARAM_PIN_RESET
#define RN2XX3_PARAM_PIN_RESET    (GPIO_UNDEF)
#endif

#ifndef RN2XX3_PARAMS
#define RN2XX3_PARAMS             { .uart      = RN2XX3_PARAM_UART,     \
                                    .baudrate  = RN2XX3_PARAM_BAUDRATE, \
                                    .pin_reset = RN2XX3_PARAM_PIN_RESET }
#endif
/**@}*/

/**
 * @brief   RN2483/RN2903 configuration
 */
static const rn2xx3_params_t rn2xx3_params[] =
{
    RN2XX3_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
