/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_lpc11u34
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Paul RATHGEB <paul.rathgeb@skynet.be>
 * @author          Hauke Petersen<hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)

/**
 * @brief   Define number of available ADC lines
 *
 * TODO: check this value
 */
#define ADC_NUMOF           (10U)

/**
 * @brief   Override the default GPIO type
 * @{
 */
#define HAVE_GPIO_T
typedef uint16_t gpio_t;
/** @} */

/**
 * @brief   Define a custom GPIO_PIN macro for the lpc11u34
 */
#define GPIO_PIN(port, pin)     (gpio_t)((port << 8) | pin)

/**
 * @brief   Number of PWM channels per PWM peripheral
 */
#define PWM_CHAN_NUMOF      (3U)

/**
 * @brief   Override the default GPIO mode values
 * @{
 */
#define IN                  (0x0000)
#define OUT                 (0x0001)
#define PD                  (0x1 << 3)
#define PU                  (0x2 << 3)
#define OD                  (0x1 << 10)

#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = (IN),              /**< in without pull resistor */
    GPIO_IN_PD = (IN | PD),         /**< in with pull-down */
    GPIO_IN_PU = (IN | PU),         /**< in with pull-up */
    GPIO_OUT   = (OUT),             /**< push-pull output */
    GPIO_OD    = (OUT | OD),        /**< open-drain output */
    GPIO_OD_PU = (OUT | OD | PU)    /**< open-drain output with pull-up */
} gpio_mode_t;
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override the ADC resolution settings
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT = 0,       /**< ADC resolution: 6 bit */
    ADC_RES_8BIT,           /**< ADC resolution: 8 bit */
    ADC_RES_10BIT,          /**< ADC resolution: 10 bit */
    ADC_RES_12BIT,          /**< ADC resolution: 12 bit */
    ADC_RES_14BIT,          /**< ADC resolution: 14 bit */
    ADC_RES_16BIT,          /**< ADC resolution: 16 bit */
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   PWM configuration
 */
typedef struct {
    LPC_CTxxBx_Type *dev;                   /**< PWM device */
    __IO uint32_t *pins[PWM_CHAN_NUMOF];    /**< set to NULL if channel is not used */
    uint16_t clk_bit;                       /**< clock enable bit */
    uint8_t af;                             /**< alternate pin function */
} pwm_conf_t;

/**
 * @brief   Override SPI clock speed values
 *
 * @note    The values expect the CPU to run at 12MHz
 * @todo    Generalize the SPI driver
 *
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = 119,       /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ =  29,       /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   =  11,       /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ   =   2,       /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  =   0        /**< actual: 12 MHz */
} spi_clk_t;
/** @} */

/**
 * @brief   SPI configuration data
 */
typedef struct {
    LPC_SSPx_Type *dev;     /**< SPI device to configure */
    uint32_t preset_bit;    /**< mask of the corresponding preset bit */
    uint32_t ahb_bit;       /**< mask of the corresponding AHB bit */
} spi_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
