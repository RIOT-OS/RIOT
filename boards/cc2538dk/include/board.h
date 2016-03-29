/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_cc2538dk CC2538DK
 * @ingroup     boards
 * @brief       Support for the Texas Instruments CC2538DK board.
 * @{
 *
 * @file
 *
 * @author      Ian Martin <ian@locicontrols.com>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(2, 0)
#define LED1_PIN            GPIO_PIN(2, 1)
#define LED2_PIN            GPIO_PIN(2, 2)
#define LED3_PIN            GPIO_PIN(2, 3)

#define LED0_GPIO           GPIO_PC0        /**< red LED */
#define LED1_GPIO           GPIO_PC1        /**< yellow LED */
#define LED2_GPIO           GPIO_PC2        /**< green LED */
#define LED3_GPIO           GPIO_PC3        /**< orange LED */

#define LED0_ON             cc2538_gpio_set(LED0_GPIO)
#define LED0_OFF            cc2538_gpio_clear(LED0_GPIO)
#define LED0_TOGGLE         cc2538_gpio_toggle(LED0_GPIO)

#define LED1_ON             cc2538_gpio_set(LED1_GPIO)
#define LED1_OFF            cc2538_gpio_clear(LED1_GPIO)
#define LED1_TOGGLE         cc2538_gpio_toggle(LED1_GPIO)

#define LED2_ON             cc2538_gpio_set(LED2_GPIO)
#define LED2_OFF            cc2538_gpio_clear(LED2_GPIO)
#define LED2_TOGGLE         cc2538_gpio_toggle(LED2_GPIO)

#define LED3_ON             cc2538_gpio_set(LED3_GPIO)
#define LED3_OFF            cc2538_gpio_clear(LED3_GPIO)
#define LED3_TOGGLE         cc2538_gpio_toggle(LED3_GPIO)
/** @} */

/**
 * @name Flash Customer Configuration Area (CCA) parameters
 * @{
 */

#ifndef UPDATE_CCA
#define UPDATE_CCA                1
#endif

#define CCA_BACKDOOR_ENABLE       1
#define CCA_BACKDOOR_PORT_A_PIN   3 /**< Select button */
#define CCA_BACKDOOR_ACTIVE_LEVEL 0 /**< Active low */
/** @} */

/**
 * @name xtimer configuration
 * @{
 */
#define XTIMER              TIMER_0
#define XTIMER_CHAN         (0)
#define XTIMER_SHIFT        (-4)
#define XTIMER_BACKOFF      (50)
#define XTIMER_ISR_BACKOFF  (40)
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H_ */
/** @} */
