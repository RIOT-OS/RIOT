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
 * @{
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
 * @brief   Modbus RTU sizes
 *
 * @{
 */
#define MODBUS_RTU_PACKET_CRC_SIZE              2
#define MODBUS_RTU_PACKET_REQUEST_SIZE_MIN      8       /* includes CRC */
#define MODBUS_RTU_PACKET_RESPONSE_SIZE_MIN     5       /* includes CRC */
#define MODBUS_RTU_PACKET_SIZE_MAX              256     /* includes CRC */
/** @} */

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
    modbus_t dev;           /**< @ref modbus_t base class */
    const modbus_rtu_params_t* params;  /**< device parameters */
    uint32_t timeout;       /**< amount of time (usec) to wait for a slave to
                                 begin sending a */
    uint32_t rx_timeout;    /**< time between two bytes before timeing out */
    kernel_pid_t pid;       /**< PID of the thread that waits for bytes */
    uint8_t buffer[MODBUS_RTU_PACKET_SIZE_MAX];     /**< buffer for requests
                                                         and responses */
    uint8_t buffer_size;    /**< current size of @p buffer, in bytes */
    mutex_t buffer_mutex;   /**< mutex for interacting with @p buffer */
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
 */
int modbus_rtu_init(modbus_rtu_t *modbus, const modbus_rtu_params_t *params);

/**
 * @brief   Send request to slave
 *
 * Send request to slave. This will block until a response message is received.
 *
 * @param[in] modbus    pointer modbus
 * @param[in] message   pointer modbus message
 *
 * @return              MODBUS_OK on success
 */
int modbus_rtu_send_request(modbus_rtu_t *modbus,
                            modbus_message_t *message);

/**
 * @brief   Wait and handle request from master
 *
 * Wait for a request from a master device. This will block until a message is
 * received.
 *
 * Once a request is received, it will be checked and the callback function
 * @p cb will be invoked. It is up to the callback to properly handle the
 * request if deemed supported.
 *
 * Read requests must ensure that @p message->data points to the requested1
 * data, so that the implementation can return it in the response. To support
 * zero-copy, the callback can also write directly to @p message->data
 * (at most @p message->data_size bytes), but is also valid to point this
 * pointer to a different application buffer of sufficient size. In that case,
 * @p message->data_size must be set updated as well.
 *
 * Write requests can copy data from @p message->data to application buffers,
 * taking into account @p message->data_size.
 *
 * If the message cannot be handled, @p message->exc must be set. For example,
 * when the function code is not applicable, or the read/write exceeds bounds.
 *
 * Note that @p message->data is a raw buffer. Copy registers or bits using
 * the appropriate helpers in @ref drivers_modbus.
 *
 * If the callback returns any other code than @p MODBUS_OK, no response will
 * be sent to the master. This also applies to sending exception codes.
 *
 * Use as slave only.
 *
 * @param[in] modbus    pointer to modbus
 * @param[in] message   pointer to modbus message
 * @param[in] cb        callback function that handles the request
 *
 * @return              MODBUS_OK on success
 */
int modbus_rtu_recv_request(modbus_rtu_t *modbus, modbus_message_t *message,
                            modbus_request_cb_t cb);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_RTU_H */
/** @} */
