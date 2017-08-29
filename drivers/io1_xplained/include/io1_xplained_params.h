/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_io1_xplained
 *
 * @{
 * @file
 * @brief       Default configuration for IO1 Xplained
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef IO1_XPLAINED_PARAMS_H
#define IO1_XPLAINED_PARAMS_H

#include "board.h"
#include "io1_xplained.h"
#include "io1_xplained_internals.h"
#include "saul_reg.h"
#include "saul/periph.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the IO1 Xplained extension
 * @{
 */
#ifndef IO1_XPLAINED_PARAM_ADDR
#define IO1_XPLAINED_PARAM_ADDR       (0x07)
#endif

#define IO1_XPLAINED_PARAMS_DEFAULT   { .addr = IO1_XPLAINED_PARAM_ADDR }
/**@}*/

/**
 * @brief   Configure IO1 Xplained extension
 */
static const io1_xplained_params_t io1_xplained_params[] =
{
#ifdef IO1_XPLAINED_PARAMS_BOARD
    IO1_XPLAINED_PARAMS_BOARD,
#else
    IO1_XPLAINED_PARAMS_DEFAULT,
#endif
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
saul_reg_info_t io1_xplained_saul_reg_info[][4] =
{
    {
        { .name = "Temperature (IO1 Xplained)" },
        { .name = "LED (IO1 Xplained)" },
        { .name = "GPIO1 (IO1 Xplained)" },
        { .name = "GPIO2 (IO1 Xplained)" }
    }
};

/**
 * @brief   Allocate and configure the extension LED gpios
 */
gpio_t io1_xplained_saul_gpios[3] =
{
    IO1_LED_PIN,
    IO1_GPIO1_PIN,
    IO1_GPIO2_PIN,
};


#ifdef __cplusplus
}
#endif

#endif /* IO1_XPLAINED_PARAMS_H */
/** @} */
