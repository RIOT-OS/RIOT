/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_arduino-mkrfox1200
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Arduino MKRFOX1200
 *              board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "cpu.h"
#include "periph_conf.h"
#include "board_common.h"
#include "arduino_pinmap.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    The on-board ATA8520E configuration
 * @{
 */
#define ATA8520E_PARAM_SPI         SPI_DEV(1)
#define ATA8520E_PARAM_CS_PIN      GPIO_PIN(PA, 14)
#define ATA8520E_PARAM_INT_PIN     GPIO_PIN(PB, 9)
#define ATA8520E_PARAM_POWER_PIN   GPIO_PIN(PA, 28)
#define ATA8520E_PARAM_RESET_PIN   GPIO_PIN(PA, 27)
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 20)

#define LED_PORT            PORT->Group[PA]
#define LED0_MASK           (1 << 20)

#define LED0_ON             (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)

#define LED0_NAME           "LED(Green)"
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
