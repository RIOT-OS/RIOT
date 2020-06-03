/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup        cpu_cc3200
 * @{
 *
 * @file
 * @brief           CPU specific definitions and functions for peripheral
 *
 * @author          Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 */

#include "cpu.h"
#include "periph_conf.h"

#include <stdbool.h>
#include <stdint.h>

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief convert x usecs to cpu cycles
 *
 */
#define USEC_TO_CPU_CYCLES(x) ((CLOCK_CORECLOCK / 1000000) * x)
/**
 * @brief delay CPU for x uSec. UtilsDelay uses a loop that takes 3 cycles to
 * complete. Therefore the uSec time needs to be divided by 3.
 *
 */
#define USEC_DELAY(x) ROM_UtilsDelay(USEC_TO_CPU_CYCLES(x) / 3)

/**
 * @brief   Starting offset of CPU_ID
 */
#define CPUID_ADDR (void *)(0xe000ed00)
/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN (4U)

/**
 * @brief holds the current hibernation state of the MCU. READONLY and reading
 * requires a delay of PRCM_OP_DELAY
 *
 */
#define HIBERNATION_WAKE_STATUS_REG \
    (*((volatile uint32_t *)(HIB3P3_BASE + HIB3P3_O_MEM_HIB_WAKE_STATUS)))

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_SET_LOWEST
/** @} */

/**
 * @brief   SPI device configuration
 * @{
 */
typedef struct {
    uint32_t base_addr; /**< bus address */
    gpio_t gpio_port;   /**< GPIO port */
    spi_pins_t pins;    /**< pin configuration */
    uint32_t config;    /**< SPI config */
} spi_conf_t;
/** @} */

/**
 * @brief   Declare needed generic SPI functions.
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @name   UART device configuration
 * @{
 */
typedef struct {
    cc3200_uart_t *dev; /**< pointer to the used UART device */
    gpio_t tx_pin;      /**< pin used for TX */
    gpio_t rx_pin;      /**< pin used for RX */
    gpio_t cts_pin;     /**< CTS pin - set to GPIO_UNDEF when not using */
    gpio_t rts_pin;     /**< RTS pin - set to GPIO_UNDEF when not using */
    gpio_t irqn;        /**< Interrupt code */
} uart_conf_t;
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
