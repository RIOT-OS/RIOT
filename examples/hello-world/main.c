/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#if MODULE_BASELIBC

#include "periph/uart.h"
#include "stdio_uart.h"
#include "isrpipe.h"

extern isrpipe_t stdio_uart_isrpipe;

size_t stdio_read2(FILE* dummy, char* buffer, size_t count)
{
    (void) dummy;

    return (ssize_t)isrpipe_read(&stdio_uart_isrpipe, buffer, count);
}

size_t stdio_write2(FILE* dummy, const char* buffer, size_t len)
{
    (void) dummy;
    uart_write(STDIO_UART_DEV, (const uint8_t *)buffer, len);

    return len;
}

const struct File_methods _uart_methods = {
    stdio_write2, stdio_read2
};

FILE _stdout_uart = {&_uart_methods};

FILE* const stdout = &_stdout_uart;

#endif

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    return 0;
}
