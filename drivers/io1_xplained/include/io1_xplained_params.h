/*
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_io1_xplained
 *
 * @{
 * @file
 * @brief       Default configuration for IO1 Xplained
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

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
#define IO1_XPLAINED_PARAM_ADDR   (0x07)
#endif

#ifndef IO1_XPLAINED_PARAMS
#define IO1_XPLAINED_PARAMS       { .addr = IO1_XPLAINED_PARAM_ADDR }
#endif
#ifndef IO1_XPLAINED_SAUL_INFO
#define IO1_XPLAINED_SAUL_INFO    {{ .name = "Temperature (IO1 Xplained)" }, \
                                   { .name = "LED (IO1 Xplained)" },         \
                                   { .name = "GPIO1 (IO1 Xplained)" },       \
                                   { .name = "GPIO2 (IO1 Xplained)" }}
#endif
/**@}*/

/**
 * @brief   Configure IO1 Xplained extension
 */
static const io1_xplained_params_t io1_xplained_params[] =
{
    IO1_XPLAINED_PARAMS
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
saul_reg_info_t io1_xplained_saul_info[][4] =
{
    IO1_XPLAINED_SAUL_INFO
};

#ifdef MODULE_SAUL_GPIO
/**
 * @brief   Allocate and configure the extension LED gpios
 */
saul_gpio_params_t io1_xplained_saul_gpios[] =
{
    {
        .pin = IO1_LED_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .pin = IO1_GPIO1_PIN,
        .mode = GPIO_OUT,
    },
    {
        .pin = IO1_GPIO2_PIN,
        .mode = GPIO_OUT
    }
};
#endif

#ifdef __cplusplus
}
#endif

/** @} */
