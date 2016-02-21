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
 * @brief Override values for pull register configuration
 * @{
 */
#define HAVE_GPIO_PP_T
typedef enum {
    GPIO_NOPULL = 4,        /**< do not use internal pull resistors */
    GPIO_PULLUP = 9,        /**< enable internal pull-up resistor */
    GPIO_PULLDOWN = 8       /**< enable internal pull-down resistor */
} gpio_pp_t;
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

#ifndef HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = (SPI_CSR_NCPHA),                   /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = (0),                               /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = (SPI_CSR_CPOL | SPI_CSR_NCPHA),    /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (SPI_CSR_CPOL)                     /**< CPOL=1, CPHA=1 */
} spi_mode_t;

#ifndef HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = (CLOCK_CORECLOCK / 100000),    /**< 100KHz */
    SPI_CLK_400KHZ = (CLOCK_CORECLOCK / 400000),    /**< 400KHz */
    SPI_CLK_1MHZ   = (CLOCK_CORECLOCK / 1000000),   /**< 1MHz */
    SPI_CLK_5MHZ   = (CLOCK_CORECLOCK / 5000000),   /**< 5MHz */
    SPI_CLK_10MHZ  = (CLOCK_CORECLOCK / 10000000)   /**< 10MHz */
} spi_clk_t;

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

/**
 * @brief   SPI configuration data
 */
typedef struct {
    Spi *dev;
    uint8_t id;
    gpio_t clk;
    gpio_t mosi;
    gpio_t miso;
    gpio_mux_t mux;
} spi_conf_t;



void gpio_init_mux(gpio_t pin, gpio_mux_t mux);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
