/*
 * Copyright (C) 2017 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_fe310
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Ken Rabold
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <inttypes.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (12U)

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default gpio_t type definition
 */
#define HAVE_GPIO_T
typedef uint8_t gpio_t;
#endif

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)      (x | y)

/**
 * @brief   GPIO interrupt priority
 */
#define GPIO_INTR_PRIORITY  (3)

/**
 * @brief   Structure for UART configuration data
 */
typedef struct {
    uint32_t addr;          /**< UART control register address */
    gpio_t rx;              /**< RX pin */
    gpio_t tx;              /**< TX pin */
    plic_source isr_num;    /**< ISR source number */
} uart_conf_t;

/**
 * @brief   UART interrupt priority
 */
#define UART_ISR_PRIO       (2)

/**
 * @name    This CPU makes use of the following shared SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE  1
#define PERIPH_SPI_NEEDS_TRANSFER_REG   1
#define PERIPH_SPI_NEEDS_TRANSFER_REGS  1
/** @} */

/**
 * @brief   Structure for SPI configuration data
 */
typedef struct {
    uint32_t addr;          /**< SPI control register address */
    gpio_t mosi;            /**< MOSI pin */
    gpio_t miso;            /**< MISO pin */
    gpio_t sclk;            /**< SCLK pin */
} spi_conf_t;

/**
 * @brief   Prevent shared timer functions from being used
 */
#define PERIPH_TIMER_PROVIDES_SET

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
/** Use write regs function from periph common */
#define PERIPH_I2C_NEED_WRITE_REGS
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
} i2c_speed_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   I2C configuration options
 */
typedef struct {
    uint32_t addr;              /**< device base address */
    gpio_t scl;                 /**< SCL pin */
    gpio_t sda;                 /**< SDA pin */
    i2c_speed_t speed;          /**< I2C speed */
} i2c_conf_t;

/**
 * @brief   PWM channel configuration data structure
 */
typedef struct {
    gpio_t pin;                 /**< GPIO pin */
    uint8_t cmp;                /**< PWM comparator to use */
} pwm_conf_chan_t;

/**
 * @brief   PWM device configuration data structure
 */
typedef struct {
    uint32_t addr;              /**< PWM address to use */
    pwm_conf_chan_t chan[4];    /**< channel configuration */
} pwm_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
