/*
 * Copyright (C) 2015-17 Freie Universität Berlin
 * Copyright (C) 2025 HAW Hamburg
 * Copyright (C) 2025 Bennet Hattesen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

#include <stdint.h>

#include "cib.h"
#include "event.h"
#include "net/netdev.h"
#include "periph/uart.h"
#include "chunked_ringbuffer.h"
#include "sched.h"

/**
 * @defgroup    drivers_slipmux Serial Line Internet Protocol Multiplexing
 * @brief       Provides de-/encoding for SLIPMUX
 * @see         [draft-bormann-t2trg-slipmux-03](https://datatracker.ietf.org/doc/html/draft-bormann-t2trg-slipmux-03)
 *
 * This is the base module, see @ref drivers_slipmux_net, @ref drivers_slipmux_coap and
 * @ref drivers_slipmux_stdio for more information.
 *
 * @{
 *
 * @file
 * @brief   SLIPMUX definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup    drivers_slipmux_stdio   STDIO via SLIPMUX
 * @ingroup     sys_stdio
 * @brief       Standard input/output backend multiplexed via SLIPMUX
 * @see         [draft-bormann-t2trg-slipmux-03](https://datatracker.ietf.org/doc/html/draft-bormann-t2trg-slipmux-03)
 *
 * This extension is part of the [Slipmux draft](https://datatracker.ietf.org/doc/html/draft-bormann-t2trg-slipmux-03).
 * @warning This module is under development for optimizations and module names might change!
 *
 * This multiplexes STDIO together with the Serial Line Internet Protocol via SLIPMUX.
 * The shell can be accessed via the `sliptty` tool.
 *
 * To enable this stdio implementation, select
 *
 *     USEMODULE += slipmux_stdio
 *
 * @see         drivers_slipmux
 */

/**
 * @defgroup    drivers_slipmux_coap CoAP via SLIP
 * @ingroup     drivers_slipmux
 * @brief       Exchange CoAP requests and responses via SLIPMUX
 * @see         [draft-bormann-t2trg-slipmux-03](https://datatracker.ietf.org/doc/html/draft-bormann-t2trg-slipmux-03)
 *
 * This extension is part of the [Slipmux draft](https://datatracker.ietf.org/doc/html/draft-bormann-t2trg-slipmux-03).
 * @warning This module is under development for optimizations and module names might change!
 *
 * This will multiplex CoAP messages together with the Serial Line Internet Protocol via SLIPMUX.
 * It spawns an extra thread as a CoAP server. The incoming requests are handled with `nanocoap`
 * according to any `NANOCOAP_RESOURCE`s present in the binary. See @ref net_nanocoap for more.
 *
 * To enable this implementation, select
 *
 *     USEMODULE += slipmux_coap
 *
 * @see         drivers_slipmux
 * @{
 */

/**
 * @brief   UART buffer size used for TX and RX buffers
 *
 * Reduce this value if your expected traffic does not include full IPv6 MTU
 * sized packets.
 */
#ifndef CONFIG_SLIPMUX_COAP_BUFSIZE
#define CONFIG_SLIPMUX_COAP_BUFSIZE (512U)
#endif
/** @} */


/**
 * @defgroup    drivers_slipmux_net SLIP network device
 * @ingroup     drivers_netdev
 * @brief       SLIP network device over @ref drivers_periph_uart
 * @see         [RFC 1055](https://datatracker.ietf.org/doc/html/rfc1055)
 *
 * This extension is part of the [Slipmux draft](https://datatracker.ietf.org/doc/html/draft-bormann-t2trg-slipmux-03).
 * @warning This module is under development for optimizations and module names might change!
 *
 * This will offer the traditional Serial Line Internet Protocol as a network interface
 * using GNRC.
 *
 * To enable this implementation, select
 *
 *     USEMODULE += slipmux_net
 *
 * @see         drivers_slipmux
 * 
 * @{
 */

/**
 * @brief   Buffer size used for TX and RX buffers
 *
 * Reduce this value if your expected traffic does not include full IPv6 MTU
 * sized packets.
 */
#ifdef CONFIG_SLIPMUX_NET_BUFSIZE_EXP
#define CONFIG_SLIPMUX_NET_BUFSIZE (1<<CONFIG_SLIPDEV_BUFSIZE_EXP)
#endif

#ifndef CONFIG_SLIPMUX_NET_BUFSIZE
#define CONFIG_SLIPMUX_NET_BUFSIZE (2048U)
#endif
/** @} */

/**
 * @brief   Configuration parameters for a slipdev
 */
typedef struct {
    uart_t uart;        /**< UART interface the device is connected to */
    uint32_t baudrate;  /**< baudrate to use with slipdev_params_t::uart */
} slipmux_params_t;

/**
 * @brief   Device descriptor for slipdev
 *
 * @extends netdev_t
 */
typedef struct {
    slipmux_params_t config;                /**< configuration parameters */
#if IS_USED(MODULE_SLIPMUX_NET)
    netdev_t netdev;                        /**< netdev */
    chunk_ringbuf_t net_rb;                 /**< Ringbuffer to store received networking frames.*/
                                            /* Written to from interrupts (with irq_disable */
                                            /* to prevent any simultaneous writes),         */
                                            /* consumed exclusively in the network stack's  */
                                            /* loop at _isr.                                */

    uint8_t net_rx[CONFIG_SLIPMUX_NET_BUFSIZE]; /**< memory used by RX buffer */
#endif
#if IS_USED(MODULE_SLIPMUX_COAP)
    chunk_ringbuf_t coap_rb;                /**< Ringbuffer stores received configuration frames */
    uint8_t coap_rx[CONFIG_SLIPMUX_COAP_BUFSIZE];/**< memory used by RX buffer */
    kernel_pid_t coap_server_pid;           /**< The PID of the CoAP server */
    event_t event;
#endif
    /**
     * @brief   Device state
     * @see     [Device state definitions](@ref drivers_slipdev_states)
     */
    uint8_t state;
} slipmux_t;

void slipmux_coap_set_event_queue(event_queue_t *q);
void unicoap_slipmux_recv_handler(event_t * event);
int slipmux_coap_recv(uint8_t *buf, size_t buf_size, slipmux_t *dev);
void slipmux_coap_send(uint8_t *buf, size_t len, const slipmux_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
