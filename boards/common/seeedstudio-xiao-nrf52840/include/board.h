/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_seeedstudio-xiao-nrf52840
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Seeed Studio XIAO nRF52840
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "board_common.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 26)
#define LED1_PIN            GPIO_PIN(0, 30)
#define LED2_PIN            GPIO_PIN(0, 6)

#define LED_PORT            (NRF_P0)
#define LED0_MASK           (1 << 26)
#define LED1_MASK           (1 << 30)
#define LED2_MASK           (1 << 6)
#define LED_MASK            (LED0_MASK | LED1_MASK | LED2_MASK)

#define LED0_ON             (LED_PORT->OUTCLR = LED0_MASK)
#define LED0_OFF            (LED_PORT->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK)

#define LED1_ON             (LED_PORT->OUTCLR = LED1_MASK)
#define LED1_OFF            (LED_PORT->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->OUT   ^= LED1_MASK)

#define LED2_ON             (LED_PORT->OUTCLR = LED2_MASK)
#define LED2_OFF            (LED_PORT->OUTSET = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT->OUT   ^= LED2_MASK)
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
#define XIAO_NRF52840_NOR_PAGE_SIZE          (256)
#define XIAO_NRF52840_NOR_PAGES_PER_SECTOR   (16)
#define XIAO_NRF52840_NOR_SECTOR_COUNT       (512)
#define XIAO_NRF52840_NOR_FLAGS \
    (SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_32K | SPI_NOR_F_SECT_64K)
#define XIAO_NRF52840_NOR_SPI_DEV            SPI_DEV(1)
#define XIAO_NRF52840_NOR_SPI_CLK            SPI_CLK_10MHZ
#define XIAO_NRF52840_NOR_SPI_CS             GPIO_PIN(0, 25)
#define XIAO_NRF52840_NOR_SPI_WP             GPIO_PIN(0, 22)
#define XIAO_NRF52840_NOR_SPI_HOLD           GPIO_PIN(0, 23)
#define XIAO_NRF52840_NOR_SPI_MODE           SPI_MODE_0
/** @} */

/** Default MTD device */
#define MTD_0 mtd_dev_get(0)

/**
 * @name    ztimer configuration values
 * @{
 */
#define CONFIG_ZTIMER_USEC_ADJUST_SET    7
#define CONFIG_ZTIMER_USEC_ADJUST_SLEEP  22
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
