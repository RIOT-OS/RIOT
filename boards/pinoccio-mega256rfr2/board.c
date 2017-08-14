/*
 * Copyright (C) 2016 RWTH Aachen, Josua Arndt
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_pinoccio-mega256rfr2
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Pinoccio Mega 256rfr2 board
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */

#include "board.h"

#include <stdio.h>
#include <avr/io.h>

#include "cpu.h"
#include "uart_stdio.h"

void system_stdio_init(void);
static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE uart_stdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void board_init(void)
{
    /* PD4 BKPKVCC-EN to low  */
	DDRD |= 1<<DDRD4; 			// set port D4 as output
	PORTD &= ~(1<<PORTD4);    	// set D4 low

	/* PD7 CHGSTATUS to input */
	DDRD &= ~(1<<DDRD7); 		// set port D7 as input

	/* PE7 BATTALRT to input  */
	DDRE &= ~(1<<DDRE7); 		// set port D7 as input

	/* enable all power saving */
	/* Turn off all peripheral clocks that can be turned off.
	 * The debugWIRE system of some devices that shares system clock with the SPI module.
	 * Thus the PRSPI bit in the PRR register must not be set when debugging.
	 */
	/* keep
	 * uart 					PRUSART0
	 * xtimer clock 			PRTIM4
	 * transceiver, XTAL		PRTRX24
	 */
	// PRR0 |= (1<<PRTWI)|(1<<PRTIM2)|(1<<PRTIM0)|(1<<PRPGA)|(1<<PRTIM1)|(1<<PRSPI)|(1<<PRADC);
	// PRR1 |= (1<<PRTIM5)|(1<<PRTIM3)|(1<<PRUSART1);
	// PRR0 = 0xFD; // keep PRUSART0
	// PRR1 = 0x29; // keep PRTRX24 and PRTIM4
	// PRR2 = 0x0F; // keep SRAM

	/* disable unwanted Interrupts */
	/* External Interrupt INT0 is somehow triggered, stop that */
	// DDRD = 1<<DDRD0; // set port D0 as output
	// PORTD &= 1<<DDRD0;    // set D0 low


	/* initialize stdio via USART_0 */
	system_stdio_init();

    /* Disable power saving for TIMER4 which is used as xtimer, PRTIM4*/
	PRR1 &= ~(1<<PRTIM4);

    /* initialize the CPU */
    cpu_init();

    //TODO led init

    /* initialize the board LED (connected to pin PB7) */
    /* Ports Pins as Output */
    LED_PORT_DDR |= BLUE|RED|GREEN;
    /* All Pins High so LEDs are off */
    LED_PORT |= BLUE|RED|GREEN;

    irq_enable();

    // puts("Board init");
}

/**
 * @brief Initialize the System, initialize IO via UART_0
 */
void system_stdio_init(void)
{
    /* Disable power saving for UART0 PRUSART0*/
	PRR0 &= ~(1<<PRUSART0);

	/* initialize UART_0 for use as stdout */
    uart_stdio_init();

    stdout = &uart_stdout;
    stdin = &uart_stdin;

    /* Flush stdout */
    /* Make a very visible start printout */
    puts("\n_________________________________\n");
}

static int uart_putchar(char c, FILE *stream)
{
    (void) stream;
    uart_stdio_write(&c, 1);
    return 0;
}

int uart_getchar(FILE *stream)
{
    (void) stream;
    char c;
    uart_stdio_read(&c, 1);
    return (int)c;
}
