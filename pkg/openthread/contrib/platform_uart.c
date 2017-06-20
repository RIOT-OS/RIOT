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
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "periph/uart.h"
#include "openthread/platform/uart.h"

/* OpenThread will call this for enabling UART (required for OpenThread's CLI)*/
ThreadError otPlatUartEnable(void)
{
    return kThreadError_None;
}

/* OpenThread will call this for disabling UART */
ThreadError otPlatUartDisable(void)
{
    return kThreadError_None;
}

/* OpenThread will call this for sending data through UART */
ThreadError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    uart_write(UART_DEV(0), aBuf, aBufLength);

    /* Tell OpenThread the sending of UART is done */
    otPlatUartSendDone();

    return kThreadError_None;
}
