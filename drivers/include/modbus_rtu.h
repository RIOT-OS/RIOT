/*
 * Copyright (C) 2020-2023 Denis Litvinov <li.denis.iv@gmail.com>
 * Copyright (C) 2023-2024 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_modbus_rtu Modbus RTU
 * @ingroup     drivers
 * @brief       Modbus RTU
 *
 * Modbus RTU implementation with support for master and slave modes.
 *
 * @{
 *
 * @file
 * @brief       Modbus RTU interface definitions
 *
 * @author      Denis Litvinov <li.denis.iv@gmail.com>
 * @author      Bas Stottelaar <bas.stottelaar@gmail.com>
 */

#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include "mutex.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "thread.h"

#include "modbus.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Modbus RTU packet sizes
 */
#define MODBUS_RTU_PACKET_CRC_SIZE              2       /**< size of CRC */
#define MODBUS_RTU_PACKET_REQUEST_SIZE_MIN      8       /**< minimal request size (with CRC) */
#define MODBUS_RTU_PACKET_RESPONSE_SIZE_MIN     5       /**< minimal response size (with CRC) */
#define MODBUS_RTU_PACKET_SIZE_MAX              256     /**< maximum response size (with CRC) */

/**
 * @brief   Modbus RTU device parameters
 */
typedef struct {
    gpio_t pin_rts;         /**< RTS pin, @p GPIO_UNDEF if not used */
    int pin_rts_enable;     /**< RTS pin state when transmitting */
    uart_t uart;            /**< UART device */
    uint32_t baudrate;      /**< UART Baudrate */
} modbus_rtu_params_t;

/**
 * @brief   Modbus RTU device structure
 */
typedef struct {
    modbus_t dev;                               /**< @ref modbus_t base class */
    const modbus_rtu_params_t *params;          /**< device parameters */
    uint32_t timeout;                           /**< amount of time (usec) to wait for a slave to
                                                     begin sending */
    uint32_t rx_timeout;                        /**< timeout between two bytes */
    kernel_pid_t pid;                           /**< PID of the thread that waits for bytes */
    uint8_t buffer[MODBUS_RTU_PACKET_SIZE_MAX]; /**< buffer for requests and responses */
    uint8_t buffer_size;                        /**< current size of @p buffer, in bytes */
    mutex_t buffer_mutex;                       /**< mutex for interacting with @p buffer */
} modbus_rtu_t;

/**
 * @brief   Initialize the given Modbus device
 *
 * Initialize fields of modbus and UART, RTS pin.
 *
 * @param[in] modbus    pointer modbus
 * @param[in] params    device initialization parameters
 *
 * @return              MODBUS_OK on success
 * @return              other error code on failure
 */
int modbus_rtu_init(modbus_rtu_t *modbus, const modbus_rtu_params_t *params);

/**
 * @brief   Send request to slave
 *
 * Send request to slave and wait for a response message. Request messages
 * must be valid and have sufficient sized buffer.
 *
 * In case of a read request message, @p message->data can be set to @c NULL.
 * In this case, the response message will modify @p message->data to point
 * to the response data within the internal buffer, instead of copying the
 * data.
 *
 * This will block until a response message is received, or a timeout occurs.
 *
 * Use as master only.
 *
 * @param[in] modbus    pointer modbus
 * @param[in] message   pointer modbus message
 *
 * @return              MODBUS_ERR_CRC on CRC error
 * @return              MODBUS_ERR_REQUEST on response message error
 * @return              MODBUS_ERR_RESPONSE on response message error
 * @return              MODBUS_ERR_TIMEOUT on timeout
 * @return              MODBUS_OK on success
 * @return              other error code on failure
 */
int modbus_rtu_send_request(modbus_rtu_t *modbus,
                            modbus_message_t *message);

/**
 * @brief   Wait and handle request from master
 *
 * Wait for any request message from a master device. Once a request message is
 * received, the callback will be invoked to handle the request. Depending on
 * the return value of the callback, a response message will be sent back.
 *
 * This will block until a request message is received.
 *
 * Use as slave only.
 *
 * @see @ref modbus_request_cb_t
 *
 * @param[in] modbus    pointer to modbus
 * @param[in] message   pointer to modbus message
 * @param[in] cb        callback function that handles the request
 *
 * @return              MODBUS_OK on success
 * @return              MODBUS_ERR_CRC on CRC error
   & @return              other error code on failure
 */
int modbus_rtu_recv_request(modbus_rtu_t *modbus, modbus_message_t *message,
                            modbus_request_cb_t cb);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_RTU_H */
/** @} */
