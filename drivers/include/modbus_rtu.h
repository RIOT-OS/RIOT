/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_modbus_rtu Modbus RTU transport
 * @ingroup     drivers_modbus
 * @brief       Modbus RTU transport
 *
 * Modbus RTU transport implementation. This implementation focuses on the
 * Modbus application data unit (ADU), which encapsulates the protocol data
 * unit (PDU) with additional framing and error-checking specific to the RTU
 * transport.
 *
 * Bytes are received using UART interrupts and buffered in an internal buffer
 * before being processed.
 *
 * With respect to the 'Modbus over Serial Line' specification, this
 * implementation deviates a bit:
 *
 * - Received bytes (characters) are buffered using a ringbuffer, before being
 *   processed, to prevent excessive blocking in an interrupt context.
 * - Only no-parity UART mode is currently supported, which is most common (see
 *   section 2.5.1 of the specification).
 * - The inter-character and inter-frame timeouts (see section 2.5.1.1 of the
 *   specification) are defined as the time between the end of a character and
 *   the start of the next. Since interrupts are used to receive characters,
 *   these timeouts are compensated for the additional time for a full
 *   character to be received.
 * - Sending fails fast: when the bus is not idle at the time of sending,
 *   @c -EBUSY is returned instead of waiting for the frame in flight to
 *   complete.
 *
 * To keep the receiver aligned to frame boundaries, buffered data is discarded
 * whenever the bus becomes idle (3.5 character times without bus activity)
 * before a receive call has consumed it. In practice, a receive call consumes
 * bytes while the frame is still arriving, but this means that:
 *
 * - Requests that arrive while the application is not listening are dropped,
 *   and the client is expected to retry.
 * - A receive call that is delayed for more than the inter-frame timeout (e.g.
 *   by a higher-priority thread) observes a truncated frame, which is reported
 *   as a checksum error.
 *
 * @note When an RS-485 transceiver is used, it must not echo transmitted
 *       bytes back to the receiver, as the echo would be interpreted as a
 *       response.
 *
 * @{
 *
 * @file
 * @brief       Modbus RTU transport definitions
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include <stdbool.h>
#include <stdint.h>

#include "mutex.h"
#include "isrpipe.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "ztimer.h"

#include "modbus.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Modbus RTU sizes
 * @{
 */
#define MODBUS_RTU_ADU_ID_SIZE      (1)     /**< size of ID */
#define MODBUS_RTU_ADU_CRC_SIZE     (2)     /**< size of CRC */
#define MODBUS_RTU_ADU_MAX_SIZE     (MODBUS_PDU_MAX_SIZE + \
                                     MODBUS_RTU_ADU_ID_SIZE + \
                                     MODBUS_RTU_ADU_CRC_SIZE)   /**< maximum size of RTU ADU */
/** @} */

/**
 * @brief   Size of the isrpipe buffer (in bytes) for incoming data
 *
 * The default should be sufficient if the receive function is called
 * frequently enough to pass on incoming data.
 */
#ifndef MODBUS_RTU_ISR_BUF_SIZE
#  define MODBUS_RTU_ISR_BUF_SIZE   (64)
#endif

/**
 * @brief   Modbus RTU device parameters
 */
typedef struct {
    gpio_t pin_rts;             /**< RTS pin, @ref GPIO_UNDEF if not used */
    bool pin_rts_enable;        /**< RTS pin state when transmitting */
    uart_t uart;                /**< UART device */
    uint32_t baudrate;          /**< UART baudrate */
    uint32_t response_timeout;  /**< timeout (in usec) when waiting for a response */
} modbus_rtu_params_t;

/**
 * @brief   Modbus RTU device structure
 *
 * These fields are internal to the implementation and should not be used
 * directly.
 */
typedef struct {
    uint32_t byte_timeout;      /**< precomputed timeout (in usec) between two bytes
                                     (inter-character timeout) */
    uint32_t frame_timeout;     /**< precomputed timeout (in usec) between two frames
                                     (inter-frame timeout) */

    uint8_t isr_buf[MODBUS_RTU_ISR_BUF_SIZE];   /**< buffer for the isrpipe */
    isrpipe_t isrpipe;          /**< internal pipe for incoming bytes */

    uint8_t buf[MODBUS_RTU_ADU_MAX_SIZE];       /**< buffer for the application data unit (ADU) */

    ztimer_t idle_timer;        /**< timer to detect idle time between frames */
    mutex_t idle_lock;          /**< lock for idle timer */
} modbus_rtu_t;

/**
 * @brief   Modbus RTU transport descriptor
 */
extern const modbus_transport_desc_t modbus_rtu_transport;

#ifdef __cplusplus
}
#endif

/** @} */
