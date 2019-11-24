/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_lpc2387
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "cpu.h"

/**
 * @brief   LPC2387 MCU defines
 * @{
 */
#define __IO volatile

/**
 * @name    Power mode configuration
 * @{
 */
#define PM_NUM_MODES        (4)
/** @} */

/**
 * @brief Fast GPIO register definition struct
 */
typedef struct {
    /** @brief Direction: Output if corresponding bit is set, otherwise input */
    __IO uint32_t DIR;
    /** @brief 12 bytes of reserved memory we don't need to access */
    uint32_t _reserved[3];
    /** @brief Set bits to ignore corresponding bits when accessing `PIN`, `SET`
     *         or `CLR` register of this port
     */
    __IO uint32_t MASK;
    /** @brief The current state of each pin of this port is accessible here
     *         (regardless of direction): If bit is set input is high
     */
    __IO uint32_t PIN;
    /** @brief Output pins are set to high by setting the corresponding bit */
    __IO uint32_t SET;
    /** @brief Output pins are set to low by setting the corresponding bit */
    __IO uint32_t CLR;
} FIO_PORT_t;

/**
 * @brief   Pointer to the Fast GPIO register
 */
#define FIO_PORTS   ((FIO_PORT_t*)FIO_BASE_ADDR)

/**
 * @brief   Pointer to the PINSEL register
 */
#define PINSEL      ((__IO uint32_t *)(PINSEL_BASE_ADDR))

/**
 * @brief   Pointer to the PINMODE register
 */
#define PINMODE     ((__IO uint32_t *)(PINSEL_BASE_ADDR + 0x40))

/**
 * @brief   Set up alternate function (PMUX setting) for a PORT pin
 *
 * @param[in] pin   Pin to set the multiplexing for
 * @param[in] mux   Mux value
 */
int gpio_init_mux(unsigned pin, unsigned mux);

/**
 * @brief   Macro for accessing GPIO pins
 */
#define GPIO_PIN(port, pin) (port<<5 | pin)

#ifndef DOXYGEN
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 1,       /**< emit interrupt on falling flank */
    GPIO_RISING = 2,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
#endif /* ndef DOXYGEN */

/**
 * @brief   UART device configuration
 */
typedef struct {
    lpc23xx_uart_t *dev;    /**< pointer to the UART device */
    uint8_t irq_prio_rx;    /**< priority of the RX IRQ */
    uint8_t pinsel_rx;      /**< PINSEL# of the RX pin  */
    uint8_t pinsel_tx;      /**< PINSEL# of the TX pin  */
    uint32_t pinsel_msk_rx; /**< RX PINSEL Mask         */
    uint32_t pinsel_msk_tx; /**< TX PINSEL Mask         */
} uart_conf_t;

/**
 * @brief   Number of available timer channels
 */
#define TIMER_CHAN_NUMOF        (4U)

/**
 * @brief   Declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/* @} */

#ifndef DOXYGEN
/**
 * @brief   Override SPI clock speed values
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = 100,   /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ = 400,   /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   = 1000,  /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ   = 5000,  /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = 10000  /**< drive the SPI bus with 10MHz */
} spi_clk_t;
/** @} */
#endif /* ndef DOXYGEN */

/* @} */
#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
