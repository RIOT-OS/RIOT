/*
 * Copyright (C) 2014 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_mulle Eistec Mulle
 * @ingroup     boards
 * @brief       Board specific files for Eistec Mulle IoT boards
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Eistec Mulle IoT board
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef BOARD_H
#define BOARD_H

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
 * @brief   xtimer configuration
 * @{
 */
#if 0
/* LPTMR xtimer configuration */
/* WIP, Use PIT for now */
#define XTIMER_DEV                  (TIMER_LPTMR_DEV(0))
#define XTIMER_CHAN                 (0)
/* LPTMR is 16 bits wide */
#define XTIMER_WIDTH                (16)
#define XTIMER_BACKOFF              (4)
#define XTIMER_ISR_BACKOFF          (4)
#define XTIMER_OVERHEAD             (3)
#define XTIMER_HZ                   (32768ul)
#define XTIMER_SHIFT                (0)
#else
/* PIT xtimer configuration */
#define XTIMER_DEV                  (TIMER_PIT_DEV(0))
#define XTIMER_CHAN                 (0)
#define XTIMER_WIDTH                (32)
#define XTIMER_BACKOFF              (40)
#define XTIMER_ISR_BACKOFF          (40)
#define XTIMER_OVERHEAD             (30)
#define XTIMER_HZ                   (1000000ul)
#define XTIMER_SHIFT                (0)
#endif
/** @} */

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED_PORT            PTC
#define LED0_BIT            (15)
#define LED1_BIT            (14)
#define LED2_BIT            (13)

#define LED0_PIN            GPIO_PIN(PORT_C, LED0_BIT)
#define LED1_PIN            GPIO_PIN(PORT_C, LED1_BIT)
#define LED2_PIN            GPIO_PIN(PORT_C, LED2_BIT)

#define LED0_ON             (BITBAND_REG32(LED_PORT->PSOR, LED0_BIT) = 1)
#define LED0_OFF            (BITBAND_REG32(LED_PORT->PCOR, LED0_BIT) = 1)
#define LED0_TOGGLE         (BITBAND_REG32(LED_PORT->PTOR, LED0_BIT) = 1)

#define LED1_ON             (BITBAND_REG32(LED_PORT->PSOR, LED1_BIT) = 1)
#define LED1_OFF            (BITBAND_REG32(LED_PORT->PCOR, LED1_BIT) = 1)
#define LED1_TOGGLE         (BITBAND_REG32(LED_PORT->PTOR, LED1_BIT) = 1)

#define LED2_ON             (BITBAND_REG32(LED_PORT->PSOR, LED2_BIT) = 1)
#define LED2_OFF            (BITBAND_REG32(LED_PORT->PCOR, LED2_BIT) = 1)
#define LED2_TOGGLE         (BITBAND_REG32(LED_PORT->PTOR, LED2_BIT) = 1)
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

/**
 * @name Define the interface to the AT86RF212B radio
 *
 * @todo Work around missing RESET pin on Mulle v0.6x
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 */
#define AT86RF2XX_PARAMS_BOARD      {.spi = SPI_DEV(0), \
                                     .spi_clk = SPI_CLK_5MHZ, \
                                     .cs_pin = GPIO_PIN(PORT_D, 4), \
                                     .int_pin = GPIO_PIN(PORT_B, 9), \
                                     .sleep_pin = GPIO_PIN(PORT_E, 6), \
                                     .reset_pin = GPIO_PIN(PORT_C, 12)}


/**
 * @name LIS3DH configuration
 * @{
 */
#define LIS3DH_INT1                 GPIO_PIN(PORT_C, 18)
#define LIS3DH_INT2                 GPIO_PIN(PORT_C, 17)
#define LIS3DH_CS                   GPIO_PIN(PORT_D, 0)
#define LIS3DH_CLK                  SPI_CLK_5MHZ
#define LIS3DH_SPI                  SPI_DEV(0)
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
#define MULLE_NVRAM_SPI_CS              GPIO_PIN(PORT_D, 6) /**< FRAM CS pin */
#define MULLE_NVRAM_CAPACITY            512     /**< FRAM size, in bytes */
#define MULLE_NVRAM_SPI_ADDRESS_COUNT   1       /**< FRAM addressing size, in bytes */
/** @} */

/**
 * @name Mulle Vchr, Vbat ADC lines
 */
/** @{ */
#define MULLE_VBAT_ADC_LINE           ADC_LINE(6)
#define MULLE_VCHR_ADC_LINE           ADC_LINE(7)
/** @} */

/**
 * @name K60 clock dividers
 */
/** @{ */
/**
 * System clock divider setting, the actual hardware register value, see reference manual for details.
 */
#define CONFIG_CLOCK_K60_SYS_DIV 0x00

/**
 * Bus clock divider setting, the actual hardware register value, see reference manual for details
 */
#define CONFIG_CLOCK_K60_BUS_DIV 0x01

/**
 * Flexbus clock divider setting, the actual hardware register value, see reference manual for details
 */
#define CONFIG_CLOCK_K60_FB_DIV 0x01

/**
 * Flash clock divider setting, the actual hardware register value, see reference manual for details
 */
#define CONFIG_CLOCK_K60_FLASH_DIV 0x03

/** @} */

#endif /* BOARD_H */
/** @} */
