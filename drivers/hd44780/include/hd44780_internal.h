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
 * @brief       Internal config and parameters for the HD44780 display
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    HD44780 LCD commands
 * @{
 */
#define HD44780_CLEARDISPLAY            (0x01)
#define HD44780_RETURNHOME              (0x02)
#define HD44780_ENTRYMODESET            (0x04)
#define HD44780_DISPLAYCONTROL          (0x08)
#define HD44780_CURSORSHIFT             (0x10)
#define HD44780_FUNCTIONSET             (0x20)
#define HD44780_SETCGRAMADDR            (0x40)
#define HD44780_SETDDRAMADDR            (0x80)
/** @} */

/**
 * @name    HD44780 LCD entry modes flags
 * @{
 */
#define HD44780_ENTRYRIGHT              (0x00)
#define HD44780_ENTRYLEFT               (0x02)
#define HD44780_ENTRYSHIFTINCREMENT     (0x01)
#define HD44780_ENTRYSHIFTDECREMENT     (0x00)
/** @} */

/**
 * @name    HD44780 LCD control flags
 * @{
 */
#define HD44780_DISPLAYON               (0x04)
#define HD44780_DISPLAYOFF              (0x00)
#define HD44780_CURSORON                (0x02)
#define HD44780_CURSOROFF               (0x00)
#define HD44780_BLINKON                 (0x01)
#define HD44780_BLINKOFF                (0x00)
/** @} */

/**
 * @name    HD44780 display and cursor shift flags
 * @{
 */
#define HD44780_DISPLAYMOVE             (0x08)
#define HD44780_CURSORMOVE              (0x00)
#define HD44780_MOVERIGHT               (0x04)
#define HD44780_MOVELEFT                (0x00)
/**@}*/

/**
 * @name    HD44780 LCD functional flags
 * @{
 */
#define HD44780_8BITMODE                (0x10)
#define HD44780_4BITMODE                (0x00)
#define HD44780_2LINE                   (0x08)
#define HD44780_1LINE                   (0x00)
#define HD44780_5x10DOTS                (0x04)
#define HD44780_5x8DOTS                 (0x00)
/** @} */

/**
 * @name    HD44780 LCD timings
 * @{
 */
#define HD44780_CMD_WAIT                (2000U)
#define HD44780_INIT_WAIT_XXL           (50000U)
#define HD44780_INIT_WAIT_LONG          (4500U)
#define HD44780_INIT_WAIT_SHORT         (150U)
#define HD44780_PULSE_WAIT_SHORT        (1U)
#define HD44780_PULSE_WAIT_LONG         (100U)
/**@}*/

#ifdef __cplusplus
}
#endif

/** @} */
