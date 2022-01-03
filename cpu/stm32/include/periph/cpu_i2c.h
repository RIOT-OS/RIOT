/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           I2C CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
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
#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || \
    defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32MP1)
/** Use write regs function from periph common */
#define PERIPH_I2C_NEED_WRITE_REGS
#endif
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Default mapping of I2C bus speed values
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L1) || \
    defined(CPU_FAM_STM32MP1)
    I2C_SPEED_LOW,          /**< low speed mode: ~10kit/s */
#endif
    I2C_SPEED_NORMAL,       /**< normal mode:  ~100kbit/s */
    I2C_SPEED_FAST,         /**< fast mode:    ~400kbit/s */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32U5) || \
    defined(CPU_FAM_STM32WL)
    I2C_SPEED_FAST_PLUS,    /**< fast plus mode: ~1Mbit/s */
#endif
} i2c_speed_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Structure for I2C configuration data
 */
typedef struct {
    I2C_TypeDef *dev;       /**< i2c device */
    i2c_speed_t speed;      /**< i2c bus speed */
    gpio_t scl_pin;         /**< scl pin number */
    gpio_t sda_pin;         /**< sda pin number */
#ifndef CPU_FAM_STM32F1
    gpio_af_t scl_af;       /**< scl pin alternate function value */
    gpio_af_t sda_af;       /**< sda pin alternate function value */
#endif
    uint8_t bus;            /**< APB bus */
    uint32_t rcc_mask;      /**< bit in clock enable register */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
    uint32_t rcc_sw_mask;   /**< bit to switch I2C clock */
#endif
#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L1) || \
    defined(CPU_FAM_STM32MP1)
    uint32_t clk;           /**< bus frequency as defined in board config */
#endif
    uint8_t irqn;           /**< I2C event interrupt number */
} i2c_conf_t;

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32U5) || \
    defined(CPU_FAM_STM32WL)
/**
 * @brief   Structure for I2C timing register settings
 *
 * These parameters are valid for 48MHz (16MHz for L0) input clock.
 * See reference manual of supported CPU for example of timing settings:
 * - STM32F030/F070: see RM0360, section 22.4.10, p.560, table 76
 * - STM32F303: see RM0316, section 28.4.9, p.849, table 148
 * - STM32F72X: see RM0431, section 26.4.9, p.851, table 149
 * - STM32L0x2: see RM0376, section 27.4.10, p.686, table 117
 * - STM32L4X5/6: see RM0351, section 39.4.9, p.1297, table 234
 */
typedef struct {
    uint8_t presc;          /**< Timing prescaler value */
    uint8_t scll;           /**< SCL Low period */
    uint8_t sclh;           /**< SCL High period */
    uint8_t sdadel;         /**< Data hold time */
    uint8_t scldel;         /**< Data setup time */
} i2c_timing_param_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_I2C_H */
/** @} */
