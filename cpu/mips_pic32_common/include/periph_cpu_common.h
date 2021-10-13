/*
 * Copyright(C) 2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup         cpu_mips_pic32_common
 * @brief           Shared CPU specific definitions for the MIPS family.
 * @{
 *
 * @file
 * @brief           Shared CPU specific definitions for the MIPS family.
 *
 * @author          Francois Berder <francois.berder@imgtec.com>
 */

#ifndef PERIPH_CPU_COMMON_H
#define PERIPH_CPU_COMMON_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief We run from flash on PIC32
 */
#define FLASH_XIP (1)

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_SET_LOWEST
/** @} */

/**
 * @brief   Length of the CPU_ID in bytes
 */
#define CPUID_LEN           (4U)

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */
#endif

/**
 * @brief   Override GPIO pin selection macro
 */
#define GPIO_PIN(x, y)      (((_PORTB_BASE_ADDRESS & 0xFFFFF000) + (x << 8)) | y)

/**
 * @brief   Available ports on the PIC32 family
 */
enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 5,             /**< port F */
    PORT_G = 6,             /**< port G */
};

/**
 * @brief   Prevent shared timer functions from being used
 */
#define PERIPH_TIMER_PROVIDES_SET

/**
 * @brief   Available MUX values for configuring a pin's alternate function
 */
typedef enum {
    GPIO_AF0 = 0,           /**< use alternate function 0 */
    GPIO_AF1,               /**< use alternate function 1 */
    GPIO_AF2,               /**< use alternate function 2 */
    GPIO_AF3,               /**< use alternate function 3 */
    GPIO_AF4,               /**< use alternate function 4 */
    GPIO_AF5,               /**< use alternate function 5 */
    GPIO_AF6,               /**< use alternate function 6 */
    GPIO_AF7,               /**< use alternate function 7 */
    GPIO_AF8,               /**< use alternate function 8 */
    GPIO_AF9,               /**< use alternate function 9 */
    GPIO_AF10,              /**< use alternate function 10 */
    GPIO_AF11,              /**< use alternate function 11 */
    GPIO_AF12,              /**< use alternate function 12 */
    GPIO_AF13,              /**< use alternate function 13 */
    GPIO_AF14,              /**< use alternate function 14 */
    GPIO_AF15               /**< use alternate function 15 */
} gpio_af_t;

/**
 * @brief   Structure for UART configuration data
 */
typedef struct {
    volatile unsigned int * base;           /**< UART device base register address */
    uint32_t clock;                         /**< Peripheral clock frequency */
    gpio_t rx_pin;                          /**< RX pin */
    gpio_t tx_pin;                          /**< TX pin */
    volatile unsigned int *rx_mux_reg;      /**< Address of RX mux register*/
    volatile unsigned int *tx_mux_reg;      /**< Address of TX mux register */
    gpio_af_t rx_af;                        /**< alternate function for RX pin */
    gpio_af_t tx_af;                        /**< alternate function for TX pin */
    uint32_t vector;                        /**< vector number */
#ifdef CPU_FAM_PIC32MX
    uint32_t irq;                           /**< interrupt number */
#endif
} uart_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_COMMON_H */
/** @} */
