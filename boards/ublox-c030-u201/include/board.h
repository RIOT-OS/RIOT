/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_ublox-c030-u201 Ublox C030-U201
 * @ingroup     boards
 * @brief       Support for the Ublox C030-U201 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Ublox C030-U201 board
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        3
#define LED0_PORT           GPIO_PORT_E /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_E

#define LED1_PIN_NUM        4
#define LED1_PORT           GPIO_PORT_E /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_E

#define LED2_PIN_NUM        1
#define LED2_PORT           GPIO_PORT_E /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_E
/** @} */

/**
 * @name    User button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 13)    /**< User button pin */
#define BTN0_MODE           GPIO_IN                 /**< User button pin mode */
/** @} */

/**
 * @name    si7034 temperature sensor configuration
 * TODO:    Check this when support is added in si70xx driver
 * @{
 */
#define SI70XX_PARAM_I2C_DEV    I2C_DEV(1)
#define SI70XX_PARAM_ADDR       (0x70)
#define SI70XX_SAUL_INFO        { .name = "si7034" }
/** @} */

/**
 * @name    SDCard configuration
 * @{
 */
#define SDCARD_SPI_PARAM_SPI    SPI_DEV(0)
#define SDCARD_SPI_PARAM_CS     GPIO_PIN(PORT_E, 11)
#define SDCARD_SPI_PARAM_CLK    GPIO_PIN(PORT_E, 2)
#define SDCARD_SPI_PARAM_MOSI   GPIO_PIN(PORT_E, 6)
#define SDCARD_SPI_PARAM_MISO   GPIO_PIN(PORT_E, 5)
/** @} */

/**
 * @name    Modem/GPS pins
 * @{
 */
#define UB_PWRON_PIN            GPIO_PIN(PORT_E, 14)
#define UB_M_RST_PIN            GPIO_PIN(PORT_B, 5)
#define M_GPIO2_PIN             GPIO_PIN(PORT_D, 1)
#define M_GPIO3_PIN             GPIO_PIN(PORT_B, 4)
#define GPS_RST_PIN             GPIO_PIN(PORT_C, 10) /* Not connected */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

#endif /* BOARD_H */
/** @} */
