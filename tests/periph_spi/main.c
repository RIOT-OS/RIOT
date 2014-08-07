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
 * @brief       Low-level SPI driver test application
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "periph/spi.h"
#include "periph/gpio.h"

#define SHELL_BUFSIZE       (128U)
#define SHELL_BUF           (128U)

#define DEV                 SPI_0
#define MODE                SPI_CONF_FIRST_RISING
#define CS                  GPIO_3

enum {
    READ = 0,
    WRITE,
    INIT
} rw;

static volatile int state;
static char* mem = "Hello Master! abcdefghijklmnopqrstuvwxyz 0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static const shell_command_t shell_commands[] = {
    { NULL, NULL, NULL }
};

void on_cs(void)
{
    spi_transmission_begin(DEV, '3');
    state = 0;
    rw = INIT;
}

char on_data(char data)
{
    puts ("char");
    switch (rw) {
        case READ:
            return mem[state++];
        case WRITE:
            mem[state++] = data;
            return 'o';
        case INIT:
            if (data == ' ') {
                rw = READ;
                return mem[state++];
            } else if (data & 0x80) {
                rw = WRITE;
                state = (data & 0x7f);
                return 'W';
            } else {
                rw = READ;
                state = data;
                return mem[state++];
            }
    }
    return 'e';
}

int shell_getchar(void)
{
    return (int)getchar();
}

void shell_putchar(int c)
{
    putchar((char)c);
}

int main(void)
{
    int res;
    shell_t shell;

    puts("\nRIOT SPI Slave test");
    puts("desc...\n");

    /* initialize the CS line */
    printf("Initializing the CS line");
    res = gpio_init_int(CS, GPIO_PULLUP, GPIO_FALLING, on_cs);
    if (res >= 0) {
        puts("      ...[ok]");
    }
    else {
        puts("      ...[failed]");
        return -1;
    }

    /* initialize the SPI master */
    printf("Initializing the SPI device");
    res = spi_init_slave(DEV, MODE, on_data);
    if (res >= 0) {
        puts("   ...[ok]");
    }
    else {
        puts("   ...[failed]");
    }

    /* run the shell */
    puts("Starting the shell            ...[ok]\n");         /* we trust it... */
    shell_init(&shell, shell_commands, SHELL_BUFSIZE, shell_getchar, shell_putchar);
    shell_run(&shell);

    return 0;
}
