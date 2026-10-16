/*
 * Copyright (C) 2019 Inria
 *               2022 Nicole Faerber
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_banglejs2
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Bangle.js 2
 *
 * @author      Nicole Faebrer <nicole.faerber@digitaluhr-manufaktur.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "board_common.h"
#include "mtd.h"
#include "periph/pm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name GPIO pin defines
 * @{
 */
#define LCD_CS                      GPIO_PIN(0, 5)
// #define LCD_MISO                    GPIO_PIN(0, )
#define LCD_MOSI                    GPIO_PIN(0, 27)
#define LCD_SCK                     GPIO_PIN(0, 26)
#define LCD_EXTCOMIN                GPIO_PIN(0, 6)
#define LCD_DISP                    GPIO_PIN(0, 7)

#define LCD_BACKLIGHT               GPIO_PIN(0, 8)

#define BUTTON0                     GPIO_PIN(0, 17)
#define VIBRATOR                    GPIO_PIN(0, 19)

#define EXTPOWER_PRESENT            GPIO_PIN(0, 23)
#define CHARGE_COMPLETE             GPIO_PIN(0, 25)
#define BATTERY_ADC                 NRF52_AIN1

#define ATM_PRESSURE_I2C_DEV        I2C_DEV(2)
#define ATM_PRESSURE_I2C_ADDR       (0x76)
//#define ATM_PRESSURE_PWR            GPIO_PIN(0, )
#define ATM_PRESSURE_SCL            GPIO_PIN(0, 2)
#define ATM_PRESSURE_SDA            GPIO_PIN(1, 15)

#define HRM_I2C_DEV                 I2C_DEV(3)
#define HRM_I2C_ADDR                (0x33)
#define HRM_PWR                     GPIO_PIN(0, 21)
#define HRM_INT                     GPIO_PIN(0, 22)
#define HRM_SCL                     GPIO_PIN(1, 0)
#define HRM_SDA                     GPIO_PIN(0, 24)

#define TOUCH_I2C_DEV               I2C_DEV(0)
#define TOUCH_I2C_ADDR              (0x15)
#define TOUCH_RESET                 GPIO_PIN(1, 3)
#define TOUCH_INT                   GPIO_PIN(1, 4)
#define TOUCH_SCL                   GPIO_PIN(1, 2)
#define TOUCH_SDA                   GPIO_PIN(1, 1)

#define ACCEL_I2C_DEV               I2C_DEV(1)
#define ACCEL_I2C_ADDR              (0x1e)
#define ACCEL_INT                   GPIO_PIN(1, 7)
#define ACCEL_SCL                   GPIO_PIN(1, 5)
#define ACCEL_SDA                   GPIO_PIN(1, 6)

#define MAGN_I2C_DEV                I2C_DEV(4)
#define MAGN_I2C_ADDR               (0x0c)
#define MAGN_SCL                    GPIO_PIN(1, 13)
#define MAGN_SDA                    GPIO_PIN(1, 12)

#define GPS_PWR                     GPIO_PIN(0, 29)
#define GPS_TXD                     GPIO_PIN(0, 30)
#define GPS_RXD                     GPIO_PIN(0, 31)

#define SPI_FLASH_DEV               SPI_DEV(3)
#define SPI_FLASH_CS                GPIO_PIN(0, 14)
#define SPI_FLASH_MISO              GPIO_PIN(0, 13)
#define SPI_FLASH_MOSI              GPIO_PIN(0, 15)
#define SPI_FLASH_SCK               GPIO_PIN(0, 16)

/** @} */

/**
 * @name    LEDs pin configuration
 * @{
 */
#define LED0_PIN               LCD_BACKLIGHT                   /**< LED0 pin */
#define LED0_MASK              (1 << 8)                        /**< LED0 mask */
#define LED0_ON                (NRF_P0->OUTSET = LED0_MASK)    /**< Turn LED0 on */
#define LED0_OFF               (NRF_P0->OUTCLR = LED0_MASK)    /**< Turn LED0 off */
#define LED0_TOGGLE            (NRF_P0->OUT   ^= LED0_MASK)    /**< Toggle LED0 */
/** @} */

/**
 * @name    Backlight control defines, default uses LCD_BACKLIGHT_LOW values
 * @{
 */
#ifndef BACKLIGHT_MASK
#define BACKLIGHT_MASK              (1 << 8)
#endif
#define BACKLIGHT_ON                (NRF_P0->OUTCLR = BACKLIGHT_MASK)
#define BACKLIGHT_OFF               (NRF_P0->OUTSET = BACKLIGHT_MASK)
/** @ */

/**
 * @name LCD configuration
 * @{
 */
/** @} */

/**
 * @name NOR flash hardware configuration
 * @{
 */
#define BANGLEJS2_NOR_PAGE_SIZE          (256)
#define BANGLEJS2_NOR_PAGES_PER_SECTOR   (16)
#define BANGLEJS2_NOR_SECTOR_COUNT       (2048)
#define BANGLEJS2_NOR_FLAGS              (SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_32K)
#define BANGLEJS2_NOR_SPI_DEV            SPI_FLASH_DEV
#define BANGLEJS2_NOR_SPI_CLK            SPI_CLK_10MHZ
#define BANGLEJS2_NOR_SPI_CS             SPI_FLASH_CS
#define BANGLEJS2_NOR_SPI_MODE           SPI_MODE_3
/** @} */

/**
 * @name MTD configuration
 * @{
 */
extern mtd_dev_t *mtd0;
#define MTD_0 mtd0
/** @} */

/**
 * @name Touch screen configuration
 * @{
 */
#define CST816S_PARAM_I2C_DEV        TOUCH_I2C_DEV
#define CST816S_PARAM_I2C_ADDR       TOUCH_I2C_ADDR
#define CST816S_PARAM_IRQ            TOUCH_INT
#define CST816S_PARAM_IRQ_FLANK      GPIO_FALLING
#define CST816S_PARAM_RESET          TOUCH_RESET
/** @} */

void board_power_off(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
