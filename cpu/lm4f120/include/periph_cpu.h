/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_lm4f120
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Rakendra Thapa <rakendrathapa@gmail.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph/dev_enums.h"
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (12U)

/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
#define GPIO_PIN(x,y) ((gpio_t)((x<<4) | y))
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

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
