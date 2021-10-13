/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *               2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   Implements common avr8 libc stdio initialization
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */
#include <stdio.h>
#include <avr/io.h>

#include "stdio_uart.h"

static int _uart_putchar(char c, FILE *stream);
static int _uart_getchar(FILE *stream);
static FILE _uart_stdout = FDEV_SETUP_STREAM(_uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE _uart_stdin = FDEV_SETUP_STREAM(NULL, _uart_getchar, _FDEV_SETUP_READ);

static int _uart_putchar(char c, FILE *stream)
{
    (void) stream;
    stdio_write(&c, 1);
    return 0;
}

static int _uart_getchar(FILE *stream)
{
    (void) stream;
    char c;
    stdio_read(&c, 1);
    return (int)c;
}

void avr8_stdio_init(void)
{
    stdio_init();

    stdout = &_uart_stdout;
    stdin = &_uart_stdin;

    /* Flush stdout */
    puts("\f");
}
