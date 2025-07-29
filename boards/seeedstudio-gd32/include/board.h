/*
 * SPDX-FileCopyrightText: 2020 Koen Zandberg <koen@bergzand.net>
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht <gunar@schorcht.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-gd32
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SeeedStudio GD32 RISC-V board
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "board_common.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Button pin definitions
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
#define BTN0_MODE           GPIO_IN
#define BTN0_INT_FLANK      GPIO_RISING

#define BTN1_PIN            GPIO_PIN(PORT_C, 13)
#define BTN1_MODE           GPIO_IN
#define BTN1_INT_FLANK      GPIO_RISING
/** @} */

/**
 * @name    LED (on-board) configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B, 5)
#define LED0_MASK           (1 << 5)
#define LED0_ON             (GPIOB->BC = LED0_MASK)
#define LED0_OFF            (GPIOB->BOP = LED0_MASK)
#define LED0_TOGGLE         (GPIOB->OCTL ^= LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_B, 0)
#define LED1_MASK           (1 << 0)
#define LED1_ON             (GPIOB->BC = LED1_MASK)
#define LED1_OFF            (GPIOB->BOP = LED1_MASK)
#define LED1_TOGGLE         (GPIOB->OCTL ^= LED1_MASK)

#define LED2_PIN            GPIO_PIN(PORT_B, 1)
#define LED2_MASK           (1 << 1)
#define LED2_ON             (GPIOB->BC = LED2_MASK)
#define LED2_OFF            (GPIOB->BOP = LED2_MASK)
#define LED2_TOGGLE         (GPIOB->OCTL ^= LED2_MASK)

#define LED_RED_PIN         LED0_PIN    /**< LED0 is red */
#define LED_GREEN_PIN       LED1_PIN    /**< LED1 is green */
#define LED_BLUE_PIN        LED2_PIN    /**< LED2 is blue */
/** @} */

/**
 * @name MTD configuration
 * @{
 */
#define MTD_0 mtd_dev_get(0)          /**< MTD device for the SD Card */
/** @} */

/**
 * @name   SD-Card interface configuration
 * @{
 */
#define SDCARD_SPI_PARAM_SPI         SPI_DEV(0)
#define SDCARD_SPI_PARAM_CS          GPIO_PIN(PORT_B, 12)
#define SDCARD_SPI_PARAM_CLK         GPIO_PIN(PORT_B, 13)
#define SDCARD_SPI_PARAM_MISO        GPIO_PIN(PORT_B, 14)
#define SDCARD_SPI_PARAM_MOSI        GPIO_PIN(PORT_B, 15)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
