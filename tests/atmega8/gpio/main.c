/*
 * Copyright (C) 2014,2017 Freie Universität Berlin
 *               2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for GPIO peripheral drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "shell.h"
#include "periph/gpio.h"

#define SHELL_BUFSIZE   (16)

static int init_pin(int argc, char **argv, gpio_mode_t mode)
{
    int po, pi;

    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    po = atoi(argv[1]);
    pi = atoi(argv[2]);

    if (gpio_init(GPIO_PIN(po, pi), mode) < 0) {
        printf("Error to initialize GPIO_PIN(%i, %02i)\n", po, pi);
        return 1;
    }

    return 0;
}

static int init_out(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_OUT);
}

static int init_in(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_IN);
}

static int init_in_pu(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_IN_PU);
}

static int init_in_pd(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_IN_PD);
}

static int init_od(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_OD);
}

static int init_od_pu(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_OD_PU);
}

static int cmd_read(int argc, char **argv)
{
    int port, pin;

    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    port = atoi(argv[1]);
    pin = atoi(argv[2]);

    if (gpio_read(GPIO_PIN(port, pin))) {
        printf("GPIO_PIN(%i.%02i) is HIGH\n", port, pin);
    }
    else {
        printf("GPIO_PIN(%i.%02i) is LOW\n", port, pin);
    }

    return 0;
}

static int cmd_set(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    gpio_set(GPIO_PIN(atoi(argv[1]), atoi(argv[2])));

    return 0;
}

static int cmd_clear(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    gpio_clear(GPIO_PIN(atoi(argv[1]), atoi(argv[2])));

    return 0;
}

static int cmd_toggle(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    gpio_toggle(GPIO_PIN(atoi(argv[1]), atoi(argv[2])));

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init_out", "init as output (push-pull mode)", init_out },
    { "init_in", "init as input w/o pull resistor", init_in },
    { "init_in_pu", "init as input with pull-up", init_in_pu },
    { "init_in_pd", "init as input with pull-down", init_in_pd },
    { "init_od", "init as output (open-drain without pull resistor)", init_od },
    { "init_od_pu", "init as output (open-drain with pull-up)", init_od_pu },
    { "read", "read pin status", cmd_read },
    { "set", "set pin to HIGH", cmd_set },
    { "clear", "set pin to LOW", cmd_clear },
    { "toggle", "toggle pin", cmd_toggle },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("GPIO peripheral driver test\n");

    /* start the shell */
    char line_buf[SHELL_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_BUFSIZE);

    return 0;
}
