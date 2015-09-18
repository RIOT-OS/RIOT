/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32f4discovery
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
 *
 * @author      Hauke Petersen  <hauke.petersen@fu-berlin.de>
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ARDUINO_LED         (2)

static const gpio_t arduino_pinmap[] = {
    GPIO_PIN(PORT_D, 12),
    GPIO_PIN(PORT_D, 13),
    GPIO_PIN(PORT_D, 14),
    GPIO_PIN(PORT_D, 15),
    GPIO_PIN(PORT_A, 12),
    GPIO_PIN(PORT_A, 15),
    GPIO_PIN(PORT_B, 1),
    GPIO_PIN(PORT_B, 2),
};

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
