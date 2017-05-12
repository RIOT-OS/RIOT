/*
 * Copyright(C) 2016,2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include "periph/uart.h"
#include "bitarithm.h"
#include "board.h"

extern void dummy(void);

void board_init(void)
{
    /*
     * Setup pin mux for UART4 this is the one connected
     * to the ftdi chip (usb<->uart)
     */
    U4RXR =   0xb;            /* connect pin RPF2 to UART 4 RX */
    RPF8R =   0x2;            /* connect pin RPF8 to UART 4 TX */

    /* intialise UART used for debug (printf) */
#ifdef DEBUG_VIA_UART
    uart_init(DEBUG_VIA_UART, DEBUG_UART_BAUD, NULL, 0);
#endif

    /* Stop the linker from throwing away the PIC32 config register settings */
    dummy();
}

void pm_reboot(void)
{
    /* TODO, note this is needed to get 'default' example to build */
}
