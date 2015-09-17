/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Manual test application for UART peripheral drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "shell.h"
#include "periph/uart.h"

#define SHELL_BUFSIZE       (128U)
#define UART_BUFSIZE        (128U)

#ifndef STDIO
#define STDIO               (UART_UNDEF)
#endif

static int parse_dev(char *arg)
{
    int dev = atoi(arg);
    if (dev == STDIO) {
        printf("Error: The selected UART_DEV(%i) is used for the shell!\n", dev);
        return -2;
    }
    if (dev < UART_ACM_0 || dev >= UART_NUMOF) {
        printf("Error: Invalid UART_DEV device specified (%i).\n", dev);
        return -1;
    }
    return dev;
}

static void rx_cb(void *arg, char data)
{
    int dev = (int)arg;
    uart_write(UART_DEV(dev), (uint8_t*)&data, 1);
}

static int cmd_init_acm(int argc, char **argv)
{
    int dev, res;
    uint32_t baud;

    if (argc < 3) {
        printf("usage: %s <dev> <baudrate>\n", argv[0]);
        return 1;
    }
    /* parse parameters */
    dev = parse_dev(argv[1]);
    if (dev < 0) {
        return 1;
    }
    baud = (uint32_t)atoi(argv[2]);

    /* initialize UART */
    res = uart_init(UART_DEV(dev), baud, rx_cb, (void *)dev);
    if (res == -1) {
        printf("Error: Given baudrate (%u) not possible\n", (unsigned int)baud);
        return 1;
    }
    else if (res < -1) {
        puts("Error: Unable to initialize UART device\n");
        return 1;
    }
    printf("Successfully initialized UART_DEV(%i)\n", dev);
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init-acm", "Initialize a UART device with a given baudrate", cmd_init_acm },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("\nUSB device stack test application");
    puts("===================================");
    puts("This application is intended for testing USB interfaces.\n"
         );

    puts("UART INFO:");
    printf("Available devices:               %i\n", UART_NUMOF);
    printf("UART used for STDIO (the shell): UART_DEV(%i)\n\n", STDIO);

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
