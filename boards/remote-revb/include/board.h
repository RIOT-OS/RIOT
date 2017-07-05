/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_remote-revb RE-Mote Revision B
 * @ingroup     boards
 * @brief       Support for the RE-Mote board Revision B
 * @{
 *
 * @file
 * @brief       Board specific definitions for the RE-Mote board Revision B
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Antonio Lignan <alinan@zolertia.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name LED pin definitions
 * @{
 */
#define LED0_PIN        GPIO_PIN(3, 4)
#define LED1_PIN        GPIO_PIN(1, 7)
#define LED2_PIN        GPIO_PIN(1, 6)
/** @} */

#include "periph/led.h"
#include "board_common.h"

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
 * @name Power management enable pin
 * @{
 */
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
 * @name Onboard micro-sd slot pin definitions
 * @{
 */
#define SDCARD_SPI_PARAM_SPI       SPI_DEV(1)
#define SDCARD_SPI_PARAM_CS        GPIO_PIN(0,7)
#define SDCARD_SPI_PARAM_CLK       GPIO_PIN(2,4)
#define SDCARD_SPI_PARAM_MOSI      GPIO_PIN(2,5)
#define SDCARD_SPI_PARAM_MISO      GPIO_PIN(2,6)
#define SDCARD_SPI_PARAM_POWER     GPIO_PIN(0,6)
#define SDCARD_SPI_PARAM_POWER_AH  false
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
