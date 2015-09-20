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

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "periph_conf.h"

/* Use the on board RTC 32kHz clock for LPTMR clocking. */
#undef LPTIMER_CLKSRC
/** @brief Clock source for the LPTMR module */
#define LPTIMER_CLKSRC LPTIMER_CLKSRC_ERCLK32K

/** Disable hardware watchdog, for debugging purposes, don't use this on production builds. */
#define DISABLE_WDOG    1

/**
 * @name Define UART device and baudrate for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name LEDs configuration
 * @{
 */

#define LED_RED_GPIO        GPIO_0
#define LED_RED_PORT        GPIO_0_DEV
#define LED_RED_PIN         GPIO_0_PIN
#define LED_YELLOW_GPIO     GPIO_1
#define LED_YELLOW_PORT     GPIO_1_DEV
#define LED_YELLOW_PIN      GPIO_1_PIN
#define LED_GREEN_GPIO      GPIO_2
#define LED_GREEN_PORT      GPIO_2_DEV
#define LED_GREEN_PIN       GPIO_2_PIN

/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_ON          (BITBAND_REG32(LED_RED_PORT->PSOR, LED_RED_PIN) = 1)
#define LED_RED_OFF         (BITBAND_REG32(LED_RED_PORT->PCOR, LED_RED_PIN) = 1)
#define LED_RED_TOGGLE      (BITBAND_REG32(LED_RED_PORT->PTOR, LED_RED_PIN) = 1)
#define LED_YELLOW_ON       (BITBAND_REG32(LED_YELLOW_PORT->PSOR, LED_YELLOW_PIN) = 1)
#define LED_YELLOW_OFF      (BITBAND_REG32(LED_YELLOW_PORT->PCOR, LED_YELLOW_PIN) = 1)
#define LED_YELLOW_TOGGLE   (BITBAND_REG32(LED_YELLOW_PORT->PTOR, LED_YELLOW_PIN) = 1)
#define LED_GREEN_ON        (BITBAND_REG32(LED_GREEN_PORT->PSOR, LED_GREEN_PIN) = 1)
#define LED_GREEN_OFF       (BITBAND_REG32(LED_GREEN_PORT->PCOR, LED_GREEN_PIN) = 1)
#define LED_GREEN_TOGGLE    (BITBAND_REG32(LED_GREEN_PORT->PTOR, LED_GREEN_PIN) = 1)
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
 * @{
 */
#define AT86RF231_SPI       SPI_0
#define AT86RF231_CS        GPIO_14
#define AT86RF231_INT       GPIO_12
/** @todo work around missing RESET pin on Mulle v0.6x */
#define AT86RF231_RESET     GPIO_5
#define AT86RF231_SLEEP     GPIO_13
#define AT86RF231_SPI_CLK   SPI_SPEED_5MHZ
/** @} */

/**
 * @name LIS3DH configuration
 * @{
 */

#define LIS3DH_INT1   GPIO_3
#define LIS3DH_INT2   GPIO_4
#define LIS3DH_CS     GPIO_15
#define LIS3DH_SPI    SPI_2

/** @} */

/**
 * @name Mulle power control configuration
 */
/** @{ */
#define MULLE_POWER_AVDD    GPIO_6 /**< AVDD enable pin */
#define MULLE_POWER_VPERIPH GPIO_7 /**< VPERIPH enable pin */
#define MULLE_POWER_VSEC    GPIO_5 /**< VSEC enable pin */
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

#endif /* BOARD_H_ */
/** @} */
