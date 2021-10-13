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
#define LED0_PIN            GPIO_PIN(PORT_E, 3)
#define LED0_MASK           (1 << 3)

#define LED0_ON             (GPIOE->BSRR = LED0_MASK)
#define LED0_OFF            (GPIOE->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (GPIOE->ODR  ^= LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_E, 4)
#define LED1_MASK           (1 << 4)

#define LED1_ON             (GPIOE->BSRR = LED1_MASK)
#define LED1_OFF            (GPIOE->BSRR = (LED1_MASK << 16))
#define LED1_TOGGLE         (GPIOE->ODR  ^= LED1_MASK)

#define LED2_PIN            GPIO_PIN(PORT_E, 1)
#define LED2_MASK           (1 << 1)

#define LED2_ON             (GPIOE->BSRR = LED2_MASK)
#define LED2_OFF            (GPIOE->BSRR = (LED2_MASK << 16))
#define LED2_TOGGLE         (GPIOE->ODR  ^= LED2_MASK)
/** @} */

/**
 * @brief   User button
 */
#define BTN_B1_PIN          GPIO_PIN(PORT_C, 13)

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

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
