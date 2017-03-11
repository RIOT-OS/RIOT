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
#include "saul.h"
#include "saul_reg.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the IO1 Xplained extension
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
 * @brief   Get the number of configured IO1 Xplained extension
 */
#define IO1_XPLAINED_NUMOF   (sizeof(io1_xplained_params) / sizeof(io1_xplained_params[0]))

/**
 * @brief   Reference the gpio driver struct
 */
extern saul_driver_t gpio_saul_driver;

#ifdef MODULE_SAUL_REG
/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
saul_reg_t io1_xplained_saul_reg[][4] =
{
    {
        {
            .name = "Temperature (IO1 Xplained)",
            .driver = &io1_xplained_temperature_saul_driver
        },
        {
            .name = "LED (IO1 Xplained)",
            .driver = &gpio_saul_driver
        },
        {
            .name = "GPIO1 (IO1 Xplained)",
            .driver = &gpio_saul_driver
        },
        {
            .name = "GPIO2 (IO1 Xplained)",
            .driver = &gpio_saul_driver
        },
    }
};
#endif

#ifdef MODULE_SAUL_GPIO
/**
 * @brief   Allocate and configure the extension LED gpios
 */
static gpio_t io1_xplained_saul_gpios[3] =
{
    IO1_LED_PIN,
    IO1_GPIO1_PIN,
    IO1_GPIO2_PIN,
};
#endif


#ifdef __cplusplus
}
#endif

#endif /* IO1_XPLAINED_PARAMS_H */
/** @} */
