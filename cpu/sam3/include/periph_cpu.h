/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2015 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3x8e
 * @{
 *
 * @file
 * @brief       CPU specific definitions for internal peripheral handling
 *
 * @author      Hauke Petersen <hauke.peterse@fu-berlin.de>
 * @author      Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 *
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "cpu.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Overwrite the default gpio_t type definition
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @brief Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)      (((uint32_t)PIOA + (x << 9)) | y)

/**
 * @brief Declare needed generic SPI functions
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
 * @brief   All SAM3 timers are 32-bit wide
 */
#define TIMER_MAX_VAL       (0xffffffff)

/**
 * @brief   We use 3 channels for each defined timer
 */
#define TIMER_CHANNELS      (3)

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 3 bit to determine the pin functions:
 * - bit 0: in/out
 * - bit 1: PU enable
 * - bit 2: OD enable
 */
#define GPIO_MODE(io, pu, od)   (io | (pu << 1) | (od << 2))

/**
 * @brief   Override GPIO modes
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 0, 0),    /**< IN */
    GPIO_IN_PD = 0xf,                   /**< not supported by HW */
    GPIO_IN_PU = GPIO_MODE(0, 1, 0),    /**< IN with pull-up */
    GPIO_OUT   = GPIO_MODE(1, 0, 0),    /**< OUT (push-pull) */
    GPIO_OD    = GPIO_MODE(1, 0, 1),    /**< OD */
    GPIO_OD_PU = GPIO_MODE(1, 1, 1),    /**< OD with pull-up */
} gpio_mode_t;
/** @} */

/**
 * @brief Override flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */

/**
 * @brief Available ports on the SAM3X8E
 */
enum {
    PA = 0,                 /**< port A */
    PB = 1,                 /**< port B */
    PC = 2,                 /**< port C */
    PD = 3,                 /**< port D */
};

/**
 * @brief   GPIO mux configuration
 */
typedef enum {
    GPIO_MUX_A = 0,         /**< alternate function A */
    GPIO_MUX_B = 1,         /**< alternate function B */
} gpio_mux_t;

/**
 * @brief   Timer configuration data
 */
typedef struct {
    Tc *dev;                /**< timer device */
    uint8_t id_ch0;         /**< ID of the timer's first channel */
} timer_conf_t;

/**
 * @brief   UART configuration data
 */
typedef struct {
    Uart *dev;              /**< U(S)ART device used */
    Pio *rx_port;           /**< port for RX pin */
    Pio *tx_port;           /**< port for TX pin */
    uint8_t rx_pin;         /**< RX pin */
    uint8_t tx_pin;         /**< TX pin */
    gpio_mux_t mux;         /**< MUX used for pins */
    uint8_t pmc_id;         /**< bit in the PMC register of the device*/
    uint8_t irqn;           /**< interrupt number of the device */
} uart_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
