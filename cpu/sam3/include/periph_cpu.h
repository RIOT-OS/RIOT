/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_sam3x8e
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H_
#define PERIPH_CPU_H_

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTES
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @brief   GPIO mux configuration
 */
typedef enum {
    GPIO_MUX_A = 0,         /**< alternate function A */
    GPIO_MUX_B = 1,         /**< alternate function B */
} gpio_mux_t;

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

#endif /* PERIPH_CPU_H_ */
/** @} */
