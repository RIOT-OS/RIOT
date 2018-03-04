/*
 * Copyright (C) 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_blu-wmn
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to BLU-WMN001 pins
 *
 * You can use the defines in this file for simplified interaction with the
 * BLU-WMN001 specific pin numbers.
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef BLU_WMN_PINMAP_H
#define BLU_WMN_PINMAP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mapping of MCU pins to BLU-WMN001 pins
 */
/*
 * DESCRIPTION BLU-WMN API PIN          PORT   PIN
 */
#define PORT_MOSI               GPIO_PIN(PORT_B, 5)
#define PORT_MISO               GPIO_PIN(PORT_B, 6)
#define PORT_SCK                GPIO_PIN(PORT_B, 7)
#define PORT_SEL                GPIO_PIN(PORT_D, 3)
#define PORT_INT                GPIO_PIN(PORT_D, 2)
#define WIRELESS_MOSI           GPIO_PIN(PORT_B, 5)
#define WIRELESS_MISO           GPIO_PIN(PORT_B, 6)
#define WIRELESS_SCK            GPIO_PIN(PORT_B, 7)
#define WIRELESS_SEL            GPIO_PIN(PORT_D, 7)
#define WIRELESS_INT            GPIO_PIN(PORT_D, 4)
#define WIRELESS_SLP            GPIO_PIN(PORT_D, 5)
#define WIRELESS_RST            GPIO_PIN(PORT_D, 6)
#define I2C_SDA                 GPIO_PIN(PORT_C, 1)
#define I2C_SCL                 GPIO_PIN(PORT_C, 0)
#define I2C_INT                 GPIO_PIN(PORT_B, 4)
#define DIEL_BUS_0              GPIO_PIN(PORT_A, 0)
#define DIEL_BUS_1              GPIO_PIN(PORT_A, 1)
#define DIEL_BUS_2              GPIO_PIN(PORT_A, 2)
#define DIEL_BUS_3              GPIO_PIN(PORT_A, 3)
#define DIEL_BUS_4              GPIO_PIN(PORT_A, 4)
#define DIEL_BUS_5              GPIO_PIN(PORT_A, 5)
#define DIEL_BUS_6              GPIO_PIN(PORT_A, 6)
#define DIEL_BUS_7              GPIO_PIN(PORT_A, 7)
#define DIEL_CLK                GPIO_PIN(PORT_B, 1)
#define VGPIO_EN                GPIO_PIN(PORT_B, 3)
#define SEISMIC_ACT             GPIO_PIN(PORT_B, 2)
#define JTAG_TDO                GPIO_PIN(PORT_C, 4)
#define JTAG_TDI                GPIO_PIN(PORT_C, 5)
#define JTAG_TCK                GPIO_PIN(PORT_C, 2)
#define JTAG_TMS                GPIO_PIN(PORT_C, 3)
#define RTC_CRYSTAL1            GPIO_PIN(PORT_C, 6)
#define RTC_CRYSTAL2            GPIO_PIN(PORT_C, 7)
/** @ */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
