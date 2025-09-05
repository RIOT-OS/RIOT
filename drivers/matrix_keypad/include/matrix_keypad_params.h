/*
 * SPDX-FileCopyrightText: 2021 Koen Zandberg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_matrix_keypad
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include "board.h"
#include "matrix_keypad.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */

/**
 * @brief Number of configured rows
 */
#ifndef MATRIX_KEYPAD_PARAM_ROWS
#define MATRIX_KEYPAD_PARAM_ROWS { GPIO_PIN(0, 0), GPIO_PIN(0, 1) }
#endif

/**
 * @brief Number of configured columns
 */
#ifndef MATRIX_KEYPAD_PARAM_COLUMNS
#define MATRIX_KEYPAD_PARAM_COLUMNS { GPIO_PIN(0, 2), GPIO_PIN(0, 3) }
#endif

/**
 * @brief Delay between switching the row state and reading the columns.
 */
#ifndef MATRIX_KEYPAD_PARAM_ROW2COL_DELAY
#define MATRIX_KEYPAD_PARAM_ROW2COL_DELAY 1
#endif

/**
 * @brief Default param configuration
 */
#ifndef MATRIX_KEYPAD_PARAMS
#define MATRIX_KEYPAD_PARAMS { \
    .rows           = MATRIX_KEYPAD_PARAM_ROWS, \
    .columns        = MATRIX_KEYPAD_PARAM_COLUMNS, \
    .row2col_delay  = MATRIX_KEYPAD_PARAM_ROW2COL_DELAY, \
}
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const matrix_keypad_params_t matrix_keypad_params[] =
{
    MATRIX_KEYPAD_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
