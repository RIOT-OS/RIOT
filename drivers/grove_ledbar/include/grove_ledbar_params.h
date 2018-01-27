/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_grove_ledbar
 *
 * @{
 * @file
 * @brief       Config for the Grove LED bar based on MY9221 LED controller
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */
#ifndef GROVE_LEDBAR_PARAMS_H
#define GROVE_LEDBAR_PARAMS_H

#include "board.h"
#include "periph/gpio.h"
#include "saul_reg.h"

#include "grove_ledbar.h"
#include "my9221.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Clock GPIO pin
 */
#ifndef GROVE_LEDBAR_CLK
#define GROVE_LEDBAR_CLK        (GPIO_PIN(0,1))
#endif

/**
 * @brief   Data GPIO pin
 */
#ifndef GROVE_LEDBAR_DAT
#define GROVE_LEDBAR_DAT        (GPIO_PIN(0,2))
#endif

/**
 * @brief   Direction of LEDs
 */
#ifndef GROVE_LEDBAR_DIR
#define GROVE_LEDBAR_DIR        GROVE_LEDBAR_G2R
#endif

/**
 * @brief   Default parameter settings
 */
#define GROVE_LEDBAR_PARAMS {       \
    .leds       = 10,               \
    .dir        = GROVE_LEDBAR_DIR, \
    .clk        = GROVE_LEDBAR_CLK, \
    .dat        = GROVE_LEDBAR_DAT, \
}

/**
 * @brief   Grove LED bar configuration
 */
static const grove_ledbar_params_t grove_ledbar_params[] =
{
#ifdef GROVE_LEDBAR_CUSTOM
    GROVE_LEDBAR_CUSTOM,
#else
    GROVE_LEDBAR_PARAMS,
#endif
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t grove_ledbar_saul_info[] =
{
    {
        .name = "Grove LED bar"
    }
};

#ifdef __cplusplus
}
#endif

#endif /* GROVE_LEDBAR_PARAMS_H */
/** @} */
