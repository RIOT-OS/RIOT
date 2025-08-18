/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "periph/gpio.h"

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
 * @name    Configuration for Feather M0 LoRa and the SX1276 module
 * @{
 **/
#define SX127X_PARAM_SPI                SPI_DEV(0)
#if defined(BOARD_FEATHER_M0_LORA)
#define SX127X_PARAM_SPI_NSS            GPIO_PIN(PA, 6)
#define SX127X_PARAM_RESET              GPIO_PIN(PA, 8)
#define SX127X_PARAM_DIO0               GPIO_PIN(PA, 9)
#endif
#define SX127X_PARAM_DIO1               GPIO_UNDEF
#define SX127X_PARAM_DIO2               GPIO_UNDEF
#define SX127X_PARAM_DIO3               GPIO_UNDEF
#define SX127X_PARAM_PASELECT           (SX127X_PA_BOOST)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
