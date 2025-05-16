/*
 * Copyright (C) 2024 Nico Behrens <nifrabe@outlook.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_tm1637
 *
 * @{
 * @file
 * @brief       Config for the TM1637 display
 *
 * @author      Nico Behrens <nifrabe@outlook.de>
 */

#include "board.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TM1637_PARAM_CLK
/**
 * @brief see @ref tm1637_params_t
 */
#  define TM1637_PARAM_CLK GPIO_UNDEF
#endif

#ifndef TM1637_PARAM_DIO
/**
 * @brief see @ref tm1637_params_t
 */
#  define TM1637_PARAM_DIO GPIO_UNDEF
#endif

#ifndef TM1637_PARAMS
/**
 * @brief see @ref tm1637_params_t
 */
#  define TM1637_PARAMS { .clk = TM1637_PARAM_CLK, \
                          .dio = TM1637_PARAM_DIO }
#endif

/**
 * @brief see @ref tm1637_params_t
 */
static const tm1637_params_t tm1637_params[] = {
    TM1637_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
