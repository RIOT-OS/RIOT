/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_samr21-xpro Atmel SAM R21 Xplained Pro
 * @ingroup     boards
 * @brief       Support for the Atmel SAM R21 Xplained Pro board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Atmel SAM R21 Xplained Pro
 *              board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
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
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_DEV          TIMER_DEV(1)
#define XTIMER_CHAN         (0)
/** @} */

/**
 * @name    AT86RF233 configuration
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 */
#define AT86RF2XX_PARAM_CS         GPIO_PIN(PB, 31)
#define AT86RF2XX_PARAM_INT        GPIO_PIN(PB, 0)
#define AT86RF2XX_PARAM_SLEEP      GPIO_PIN(PA, 20)
#define AT86RF2XX_PARAM_RESET      GPIO_PIN(PB, 15)

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 19)

#define LED_PORT            PORT->Group[0]
#define LED0_MASK           (1 << 19)

#define LED0_ON             (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

/**
 * @name    SW0 (Button) pin definitions
 * @{
 */
#define BTN0_PORT           PORT->Group[0]
#define BTN0_PIN            GPIO_PIN(0, 28)
#define BTN0_MODE           GPIO_IN_PU
/** @} */
/**
 * @name    RFCTL pin definitions and handlers
 * @{
 */
#define RFCTL1_PIN            GPIO_PIN(0, 9)
#define RFCTL2_PIN            GPIO_PIN(0, 12)

#define RFCTL_PORT            PORT->Group[0]
#define RFCTL1_MASK           (1 << 9)
#define RFCTL2_MASK           (1 << 12)

#define RFCTL1_ON             (RFCTL_PORT.OUTCLR.reg = RFCTL1_MASK)
#define RFCTL1_OFF            (RFCTL_PORT.OUTSET.reg = RFCTL1_MASK)
#define RFCTL1_TOGGLE         (RFCTL_PORT.OUTTGL.reg = RFCTL1_MASK)
#define RFCTL2_ON             (RFCTL_PORT.OUTCLR.reg = RFCTL2_MASK)
#define RFCTL2_OFF            (RFCTL_PORT.OUTSET.reg = RFCTL2_MASK)
#define RFCTL2_TOGGLE         (RFCTL_PORT.OUTTGL.reg = RFCTL2_MASK)
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

/**
 * @brief  controls whether antenna 1 or 2 is used
 */
int board_choose_antenna(uint8_t antenna);
#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
