/*
 * Copyright (C) 2014 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup     boards_mulle
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Eistec Mulle IoT board
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include "cpu.h"
#include "periph_conf.h"
#include "mulle-nvram.h"

/* Use the on board RTC 32kHz clock for LPTMR clocking. */
#undef LPTIMER_CLKSRC
/** @brief Clock source for the LPTMR module */
#define LPTIMER_CLKSRC LPTIMER_CLKSRC_ERCLK32K

/** Disable hardware watchdog, for debugging purposes, don't use this on production builds. */
#define DISABLE_WDOG    1

/**
 * @brief Use the UART1 for STDIO on this board
 */
#define STDIO_UART_DEV      UART_DEV(1)

/**
 * @name    xtimer configuration
 * @{
 */
#if 0
/* LPTMR xtimer configuration */
/* WIP, Use PIT for now */
#define XTIMER_DEV                  (TIMER_LPTMR_DEV(0))
/* LPTMR is 16 bits wide */
#define XTIMER_WIDTH                (16)
#define XTIMER_BACKOFF              (4)
#define XTIMER_ISR_BACKOFF          (4)
#define XTIMER_HZ                   (32768ul)
#else
/* PIT xtimer configuration */
#define XTIMER_DEV                  (TIMER_PIT_DEV(0))
#define XTIMER_CHAN                 (0)
#define XTIMER_BACKOFF              (40)
#define XTIMER_ISR_BACKOFF          (40)
#endif
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED_PORT            PTC
#define LED0_BIT            (15)
#define LED1_BIT            (14)
#define LED2_BIT            (13)

#define LED0_PIN            GPIO_PIN(PORT_C, LED0_BIT)
#define LED1_PIN            GPIO_PIN(PORT_C, LED1_BIT)
#define LED2_PIN            GPIO_PIN(PORT_C, LED2_BIT)

#define LED0_ON             (LED_PORT->PSOR = (1 << LED0_BIT))
#define LED0_OFF            (LED_PORT->PCOR = (1 << LED0_BIT))
#define LED0_TOGGLE         (LED_PORT->PTOR = (1 << LED0_BIT))

#define LED1_ON             (LED_PORT->PSOR = (1 << LED1_BIT))
#define LED1_OFF            (LED_PORT->PCOR = (1 << LED1_BIT))
#define LED1_TOGGLE         (LED_PORT->PTOR = (1 << LED1_BIT))

#define LED2_ON             (LED_PORT->PSOR = (1 << LED2_BIT))
#define LED2_OFF            (LED_PORT->PCOR = (1 << LED2_BIT))
#define LED2_TOGGLE         (LED_PORT->PTOR = (1 << LED2_BIT))
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/**
 * @name Define the interface to the AT86RF212B radio
 * @{
 */

 /** @todo Work around missing RESET pin on Mulle v0.6x
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 */
#define AT86RF2XX_PARAM_CS             SPI_HWCS(1)
#define AT86RF2XX_PARAM_INT            GPIO_PIN(PORT_B, 9)
#define AT86RF2XX_PARAM_SLEEP          GPIO_PIN(PORT_E, 6)
#define AT86RF2XX_PARAM_RESET          GPIO_PIN(PORT_C, 12)
/** @} */

/**
 * @name LIS3DH configuration
 * @{
 */
#define LIS3DH_PARAM_INT1           GPIO_PIN(PORT_C, 18)
#define LIS3DH_PARAM_INT2           GPIO_PIN(PORT_C, 17)
#define LIS3DH_PARAM_CS             SPI_HWCS(0)
#define LIS3DH_PARAM_CLK            (SPI_CLK_5MHZ)
/** @} */

/**
 * @name Mulle power control configuration
 */
/** @{ */
#define MULLE_POWER_AVDD        GPIO_PIN(PORT_B, 17) /**< AVDD enable pin */
#define MULLE_POWER_VPERIPH     GPIO_PIN(PORT_D,  7) /**< VPERIPH enable pin */
#define MULLE_POWER_VSEC        GPIO_PIN(PORT_B, 16) /**< VSEC enable pin */
/** @} */

/**
 * @name Mulle NVRAM hardware configuration
 */
/** @{ */
#define MULLE_NVRAM_SPI_DEV             SPI_DEV(0)
#define MULLE_NVRAM_SPI_CLK             SPI_CLK_5MHZ
#define MULLE_NVRAM_SPI_CS              SPI_HWCS(3) /**< FRAM CS pin */
#define MULLE_NVRAM_CAPACITY            512     /**< FRAM size, in bytes */
#define MULLE_NVRAM_SPI_ADDRESS_COUNT   1       /**< FRAM addressing size, in bytes */
/** @} */

/**
 * @name Mulle NOR flash hardware configuration
 */
/** @{ */
#define MULLE_NOR_SPI_DEV               SPI_DEV(0)
#define MULLE_NOR_SPI_CLK               SPI_CLK_5MHZ
#define MULLE_NOR_SPI_CS                SPI_HWCS(2) /**< Flash CS pin */
/** @} */
/**
 * @name MTD configuration
 */
/** @{ */
#define MTD_0 mtd_dev_get(0)
/** @} */

/**
 * @name Mulle Vchr, Vbat ADC lines
 */
/** @{ */
#define MULLE_VBAT_ADC_LINE           ADC_LINE(6)
#define MULLE_VCHR_ADC_LINE           ADC_LINE(7)
/** @} */
/** @} */
