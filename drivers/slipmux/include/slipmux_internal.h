/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup drivers_slipmux
 * @{
 *
 * @file
 * @internal
 * @brief   Internal SLIPMUX device definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Bennet Hattesen
 */

#include <stddef.h>
#include <stdint.h>

#include "periph/uart.h"
#include "mutex.h"

#include "slipmux_params.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of slipmux devices, Parameters and array defined in slipmux_params.h
 */
#define SLIPMUX_DEV_NUM ARRAY_SIZE(slipmux_params)

/**
 * @brief   List of slipmux devices.
 */
extern slipmux_t slipmux_devs[SLIPMUX_DEV_NUM];

/**
 * @name    SLIP marker bytes
 * @see     [RFC 1055](https://tools.ietf.org/html/rfc1055)
 * @{
 */
/**
 * @brief   Ends a frame
 */
#define SLIPMUX_END         (0xc0U)
/**
 * @brief   Initiates escape sequence
 */
#define SLIPMUX_ESC         (0xdbU)
/**
 * @brief   Ends escape sequence, next byte is END
 */
#define SLIPMUX_END_ESC     (0xdcU)
/**
 * @brief   Ends escape sequence, next byte is ESC
 */
#define SLIPMUX_ESC_ESC     (0xddU)
/** @} */

/**
 * @name    SLIPMUX marker bytes
 * @see     [SLIPMUX](https://tools.ietf.org/html/draft-bormann-t2trg-slipmux-03#section-4)
 * @{
 */
/**
 * @brief   Starts a diagnostic frame
 */
#define SLIPMUX_START_STDIO (0x0aU)
/**
 * @brief   Starts a configuration frame
 */
#define SLIPMUX_START_COAP (0xa9U)
/**
 * @brief   Starts an IP packet frame
 */
#define SLIPMUX_START_NET(byte) (\
            /* is it an IPv4 packet? */ \
            (byte >= 0x45 && byte <= 0x4f) || \
            /* or is it an IPv6 packet? */ \
            (byte >= 0x60 && byte <= 0x6f) \
        )
/** @} */

/**
 * @brief   Decoder internal state
 */
enum {
    /* Device is in no mode (currently did not receiving any data frame) */
    SLIPMUX_STATE_NONE = 0,
    /* Device handles data as network device */
    SLIPMUX_STATE_NET,
    /* Device handles data as network device, next byte is escaped */
    SLIPMUX_STATE_NET_ESC,
    /* Device handles received data to stdin */
    SLIPMUX_STATE_STDIN,
    /* Device handles received data to stdin, next byte is escaped */
    SLIPMUX_STATE_STDIN_ESC,
    /* Device handles received data as CoAP message */
    SLIPMUX_STATE_COAP,
    /* Device handles received data as CoAP message, next byte is escaped */
    SLIPMUX_STATE_COAP_ESC,
    /* Device is in standby, will wake up when sending data */
    SLIPMUX_STATE_STANDBY,
    /* Device is in sleep mode */
    SLIPMUX_STATE_SLEEP,
};

/**
 * @brief   Callback for the UART on receiving data
 */
void slipmux_rx_cb(void *arg, uint8_t byte);

/**
 * @brief   Writes one byte to UART without escaping
 *
 * @param[in] uart  The UART device to write to.
 * @param[in] byte  The byte to write to @p uart.
 */
static inline void slipmux_write_byte(uart_t uart, uint8_t byte)
{
    uart_write(uart, &byte, 1U);
}

/**
 * @brief   Write multiple bytes SLIPMUX-escaped to UART
 *
 * @param[in] uart  The UART device to write to.
 * @param[in] data  The bytes to write SLIPMUX-escaped to @p uart.
 * @param[in] len   Number of bytes in @p data.
 */
void slipmux_write_bytes(uart_t uart, const uint8_t *data, size_t len);

/**
 * @brief   Acquire exclusive access for writing
 *          Guarded for _stdio or _coap, the idea being that if only _net is
 *          used, you are most likely to care about performance.
 */
static inline void slipmux_lock(slipmux_t *dev)
{
    (void) dev;
#if IS_USED(MODULE_SLIPMUX_STDIO) || IS_USED(MODULE_SLIPMUX_COAP)
    mutex_lock(&dev->mutex);
#endif
}

/**
 * @brief   Release the exclusive access
 */
static inline void slipmux_unlock(slipmux_t *dev)
{
    (void) dev;
#if IS_USED(MODULE_SLIPMUX_STDIO) || IS_USED(MODULE_SLIPMUX_COAP)
    mutex_unlock(&dev->mutex);
#endif
}

#if IS_USED(MODULE_SLIPMUX_COAP) || defined(DOXYGEN)
/**
 * @brief   Initialise the CoAP handling
 *
 * @param[in] dev  The Slipmux device to initialise.
 * @param[in] index  Device number.
 */
void slipmux_coap_init(slipmux_t *dev, unsigned index);

/**
 * @brief   Inform the CoAP server there is new work to do.
 *          Called in ISR context!
 *
 * @param[in] dev  The Slipmux device to notify.
 */
void slipmux_coap_notify(slipmux_t *dev);
#endif /* MODULE_SLIPMUX_COAP */

#if IS_USED(MODULE_SLIPMUX_NET) || defined(DOXYGEN)
/**
 * @brief   Initialise the network handling
 *
 * @param[in] dev  The Slipmux device to initialise.
 * @param[in] index  Device number.
 */
void slipmux_net_init(slipmux_t *dev, unsigned index);

/**
 * @brief   Inform the Network interface there is new work to do.
 *          Called in ISR context!
 *
 * @param[in] dev  The Slipmux device to notify.
 */
void slipmux_net_notify(slipmux_t *dev);
#endif /* MODULE_SLIPMUX_NET */

#ifdef __cplusplus
}
#endif

/** @} */
