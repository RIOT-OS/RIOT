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
     * Setup pin mux for UART3 this is the one connected
     * to the mickroBUS
     */
    w32(U3RXR, 0x2);            /*connect pin RPF5 to UART3 RX*/
    w32(RPF4R, 0x1);            /*connect pin RPF4 to UART3 TX*/
    w32(PORTFCLR, BIT5 | BIT4); /*set '0' on Porf F pins 4 and 5 */
    w32(TRISFCLR, BIT4);        /*set PortF pin 4 for output */
    w32(TRISFSET, BIT5);        /*set PortF pin 5 for input */
    w32(ODCFCLR, BIT5 | BIT4);  /*set PortF pin 4 and 5 as not open-drain */

    /* intialise UART used for debug (printf) */
#ifdef DEBUG_VIA_UART
    uart_init(DEBUG_VIA_UART, DEBUG_UART_BAUD, NULL, 0);
#endif
}

