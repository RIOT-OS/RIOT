/*
 * SPDX-FileCopyrightText: 2018 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2021 Gerson Fernando Budke <nandojve@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "kernel_init.h"
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
    early_init();

    stdout = &_uart_stdout;
    stdin = &_uart_stdin;

    /* Flush stdout */
    puts("\f");
}
