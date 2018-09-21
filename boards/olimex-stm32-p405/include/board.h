/*
 * Copyright (C) 2018 Nalys bvba
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_olimex-stm32-p405
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Olimex STM32-P405
 *
 * @author      Toon Stegen <tstegen@nalys-group.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LED pin definitions
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_C, 12)
/** @} */

/**
 * @name Macros for controlling the on-board LED.
 * @{
 */
#define LED_PORT            GPIOC
#define LED0_MASK           (1 << 12)

#define LED0_ON             (LED_PORT->BSRR = LED0_MASK)
#define LED0_OFF            (LED_PORT->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (LED_PORT->ODR  ^= LED0_MASK)

/** @} */

/**
 * @name User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
#define BTN0_MODE           GPIO_IN
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
