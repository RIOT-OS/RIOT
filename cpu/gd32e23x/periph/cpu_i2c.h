/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32e23x
 * @{
 *
 * @file
 * @brief       I2C CPU specific definitions for the GD32E23x family
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef PERIPH_CPU_I2C_H
#define PERIPH_CPU_I2C_H

#include <stdint.h>

#include "cpu.h"
#include "periph/cpu_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Use the shared I2C functions
 * @{
 */
/** Use read reg function from periph common */
#define PERIPH_I2C_NEED_READ_REG
/** Use write reg function from periph common */
#define PERIPH_I2C_NEED_WRITE_REG
/** Use read regs function from periph common */
#define PERIPH_I2C_NEED_READ_REGS
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Default mapping of I2C bus speed values
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_NORMAL,       /**< normal mode:  ~100kbit/s */
    I2C_SPEED_FAST,         /**< fast mode:    ~400kbit/s */
    I2C_SPEED_FAST_PLUS,    /**< fast plus mode: ~1Mbit/s */
} i2c_speed_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Structure for I2C configuration data
 */
typedef struct {
    I2C_Type *dev;       /**< i2c device */
    i2c_speed_t speed;      /**< i2c bus speed */
    gpio_t scl_pin;         /**< scl pin number */
    gpio_t sda_pin;         /**< sda pin number */
    uint8_t bus;            /**< APB bus */
    uint32_t rcu_mask;      /**< bit in clock enable register */
    uint32_t rcu_sw_mask;   /**< bit to switch I2C clock */
    uint8_t irqn;           /**< I2C event interrupt number */
    uint32_t timing;        /**< I2C Timing Params for TIMINGR register */
} i2c_conf_t;

/**
 * @brief   Structure for I2C timing register settings
 */
typedef struct {
    uint8_t presc;          /**< Timing prescaler value */
    uint8_t scll;           /**< SCL Low period */
    uint8_t sclh;           /**< SCL High period */
    uint8_t sdadel;         /**< Data hold time */
    uint8_t scldel;         /**< Data setup time */
} i2c_timing_param_t;

/**
 * @brief   Timing register settings
 *
 * @ref i2c_timing_param_t
 */
static const i2c_timing_param_t timing_params[] = {
    [ I2C_SPEED_NORMAL ]    = {
        .presc  = 3,
        .scll   = 0x13,     /* t_SCLL   = 5.0us  */
        .sclh   = 0xF,      /* t_SCLH   = 4.0us  */
        .sdadel = 0x2,      /* t_SDADEL = 500ns  */
        .scldel = 0x4,      /* t_SCLDEL = 1250ns */
    },
    [ I2C_SPEED_FAST ]      = {
        .presc  = 1,
        .scll   = 0x9,      /* t_SCLL   = 1250ns */
        .sclh   = 0x3,      /* t_SCLH   = 500ns  */
        .sdadel = 0x2,      /* t_SDADEL = 250ns  */
        .scldel = 0x3,      /* t_SCLDEL = 500ns  */
    },
    [ I2C_SPEED_FAST_PLUS ] = {
        .presc =  0,
        .scll =   0x4,      /* t_SCLL   = 312.5ns */
        .sclh =   0x2,      /* t_SCLH   = 187.5ns */
        .sdadel = 0x0,      /* t_SDADEL = 0ns     */
        .scldel = 0x2,      /* t_SCLDEL = 187.5ns */
    }
};

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_I2C_H */
/** @} */
