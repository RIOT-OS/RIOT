/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_kea128ledlightrd NXP/Freescale KEA128LEDLIGHTRD Board
 * @ingroup     boards
 * @brief       Support for the NXP KEA128LEDLIGHTRD
 * @{
 *
 * @file
 * @brief       Board specific definitions for the KEA128LEDLIGHTRD
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Anton Gerasimov <anton.gerasimov@here.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_A, 10)
#define LED1_PIN            GPIO_PIN(PORT_A, 11)
#define LED2_PIN            GPIO_PIN(PORT_A, 12)
#define LED3_PIN            GPIO_PIN(PORT_A, 13)

#define LED0_MASK           (1 << 10)
#define LED1_MASK           (1 << 11)
#define LED2_MASK           (1 << 12)
#define LED3_MASK           (1 << 13)

#define LED0_ON            (GPIOA->PCOR = LED0_MASK)
#define LED0_OFF           (GPIOA->PSOR = LED0_MASK)
#define LED0_TOGGLE        (GPIOA->PTOR = LED0_MASK)

#define LED1_ON            (GPIOA->PCOR = LED1_MASK)
#define LED1_OFF           (GPIOA->PSOR = LED1_MASK)
#define LED1_TOGGLE        (GPIOA->PTOR = LED1_MASK)

#define LED2_ON            (GPIOA->PCOR = LED2_MASK)
#define LED2_OFF           (GPIOA->PSOR = LED2_MASK)
#define LED2_TOGGLE        (GPIOA->PTOR = LED2_MASK)

#define LED3_ON            (GPIOA->PCOR = LED3_MASK)
#define LED3_OFF           (GPIOA->PSOR = LED3_MASK)
#define LED3_TOGGLE        (GPIOA->PTOR = LED3_MASK)

/** @} */

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_DEV                  (TIMER_PIT_DEV(0))
#define XTIMER_CHAN                 (0)
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
