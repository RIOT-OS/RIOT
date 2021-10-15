/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_mpfs-icicle-kit-es
 * @{
 *
 * @file
 * @brief       Board specific definitions for MPFS Icicle Kit.
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(2, 16)
#define LED1_PIN            GPIO_PIN(2, 17)
#define LED2_PIN            GPIO_PIN(2, 18)
#define LED3_PIN            GPIO_PIN(2, 19)

#define LED_PORT            GPIO2_LO
#define LED0_MASK           (1 << 16)
#define LED1_MASK           (1 << 17)
#define LED2_MASK           (1 << 18)
#define LED3_MASK           (1 << 19)

#define LED0_ON             (LED_PORT->GPIO_SET_BITS = LED0_MASK)
#define LED0_OFF            (LED_PORT->GPIO_CLR_BITS = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->GPIO_OUT   ^= LED0_MASK)

#define LED1_ON             (LED_PORT->GPIO_SET_BITS = LED1_MASK)
#define LED1_OFF            (LED_PORT->GPIO_CLR_BITS = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->GPIO_OUT   ^= LED1_MASK)

#define LED2_ON             (LED_PORT->GPIO_SET_BITS = LED2_MASK)
#define LED2_OFF            (LED_PORT->GPIO_CLR_BITS = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT->GPIO_OUT   ^= LED2_MASK)

#define LED3_ON             (LED_PORT->GPIO_SET_BITS = LED3_MASK)
#define LED3_OFF            (LED_PORT->GPIO_CLR_BITS = LED3_MASK)
#define LED3_TOGGLE         (LED_PORT->GPIO_OUT   ^= LED3_MASK)

/** @} */

/**
 * @name SW0 (Button) pin definitions
 * @{
 */
#define BTN0_PORT           2
#define BTN0_PIN            GPIO_PIN(2, 30)
#define BTN0_MODE           GPIO_IN

#define BTN1_PORT           2
#define BTN1_PIN            GPIO_PIN(2, 31)
#define BTN1_MODE           GPIO_IN
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
