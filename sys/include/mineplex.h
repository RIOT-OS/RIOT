/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_mineplex 5x5 Font 'Mineplex'
 * @ingroup     sys
 * @brief       The Mineplex font for containing 5x5 pixel ASCII characters
 *
 * The font has a character representation for all ASCII characters from
 * 0x20 (Space) to 0x7E (~).
 *
 * @{
 *
 * @file
 * @brief       Interface definition to access the Mineplex font
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The width of a single character in pixel
 */
#define MINEPLEX_CHAR_W     (5U)

/**
 * @brief   The height of a single character in pixel
 */
#define MINEPLEX_CHAR_H     (5U)

/**
 * @brief   Get the Mineplex representation of a given ASCII character
 *
 * The function returns the pointer to a 5 byte pointer containing the Mineplex
 * representation of the given ASCII character. The Mineplex character is
 * encoded row wise from top to bottom using the least significant 5 bit, where
 * byte 0, bit 0 is the top left pixel of the encoded character.
 * The character is therefore stored mirrored, as shown in the example below
 * for the character "F":
 *
 * ~~~~~~~~~~~~~~~~~
 * 0x0F = 0b00001111
 * 0x01 = 0b00000001
 * 0x07 = 0b00000111
 * 0x01 = 0b00000001
 * 0x01 = 0b00000001
 * ~~~~~~~~~~~~~~~~~
 *
 * @param[in] c         character to translate
 *
 * @return  A pointer to the 5 byte sized character bitmap
 * @retval  "Requested Char" For valid ASCII input characters
 * @retval  "Space"          For invalid input values
 */
const uint8_t *mineplex_char(char c);

#ifdef __cplusplus
}
#endif

/** @} */
