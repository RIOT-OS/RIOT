/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_hd44780
 *
 * @{
 * @file
 * @brief       Pinout config for the HD44780 display
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "board.h"
#ifdef MODULE_PCF857X
#include "pcf857x.h"
#else
#include "periph/gpio.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* The default pins corresponds to Arduino pins on the Arduino Zero board. */

#ifndef HD44780_PARAM_COLS
#define HD44780_PARAM_COLS              (16U)
#endif
#ifndef HD44780_PARAM_ROWS
#define HD44780_PARAM_ROWS              (2U)
#endif

#ifndef MODULE_PCF857X

#ifndef HD44780_PARAM_PIN_RS
#define HD44780_PARAM_PIN_RS            GPIO_PIN(0, 14)         /* Arduino D2 */
#endif
#ifndef HD44780_PARAM_PIN_RW
#define HD44780_PARAM_PIN_RW            GPIO_UNDEF
#endif
#ifndef HD44780_PARAM_PIN_ENABLE
#define HD44780_PARAM_PIN_ENABLE        GPIO_PIN(0, 9)          /* Arduino D3 */
#endif
#ifndef HD44780_PARAM_PINS_DATA
#define HD44780_PARAM_PINS_DATA         {   GPIO_PIN(0, 8),     /* Arduino D4 */  \
                                            GPIO_PIN(0, 15),    /* Arduino D5 */  \
                                            GPIO_PIN(0, 20),    /* Arduino D6 */  \
                                            GPIO_PIN(0, 21),    /* Arduino D7 */  \
                                            GPIO_UNDEF,                           \
                                            GPIO_UNDEF,                           \
                                            GPIO_UNDEF,                           \
                                            GPIO_UNDEF }
#endif

#else /* !MODULE_PCF857X */

#ifndef HD44780_PARAM_PIN_RS
#define HD44780_PARAM_PIN_RS            PCF857X_GPIO_PIN(0, 0)      /* Bit 0 */
#endif
#ifndef HD44780_PARAM_PIN_RW
#define HD44780_PARAM_PIN_RW            PCF857X_GPIO_PIN(0, 1)      /* Bit 1 */
#endif
#ifndef HD44780_PARAM_PIN_ENABLE
#define HD44780_PARAM_PIN_ENABLE        PCF857X_GPIO_PIN(0, 2)      /* Bit 2 */
#endif
#ifndef HD44780_PARAM_PINS_DATA
#define HD44780_PARAM_PINS_DATA         {   PCF857X_GPIO_PIN(0, 4), /* Bit 4 */  \
                                            PCF857X_GPIO_PIN(0, 5), /* Bit 5 */  \
                                            PCF857X_GPIO_PIN(0, 6), /* Bit 6 */  \
                                            PCF857X_GPIO_PIN(0, 7), /* Bit 7 */  \
                                            GPIO_UNDEF,                          \
                                            GPIO_UNDEF,                          \
                                            GPIO_UNDEF,                          \
                                            GPIO_UNDEF }
#endif

#endif /* !MODULE_PCF857X */

#ifndef HD44780_PARAMS
#define HD44780_PARAMS               {  .cols   = HD44780_PARAM_COLS,       \
                                        .rows   = HD44780_PARAM_ROWS,       \
                                        .rs     = HD44780_PARAM_PIN_RS,     \
                                        .rw     = HD44780_PARAM_PIN_RW,     \
                                        .enable = HD44780_PARAM_PIN_ENABLE, \
                                        .data   = HD44780_PARAM_PINS_DATA }
#endif

/**
 * @brief   LCM1602C configuration
 */
static const hd44780_params_t hd44780_params[] =
{
    HD44780_PARAMS,
};

#ifdef __cplusplus
}
#endif

/** @} */
