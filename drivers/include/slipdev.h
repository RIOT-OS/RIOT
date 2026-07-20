/*
 * SPDX-FileCopyrightText: 2015-2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_slipdev_stdio   STDIO via SLIP
 * @ingroup     sys_stdio
 * @brief       Standard input/output backend multiplexed via SLIP
 * @see         [draft-bormann-t2trg-slipmux-03](https://datatracker.ietf.org/doc/html/draft-bormann-t2trg-slipmux-03)
 *
 * This extension is part of the [Slipmux draft](https://datatracker.ietf.org/doc/html/draft-bormann-t2trg-slipmux-03).
 * @warning This module is under development for optimizations and module names might change!
 *
 * This will multiplex STDIO via the Serial Line Internet Protocol.
 * The shell can be accessed via the `sliptty` tool.
 *
 * To enable this stdio implementation, select
 *
 *     USEMODULE += stdio_slipdev
 *
 * @see         drivers_slipdev
 */

/**
 * @defgroup    drivers_slipdev_configuration CoAP via SLIP
 * @ingroup     drivers_slipdev
 * @brief       Exchange CoAP requests and responses via SLIP
 * @see         [draft-bormann-t2trg-slipmux-03](https://datatracker.ietf.org/doc/html/draft-bormann-t2trg-slipmux-03)
 *
 * This extension is part of the [Slipmux draft](https://datatracker.ietf.org/doc/html/draft-bormann-t2trg-slipmux-03).
 * @warning This module is under development for optimizations and module names might change!
 *
 * This will multiplex CoAP messages via the Serial Line Internet Protocol.
 * It spawns an extra thread as a CoAP server. The incoming requests are handled with `nanocoap`
 * according to any `NANOCOAP_RESOURCE`s present in the binary. See @ref net_nanocoap for more.
 *
 * To enable this implementation, select
 *
 *     USEMODULE += slipdev_config
 *
 * @see         drivers_slipdev
 */

/**
 * @defgroup    drivers_slipdev SLIP network device
 * @ingroup     drivers_netdev
 * @brief       SLIP network device over @ref drivers_periph_uart
 * @see         [RFC 1055](https://datatracker.ietf.org/doc/html/rfc1055)
 *
 * @{
 *
 * @file
 * @brief   SLIP device definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdint.h>

#include "cib.h"
#include "event.h"
#include "net/netdev.h"
#include "periph/uart.h"
#include "chunked_ringbuffer.h"
#include "sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup drivers_slipdev_config     SLIP Network driver compile configuration
 * @ingroup config_drivers_netdev
 * @{
 */
/**
 * @brief   UART buffer size used for TX and RX buffers
 *
 * Reduce this value if your expected traffic does not include full IPv6 MTU
 * sized packets.
 */
#ifdef CONFIG_SLIPDEV_BUFSIZE_EXP
#define CONFIG_SLIPDEV_BUFSIZE (1 << CONFIG_SLIPDEV_BUFSIZE_EXP)
#endif

#ifndef CONFIG_SLIPDEV_BUFSIZE
#define CONFIG_SLIPDEV_BUFSIZE (2048U)
#endif
/** @} */

/**
 * @brief   States for the slipdev and its parser
 */
typedef enum {
    /**
     * @brief  Device is in no mode (currently not receiving any frame), this is the idle state.
     *
     * Waits for any byte, if the byte is a valid frame start byte (diagnostic, configuration,
     * IP packet), it starts a new frame of the respective type and switches to the corresponding
     * state. If the byte is a frame end byte, it is a no-op (we received an empty frame).
     * If the byte has any other value, an unknown frame type is assumed. The state switches
     * to UNKNOWN state.
     */
    SLIPDEV_STATE_NONE = 0,
    /**
     * @brief   Device discards incoming data.
     *
     * It switches back to the NONE (idle) state once the unknown frame is ended via a frame
     * end byte.
     */
    SLIPDEV_STATE_UNKNOWN,
    /**
     * @brief   Device writes handles data as network device
     */
    SLIPDEV_STATE_NET,
    /**
     * @brief   Device writes handles data as network device, next byte is escaped
     */
    SLIPDEV_STATE_NET_ESC,
    /**
     * @brief   Device writes received data to stdin
     */
    SLIPDEV_STATE_STDIN,
    /**
     * @brief   Device writes received data to stdin, next byte is escaped
     */
    SLIPDEV_STATE_STDIN_ESC,
    /**
     * @brief   Device writes received data as CoAP message
     */
    SLIPDEV_STATE_CONFIG,
    /**
     * @brief   Device writes received data as CoAP message, next byte is escaped
     */
    SLIPDEV_STATE_CONFIG_ESC,
    /**
     * @brief   Device is in standby, will wake up when sending data
     */
    SLIPDEV_STATE_STANDBY,
    /**
     * @brief   Device is in sleep mode
     */
    SLIPDEV_STATE_SLEEP,
} slipdev_state_t;

