/*
 * SPDX-FileCopyrightText: 2019 Philipp-Alexander Blum <philipp-blum@jakiku.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup   gpio_util GPIO I/O Utils
 * @ingroup    drivers_periph_gpio
 * @brief      GPIO I/O utility functions
 *
 * @{
 *
 * @file
 * @brief      GPIO I/O utility function implementations
 *
 * @author     Philipp-Alexander Blum <philipp-blum@jakiku.de>
 */

#include "gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Shift in a byte from data_pin, create clock pulses on clock_pin
 *
 * This function has the same functionality as the Arduino shiftIn() in Advanced
 * I/O.
 *
 * @param[in]   data_pin    Pin to read data from
 * @param       clock_pin   Pin to create clock pulses on
 * @return      the resulting uint8_t of the shift (MSB first)
 */
uint8_t gpio_util_shiftin(gpio_t data_pin, gpio_t clock_pin);

#ifdef __cplusplus
}
#endif
/** @} */
