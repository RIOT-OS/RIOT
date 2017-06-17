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

void SystemInit(void);
static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE uart_stdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void board_init(void)
{
	OSCCAL = 0xa3;
    /* initialize stdio via USART_0 */
    SystemInit();

    /* initialize the CPU */
    cpu_init();

    //TODO led init

    /* initialize the board LED (connected to pin PB7) */
    /* Ports Pins as Output */
    LED_PORT_DDR |= BLUE|RED|GREEN;
    /* All Pins High so LEDs are off */
    LED_PORT |= BLUE|RED|GREEN;

    irq_enable();
}

/**
 * @brief Initialize the System, initialize IO via UART_0
 */
void SystemInit(void)
{
    /* initialize UART_0 for use as stdout */
    uart_stdio_init();

    stdout = &uart_stdout;
    stdin = &uart_stdin;

    /* Flush stdout */
    puts("\f");
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
