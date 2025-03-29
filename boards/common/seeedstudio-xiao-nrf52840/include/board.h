/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BOARD_H
#define BOARD_H
/**
 * @ingroup     boards_seeedstudio-xiao-nrf52840
 * @ingroup     boards_seeedstudio-xiao-nrf52840-sense
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Seeed Studio XIAO nRF52840
 *              and Seeed Studio XIAO nRF52840 Sense
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 */

#include "cpu.h"
#include "board_common.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @brief   The Seeedstudio Xiao nRF52840 boards have an RGB LED next to the
 *          USB-C connector.
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 26) /**< Pin Definition for the Red LED */
#define LED1_PIN            GPIO_PIN(0, 30) /**< Pin Definition for the Green LED */
#define LED2_PIN            GPIO_PIN(0, 6)  /**< Pin Definition for the Blue LED */

#define LED_PORT            (NRF_P0)        /**< GPIO Port of the LED GPIOs */
#define LED0_MASK           (1 << 26)       /**< Bit position of the Red LED GPIO Output Bit */
#define LED1_MASK           (1 << 30)       /**< Bit position of the Green LED GPIO Output Bit */
#define LED2_MASK           (1 << 6)        /**< Bit position of the Blue LED GPIO Output Bit */
#define LED_MASK            (LED0_MASK | LED1_MASK | LED2_MASK) /**< Bit position of all LED
                                                                     GPIO Output Bits  */

#define LED0_ON             (LED_PORT->OUTCLR = LED0_MASK)  /**< Turn the Red LED on */
#define LED0_OFF            (LED_PORT->OUTSET = LED0_MASK)  /**< Turn the Red LED off */
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK)  /**< Toggle the Red LED */

#define LED1_ON             (LED_PORT->OUTCLR = LED1_MASK)  /**< Turn the Green LED on */
#define LED1_OFF            (LED_PORT->OUTSET = LED1_MASK)  /**< Turn the Green LED off */
#define LED1_TOGGLE         (LED_PORT->OUT   ^= LED1_MASK)  /**< Toggle the Green LED */

#define LED2_ON             (LED_PORT->OUTCLR = LED2_MASK)  /**< Turn the Blue LED on */
#define LED2_OFF            (LED_PORT->OUTSET = LED2_MASK)  /**< Turn the Blue LED off */
#define LED2_TOGGLE         (LED_PORT->OUT   ^= LED2_MASK)  /**< Toggle the Blue LED */
/** @} */

/**
 * @name SPI NOR flash hardware configuration
 *
 * A 2MB P25Q16H flash is present on the board.
 *
 * @see https://files.seeedstudio.com/wiki/github_weiruanexample/Flash_P25Q16H-UXH-IR_Datasheet.pdf
 *
 * @{
 */
#define XIAO_NRF52840_NOR_PAGE_SIZE         (256)   /**< Page Size of the P25Q16H flash */
#define XIAO_NRF52840_NOR_PAGES_PER_SECTOR  (16)  /**< Page Count per Sector of the P25Q16H flash */
#define XIAO_NRF52840_NOR_SECTOR_COUNT      (512)   /**< Sector Count of the P25Q16H flash */
#define XIAO_NRF52840_NOR_FLAGS \
    (SPI_NOR_F_SECT_4K | \
     SPI_NOR_F_SECT_32K | \
     SPI_NOR_F_SECT_64K)                            /**< Supported Modes of the P25Q16H flash */
#define XIAO_NRF52840_NOR_SPI_DEV   SPI_DEV(1)      /**< SPI Bus of the P25Q16H flash */
#define XIAO_NRF52840_NOR_SPI_CLK   SPI_CLK_10MHZ   /**< SPI Clock Frequency */
#define XIAO_NRF52840_NOR_SPI_CS    GPIO_PIN(0, 25) /**< Chip Select Pin of the P25Q16H flash */
#define XIAO_NRF52840_NOR_SPI_WP    GPIO_PIN(0, 22) /**< Write Protect Pin of the P25Q16H flash */
#define XIAO_NRF52840_NOR_SPI_HOLD  GPIO_PIN(0, 23) /**< Hold Pin of the P25Q16H flash */
#define XIAO_NRF52840_NOR_SPI_MODE  SPI_MODE_0      /**< SPI Clock Mode of the P25Q16H flash */
/** @} */

/** Default MTD device */
#define MTD_0 mtd_dev_get(0)

#ifdef BOARD_SEEEDSTUDIO_XIAO_NRF52840_SENSE
/**
 * @name    LSM6DS3TR-C IMU sensor configuration
 *
 * The IMU is directly supplied by GPIO Pin P1.08
 * @{
 */
#define LSM6DSXX_PARAM_I2C  I2C_DEV(1)      /**< I2C device of the IMU */
#define LSM6DSXX_PARAM_ADDR (0x6A)          /**< I2C address of the IMU  */

#define LSM6DS3_PWR_PIN     GPIO_PIN(1, 8)  /**< Pin of the IMU supply pin */
/** @} */
#endif

/**
 * @name    ztimer configuration values
 * @{
 */
#define CONFIG_ZTIMER_USEC_ADJUST_SET    7      /**< overhead for the ztimer_set function in us */
#define CONFIG_ZTIMER_USEC_ADJUST_SLEEP  22     /**< overhead for the ztimer_sleep function in us */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
