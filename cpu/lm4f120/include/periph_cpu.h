/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com>
 * Copyright (C) 2017 Marc Poulhiès <dkm@kataplop.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_lm4f120
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Rakendra Thapa <rakendrathapa@gmail.com>
 * @author          Marc Poulhiès <dkm@kataplop.net>
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
#define GPIO_PIN(x, y) ((gpio_t)((x<<4) | y))
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override GPIO modes
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = (GPIO_DIR_MODE_IN | (GPIO_PIN_TYPE_STD << 4)),     /**< IN */
    GPIO_IN_PD = (GPIO_DIR_MODE_IN | (GPIO_PIN_TYPE_STD_WPD << 4)), /**< IN with pull-down */
    GPIO_IN_PU = (GPIO_DIR_MODE_IN | (GPIO_PIN_TYPE_STD_WPU << 4)), /**< IN with pull-up */
    GPIO_OUT   = (GPIO_DIR_MODE_OUT | (GPIO_PIN_TYPE_STD << 4)),    /**< OUT (push-pull) */
    GPIO_OD    = (GPIO_DIR_MODE_OUT | (GPIO_PIN_TYPE_OD << 4)),     /**< OD */
    GPIO_OD_PU = (GPIO_DIR_MODE_OUT | (GPIO_PIN_TYPE_OD_WPU << 4)), /**< OD with pull-up */
} gpio_mode_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Override values for pin direction configuration
 * @{
 */
#define HAVE_GPIO_DIR_T
typedef enum {
  GPIO_DIR_IN = GPIO_DIR_MODE_IN,       /**< configure pin as input */
  GPIO_DIR_OUT = GPIO_DIR_MODE_OUT      /**< configure pin as output */
} gpio_dir_t;
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override active flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = GPIO_FALLING_EDGE,    /**< emit interrupt on falling flank */
    GPIO_RISING = GPIO_RISING_EDGE,      /**< emit interrupt on rising flank */
    GPIO_BOTH = GPIO_BOTH_EDGES          /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Available ports on the LM4F120
 */
enum {
  PORT_A = 0,       /**< port A */
  PORT_B = 1,       /**< port B */
  PORT_C = 2,       /**< port C */
  PORT_D = 3,       /**< port D */
  PORT_E = 4,       /**< port E */
  PORT_F = 5,       /**< port F */
};

/**
 * @brief   Override resolution options
 */
#ifndef DOXYGEN
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0xa00,            /**< not supported by hardware */
    ADC_RES_8BIT  = 0xb00,            /**< not supported by hardware */
    ADC_RES_10BIT = ADC_RES_10BIT_S,  /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = ADC_RES_12BIT_S,  /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 0xc00,            /**< not supported by hardware */
    ADC_RES_16BIT = 0xd00,            /**< not supported by hardware */
} adc_res_t;
#endif /* ndef DOXYGEN */

/**
 * @brief   Define timer configuration values
 */
typedef struct {
    uint32_t dev;       /**< Address of timer base */
    uint32_t max;       /**< Max tick value of timer */
    int irqn;           /**< Number of the higher timer IRQ channel */
    uint32_t sysctl;    /**< Address of timer system control */
    uint32_t intbase;   /**< Interrupt base of timer */
    int channels;       /**< Number of channels for the timer */
} timer_conf_t;

/**
 * @brief   Override SPI hardware chip select macro
 *
 * As of now, we do not support HW CS, so we always set it to a fixed value
 */
#define SPI_HWCS(x)     (UINT_MAX - 1)

/**
 * @brief   SPI configuration data structure
 * @{
 */
typedef struct {
    unsigned long ssi_sysctl;          /**< SSI device in sysctl */
    unsigned long ssi_base;            /**< SSI base address */
    unsigned long gpio_sysctl;         /**< GPIO device in sysctl */
    unsigned long gpio_port;           /**< GPIO port */
    struct {
        unsigned long clk;             /**< pin used for SCK */
        unsigned long fss;             /**< pin used for FSS */
        unsigned long rx;              /**< pin used for MISO */
        unsigned long tx;              /**< pin used for MOSI */
        unsigned long mask;            /**< Pin mask */
    } pins;                            /**< Pin setting */
} spi_conf_t;
/** @} */

/**
 * @brief declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE 1
#define PERIPH_SPI_NEEDS_TRANSFER_REG 1
#define PERIPH_SPI_NEEDS_TRANSFER_REGS 1
#define PERIPH_SPI_NEEDS_INIT_CS 1
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override SPI clock speed values
 * @{
 */
#define HAVE_SPI_CLK_T 1
typedef enum {
    SPI_CLK_100KHZ = 100000,        /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ = 400000,        /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   = 1000000,       /**< drive the SPI bus with 1MHz */
    SPI_CLK_4MHZ   = 4000000,       /**< drive the SPI bus with 4MHz */
    SPI_CLK_5MHZ   = 5000000,       /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = 10000000,      /**< drive the SPI bus with 10MHz */
} spi_clk_t;
/** @} */

/**
 * @brief   Override SPI mode settings
 * @{
 */
#define HAVE_SPI_MODE_T 1
typedef enum {
    SPI_MODE_0 = SSI_FRF_MOTO_MODE_0,       /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = SSI_FRF_MOTO_MODE_1,       /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = SSI_FRF_MOTO_MODE_2,       /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = SSI_FRF_MOTO_MODE_0,       /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */
#endif /* ndef DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
