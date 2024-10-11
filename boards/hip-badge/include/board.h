/*
 * Copyright (C) 2023 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_hip_badge
 * @brief       Board definitions for the HiP Badge
 * @{
 *
 * @file
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

/**
 * @name    Button pin definitions
 * @{
 */

/**
 * @brief   Tact A GPIO pin definition
 */
#define BTN0_PIN        GPIO2

/**
 * @brief    button GPIO mode definition
 *
 * Since the GPIO of the button is pulled up with an external resistor, the
 * mode for the GPIO pin has to be GPIO_IN.
 */
#define BTN0_MODE       GPIO_IN

/**
 * @brief   Default interrupt flank definition for the button GPIO
 */
#ifndef BTN0_INT_FLANK
#define BTN0_INT_FLANK  GPIO_FALLING
#endif

/**
 * @brief   Tact B GPIO pin definition
 */
#define BTN1_PIN        GPIO8

/**
 * @brief   Default button GPIO mode definition
 *
 * Since the GPIO of the button is pulled up with an external resistor, the
 * mode for the GPIO pin has to be GPIO_IN.
 */
#define BTN1_MODE       GPIO_IN

/**
 * @brief   Default interrupt flank definition for the button GPIO
 */
#ifndef BTN1_INT_FLANK
#define BTN1_INT_FLANK  GPIO_FALLING
#endif

/**
 * @brief   BOOT button GPIO pin definition
 *
 * The HiP-Badge has a BOOT button connected to GPIO9, which can be
 * used as button during normal operation. Since the GPIO9 pin is pulled up,
 * the button signal is inverted, i.e., pressing the button will give a
 * low signal.
 */
#define BTN2_PIN        GPIO9

/**
 * @brief   Default button GPIO mode definition
 */
#define BTN2_MODE       GPIO_IN_PU

/**
 * @brief   Default interrupt flank definition for the button GPIO
 */
#ifndef BTN2_INT_FLANK
#define BTN2_INT_FLANK  GPIO_FALLING
#endif
/** @} */

/**
 * @brief   WS281x LEDs are connected to GPIO10
 */
#ifndef WS281X_PARAM_PIN
#define WS281X_PARAM_PIN    GPIO10
#endif

/**
 * @brief   There are 16 WS281x LEDs
 */
#ifndef WS281X_PARAM_NUMOF
#define WS281X_PARAM_NUMOF  16
#endif

/* include common board definitions as last step */
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
#endif /* BOARD_H */
