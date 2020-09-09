/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_feather-m0
 *
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Adafruit Feather M0
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
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
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 17)

#define LED_PORT            PORT->Group[PA]
#define LED0_MASK           (1 << 17)

#define LED0_ON             (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

/**
 * @name    Configuration for Feather M0 WiFi and the ATWINC15x0 WiFi netdev
 * @{
 */
#define ATWINC15X0_PARAM_SPI            SPI_DEV(0)
#define ATWINC15X0_PARAM_SSN_PIN        GPIO_PIN(0, 6)
#define ATWINC15X0_PARAM_RESET_PIN      GPIO_PIN(0, 8)
#define ATWINC15X0_PARAM_CHIP_EN_PIN    GPIO_PIN(0, 14)
#define ATWINC15X0_PARAM_IRQ_PIN        GPIO_PIN(0, 21)
#define ATWINC15X0_PARAM_WAKE_PIN       GPIO_UNDEF
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
