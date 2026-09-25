/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_modbus_ascii Modbus ASCII transport
 * @ingroup     drivers_modbus
 * @brief       Modbus ASCII transport
 *
 * Modbus ASCII transport implementation. Each binary byte of the ADU is
 * encoded as two ASCII hex characters, framed by a leading ':' (0x3A) and a
 * trailing 'CR LF' (0x0D 0x0A) sequence. Frames are protected by a
 * Longitudinal Redundancy Check (LRC) byte instead of the CRC used by the RTU
 * transport.
 *
 * With respect to the 'Modbus over Serial Line' specification, this
 * implementation deviates a bit:
 *
 * - Received bytes (characters) are buffered using a ringbuffer, before being
 *   processed, to prevent excessive blocking in an interrupt context.
 * - Only no-parity UART mode is supported, which is most common (see
 *   section 2.5.2 of the specification).
 * - No bus-idle or turnaround handling is performed before sending: ASCII
 *   frames are explicitly delimited by ':' and CR LF, so receivers can
 *   resynchronize on the next start of frame.
 *
 * @note When an RS-485 transceiver is used, it must not echo transmitted
 *       bytes back to the receiver, as the echo would be interpreted as a
 *       response.
 *
 * @{
 *
 * @file
 * @brief       Modbus ASCII transport definitions
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include <stdbool.h>
#include <stdint.h>

#include "isrpipe.h"
#include "periph/gpio.h"
#include "periph/uart.h"

#include "modbus.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Modbus ASCII frame sizes (binary form, before hex encoding)
 * @{
 */
#define MODBUS_ASCII_ADU_ID_SIZE    (1)     /**< size of address byte */
#define MODBUS_ASCII_ADU_LRC_SIZE   (1)     /**< size of LRC byte */
#define MODBUS_ASCII_ADU_MAX_SIZE   (MODBUS_PDU_MAX_SIZE + \
                                     MODBUS_ASCII_ADU_ID_SIZE + \
                                     MODBUS_ASCII_ADU_LRC_SIZE) /**< maximum size of ASCII ADU in
                                                                     binary form */
/** @} */

/**
 * @brief   Size of the isrpipe buffer (in bytes) for incoming data
 *
 * The default should be sufficient if the receive function is called
 * frequently enough to pass on incoming data.
 */
#ifndef MODBUS_ASCII_ISR_BUF_SIZE
#  define MODBUS_ASCII_ISR_BUF_SIZE (64)
#endif

/**
 * @brief   Modbus ASCII device parameters
 */
typedef struct {
    gpio_t pin_rts;             /**< RTS pin, @ref GPIO_UNDEF if not used */
    bool pin_rts_enable;        /**< RTS pin state when transmitting */
    uart_t uart;                /**< UART device */
    uint32_t baudrate;          /**< UART baudrate */
    uint32_t response_timeout;  /**< timeout (in usec) when waiting for a response */
    uint32_t char_timeout;      /**< inter-character timeout (in usec), 0 blocks indefinitely */
} modbus_ascii_params_t;

/**
 * @brief   Modbus ASCII device structure
 *
 * These fields are internal to the implementation, and should not be used
 * directly.
 */
typedef struct {
    uint8_t isr_buf[MODBUS_ASCII_ISR_BUF_SIZE];   /**< buffer for the isrpipe */
    isrpipe_t isrpipe;          /**< internal pipe for incoming bytes */

    uint8_t buf[MODBUS_ASCII_ADU_MAX_SIZE];       /**< binary ADU buffer */
} modbus_ascii_t;

/**
 * @brief   Modbus ASCII transport descriptor
 */
extern const modbus_transport_desc_t modbus_ascii_transport;

#ifdef __cplusplus
}
#endif

/** @} */
