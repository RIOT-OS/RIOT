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

#include "uart_stdio.h"
#include "periph/uart.h"
#include "openthread/types.h"
#include "openthread/platform/uart.h"
#include "ot.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define OPENTHREAD_SPINEL_FRAME_MARKER      (0x7e)

static serial_msg_t gSerialMessage[OPENTHREAD_NUMBER_OF_SERIAL_BUFFER];
static uint16_t frameLength = 0;

#ifdef MODULE_OPENTHREAD_NCP_FTD
static int8_t currentSerialBufferNumber = 0;
static bool gOnGoingSpinelReception = false;

int8_t getFirstEmptySerialBuffer(void) {
    int8_t i = 0;
    for (i = 0; i < OPENTHREAD_NUMBER_OF_SERIAL_BUFFER; i++) {
        if (gSerialMessage[i].serial_buffer_status == OPENTHREAD_SERIAL_BUFFER_STATUS_FREE) {
            break;
        }
    }

    if (i >= OPENTHREAD_NUMBER_OF_SERIAL_BUFFER) {
        return OPENTHREAD_ERROR_NO_EMPTY_SERIAL_BUFFER;
    } else {
        return i;
    }
}

/* UART interrupt handler (required for OpenThread's NCP)*/
static void uart_handler(void* arg, char c)  {
    (void) arg;

    if ((c == OPENTHREAD_SPINEL_FRAME_MARKER) && (gOnGoingSpinelReception == false)) {      /* Start of Spinel Frame */
        currentSerialBufferNumber = getFirstEmptySerialBuffer();
        if (OPENTHREAD_ERROR_NO_EMPTY_SERIAL_BUFFER == currentSerialBufferNumber) {
            DEBUG("SERIAL: ERROR => OPENTHREAD_ERROR_NO_EMPTY_SERIAL_BUFFER found\n");
            return;
        }
        frameLength = 0;

        gSerialMessage[currentSerialBufferNumber].buf[frameLength] = c;

        gOnGoingSpinelReception = true;
    }
    else if ((c == OPENTHREAD_SPINEL_FRAME_MARKER) && (gOnGoingSpinelReception == true)) {  /* End of Spinel Frame */
        if (currentSerialBufferNumber == OPENTHREAD_ERROR_NO_EMPTY_SERIAL_BUFFER) {
            return;
        }
        if (frameLength == 1) {  /* It means that we handle the Start of a Spinel frame instead of the end */

            frameLength--;
            return;
        }
        if(gSerialMessage[currentSerialBufferNumber].serial_buffer_status != OPENTHREAD_SERIAL_BUFFER_STATUS_FULL) {
            gSerialMessage[currentSerialBufferNumber].buf[frameLength] = (uint8_t) c;
            gSerialMessage[currentSerialBufferNumber].serial_buffer_status = OPENTHREAD_SERIAL_BUFFER_STATUS_READY_TO_PROCESS;
            gSerialMessage[currentSerialBufferNumber].length = frameLength + 1;
            msg_t msg;
            msg.type = OPENTHREAD_SERIAL_MSG_TYPE_EVENT;
            msg.content.ptr = &gSerialMessage[currentSerialBufferNumber];
            msg_send_int(&msg, openthread_get_pid());
        }
        else {
            gSerialMessage[currentSerialBufferNumber].serial_buffer_status = OPENTHREAD_SERIAL_BUFFER_STATUS_FREE;
        }
        gOnGoingSpinelReception = false;
        frameLength = 0;
    }
    else if (gOnGoingSpinelReception == true) {         /* Payload of Spinel Frame */
        if (currentSerialBufferNumber == OPENTHREAD_ERROR_NO_EMPTY_SERIAL_BUFFER) {
            return;
        }
        if (gSerialMessage[currentSerialBufferNumber].serial_buffer_status != OPENTHREAD_SERIAL_BUFFER_STATUS_FULL) {
            gSerialMessage[currentSerialBufferNumber].buf[frameLength] = (uint8_t) c;
        }
    }

    if (gOnGoingSpinelReception == true) {
        frameLength++;
        if (frameLength >= OPENTHREAD_SERIAL_BUFFER__PAYLOAD_SIZE) {
            DEBUG("SERIAL: ERROR => OPENTHREAD_SERIAL_BUFFER__PAYLOAD_SIZE overflowed\n");
            gSerialMessage[currentSerialBufferNumber].serial_buffer_status = OPENTHREAD_SERIAL_BUFFER_STATUS_FULL;
        }
    }
}

#else

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
                msg_t msg;
                msg.type = OPENTHREAD_SERIAL_MSG_TYPE_EVENT;
                msg.content.ptr = &gSerialMessage[0];
                msg_send_int(&msg, openthread_get_pid());
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

#endif /* MODULE_OPENTHREAD_NCP_FTD */

/* OpenThread will call this for enabling UART (required for OpenThread's CLI)*/
otError otPlatUartEnable(void)
{
    for (uint8_t i = 0; i < OPENTHREAD_NUMBER_OF_SERIAL_BUFFER; i++) {
        gSerialMessage[i].serial_buffer_status = OPENTHREAD_SERIAL_BUFFER_STATUS_FREE;
    }

    uart_init(UART_STDIO_DEV, UART_STDIO_BAUDRATE, (uart_rx_cb_t) uart_handler, NULL);
    return OT_ERROR_NONE;
}

/* OpenThread will call this for disabling UART */
otError otPlatUartDisable(void)
{
    uart_poweroff(UART_STDIO_DEV);
    return OT_ERROR_NONE;
}

/* OpenThread will call this for sending data through UART */
otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    uart_write(UART_STDIO_DEV, aBuf, aBufLength);

    /* Tell OpenThread the sending of UART is done */
    otPlatUartSendDone();

    return OT_ERROR_NONE;
}
