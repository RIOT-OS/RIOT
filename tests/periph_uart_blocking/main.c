/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for low-level UART driver in blocking mode
 *
 * This test will test the functionality of all configured UART interface. While instructions are
 * given on the STDIO UART interface, the test will ask the user to iteratively connect to every
 * other UART interface and make some input there.
 *
 * The test makes only sense, if at least 2 UART devices are configured...
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "board.h"
#include "periph/uart.h"

/* only build this test, if the target supports the UART driver interface */
#if UART_NUMOF

static int baudrates[] = {115200, 57600, 9600, 38400, 115200, 115200};

void uart_print(uart_t dev, char *str)
{
    int i = 0;

    while (str[i] != '\0') {
        uart_write_blocking(dev, str[i++]);
    }
}

int main(void)
{
    char *hello = "Testing this UART device:\n";
    char *ask = "Please enter a few chars with newline in the end\n";
    char buf[128];
    int p, i;
    char tmp;
    uart_init_blocking(0, F_BAUD);
    while(1)
    {
        uart_write_blocking(0,'c');
    }
    puts("Setting up remaining UART devices:");
    for (int i = UART_0; i < UART_NUMOF; i++) {
        if (i != STDIO) {
            printf("Setting up UART_%i @ %i", i, baudrates[i]);
            if (uart_init_blocking(i, baudrates[i]) >= 0) {
                puts("   ...ok");
            }
            else {
                puts("   ...failed");
                return 1;
            }
        }
    }
    printf("\n");

    for (i = UART_0; i < UART_NUMOF; i++) {
        if (i != STDIO) {
            printf("Please Connect to UART_%i @ %i now, press return when done\n", i, baudrates[i]);
            do {
                tmp = getchar();
            } while (tmp != '\n');

            uart_print(i, hello);
            uart_print(i, ask);
            p = 0;
            memset(buf, 0, 128);
            printf("Input was: ");
            do {
                uart_read_blocking(i, &buf[p++]);
                printf("%c", buf[p - 1]);
            } while (buf[p - 1] != '\n');
            buf[p] = '\0';
            uart_print(i, buf);
            printf("\n");
        }
    }

    puts("If you were able to see your inputs on each UART device, the test was successful!");

    return 0;
}

#else

int main(void)
{
    puts("This board does not support the low-level UART driver interface.");

    return 0;
}

#endif /* UART_NUMOF */
