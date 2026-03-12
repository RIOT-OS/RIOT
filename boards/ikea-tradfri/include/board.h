/*
 * SPDX-FileCopyrightText: 2017-2020 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_ikea-tradfri
 * @{
 *
 * @file
 * @brief       Board specific definitions for the IKEA TRÅDFRI modules
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ztimer configuration
 *
 * The timer runs at 250 kHz to increase accuracy, or at 32.768 kHz if
 * LETIMER is used.
 * @{
 */
#if IS_ACTIVE(CONFIG_EFM32_ZTIMER_USE_LETIMER)
#  define CONFIG_ZTIMER_USEC_DEV            (TIMER_DEV(1))  /**< Timer peripheral for ztimer */
#  define CONFIG_ZTIMER_USEC_BASE_FREQ      (32768UL)       /**< Running at 32.768 kHz */
#  define CONFIG_ZTIMER_USEC_WIDTH          (16)            /**< Running on a 16-bit timer */
#  define CONFIG_ZTIMER_USEC_ADJUST_SET     (68)            /**< Overhead for ztimer_set */
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP   (37)            /**< Overhead for ztimer_sleep */
#else
#  define CONFIG_ZTIMER_USEC_DEV            (TIMER_DEV(0))  /**< Timer peripheral for ztimer */
#  define CONFIG_ZTIMER_USEC_BASE_FREQ      (250000UL)      /**< Running at 250 kHz */
#  define CONFIG_ZTIMER_USEC_WIDTH          (16)            /**< Running on a 16-bit timer */
#  define CONFIG_ZTIMER_USEC_ADJUST_SET     (44)            /**< Overhead for ztimer_set */
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP   (44)            /**< Overhead for ztimer_sleep */
#endif
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
 * @name    Core temperature sensor configuration
 *
 * Connection to the on-chip temperature sensor.
 * @{
 */
#define CORETEMP_ADC        ADC_LINE(0)
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
#define MTD_0 mtd_dev_get(0)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
