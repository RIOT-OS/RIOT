/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 Freie Universität Berlin
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega_common
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          René Herthel <rene-herthel@outlook.de>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef PERIPH_CPU_COMMON_H
#define PERIPH_CPU_COMMON_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Length of the CPU_ID in octets
 * @{
 */
#define CPUID_LEN           (4U)
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint8_t gpio_t;
/** @} */
#endif

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)          ((x << 4) | y)

#ifndef DOXYGEN
/**
 * @brief   Override the GPIO flanks
 *
 * This device has an additional mode in which the interrupt is triggered
 * when the pin is low.
 *
 * Enumeration order is important, do not modify.
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_LOW,          /**< emit interrupt when pin low */
    GPIO_BOTH,         /**< emit interrupt on both flanks */
    GPIO_FALLING,      /**< emit interrupt on falling flank */
    GPIO_RISING,       /**< emit interrupt on rising flank */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Use some common SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

#ifndef DOXYGEN
/**
 * @brief   SPI mode select macro
 *
 * The polarity is determined by bit 3 in the configuration register, the phase
 * by bit 2.
 */
#define SPI_MODE_SEL(pol, pha)          ((pol << 3) | (pha << 2))

/**
 * @brief   Override the SPI mode values
 *
 * As the mode is set in bit 3 and 2 of the configuration register, we put the
 * correct configuration there
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = SPI_MODE_SEL(0, 0),    /**< mode 0 */
    SPI_MODE_1 = SPI_MODE_SEL(0, 1),    /**< mode 1 */
    SPI_MODE_2 = SPI_MODE_SEL(1, 0),    /**< mode 2 */
    SPI_MODE_3 = SPI_MODE_SEL(1, 1)     /**< mode 3 */
} spi_mode_t;
/** @} */

/**
 * @brief   SPI speed selection macro
 *
 * We encode the speed in bits 2, 1, and 0, where bit0 and bit1 hold the SPCR
 * prescaler bits, while bit2 holds the SPI2X bit.
 */
#define SPI_CLK_SEL(s2x, pr1, pr0)    ((s2x << 2) | (pr1 << 1) | pr0)

/**
 * @brief   Override SPI speed values
 *
 * We assume a master clock speed of 16MHz here.
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = SPI_CLK_SEL(0, 1, 1),      /**< 16/128 -> 125KHz */
    SPI_CLK_400KHZ = SPI_CLK_SEL(1, 1, 0),      /**< 16/32  -> 500KHz */
    SPI_CLK_1MHZ   = SPI_CLK_SEL(0, 0, 1),      /**< 16/16  -> 1MHz */
    SPI_CLK_5MHZ   = SPI_CLK_SEL(0, 0, 0),      /**< 16/4   -> 4MHz */
    SPI_CLK_10MHZ  = SPI_CLK_SEL(1, 0, 0)       /**< 16/2   -> 8MHz */
} spi_clk_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief  Bitmasks indicating which are the possible dividers for a timer
 * @{
 */
typedef enum {
    TIMER_DIV1_8_64_128_1024 = 0x549,           /**< 1/{1,8,64,128,1024} */
    TIMER_DIV1_8_32_64_128_256_1024 = 0x5E9,    /**< 1/{1,8,32,64,128,256,1024} */
} timer_div_t;
/** @} */

/**
 * @brief   PWM configuration
 * @{
 */
typedef struct {
    mini_timer_t *dev;                  /**< Timer used */
    gpio_t pin_ch[2];                   /**< Output Pins */
    timer_div_t div;                    /**< Timer divider mask */
} pwm_conf_t;
/** @} */

/**
 * @brief   EEPROM clear byte
 */
#define EEPROM_CLEAR_BYTE              (0xff)

/**
 * @name    WDT upper and lower bound times in ms
 * @{
 */
#define NWDT_TIME_LOWER_LIMIT           (1)
#define NWDT_TIME_UPPER_LIMIT           (8192U)
/** @} */

/**
 * @brief   WDT can be stopped on AVR
 */
#define WDT_HAS_STOP                    (1)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_COMMON_H */
/** @} */
