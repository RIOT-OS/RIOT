/*
 * SPDX-FileCopyrightText: 2025 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_olimexino-stm32f3
 * @{
 *
 * @file
 * @brief       Board specific definitions for the OLIMEXINO STM32F3 board
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        5           /**< GPIO pin number of green LED */
#define LED0_PORT           GPIO_PORT_A /**< GPIO port of green LED */
#define LED0_PORT_NUM       PORT_A      /**< GPIO port number of green LED */

#define LED1_PIN_NUM        1           /**< GPIO pin number of yellow LED */
#define LED1_PORT           GPIO_PORT_A /**< GPIO port of yellow LED */
#define LED1_PORT_NUM       PORT_A      /**< GPIO port number of yellow LED */
/** @} */

/**
 * @name User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 9) /**< GPIO pin of BOOT button */
#define BTN0_MODE           GPIO_IN             /**< GPIO mode to configure BOOT buttonw ith */
/** @} */

/**
 * @name   SD card interface configuration
 *
 * SD card interface uses SPI_DEV(0) on this board. This configuration cannot
 * be overridden.
 * @{
 */
#define SDCARD_SPI_PARAM_SPI    SPI_DEV(1)
#define SDCARD_SPI_PARAM_CLK    GPIO_PIN(PORT_B, 13)
#define SDCARD_SPI_PARAM_MOSI   GPIO_PIN(PORT_B, 15)
#define SDCARD_SPI_PARAM_MISO   GPIO_PIN(PORT_B, 14)
#define SDCARD_SPI_PARAM_CS     GPIO_PIN(PORT_D, 2)
#define SDCARD_SPI_PARAM_POWER  GPIO_UNDEF
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