/**
 * @brief   Configuration parameters for a slipdev
 */
typedef struct {
    uart_t uart;        /**< UART interface the device is connected to */
    uint32_t baudrate;  /**< baudrate to use with slipdev_params_t::uart */
} slipdev_params_t;

/**
 * @brief   Device descriptor for slipdev
 *
 * @extends netdev_t
 */
typedef struct {
#if IS_USED(MODULE_SLIPDEV_NET)
    netdev_t netdev;                        /**< parent class */
    chunk_ringbuf_t rb;                     /**< Ringbuffer to store received networking frames.*/
                                            /* Written to from interrupts (with irq_disable */
                                            /* to prevent any simultaneous writes),         */
                                            /* consumed exclusively in the network stack's  */
                                            /* loop at _isr.                                */

    uint8_t rxmem[CONFIG_SLIPDEV_BUFSIZE];  /**< memory used by RX buffer */
#endif

#if IS_USED(MODULE_SLIPDEV_CONFIG)
    chunk_ringbuf_t rb_config;                      /**< Ringbuffer stores received configuration frames */
    uint8_t rxmem_config[CONFIG_SLIPDEV_BUFSIZE];   /**< memory used by RX buffer */
    event_t rxevent;                                  /**< The event that is send to the CoAP server */
#endif

    slipdev_params_t config;                /**< configuration parameters */
    /**
     * @brief   Device state
     * @see     [Device state definitions](@ref slipdev_state_t)
     */
    slipdev_state_t state;
} slipdev_t;

/**
 * @brief   Setup a slipdev device state
 *
 * @param[in] dev       device descriptor
 * @param[in] params    parameters for device initialization
 * @param[in] index     index of @p params in a global parameter struct array.
 *                      If initialized manually, pass a unique identifier instead.
 */
void slipdev_setup(slipdev_t *dev, const slipdev_params_t *params, uint8_t index);

/**
 * @brief   Setup the event queue that will be notified when a config frame arrives
 *
 * @param[in] q         event queue
 */
void slipdev_coap_set_event_queue(event_queue_t *q);

/**
 * @brief   Teardown the event queue
 */
void slipdev_coap_unset_event_queue(void);

/**
 * @brief   Unicoap callback to handle a received config frame
 *
 * @param[in] event     event of the received frame
 */
void unicoap_slipdev_recv_handler(event_t * event);


/**
 * @brief   Called by unicoap to receive a config frame
 *
 * @param[in] buf       buffer into which the received frame is copied.
 * @param[in] buf_size  length of @p buf.
 * @param[in] dev       device descriptor to identify the slipdev instance.
 * @return
 *    - >0: length of the stored frame in bytes
 *    -  0: No frame available, the buffer was not modified
 *    - -1: Frame length exceeds buffer size or the frame was too short to be valid
 *    - -2: The checksum was invalid, the buffer was modified
 */
int slipdev_coap_recv(uint8_t *buf, size_t buf_size, slipdev_t *dev);

/**
 * @brief   Called by unicoap to send a coap message as a config frame
 *
 * This functions handles the calculation of the needed checksum, the escaping
 * of data, the framing and finally sends the result onto the wire.
 *
 * @param[in] buf       buffer into which the received frame is copied.
 * @param[in] buf_size  length of @p buf.
 * @param[in] dev       device descriptor to identify the slipdev instance.
 */
void slipdev_coap_send(uint8_t *buf, size_t buf_size, const slipdev_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
