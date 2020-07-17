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
 * @brief           CPU specific definitions for GPIOs
 *
 * @author          René Herthel <rene-herthel@outlook.de>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef GPIO_ARCH_H
#define GPIO_ARCH_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN
/**
 * @brief   Defines for port register addresses
 * @{
 */
#define GPIO_PORT_H          (7)
#define GPIO_BASE_PORT_A     (0x20)
#define GPIO_OFFSET_PORT_H   (0xCB)
#define GPIO_OFFSET_PIN_PORT (0x02)
#define GPIO_OFFSET_PIN_PIN  (0x03)
/** @} */

/**
 * @brief   Define a mask for GPIO port register addresses
 *
 * In the AVR architecture, the register addresses are between 0x000 and 0x1ff.
 * Pointers to data in SRAM start at 0x200.
 */
#define GPIO_CPU_PORT_MASK   (0xfe00)

/**
 * @brief   Base register address for MCU GPIO ports
 */
#define GPIO_CPU_PORT_BASE   (0x0000)

/**
 * @brief   Convert a MCU port number into its register address
 */
#ifndef PORTH
#define GPIO_CPU_PORT(x) \
        ((gpio_reg_t)((x * GPIO_OFFSET_PIN_PIN) + GPIO_BASE_PORT_A \
                                                + GPIO_OFFSET_PIN_PORT))
#else
#define GPIO_CPU_PORT(x) \
        ((gpio_reg_t)((x < GPIO_PORT_H) ? ((x * GPIO_OFFSET_PIN_PIN) + \
                                           GPIO_BASE_PORT_A + \
                                           GPIO_OFFSET_PIN_PORT) \
                                        : ((x * GPIO_OFFSET_PIN_PIN) + \
                                           GPIO_BASE_PORT_A + \
                                           GPIO_OFFSET_PIN_PORT + \
                                           GPIO_OFFSET_PORT_H)))
#endif

/**
 * @brief   Convert a MCU port register address into its port number
 */
#ifndef PORTH
#define GPIO_CPU_PORT_NUM(x) \
        ((gpio_pin_t)((x - GPIO_OFFSET_PIN_PORT - GPIO_BASE_PORT_A) / GPIO_OFFSET_PIN_PIN))
#else
#define GPIO_CPU_PORT_NUM(x) \
        ((gpio_pin_t)((x > GPIO_OFFSET_PORT_H) ? ((x - GPIO_OFFSET_PORT_H \
                                                     - GPIO_OFFSET_PIN_PORT \
                                                     - GPIO_BASE_PORT_A) / GPIO_OFFSET_PIN_PIN) \
                                               : ((x - GPIO_OFFSET_PIN_PORT \
                                                     - GPIO_BASE_PORT_A) / GPIO_OFFSET_PIN_PIN)))
#endif

/**
 * @brief   Override register address type for GPIO ports of the MCU
 *
 * The size of this type must match the size of a pointer to distinguish
 * between MCU GPIO register addresses and pointers on GPIO devices.
 */
#define HAVE_GPIO_REG_T
typedef uint16_t gpio_reg_t;

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

#ifdef __cplusplus
}
#endif

#endif /* GPIO_ARCH_H */
/** @} */
