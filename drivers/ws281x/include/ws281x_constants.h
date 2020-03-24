/*
 * Copyright (C) 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ws281x
 *
 * @{
 * @file
 * @brief       Constants for WS2812/SK6812 RGB LEDs
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef WS281X_CONSTANTS_H
#define WS281X_CONSTANTS_H

#ifdef MODULE_WS281X_GPIO_ABC
#include "periph/gpio_abc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MODULE_WS281X_GPIO_ABC
/*
 * The high time to encode a zero must not be longer than 450ns
 * (300ns + 150ns tolerance on the SK6812), otherwise it will be confused with
 * a one.
 */
#if GPIO_ABC_MIN_PULSE_LEN > 450
#error "NeoPixel: GPIO ABC is unable to generate short enough pulses"
#endif

/*
 * In order to support both variants, the timing tolerance is only 100ns instead
 * of 150ns, see explanation below
 */
#if GPIO_ABC_ACCURACY_NS > 100
#error "NeoPixel: GPIO ABC is unable to provide the accuracy required"
#endif
#endif /* MODULE_WS281X_GPIO_ABC */

/**
 * @name    Timing parameters for WS2812/SK6812 RGB LEDs
 * @{
 */
/**
 * @brief   Time in nanoseconds to pull the data line high when encoding a zero
 *
 * For the WS2812 it is 350ns ± 150ns, for the SK6812 it is 300ns ± 150ns.
 * We choose 325ns to remain compatible with both (± 125ns tolerance).
 */
#define WS281X_TICKS_ZERO_HIGH          (GPIO_ABC_TICKS(325))
/**
 * @brief   Time in nanoseconds to pull the data line low when encoding a zero
 *
 * For the WS2812 it is 800ns ± 150ns, for the SK6812 it is 900ns ± 150ns.
 * We choose 850ns to remain compatible with both (± 100ns tolerance).
 */
#define WS281X_TICKS_ZERO_LOW           (GPIO_ABC_TICKS(850))
/**
 * @brief   Time in nanoseconds to pull the data line high when encoding a one
 *
 * For the WS2812 it is 700ns ± 150ns, for the SK6812 it is 600ns ± 150ns.
 * We choose 650ns to remain compatible with both (± 100ns tolerance).
 */
#define WS281X_TICKS_ONE_HIGH           (GPIO_ABC_TICKS(650))
/**
 * @brief   Time in nanoseconds to pull the data line low when encoding a one
 *
 * For both the WS2812 and the SK6812 it is 600ns ± 150ns.
 */
#define WS281X_TICKS_ONE_LOW            (GPIO_ABC_TICKS(600))
/**
 * @brief   Time in microseconds to pull the data line low to signal end of data
 *
 * For the WS2812 it is ≥ 50µs, for the SK6812 it is ≥ 80µs. We choose 80µs to
 * be compatible with both.
 */
#define WS281X_T_END_US                 (80U)
/**@}*/

/**
 * @name    Data encoding parameters for WS2812/SK6812 RGB LEDs
 * @{
 */
/**
 * @brief   Offset for the red color component
 */
#define WS281X_OFFSET_R                 (1U)
/**
 * @brief   Offset for the green color component
 */
#define WS281X_OFFSET_G                 (0U)
/**
 * @brief   Offset for the blue color component
 */
#define WS281X_OFFSET_B                 (2U)
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* WS281X_CONSTANTS_H */
/** @} */
