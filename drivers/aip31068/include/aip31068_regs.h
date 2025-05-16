/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_aip31068
 * @brief       Register definitions for the AIP31068 I2C LCD controller
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 * @{
 */


#include "bitarithm.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Commands (page 18, table 3) */

/**
 * @brief Clears entire display and sets cursor to position (0, 0).
 *
 * @note: Also changes AIP31068_BIT_ENTRY_MODE_INCREMENT to 1
 */
#define AIP31068_CMD_CLEAR_DISPLAY          0x01

/**
 * @brief Sets cursor to position (0, 0) and resets display to original position
 *        before any shift operations.
 */
#define AIP31068_CMD_RETURN_HOME            0x02

/**
 * @brief Sets cursor move direction and specifies display shift.
 */
#define AIP31068_CMD_ENTRY_MODE_SET         0x04

/**
 * @brief Sets entire display on/off, cursor on/off, and blinking of cursor
 *        position character on/off.
 */
#define AIP31068_CMD_DISPLAY_CONTROL        0x08

/**
 * @brief Moves cursor and shifts display.
 */
#define AIP31068_CMD_CURSOR_DISPLAY_SHIFT   0x10

/**
 * @brief Sets interface data length, number of display lines and character
 *        font size.
 */
#define AIP31068_CMD_FUNCTION_SET           0x20

/**
 * @brief Sets CGRAM address.
 */
#define AIP31068_CMD_SET_CGRAM_ADDR         0x40

/**
 * @brief Sets DDRAM address.
 */
#define AIP31068_CMD_SET_DDRAM_ADDR         0x80

/* Bits for AIP31068_CMD_ENTRY_MODE_SET (page 16, section 3) */

/**
 * @brief 0 = Decrement cursor after insertion
 *        1 = Increment cursor after insertion
 */
#define AIP31068_BIT_ENTRY_MODE_INCREMENT       BIT1

/**
 * @brief 0 = No automated display scroll
 *        1 = Automated display scroll
 */
#define AIP31068_BIT_ENTRY_MODE_AUTOINCREMENT   BIT0

/* Bits for AIP31068_CMD_DISPLAY_CONTROL (page 16, section 4) */

/**
 * @brief 0 = Display off
 *        1 = Display on
 */
#define AIP31068_BIT_DISPLAY_CONTROL_DISPLAY            BIT2

/**
 * @brief 0 = Cursor off
 *        1 = Cursor on
 */
#define AIP31068_BIT_DISPLAY_CONTROL_CURSOR             BIT1

/**
 * @brief 0 = Cursor blinking off
 *        1 = Cursor blinking on
 */
#define AIP31068_BIT_DISPLAY_CONTROL_CURSOR_BLINKING    BIT0

/* Bits for AIP31068_CMD_CURSOR_DISPLAY_SHIFT (page 17, section 5) */

/**
 * @brief 0 = Shift the cursor position
 *        1 = Scroll the display content
 */
#define AIP31068_BIT_CURSOR_DISPLAY_SHIFT_SELECTION     BIT3

/**
 * @brief 0 = Shift to the left
 *        1 = Shift to the right
 */
#define AIP31068_BIT_CURSOR_DISPLAY_SHIFT_DIRECTION     BIT2

/* Bits for AIP31068_CMD_FUNCTION_SET (page 17, section 6) */

/**
 * @brief 0 = 4 bit interface data length
 *        1 = 8 bit interface data length
 */
#define AIP31068_BIT_FUNCTION_SET_BITMODE   BIT4

/**
 * @brief 0 = Single line
 *        1 = Two lines
 */
#define AIP31068_BIT_FUNCTION_SET_LINECOUNT BIT3

/**
 * @brief 0 = 5x8 dots per character
 *        1 = 5x10 dots per character
 */
#define AIP31068_BIT_FUNCTION_SET_FONTSIZE  BIT2

/* Bits for control byte (page 12) */

/**
 * @brief 0 = Last control byte
 *        1 = Another control byte follows data byte
 */
#define AIP31068_BIT_CONTROL_BYTE_CO    BIT7

/**
 * @brief 0 = data byte interpreted as command
 *        1 = data byte interpreted as data
 */
#define AIP31068_BIT_CONTROL_BYTE_RS    BIT6

#ifdef __cplusplus
}
#endif

/** @} */
