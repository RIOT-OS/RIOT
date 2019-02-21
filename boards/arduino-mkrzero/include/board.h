/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-mkrzero
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Arduino MKRZERO
 *              board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "board_common.h"
#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SD Card device configuration
 * @{
 */
#define SDCARD_SPI_PARAM_SPI         (SPI_DEV(1))
#define SDCARD_SPI_PARAM_CS          (GPIO_PIN(PA, 14))
#define SDCARD_SPI_PARAM_CLK         (GPIO_PIN(PA, 13))
#define SDCARD_SPI_PARAM_MOSI        (GPIO_PIN(PA, 12))
#define SDCARD_SPI_PARAM_MISO        (GPIO_PIN(PA, 15))
#define SDCARD_SPI_PARAM_POWER       (GPIO_UNDEF)
/** treated as 'don't care' if SDCARD_SPI_PARAM_POWER is GPIO_UNDEF */
#define SDCARD_SPI_PARAM_POWER_AH    (true)
/** @} */

/**
 * @brief   Card detect pin
 */
#define CARD_DETECT_PIN              (GPIO_PIN(PA, 27))

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN                     GPIO_PIN(PB, 8)

#define LED_PORT                     PORT->Group[PB]
#define LED0_MASK                    (1 << 8)

#define LED0_ON                      (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_OFF                     (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE                  (LED_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
