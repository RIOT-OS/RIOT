/*
 * Copyright (C) 2017-2020 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_ikea-tradfri
 * @{
 *
 * @file
 * @brief       Board specific definitions for the IKEA TRÅDFRI modules
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#include "mtd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Xtimer configuration
 *
 * The timer runs at 250 kHz to increase accuracy, or at 32.768 kHz if
 * LETIMER is used.
 * @{
 */
#if IS_ACTIVE(CONFIG_EFM32_XTIMER_USE_LETIMER)
#define XTIMER_DEV          (TIMER_DEV(1))
#define XTIMER_HZ           (32768UL)
#define XTIMER_WIDTH        (16)
#else
#define XTIMER_DEV          (TIMER_DEV(0))
#define XTIMER_HZ           (250000UL)
#define XTIMER_WIDTH        (16)
#endif
#define XTIMER_CHAN         (0)
/** @} */

/**
 * @name    LED pin definitions
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 1)
#define LED1_PIN            GPIO_PIN(PB, 13)
/** @} */

/**
 * @name    Macros for controlling the on-board LEDs
 * @{
 */
#define LED0_ON             gpio_set(LED0_PIN)
#define LED0_OFF            gpio_clear(LED0_PIN)
#define LED0_TOGGLE         gpio_toggle(LED0_PIN)
#define LED1_ON             gpio_set(LED1_PIN)
#define LED1_OFF            gpio_clear(LED1_PIN)
#define LED1_TOGGLE         gpio_toggle(LED1_PIN)
/** @} */

/**
 * @name    SPI NOR Flash hardware configuration
 *
 * The board has a IS25LQ020B flash chip (2MBit).
 */
/** @{ */
#define IKEA_TRADFRI_NOR_PAGE_SIZE          (256)
#define IKEA_TRADFRI_NOR_PAGES_PER_SECTOR   (16)
#define IKEA_TRADFRI_NOR_SECTOR_COUNT       (64)
#define IKEA_TRADFRI_NOR_FLAGS              (SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_32K)
#define IKEA_TRADFRI_NOR_SPI_DEV            SPI_DEV(0)
#define IKEA_TRADFRI_NOR_SPI_CLK            SPI_CLK_1MHZ
#define IKEA_TRADFRI_NOR_SPI_CS             GPIO_PIN(PB, 11)
#define IKEA_TRADFRI_NOR_SPI_MODE           SPI_MODE_0
#define IKEA_TRADFRI_NOR_EN                 GPIO_PIN(PF, 3) /**< only on the ICC-1-A */
/** @} */

/**
 * @name    MTD configuration
 */
/** @{ */
extern mtd_dev_t *mtd0;
#define MTD_0 mtd0
/** @} */

/**
 * @brief   Initialize the board (GPIO, sensors, clocks).
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
