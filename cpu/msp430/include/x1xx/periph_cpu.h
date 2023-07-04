/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_msp430_x1xx
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef X1XX_PERIPH_CPU_H
#define X1XX_PERIPH_CPU_H

#include <stdbool.h>

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Override SPI mode selection values
 * @{
 */
#define HAVE_SPI_MODE_T     /**< MSP430 x1xx has a custom spi_mode_t */
/**
 * @brief   Support SPI modes
 *
 * | Field  | Name                  |   Description                                         |
 * |:------ |:--------------------- |:----------------------------------------------------- |
 * | CKPH   | Clock phase select    | 0 = capture on second edge, 1 = capture on first      |
 * | CKPL   | Clock polarity select | 0 = clock is idle-low, 1 = clock is high idle-high    |
 */
typedef enum {
    SPI_MODE_0 = (CKPH),                /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = 0,                     /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = (CKPL | CKPH),         /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (CKPL)                 /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */

/**
 * @name    Override SPI clock speed selection values
 * @{
 */
#define HAVE_SPI_CLK_T     /**< MSP430 x1xx has a custom spi_clock_t */
/**
 * @brief   Support SPI clock frequencies
 */
typedef enum {
    SPI_CLK_100KHZ = 100000,        /**< 100 kHz */
    SPI_CLK_400KHZ = 400000,        /**< 400 kHz */
    SPI_CLK_1MHZ   = 1000000,       /**< 1 MHz */
    SPI_CLK_5MHZ   = 5000000,       /**< 5 MHz */
    SPI_CLK_10MHZ  = SPI_CLK_5MHZ,  /**< 10 MHz not supported, falling back to 5 MHz */
} spi_clk_t;
/** @} */

/**
 * @name declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS        /**< use shared spi_init_cs() */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE  /**< use shared spi_transfer_byte() */
#define PERIPH_SPI_NEEDS_TRANSFER_REG   /**< use shared spi_transfer_reg() */
#define PERIPH_SPI_NEEDS_TRANSFER_REGS  /**< use shared spi_transfer_regs() */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* X1XX_PERIPH_CPU_H */
/** @} */
