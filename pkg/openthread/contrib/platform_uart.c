/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread UART platform abstraction
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "stdio_uart.h"
#include "periph/uart.h"
#include "openthread/platform/uart.h"
#include "ot.h"
#include "event.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define OPENTHREAD_SPINEL_FRAME_MARKER      (0x7e)

static serial_msg_t gSerialMessage[OPENTHREAD_NUMBER_OF_SERIAL_BUFFER];
static uint16_t frameLength = 0;

static void _ev_serial_handler(event_t *event)
{
    (void) event;
    /* Tell OpenThread about the reception of a CLI command */
    otPlatUartReceived((uint8_t*)gSerialMessage[0].buf, gSerialMessage[0].length);
    gSerialMessage[0].serial_buffer_status = OPENTHREAD_SERIAL_BUFFER_STATUS_FREE;
}

static event_t ev_serial = {
    .handler = _ev_serial_handler
};

static void uart_handler(void* arg, char c) {
    (void)arg;

    if (frameLength == 0) {
        memset(&gSerialMessage[0], 0, sizeof(serial_msg_t));
    }
    switch (c) {
        case '\r':
        case '\n':
            if (frameLength > 0) {
                gSerialMessage[0].buf[frameLength] = c;
                frameLength++;
                gSerialMessage[0].length = frameLength;
                event_post(openthread_get_evq(), &ev_serial);
                frameLength = 0;
            }
            break;
        default:
            if (frameLength < OPENTHREAD_SERIAL_BUFFER_SIZE) {
                gSerialMessage[0].buf[frameLength] = c;
                frameLength++;
            }
            break;
    }
}

/* OpenThread will call this for enabling UART (required for OpenThread's CLI)*/
otError otPlatUartEnable(void)
{
    for (uint8_t i = 0; i < OPENTHREAD_NUMBER_OF_SERIAL_BUFFER; i++) {
        gSerialMessage[i].serial_buffer_status = OPENTHREAD_SERIAL_BUFFER_STATUS_FREE;
    }

    uart_init(STDIO_UART_DEV, STDIO_UART_BAUDRATE, (uart_rx_cb_t) uart_handler, NULL);
    return OT_ERROR_NONE;
}

/* OpenThread will call this for disabling UART */
otError otPlatUartDisable(void)
{
    uart_poweroff(STDIO_UART_DEV);
    return OT_ERROR_NONE;
}

/* OpenThread will call this for sending data through UART */
otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    uart_write(STDIO_UART_DEV, aBuf, aBufLength);

    /* Tell OpenThread the sending of UART is done */
    otPlatUartSendDone();

    return OT_ERROR_NONE;
}
