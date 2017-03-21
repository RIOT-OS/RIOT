/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_uart_half_duplex half-duplex UART Driver
 * @ingroup     drivers_actuators
 *
 * This module contains drivers for UART half-duplex communication bus.
 * It needs to manage the communication direction by enabling or disabling TX.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for half-duplex UART driver
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 */

#ifndef UART_HALF_DUPLEX_H
#define UART_HALF_DUPLEX_H

#include <stdlib.h>

#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UART_HALF_DUPLEX_DEFAULT_TIMEOUT_US
#define UART_HALF_DUPLEX_DEFAULT_TIMEOUT_US (20000) /**< Default recv timeout (in microseconds) */
#endif

/**
 * @brief half-duplex UART direction management method type
 */
typedef int uart_half_duplex_dir_t;

#define UART_HALF_DUPLEX_DIR_NONE           (0)  /**< Don't manage direction */

#define UART_HALF_DUPLEX_DIR_PIN_SET(pin)   (((pin + 1) << 1))     /**< pin set enables TX */
#define UART_HALF_DUPLEX_DIR_PIN_CLEAR(pin) (((pin + 1) << 1) | 1) /**< pin clear enables TX */

/**
 * @brief Descriptor struct for half-duplex UART
 */
typedef struct {
    uart_t uart;                 /**< the half-duplex UART bus used */
    uart_half_duplex_dir_t dir;  /**< the direction management method */
    uint8_t *buffer;             /**< the TX/RX buffer */
    size_t limit;                /**< the TX/RX buffer size */
    size_t size;                 /**< the number of available elements for TX/RX */
    uint32_t timeout_us;         /**< the maximum duration (in microseconds) for waiting data */
} uart_half_duplex_t;

/**
 * @brief Configuration for half-duplex UART
 */
typedef struct {
    uart_t uart;                /**< the half-duplex UART bus to use */
    uint32_t baudrate;          /**< the baudrate to use */
    uart_half_duplex_dir_t dir; /**< the direction management method */
    uint8_t *buffer;            /**< the buffer used for TX and RX */
    size_t limit;               /**< the buffer size */
} uart_half_duplex_params_t;

/**
 * @brief   Possible UART_HALF_DUPLEX return values
 */
enum {
    UART_HALF_DUPLEX_OK     = UART_OK,       /**< everything in order */
    UART_HALF_DUPLEX_NODEV  = UART_NODEV,    /**< invalid UART device given */
    UART_HALF_DUPLEX_NOBAUD = UART_NOBAUD,   /**< given baudrate is not applicable */
    UART_HALF_DUPLEX_INTERR = UART_INTERR,   /**< all other internal errors */
    UART_HALF_DUPLEX_NOMODE = UART_NOMODE,   /**< given mode is not applicable */
    UART_HALF_DUPLEX_NOBUFF = -5             /**< invalid buffer given */
};

/**
 * @brief Initialize the half-duplex UART bus to communicate with devices
 *
 * @param[out] dev       the device
 * @param[in] params     the initialization parameters
 *
 * @return               0 on success
 * @return               < 0 if an error occured
 */
int uart_half_duplex_init(uart_half_duplex_t *dev, const uart_half_duplex_params_t *params);

/**
 * @brief Set the half-duplex UART bus in TX mode
 *
 * @param[in] dev        the device
 */
static inline void uart_half_duplex_set_tx(uart_half_duplex_t *dev)
{
    dev->size = dev->limit;
}

/**
 * @brief Set the half-duplex UART bus in RX mode
 *
 * @param[in] dev        the device
 */
static inline void uart_half_duplex_set_rx(uart_half_duplex_t *dev)
{
    dev->size = 0;
}

/**
 * @brief Send the data contained in the driver's buffer
 *
 * @param[in] dev        the device
 * @param[in] size       the number of characters to send
 *
 * @return               the number of characters actually sent
 */
size_t uart_half_duplex_send(uart_half_duplex_t *dev, size_t size);

/**
 * @brief Recv data an fill the driver's buffer
 *
 * @param[in] dev        the device
 * @param[in] size       the number of characters to receive
 *
 * @return               the number of characters actually received
 */
size_t uart_half_duplex_recv(uart_half_duplex_t *dev, size_t size);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
