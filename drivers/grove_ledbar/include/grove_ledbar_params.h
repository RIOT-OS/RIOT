/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_grove_ledbar
 *
 * @{
 * @file
 * @brief       Config for the Grove LED bar based on MY9221 LED controller
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */

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
#define GROVE_LEDBAR_CLK        (GPIO_PIN(0, 1))
#endif

/**
 * @brief   Data GPIO pin
 */
#ifndef GROVE_LEDBAR_DAT
#define GROVE_LEDBAR_DAT        (GPIO_PIN(0, 2))
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
#ifndef GROVE_LEDBAR_PARAMS
#define GROVE_LEDBAR_PARAMS     { .leds = 10,               \
                                  .dir  = GROVE_LEDBAR_DIR, \
                                  .clk  = GROVE_LEDBAR_CLK, \
                                  .dat  = GROVE_LEDBAR_DAT }
#endif

/**
 * @brief   SAUL info
 */
#define GROVE_LEDBAR_SAUL_INFO  { .name = "Grove LED bar" }

/**
 * @brief   Grove LED bar configuration
 */
static const grove_ledbar_params_t grove_ledbar_params[] =
{
    GROVE_LEDBAR_PARAMS,
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t grove_ledbar_saul_info[] =
{
    GROVE_LEDBAR_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
