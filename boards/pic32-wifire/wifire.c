/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 */

#include <stdio.h>
#include <stdint.h>
#include "bitarithm.h"
#include "periph/uart.h"
#include "periph_uart.h"

static inline void w32(uint32_t addr, uint32_t val)
{
    *((volatile uint32_t *)addr) = val;
}

void board_init(void)
{
    /*
     * Setup pin mux for UART4 this is the one connected
     * to the ftdi chip (usb<->uart)
     */
    w32(U4RXR, 0xb);            /* connect pin RPF2 to UART 4 RX */
    w32(RPF8R, 2);              /* connect pin RPF8 to UART 4 TX */
    w32(PORTFCLR, BIT8 | BIT2); /* clear down port F pins 2 and 8 */
    w32(TRISFCLR, BIT2);        /* set portf pin 2 as input */
    w32(TRISFSET, BIT8);        /* set portf pin 8 as output */
    w32(ODCFCLR,  BIT8 | BIT2); /* set portf pint 2 and 8 as not open-drain */

    /* intialise UART used for debug (printf) */
#ifdef DEBUG_VIA_UART
    uart_init(DEBUG_VIA_UART, DEBUG_UART_BAUD, NULL, 0);
#endif
}

void reboot(void)
{
    /* TODO, note this is needed to get 'default' example to build */
}
