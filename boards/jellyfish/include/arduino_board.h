/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_jellyfish
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
 *
 * @author      Dang Minh Phuong  <kamejoko80@yahoo.com>
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ARDUINO_LED         (2)

static const gpio_t arduino_pinmap[] = {
    /* TBD */
    GPIO_PIN(1, 12), // LED1
    GPIO_PIN(1, 11), // LED2
};

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
