/*
 * SPDX-FileCopyrightText: 2024 Nico Behrens <nifrabe@outlook.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_tm1637 TM1637 display
 * @ingroup     drivers_display
 * @brief       Driver for the TM1637 4-digit 7-segment display
 *
 * @{
 * @file
 * @brief       Interface definition for the TM1637 4-digit 7-segment display driver
 *
 * @author      Nico Behrens <nifrabe@outlook.de>
 *
 */

#include "board.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Pin configuration parameters for the tm1637 display
 */
typedef struct {
    /**
     * @brief GPIO for clock
     */
    gpio_t clk;

    /**
     * @brief GPIO for data input/output
     */
    gpio_t dio;
} tm1637_params_t;

/**
 * @brief tm1637 driver descriptor
 */
typedef struct {
    /**
     * @brief Configuration parameters
     */
    const tm1637_params_t *params;
} tm1637_t;

/**
 * @brief Brightness level enum for the display
 *
 * @note The brightness must be one of the given values according to the specification.
 * It is expressed as a fraction of the full pulse width. The value has a size of 3 bits.
 */
typedef enum {
    TM1637_PW_1_16 = 0x00,
    TM1637_PW_2_16 = 0x01,
    TM1637_PW_4_16 = 0x02,
    TM1637_PW_10_16 = 0x03,
    TM1637_PW_11_16 = 0x04,
    TM1637_PW_12_16 = 0x05,
    TM1637_PW_13_16 = 0x06,
    TM1637_PW_14_16 = 0x07
} tm1637_brightness_t;

/**
 * @brief Initializes the tm1637 device
 *
 * @param[out]  dev     device descriptor of the display
 * @param[in]   params  configuration parameters
 *
 * @retval 0    on success
 * @retval -1   on error
 */
int tm1637_init(tm1637_t *dev, const tm1637_params_t *params);

/**
 * @brief Writes an integer to the display
 *
 * @note The integer can't be bigger than 9999 or smaller than
 * -999 as only 4 digits can be displayed at a time.
 * When leading zeros are enabled, the display is padded with zeros.
 * For negative integers the leading zeros are added between the minus sign
 * and the number.
 *
 * @param[in] dev           device descriptor of the display
 * @param[in] number        number to write, in the range of -999 to 9999
 * @param[in] brightness    brightness of the display according to @ref tm1637_brightness_t
 * @param[in] colon         If enabled, displays a colon in the middle
 * @param[in] leading_zeros If enabled, displays leading zeros
 *
 * @retval 0                on success
 * @retval -1               on error
 */
int tm1637_write_number(const tm1637_t *dev, int16_t number, tm1637_brightness_t brightness,
                        bool colon, bool leading_zeros);

/**
 * @brief Clear the display
 *
 * @param[in] dev device descriptor of the display
 *
 * @retval 0    on success
 * @retval -1   on error
 */
int tm1637_clear(const tm1637_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
