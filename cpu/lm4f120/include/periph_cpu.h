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

#ifndef PERIPH_CPU_H_
#define PERIPH_CPU_H_

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

/**
 * @brief   Override values for pull register configuration
 * @{
 */
#define HAVE_GPIO_PP_T
typedef enum {
  GPIO_NOPULL = GPIO_PIN_TYPE_STD,          /**< do not use internal pull resistors */
  GPIO_PULLUP = GPIO_PIN_TYPE_STD_WPU,	    /**< enable internal pull-up resistor */
  GPIO_PULLDOWN = GPIO_PIN_TYPE_STD_WPD	    /**< enable internal pull-down resistor */
} gpio_pp_t;
/** @} */


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

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
