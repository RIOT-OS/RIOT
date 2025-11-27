/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * For example, to use STDIO and also provide a network interface over SLIPMUX select:
 * 
 *     USEMODULE += slipmux_stdio
 *     USEMODULE += slipmux_net
 * 
 * @{
 *
 * @file
 * @brief   SLIPMUX definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Bennet Hattesen
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
 * @defgroup    drivers_slipmux_coap CoAP via SLIPMUX
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
 */
#if !defined(CONFIG_SLIPMUX_COAP_BUFSIZE) || defined(DOXYGEN)
#  define CONFIG_SLIPMUX_COAP_BUFSIZE (512U)
#endif
/** @} */


/**
 * @defgroup    drivers_slipmux_net IP via SLIPMUX / SLIP network device
 * @ingroup     drivers_netdev
 * @brief       SLIP network device over @ref drivers_periph_uart
 * @see         [RFC 1055](https://datatracker.ietf.org/doc/html/rfc1055)
 *
 * This extension is part of the [Slipmux draft](https://datatracker.ietf.org/doc/html/draft-bormann-t2trg-slipmux-03).
 * @warning This module is under development for optimizations and module names might change!
 *
 * This will offer the traditional Serial Line Internet Protocol as a network interface.
 * Uses GNRC by default, can also be used with LWIP.
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
#if !defined(CONFIG_SLIPMUX_NET_BUFSIZE) || defined(DOXYGEN)
#  ifdef CONFIG_SLIPMUX_NET_BUFSIZE_EXP
#    define CONFIG_SLIPMUX_NET_BUFSIZE (1<<CONFIG_SLIPMUX_NET_BUFSIZE_EXP)
#  else
#    define CONFIG_SLIPMUX_NET_BUFSIZE (2048U)
#  endif
#endif
/** @} */

/**
 * @brief   Configuration parameters for a slipmux device
 */
typedef struct {
    uart_t uart;        /**< UART interface the device is connected to */
    uint32_t baudrate;  /**< baudrate for the UART interface */
} slipmux_params_t;

/**
 * @brief   Device descriptor for slipmux
 */
typedef struct {
    slipmux_params_t config;                /**< configuration parameters */
#if IS_USED(MODULE_SLIPMUX_NET) || defined(DOXYGEN)
    netdev_t netdev;                        /**< netdev */
    chunk_ringbuf_t net_rb;                 /**< Ringbuffer to store received networking frames.*/
                                            /* Written to from interrupts (with irq_disable */
                                            /* to prevent any simultaneous writes),         */
                                            /* consumed exclusively in the network stack's  */
                                            /* loop at _isr.                                */

    uint8_t net_rx[CONFIG_SLIPMUX_NET_BUFSIZE]; /**< memory used by RX buffer */
#endif
#if IS_USED(MODULE_SLIPMUX_COAP) || defined(DOXYGEN)
    chunk_ringbuf_t coap_rb;                /**< Ringbuffer stores received configuration frames */
    uint8_t coap_rx[CONFIG_SLIPMUX_COAP_BUFSIZE];/**< memory used by RX buffer */
    kernel_pid_t coap_server_pid;           /**< The PID of the CoAP server */
    event_t event;                          /**< Event that is sent to the CoAP server */
#endif
    /**
     * @brief   Device state (decoder-, powerstate)
     */
    uint8_t state;
#if IS_USED(MODULE_SLIPMUX_STDIO) || IS_USED(MODULE_SLIPMUX_COAP)
    /**
     * @brief   Mutex to synchronize write operations to the UART between stdio,
     *          coap and net / SLIP sub-modules.
     */
    mutex_t mutex;
#endif
} slipmux_t;

#ifdef __cplusplus
}
#endif

/** @} */
