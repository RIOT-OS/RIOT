/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_ethos Ethernet-over-serial driver
 * @ingroup     drivers_netdev
 * @brief       Driver for the ethernet-over-serial module
 * @{
 *
 * @file
 * @brief       Interface definition for the ethernet-over-serial module
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef ETHOS_H
#define ETHOS_H

#include "kernel_types.h"
#include "periph/uart.h"
#include "net/netdev.h"
#include "tsrb.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/* if using ethos + stdio, use STDIO_UART values unless overridden */
#ifdef USE_ETHOS_FOR_STDIO
#include "stdio_uart.h"
#ifndef ETHOS_UART
#define ETHOS_UART     STDIO_UART_DEV
#endif
#ifndef ETHOS_BAUDRATE
#define ETHOS_BAUDRATE STDIO_UART_BAUDRATE
#endif
#endif

/**
 * @name    Escape char definitions
 * @{
 */
#define ETHOS_FRAME_DELIMITER           (0x7E)
#define ETHOS_ESC_CHAR                  (0x7D)
#define ETHOS_FRAME_TYPE_DATA           (0x0)
#define ETHOS_FRAME_TYPE_TEXT           (0x1)
#define ETHOS_FRAME_TYPE_HELLO          (0x2)
#define ETHOS_FRAME_TYPE_HELLO_REPLY    (0x3)
/** @} */

/**
 * @brief   Enum describing line state
 */
typedef enum {
    WAIT_FRAMESTART,
    IN_FRAME,
    IN_ESCAPE
} line_state_t;

/**
 * @brief   ethos netdev device
 * @extends netdev_t
 */
typedef struct {
    netdev_t netdev;        /**< extended netdev structure */
    uart_t uart;            /**< UART device the to use */
    uint8_t mac_addr[6];    /**< this device's MAC address */
    uint8_t remote_mac_addr[6]; /**< this device's MAC address */
    tsrb_t inbuf;           /**< ringbuffer for incoming data */
    line_state_t state;     /**< Line status variable */
    size_t framesize;       /**< size of currently incoming frame */
    unsigned frametype;     /**< type of currently incoming frame */
    size_t last_framesize;  /**< size of last completed frame */
    mutex_t out_mutex;      /**< mutex used for locking concurrent sends */
} ethos_t;

/**
 * @brief   Struct containing the needed configuration
 */
typedef struct {
    uart_t uart;            /**< UART device to use */
    uint32_t baudrate;      /**< baudrate to UART device */
    uint8_t *buf;           /**< buffer for incoming packets */
    size_t bufsize;         /**< size of ethos_params_t::buf */
} ethos_params_t;

/**
 * @brief   Setup an ethos based device state.
 *
 * The supplied buffer *must* have a power-of-two size, and it *must* be large
 * enough for the largest expected packet + enough buffer space to buffer
 * bytes that arrive while one packet is being handled.
 *
 * E.g., if 1536b ethernet frames are expected, 2048 is probably a good size for @p buf.
 *
 * @param[out]  dev         handle of the device to initialize
 * @param[in]   params      parameters for device initialization
 */
void ethos_setup(ethos_t *dev, const ethos_params_t *params);

/**
 * @brief   Send frame over serial port using ethos' framing
 *
 * This is used by e.g., stdio over ethos to send text frames.
 *
 * @param[in]   dev         handle of the device to initialize
 * @param[in]   data        ptr to data to be sent
 * @param[in]   len         nr of bytes to send
 * @param[in]   frame_type  frame type to use
 */
void ethos_send_frame(ethos_t *dev, const uint8_t *data, size_t len, unsigned frame_type);

#ifdef __cplusplus
}
#endif
#endif /* ETHOS_H */
/** @} */
