/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_cc3200_gpio CC3200 General-Purpose I/O
 * @ingroup         cpu_cc3200_regs
 * @{
 *
 * @file
 * @brief           Driver for the cc3200 GPIO controller
 *
 * Header file with register and macro declarations for the cc3200 GPIO module
 *
 * @author          Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 *
 * @{
 */

#ifndef CC3200_GPIO_H
#define CC3200_GPIO_H

#include "cc3200.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @name    Define a custom type for GPIO pins
 * @{
 */
#define HAVE_GPIO_T
/** @} */

/**
 * @name Numeric representation of the four GPIO ports
 * @{
 */
enum {
    PORT_A0 = 0,
    PORT_A1 = 1,
    PORT_A2 = 2,
    PORT_A3 = 3,
};
/** @} */

/**
 * @brief gpio high and low macros
 * @{
 */
#define GPIO_VALUE_HIGH (1)
#define GPIO_VALUE_LOW (0)
/** @} */

/**
 * @name    Define a custom type for GPIO pins
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @name   Override the default GPIO mode settings
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN     = 0x00000000,   /**< input, no pull */
    GPIO_OUT    = 0x00000001,   /**< output */
    GPIO_OD     = 0x00000002,   /**< open drain */
    GPIO_OD_PU  = 0x00000106,   /**< open drain pull-up */
    GPIO_IN_PU  = 0x00000107,   /**< input, pull-up */
    GPIO_IN_PD  = 0x00000208,   /**< input, pull-down */
} gpio_mode_t;
/** @} */

/**
 * @brief   Override mode flank selection values
 *
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING    = 0,    /**< emit interrupt on falling flank */
    GPIO_BOTH       = 1,    /**< emit interrupt on both flanks   */
    GPIO_LOW        = 2,    /**< emit interrupt on low level     */
    GPIO_RISING     = 4,    /**< emit interrupt on rising flank  */
    GPIO_NONE       = 5,    /**< no interrupt  */
    GPIO_HIGH       = 6     /**< emit interrupt on low level     */
} gpio_flank_t;
/** @} */

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 * @{
 */
#define GPIO_PIN(x, y) ((x << 6) | (y - 1))
/** @} */

/**
 * @brief GPIO register
 * @{
 */
typedef uint32_t cc3200_gpio_reg;
/** @} */

/**
 * @brief GPIO device register
 * @{
 */
typedef struct cc3200_gpio_t {
    cc3200_gpio_reg data;           /**< GPIO Data register */
    cc3200_gpio_reg RESERVER1[255]; /**< GPIO Reserved addresses */
    cc3200_gpio_reg dir;            /**< GPIO Direction register */
    cc3200_gpio_reg is;             /**< GPIO Interrupt Sense */
    cc3200_gpio_reg ibe;            /**< GPIO Interrupt Both Edges */
    cc3200_gpio_reg iev;            /**< GPIO Interrupt Event */
    cc3200_gpio_reg im;             /**< GPIO Interrupt Mask */
    cc3200_gpio_reg ris;            /**< GPIO Raw Interrupt Status */
    cc3200_gpio_reg mis;            /**< GPIO Masked Interrupt Status */
    cc3200_gpio_reg icr;            /**< GPIO Interrupt Clear */
} cc3200_gpio_t;
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC3200_GPIO_H */

/** @} */
