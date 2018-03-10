/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Laurent Navet <laurent.navet@gmail.com>
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_blu-wmn
 * @{
 *
 * @file
 * @brief       Board specific implementation for the BLU-WMN001
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Matthew Blue <matthew.blu.neuro@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <avr/io.h>

#include "board.h"
#include "cpu.h"
#include "uart_stdio.h"
#include "periph/gpio.h"

void SystemInit(void);
static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE uart_stdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void board_init(void)
{
    /* initialize stdio via USART_1 */
    SystemInit();

    /* initialize the CPU */
    cpu_init();

    /* Disable JTAG to be able to use PCINTs on PC2-PC5 */
    MCUCR=(1<<JTD);

    irq_enable();
}

/**
 * @brief Initialize the System, initialize IO via UART_1
 */
void SystemInit(void)
{
    /* initialize UART_1 for use as stdout */
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
