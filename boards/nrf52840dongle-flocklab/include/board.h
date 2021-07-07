/*
 * Copyright (C) 2019 Christian Amsüss <chrysn@fsfe.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52840dongle-flocklab
 * @{
 *
 * @file
 * @brief       Board specific configuration for the nRF52840-Dongle
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "board_common.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
 /** @brief The LED labelled LD1 */
#define LED0_PIN            GPIO_PIN(0, 6)

 /** @brief The red channel of the LED labelled LD2 */
#define LED1_PIN            GPIO_PIN(0, 8)
 /** @brief The green channel of the LED labelled LD2 */
#define LED2_PIN            GPIO_PIN(1, 9)
 /** @brief The blue channel of the LED labelled LD2 */
#define LED3_PIN            GPIO_PIN(0, 12)


// For flocklab GPIO pins see:
// https://gitlab.ethz.ch/tec/public/flocklab/wiki/-/wikis/Man/GpioAssignmentTargetAdapter

 /** @brief The red channel of the LED labelled LD3 */
#define FLOCKLAB_LED1_PIN            GPIO_PIN(0, 13)
 /** @brief The green channel of the LED labelled LD3 */
#define FLOCKLAB_LED2_PIN            GPIO_PIN(0, 15)
 /** @brief The blue channel of the LED labelled LD3 */
#define FLOCKLAB_LED3_PIN            GPIO_PIN(0, 17)

/** @} */

/**
 * @name    LED access macros
 * @{
 */

/** Enable LD1 */
#define LED0_ON gpio_clear(LED0_PIN)
/** Disable LD1 */
#define LED0_OFF gpio_set(LED0_PIN)
/** Toggle LD1 */
#define LED0_TOGGLE gpio_toggle(LED0_PIN)

/** Enable LD2's red channel */
#define LED1_ON gpio_clear(LED1_PIN)
/** Disable LD2's red channel */
#define LED1_OFF gpio_set(LED1_PIN)
/** Toggle LD2's red channel */
#define LED1_TOGGLE gpio_toggle(LED1_PIN)

/** Enable LD2's green channel */
#define LED2_ON gpio_clear(LED2_PIN)
/** Disable LD2's green channel */
#define LED2_OFF gpio_set(LED2_PIN)
/** Toggle LD2's green channel */
#define LED2_TOGGLE gpio_toggle(LED2_PIN)

/** Enable LD2's blue channel */
#define LED3_ON gpio_clear(LED3_PIN)
/** Disable LD2's blue channel */
#define LED3_OFF gpio_set(LED3_PIN)
/** Toggle LD2's blue channel */
#define LED3_TOGGLE gpio_toggle(LED3_PIN)


/** Enable LD3's red channel */
#define FLOCKLAB_LED1_ON gpio_clear(FLOCKLAB_LED1_PIN)
/** Disable LD3's red channel */
#define FLOCKLAB_LED1_OFF gpio_set(FLOCKLAB_LED1_PIN)
/** Toggle LD3's red channel */
#define FLOCKLAB_LED1_TOGGLE gpio_toggle(FLOCKLAB_LED1_PIN)

/** Enable LD3's green channel */
#define FLOCKLAB_LED2_ON gpio_clear(FLOCKLAB_LED2_PIN)
/** Disable LD3's green channel */
#define FLOCKLAB_LED2_OFF gpio_set(FLOCKLAB_LED2_PIN)
/** Toggle LD3's green channel */
#define FLOCKLAB_LED2_TOGGLE gpio_toggle(FLOCKLAB_LED2_PIN)

/** Enable LD3's blue channel */
#define FLOCKLAB_LED3_ON gpio_clear(FLOCKLAB_LED3_PIN)
/** Disable LD3's blue channel */
#define FLOCKLAB_LED3_OFF gpio_set(FLOCKLAB_LED3_PIN)
/** Toggle LD3's blue channel */
#define FLOCKLAB_LED3_TOGGLE gpio_toggle(FLOCKLAB_LED3_PIN)

/** @} */

/**
 * @name    FlockLab INT, SIG, EXT, NFC and SWO pins
 * @{
 */

#define FLOCKLAB_INT1            GPIO_PIN(0, 20)
#define FLOCKLAB_INT2            GPIO_PIN(0, 22)

#define FLOCKLAB_SIG1            GPIO_PIN(0, 31)
#define FLOCKLAB_SIG2            GPIO_PIN(0, 29)

#define FLOCKLAB_EXT1            GPIO_PIN(0, 2)
#define FLOCKLAB_EXT2            GPIO_PIN(1, 15)

#define FLOCKLAB_NFC1            GPIO_PIN(0, 9)
#define FLOCKLAB_NFC2            GPIO_PIN(0, 10)

#define FLOCKLAB_SWO             GPIO_PIN(1, 0)

/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
/** @brief The button labelled SW1 */
#define BTN0_PIN            GPIO_PIN(1, 6)
/** @brief The GPIO pin mode of the button labelled SW1 */
#define BTN0_MODE           GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
