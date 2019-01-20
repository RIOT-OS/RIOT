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

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
