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

#ifndef PERIPH_CPU_H_
#define PERIPH_CPU_H_

#include <stdint.h>
#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTES
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
#define GPIO_PIN(port, pin)     (gpio_t)((port << 16) | pin)

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

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
