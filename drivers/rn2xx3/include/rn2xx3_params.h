/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_rn2xx3
 * @{
 *
 * @file
 * @brief       Default configuration for RN2483/RN2903 devices
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef RN2XX3_PARAMS_H
#define RN2XX3_PARAMS_H

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

#endif /* RN2XX3_PARAMS_H */
/** @} */
