/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hd44780
 *
 * @{
 * @file
 * @brief       Pinout config for the HD44780 display
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */
#ifndef HD44780_PARAMS_H
#define HD44780_PARAMS_H

#include "board.h"
#include "periph/gpio.h"

#include "hd44780.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define HD44780_PARAMS_ARDUINO {    \
    .cols   = 16,                   \
    .rows   = 2,                    \
    .rs     = ARDUINO_PIN_2,        \
    .rw     = HD44780_RW_OFF,       \
    .enable = ARDUINO_PIN_3,        \
    .data   = {ARDUINO_PIN_4, ARDUINO_PIN_5, ARDUINO_PIN_6, ARDUINO_PIN_7,     \
               HD44780_RW_OFF, HD44780_RW_OFF, HD44780_RW_OFF, HD44780_RW_OFF} \
}

/**
 * @brief   LCM1602C configuration
 */
static const hd44780_params_t hd44780_params[] =
{
    HD44780_PARAMS_ARDUINO,
};

#ifdef __cplusplus
}
#endif

#endif /* HD44780_PARAMS_H */
/** @} */
