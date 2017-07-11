/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_stm32f3discovery STM32F3Discovery
 * @ingroup     boards
 * @brief       Board specific files for the STM32F3Discovery board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F3Discovery evaluation board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    On-board LED configuration
 */
static const gpio_t led_config[] = {
    GPIO_PIN(PORT_E, 9),
    GPIO_PIN(PORT_E, 8),
    GPIO_PIN(PORT_E, 10),
    GPIO_PIN(PORT_E, 15),
    GPIO_PIN(PORT_E, 11),
    GPIO_PIN(PORT_E, 14),
    GPIO_PIN(PORT_E, 12),
    GPIO_PIN(PORT_E, 13)
};

/**
 * @brief   Give on-board LEDs some user friendly names [optional]
 */
enum {
    LD3,
    LD4,
    LD5,
    LD6,
    LD7,
    LD8,
    LD9,
    LD10
};

/**
 * @brief User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
#define BTN0_MODE           GPIO_IN
/** @} */


/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
