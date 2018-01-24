/*
 * Copyright (C) 2016 RWTH Aachen, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_jiminy-mega256rfr2
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Jiminy Mega 256rfr2 board
 *              developed by the IAS of the RWTH Aachen University
 *
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
    /* initialize stdio via USART_0 */
    SystemInit();

    /* initialize the CPU */
    cpu_init();

    /* initialize the board LED (connected to pin PB7) */
    /* Ports Pins as Output */
    LED_PORT_DDR |= BLUE | RED | GREEN;
    /* All Pins Low so LEDs are off */
    LED_PORT &= ~(BLUE | RED | GREEN);

    irq_enable();
}

/*Initialize the System, initialize IO via UART_0*/
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
