/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


#include "periph/uart.h"
#include "board.h"

#define UxMODE(U)      U.regs[0x00/4]
#define UxMODECLR(U)   U.regs[0x04/4]
#define UxMODESET(U)   U.regs[0x08/4]
#define ON  0x8000
#define STSEL   1
#define PDSEL_9N    6
#define PDSEL_8O    4
#define PDSEL_8E    2
#define PDSEL_8N    0
#define UEN_CTSRTS  0x200
#define UxSTA(U)    U.regs[0x10/4]
#define UxSTACLR(U) U.regs[0x14/4]
#define UxSTASET(U) U.regs[0x18/4]
#define URXDA 0x1
#define TRMT 0x100
#define UTXBF 0x200
#define UTXEN 0x400
#define URXEN 0x1000
#define UxTXREG(U)  U.regs[0x20/4]
#define UxRXREG(U)  U.regs[0x30/4]
#define UxBRG(U)    U.regs[0x40/4]
#define UART_REGS_SPACING 0x200

/* PERIPHERAL_CLOCK must be defined in board file */


typedef struct PIC32_UART_tag {
	volatile uint32_t	*regs;
	uint32_t		clock;
} PIC32_UART_T;

/* pic uarts are numbered 1 to 6 */
PIC32_UART_T pic_uart[UART_NUMOF + 1];

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
	if (uart > UART_NUMOF || uart == 0) /*No uart 0 on pic32*/
		return -1;

	/* Pin Mux should be setup in board file */

	pic_uart[uart].regs = (volatile uint32_t *)(_UART1_BASE_ADDRESS + (uart-1) * UART_REGS_SPACING);
	pic_uart[uart].clock = PERIPHERAL_CLOCK;


	UxBRG(pic_uart[uart]) = (pic_uart[uart].clock / (16 * baudrate)) - 1;
	UxSTA(pic_uart[uart]) = 0;
	UxMODE(pic_uart[uart]) = ON;
	UxSTASET(pic_uart[uart]) = URXEN;
	UxSTASET(pic_uart[uart]) = UTXEN;

	return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
	if (uart > UART_NUMOF || uart == 0)
		return;

	while (len--) {
		while (UxSTA(pic_uart[uart]) & UTXBF) {}
		UxTXREG(pic_uart[uart]) = *data++;
	}
}

void uart_poweron(uart_t uart)
{
	if (uart > UART_NUMOF || uart == 0)
		return;

	UxMODESET(pic_uart[uart]) = ON;

}

void uart_poweroff(uart_t uart)
{
	if (uart > UART_NUMOF || uart == 0)
		return;

	UxMODECLR(pic_uart[uart]) = ON;
}
