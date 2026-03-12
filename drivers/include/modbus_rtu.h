/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_modbus_rtu Modbus RTU
 * @ingroup     drivers
 * @brief       Modbus RTU
 *
 * Modbus RTU implementation with support for client and servers modes
 * (formerly known as master and slaves). This implementation focusses on the
 * Modbus application data unit (ADU), which encapsulates the protocol data
 * unit (PDU) with additional framing and error-checking specific to the RTU
 * transport.
 *
 * With respect to the 'Modbus over Serial Line' specification, this
 * implementation deviates a bit:
 *
 * - Received bytes (characters) are buffered using a ringbuffer, before being
 *   processed, to prevent excessive blocking in an interrupt context.
 * - Only no-parity UART mode is supported, which is most common (see
 *   section 2.5.1 of the specification).
 * - A frame is considered to be complete when the checksum has been received
 *   and is correct. This implies that the inter-frame delay is not explicitly
 *   checked (see section 2.5.1.1 of the specification), but this improves
 *   performance by avoiding additional waiting. In some cases, where the
 *   client and server are not synchronized (e.g. a response frame is received
 *   while a request frame is expected), this may lead to frames being
 *   misinterpreted. However, the inter-character timeout and checksum
 *   verification should prevent incorrect frames from being handled.
 *
 * @{
 *
 * @file
 * @brief       Modbus RTU interface definitions
 *
 * @author      Bas Stottelaar <bas.stottelaar@gmail.com>
 */

#include "mutex.h"
#include "isrpipe.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "thread.h"
#include "ztimer.h"

#include "modbus.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Modbus RTU packet sizes
 */
#define MODBUS_RTU_ADU_ID_SIZE      1                           /**< size of ID */
#define MODBUS_RTU_ADU_CRC_SIZE     2                           /**< size of CRC */
#define MODBUS_RTU_ADU_MAX_SIZE     (MODBUS_PDU_MAX_SIZE + MODBUS_RTU_ADU_ID_SIZE + \
                                     MODBUS_RTU_ADU_CRC_SIZE)   /**< maximum size of RTU ADU */

/**
 * @brief   Modbus RTU device parameters
 */
typedef struct {
    gpio_t pin_rts;             /**< RTS pin, @p GPIO_UNDEF if not used */
    int pin_rts_enable;         /**< RTS pin state when transmitting */
    uart_t uart;                /**< UART device */
    uint32_t baudrate;          /**< UART baudrate */
    uint32_t response_timeout;  /**< timeout (in usec) when waiting for a response */
} modbus_rtu_params_t;

/**
 * @brief   Modbus RTU device structure
 *
 * These fields are internal to the implementation, and should not be used
 * directly.
 *
 * The buffer size of the isrpipe is fixed to 64 bytes, which should be
 * sufficient if the receive function is called frequently enough to pass on
 * incoming data.
 */
typedef struct {
    uint32_t byte_timeout;      /**< timeout (in usec) between two bytes (inter-character timeout) */
    uint32_t frame_timeout;     /**< timeout (in usec) between two frames (inter-frame timeout) */

    uint8_t buf[64];            /**< buffer for the isrpipe */
    isrpipe_t isrpipe;          /**< internal pipe for incoming bytes */

    ztimer_t idle_timer;        /**< timer to detect idle time between frames */
    mutex_t idle_lock;          /**< lock for idle timer */
} modbus_rtu_t;

/**
 * @brief   Modbus RTU driver descriptor
 */
extern const modbus_desc_t modbus_rtu_driver;

#ifdef __cplusplus
}
#endif

/** @} */
