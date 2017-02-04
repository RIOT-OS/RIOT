/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 */
#include <stdio.h>
#include "periph/uart.h"

void board_init(void)
{
	volatile uint32_t * const _U3RXR = (uint32_t *) 0xBF80FA60;
	volatile uint32_t * const _RPF4R = (uint32_t *) 0xBF80FC50;

	volatile uint32_t * const _PORTFCLR = (uint32_t *) 0xBF886524;
	volatile uint32_t * const _TRISFCLR = (uint32_t *) 0xBF886514;
	volatile uint32_t * const _TRISFSET = (uint32_t *) 0xBF886518;
	volatile uint32_t * const _ODCFCLR  = (uint32_t *) 0xBF886544;

	/*
	 * Setup pin mux for UART3 this is the one connected
	 * to the mickroBUS
	 */
	*_U3RXR = 0x2;
	*_RPF4R = 0x1;
	*_PORTFCLR = 0x30;
	*_TRISFCLR = 0x10;
	*_TRISFSET = 0x20;
	*_ODCFCLR = 0x30;

	/* intialise UART used for debug (printf) */
#ifdef DEBUG_VIA_UART
	uart_init(DEBUG_VIA_UART, 9600, NULL, 0);
#endif
}

