/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_frdm-k64f Freescale FRDM-K64F Board
 * @ingroup     boards
 * @brief       Board specific implementations for the FRDM-K64F
 * @{
 *
 * @file
 * @brief       Board specific definitions for the FRDM-K64F
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
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
 * @brief   xtimer configuration
 * @{
 */
/* Make sure xtimer uses the LPTMR periph/timer device */
#define XTIMER                      (TIMER_0)
#define XTIMER_CHAN                 (0)
/* LPTMR is 16 bits wide */
#define XTIMER_MASK                 (0xffff0000)
#define XTIMER_SHIFT_ON_COMPARE     (0)
#define XTIMER_BACKOFF              (4)
#define XTIMER_ISR_BACKOFF          (4)
#define XTIMER_OVERHEAD             (3)
#define XTIMER_HZ                   (32768)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_R_PORT_CLKEN()    (PORTB_CLOCK_GATE = 1) /**< Clock Enable for PORTD*/
#define LED_G_PORT_CLKEN()    (PORTE_CLOCK_GATE = 1) /**< Clock Enable for PORTE*/
#define LED_B_PORT_CLKEN()    (PORTB_CLOCK_GATE = 1) /**< Clock Enable for PORTB*/
#define LED_R_PORT            PORTB /**< PORT for Red LED*/
#define LED_R_GPIO            GPIOB /**< GPIO-Device for Red LED*/
#define LED_G_PORT            PORTE /**< PORT for Green LED*/
#define LED_G_GPIO            GPIOE /**< GPIO-Device for Green LED*/
#define LED_B_PORT            PORTB /**< PORT for Blue LED*/
#define LED_B_GPIO            GPIOB /**< GPIO-Device for Blue LED*/
#define LED_R_PIN             22    /**< Red LED connected to PINx*/
#define LED_G_PIN             26    /**< Green LED connected to PINx*/
#define LED_B_PIN             21    /**< Blue LED connected to PINx*/
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_B_ON            (LED_B_GPIO->PCOR = (1 << LED_B_PIN))
#define LED_B_OFF           (LED_B_GPIO->PSOR = (1 << LED_B_PIN))
#define LED_B_TOGGLE        (LED_B_GPIO->PTOR = (1 << LED_B_PIN))
#define LED_G_ON            (LED_G_GPIO->PCOR = (1 << LED_G_PIN))
#define LED_G_OFF           (LED_G_GPIO->PSOR = (1 << LED_G_PIN))
#define LED_G_TOGGLE        (LED_G_GPIO->PTOR = (1 << LED_G_PIN))
#define LED_R_ON            (LED_R_GPIO->PCOR = (1 << LED_R_PIN))
#define LED_R_OFF           (LED_R_GPIO->PSOR = (1 << LED_R_PIN))
#define LED_R_TOGGLE        (LED_R_GPIO->PTOR = (1 << LED_R_PIN))

/* for compatability to other boards */
#define LED_GREEN_ON        LED_G_ON
#define LED_GREEN_OFF       LED_G_OFF
#define LED_GREEN_TOGGLE    LED_G_TOGGLE
#define LED_RED_ON          LED_R_ON
#define LED_RED_OFF         LED_R_OFF
#define LED_RED_TOGGLE      LED_R_TOGGLE
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /** BOARD_H */
/** @} */
