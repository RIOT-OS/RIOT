/*
 * Copyright (C) 2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_slwstk6000b
 * @{
 *
 * @file
 * @brief       Specific definitions for SLWRB4162A module.
 *
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef BOARD_MODULE_H
#define BOARD_MODULE_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Pins on the SLWRB4162A module.
 *
 * @note The pin numbers refer to the board module, not the base board.
 *
 * @{
 */
#define MODULE_PIN_F0       GPIO_PIN(PF, 1)
#define MODULE_PIN_F1       GPIO_PIN(PF, 0)
#define MODULE_PIN_F2       GPIO_PIN(PF, 2)
#define MODULE_PIN_F3       GPIO_PIN(PF, 3)
#define MODULE_PIN_F4       GPIO_UNDEF
#define MODULE_PIN_F5       GPIO_PIN(PA, 5)
#define MODULE_PIN_F6       GPIO_PIN(PA, 0)
#define MODULE_PIN_F7       GPIO_PIN(PA, 1)
#define MODULE_PIN_F8       GPIO_PIN(PA, 2)
#define MODULE_PIN_F9       GPIO_PIN(PA, 3)
#define MODULE_PIN_F10      GPIO_PIN(PF, 4)
#define MODULE_PIN_F11      GPIO_PIN(PF, 5)
#define MODULE_PIN_F12      GPIO_PIN(PF, 6)
#define MODULE_PIN_F13      GPIO_PIN(PF, 7)
#define MODULE_PIN_F14      GPIO_PIN(PD, 15)
#define MODULE_PIN_F15      GPIO_PIN(PC, 8)
#define MODULE_PIN_F16      GPIO_PIN(PC, 6)
#define MODULE_PIN_F17      GPIO_PIN(PD, 14)
#define MODULE_PIN_F18      GPIO_PIN(PD, 13)
#define MODULE_PIN_F19      GPIO_PIN(PB, 12)
#define MODULE_PIN_F20      GPIO_PIN(PB, 12)
#define MODULE_PIN_F21      GPIO_PIN(PB, 11)
#define MODULE_PIN_P0       GPIO_PIN(PD, 8)
#define MODULE_PIN_P1       GPIO_PIN(PA, 6)
#define MODULE_PIN_P2       GPIO_PIN(PD, 9)
#define MODULE_PIN_P3       GPIO_PIN(PA, 7)
#define MODULE_PIN_P4       GPIO_PIN(PD, 10)
#define MODULE_PIN_P5       GPIO_PIN(PA, 8)
#define MODULE_PIN_P6       GPIO_PIN(PD, 11)
#define MODULE_PIN_P7       GPIO_PIN(PA, 9)
#define MODULE_PIN_P8       GPIO_PIN(PD, 12)
#define MODULE_PIN_P9       GPIO_PIN(PB, 6)
#define MODULE_PIN_P10      GPIO_PIN(PC, 9)
#define MODULE_PIN_P11      GPIO_PIN(PB, 7)
#define MODULE_PIN_P12      GPIO_PIN(PC, 10)
#define MODULE_PIN_P13      GPIO_PIN(PC, 11)
#define MODULE_PIN_P14      GPIO_PIN(PB, 8)
#define MODULE_PIN_P15      GPIO_PIN(PB, 9)
#define MODULE_PIN_P16      GPIO_PIN(PC, 4)
#define MODULE_PIN_P17      GPIO_PIN(PC, 5)
#define MODULE_PIN_P18      GPIO_UNDEF
#define MODULE_PIN_P19      GPIO_UNDEF
#define MODULE_PIN_P20      GPIO_PIN(PF, 12)
#define MODULE_PIN_P21      GPIO_PIN(PF, 14)
#define MODULE_PIN_P22      GPIO_PIN(PF, 15)
#define MODULE_PIN_P23      GPIO_PIN(PI, 0)
#define MODULE_PIN_P24      GPIO_PIN(PI, 1)
#define MODULE_PIN_P25      GPIO_PIN(PI, 2)
#define MODULE_PIN_P26      GPIO_PIN(PI, 3)
#define MODULE_PIN_P27      GPIO_PIN(PJ, 14)
#define MODULE_PIN_P28      GPIO_PIN(PJ, 15)
#define MODULE_PIN_P29      GPIO_PIN(PK, 0)
#define MODULE_PIN_P30      GPIO_PIN(PK, 1)
#define MODULE_PIN_P31      GPIO_PIN(PK, 2)
#define MODULE_PIN_P32      GPIO_UNDEF
#define MODULE_PIN_P33      GPIO_PIN(PA, 0)
#define MODULE_PIN_P34      GPIO_PIN(PA, 1)
#define MODULE_PIN_P35      GPIO_PIN(PA, 2)
#define MODULE_PIN_P36      GPIO_PIN(PA, 3)
#define MODULE_PIN_P37      GPIO_PIN(PB, 10)
#define MODULE_PIN_P38      GPIO_UNDEF
#define MODULE_PIN_P39      GPIO_UNDEF
#define MODULE_PIN_P40      GPIO_UNDEF
#define MODULE_PIN_P41      GPIO_PIN(PF, 8)
#define MODULE_PIN_P42      GPIO_PIN(PF, 9)
#define MODULE_PIN_P43      GPIO_PIN(PF, 10)
#define MODULE_PIN_P44      GPIO_PIN(PF, 11)
#define MODULE_PIN_P45      GPIO_PIN(PF, 12)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_MODULE_H */
