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

#include "periph/uart.h"
#include "openthread/types.h"
#include "openthread/platform/uart.h"
#include "ot.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define UART_OPENTHREAD_BAUDRATE            (115200U)
#define UART_OPENTHREAD                     UART_DEV(0)
#define OPENTHREAD_SPINEL_FRAME_MARKER      (0x7e)

serial_msg_t * gSerialMessage[OPENTHREAD_NUMBER_OF_SERIAL_BUFFER];
uint8_t gSerialBuff[OPENTHREAD_NUMBER_OF_SERIAL_BUFFER][OPENTHREAD_SERIAL_BUFFER_SIZE];

#ifdef MODULE_OPENTHREAD_NCP_FTD
int8_t getFirstEmptySerialBuffer(void) {
    uint8_t i = 0;
    for (i = 0; i < OPENTHREAD_NUMBER_OF_SERIAL_BUFFER; i++) {
        if ( OPENTHREAD_SERIAL_BUFFER_STATUS_FREE == gSerialMessage[i]->serial_buffer_status ) {
            break;
        }
    }

    if ( i >= OPENTHREAD_NUMBER_OF_SERIAL_BUFFER ) {
        return OPENTHREAD_ERROR_NO_EMPTY_SERIAL_BUFFER;
    } else {
        return i;
    }
}

/* OpenThread will call this for enabling UART (required for OpenThread's NCP)*/
void uart_handler(void* arg, char c)  {

    static int16_t currentSerialBufferNumber = 0;
    static uint8_t frameLength = 0;
    static uint8_t gOnGoingSpinelReception = 0;

    msg_t msg;
    msg.type = OPENTHREAD_SERIAL_MSG_TYPE_EVENT;

    if ((c == OPENTHREAD_SPINEL_FRAME_MARKER) && (gOnGoingSpinelReception == false)) {      /* Start of Spinel Frame */
        currentSerialBufferNumber = getFirstEmptySerialBuffer();
        if (OPENTHREAD_ERROR_NO_EMPTY_SERIAL_BUFFER == currentSerialBufferNumber) {
            DEBUG("SERIAL: ERROR => OPENTHREAD_ERROR_NO_EMPTY_SERIAL_BUFFER found\n");
            return;
        }
        frameLength = 0;

        gSerialMessage[currentSerialBufferNumber]->buf[frameLength] = c;

        gOnGoingSpinelReception = true;
    }
    else if ((c == OPENTHREAD_SPINEL_FRAME_MARKER) && (gOnGoingSpinelReception == true)) {  /* End of Spinel Frame */
        if ( OPENTHREAD_ERROR_NO_EMPTY_SERIAL_BUFFER == currentSerialBufferNumber ) {
            return;
        }
        if (frameLength == 1) {  /* It means that we handle the Start of a Spinel frame instead of the end */

            frameLength--;
            return;
        }
        if( OPENTHREAD_SERIAL_BUFFER_STATUS_FULL != gSerialMessage[currentSerialBufferNumber]->serial_buffer_status) {
            gSerialMessage[currentSerialBufferNumber]->buf[frameLength] = (uint8_t) c;
            gSerialMessage[currentSerialBufferNumber]->serial_buffer_status = OPENTHREAD_SERIAL_BUFFER_STATUS_READY_TO_PROCESS;
            gSerialMessage[currentSerialBufferNumber]->length = frameLength + 1;
            msg.content.ptr = gSerialMessage[currentSerialBufferNumber];
            msg_send_int(&msg, openthread_get_pid());
        }
        else {
            gSerialMessage[currentSerialBufferNumber]->serial_buffer_status = OPENTHREAD_SERIAL_BUFFER_STATUS_FREE;
        }
        gOnGoingSpinelReception = false;
        frameLength = 0;
    }
    else if (gOnGoingSpinelReception == true) {         /* Payload of Spinel Frame */
        if ( OPENTHREAD_ERROR_NO_EMPTY_SERIAL_BUFFER == currentSerialBufferNumber ) {
            return;
        }
        if ( OPENTHREAD_SERIAL_BUFFER_STATUS_FULL != gSerialMessage[currentSerialBufferNumber]->serial_buffer_status) {
            gSerialMessage[currentSerialBufferNumber]->buf[frameLength] = (uint8_t) c;
        }
    }

    if (gOnGoingSpinelReception == true) {
        frameLength++;
        if ( frameLength >= OPENTHREAD_SERIAL_BUFFER__PAYLOAD_SIZE) {
            DEBUG("SERIAL: ERROR => OPENTHREAD_SERIAL_BUFFER__PAYLOAD_SIZE overflowed\n");
            gSerialMessage[currentSerialBufferNumber]->serial_buffer_status = OPENTHREAD_SERIAL_BUFFER_STATUS_FULL;
        }
    }
}

#else

void uart_handler(void* arg, char c) {
    msg_t msg;
    msg.type = OPENTHREAD_SERIAL_MSG_TYPE_EVENT;
    gSerialMessage[0]->buf[0] = (uint8_t) c;
    gSerialMessage[0]->length = 1;
    msg.content.ptr = gSerialMessage[0];
    msg_send_int(&msg, openthread_get_pid());
}

#endif /* MODULE_OPENTHREAD_NCP_FTD */

/* OpenThread will call this for enabling UART (required for OpenThread's CLI)*/
otError otPlatUartEnable(void)
{
    for (uint8_t i = 0; i < OPENTHREAD_NUMBER_OF_SERIAL_BUFFER; i++) {
        gSerialMessage[i] = (serial_msg_t*) &gSerialBuff[i];
        gSerialMessage[i]->serial_buffer_status = OPENTHREAD_SERIAL_BUFFER_STATUS_FREE;
    }

    uart_init(UART_OPENTHREAD, UART_OPENTHREAD_BAUDRATE, (uart_rx_cb_t) uart_handler, NULL);
    return OT_ERROR_NONE;
}

/* OpenThread will call this for disabling UART */
otError otPlatUartDisable(void)
{
    uart_poweroff(UART_OPENTHREAD);
    return OT_ERROR_NONE;
}

/* OpenThread will call this for sending data through UART */
otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    uart_write(UART_OPENTHREAD, aBuf, aBufLength);

    /* Tell OpenThread the sending of UART is done */
    otPlatUartSendDone();

    return OT_ERROR_NONE;
}
