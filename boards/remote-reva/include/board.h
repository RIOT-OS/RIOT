/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_remote-reva
 * @{
 *
 * @file
 * @brief       Board specific definitions for the RE-Mote board Revision A
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Antonio Lignan <alinan@zolertia.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "board_common.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name LED pin definitions
 * @{
 */
#define LED0_PIN        GPIO_PIN(3, 5)
#define LED1_PIN        GPIO_PIN(3, 4)
#define LED2_PIN        GPIO_PIN(3, 3)

#define LED0_MASK       (1 << 5)
#define LED1_MASK       (1 << 4)
#define LED2_MASK       (1 << 3)

#define LED0_ON         (GPIO_D->DATA |=  LED0_MASK)
#define LED0_OFF        (GPIO_D->DATA &= ~LED0_MASK)
#define LED0_TOGGLE     (GPIO_D->DATA ^=  LED0_MASK)

#define LED1_ON         (GPIO_D->DATA |=  LED1_MASK)
#define LED1_OFF        (GPIO_D->DATA &= ~LED1_MASK)
#define LED1_TOGGLE     (GPIO_D->DATA ^=  LED1_MASK)

#define LED2_ON         (GPIO_D->DATA |=  LED2_MASK)
#define LED2_OFF        (GPIO_D->DATA &= ~LED2_MASK)
#define LED2_TOGGLE     (GPIO_D->DATA ^=  LED2_MASK)
/** @} */

/**
 * @name User button pin definition
 * @{
 */
#define BTN0_PIN        GPIO_PIN(0, 3)
#define BTN0_MODE       GPIO_IN_PU
/** @} */

/**
 * @name  RF switch controlled by SW
 * @brief Controls which RF interface goes to the RP-SMA external antenna
 *
 * These values configure the required pin to drive the RF antenna switch, to
 * either enable the sub-1Ghz RF interface (power-up the CC1200) or the 2.4GHz
 * RF interface of the CC2538, both alternatively routed to a RP-SMA connector
 * to allow using an external antenna for both cases.
 *
 * Note it is also possible to enable both RF interfaces at the same time, by
 * switching On the sub-1GHz RF interface, and placing an 0Ohm resistor (R19),
 * to select between using a ceramic chip antenna (not mounted), or to connect
 * and external antenna over a pigtail to the U.Fl connector (not mounted).
 *
 * RF switch state:
 * - LOW: 2.4GHz RF interface on RP-SMA connector, CC1200 powered-off.
 * - HIGH: Sub-1GHz RF interface on RP-SMA connector.
 *
 * @{
 */
#define RF_SWITCH_GPIO      GPIO_PD2
#define RF_SWITCH_SUB_GHZ   gpio_set(RF_SWITCH_GPIO)
#define RF_SWITCH_2_4_GHZ   gpio_clear(RF_SWITCH_GPIO)
#define RF_SWITCH_TOGGLE    gpio_toggle(RF_SWITCH_GPIO)
/** @} */

/**
 * @name Shutdown enable/done pins
 * @{
 */
#define SHUTDOWN_DONE_GPIO  GPIO_PD0
#define SHUTDOWN_EN_GPIO    GPIO_PD1
/** @} */

/**
 * @name CC1200 SPI and pins definitions
 * @{
 */
#define CC1200_SPI_DEV      SSI0
#define CC1200_MOSI_GPIO    GPIO_PB1
#define CC1200_MISO_GPIO    GPIO_PB3
#define CC1200_SCLK_GPIO    GPIO_PB2
#define CC1200_CSN_GPIO     GPIO_PB5
#define CC1200_RESET_GPIO   GPIO_PC7
#define CC1200_GPD0_GPIO    GPIO_PB4
#define CC1200_GPD2_GPIO    GPIO_PB0
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif
#endif /* BOARD_H */
/** @} */
