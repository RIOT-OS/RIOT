/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @brief       Manual test application for GPIO peripheral drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "shell.h"
#include "periph/gpio.h"

static void cb(void *arg)
{
    printf("INT: external interrupt from pin %i\n", (int)arg);
}

static int parse_pull(char *val)
{
    switch (atoi(val)) {
        case 0:
            return GPIO_NOPULL;
        case 1:
            return GPIO_PULLUP;
        case 2:
            return GPIO_PULLDOWN;
        default:
            puts("Error: invalid pull configuration");
            return -1;
    }
}

static int init_out(int argc, char **argv)
{
    int port, pin, pull;

    if (argc < 3) {
        printf("usage: %s <port> <pin> [pull_config]\n", argv[0]);
        puts("      pull_config: 0: no pull resistor (default)\n"
             "                   1: pull up\n"
             "                   2: pull down");
        return 1;
    }
    port = atoi(argv[1]);
    pin = atoi(argv[2]);
    if (argc >= 4) {
        pull = parse_pull(argv[3]);
        if (pull < 0) {
            return 1;
        }
    }
    else {
        pull = GPIO_NOPULL;
    }
    if (gpio_init(GPIO_PIN(port, pin), GPIO_DIR_OUT, pull) < 0) {
        printf("Error while initializing  PORT_%i.%i as output\n", port, pin);
        return 1;
    }
    printf("PORT_%i.%i initialized successful as output\n", port, pin);

    return 0;
}

static int init_in(int argc, char **argv)
{
    int port, pin, pull;

    if (argc < 3) {
        printf("usage: %s <port> <pin> [pull_config]\n", argv[0]);
        puts("      pull_config: 0: no pull resistor (default)\n"
             "                   1: pull up\n"
             "                   2: pull down");
        return 1;
    }
    port = atoi(argv[1]);
    pin = atoi(argv[2]);
    if (argc >= 4) {
        pull = parse_pull(argv[3]);
        if (pull < 0) {
            return 1;
        }
    }
    else {
        pull = GPIO_NOPULL;
    }
    if (gpio_init(GPIO_PIN(port, pin), GPIO_DIR_IN, pull) < 0) {
        printf("Error while initializing  PORT_%i.%02i as input\n", port, pin);
        return 1;
    }
    printf("PORT_%i.%02i initialized successful as input\n", port, pin);

    return 0;
}

static int init_int(int argc, char **argv)
{
    int port, pin, flank, pull;

    if (argc < 4) {
        printf("usage: %s <port> <pin> <flank> [pull_config]\n", argv[0]);
        puts("      flank:       0: falling\n"
             "                   1: rising\n"
             "                   2: both\n"
             "      pull_config: 0: no pull resistor (default)\n"
             "                   1: pull up\n"
             "                   2: pull down");
        return 1;
    }
    port = atoi(argv[1]);
    pin = atoi(argv[2]);
    flank = atoi(argv[3]);
    switch (flank) {
        case 0:
            flank = GPIO_FALLING;
            break;
        case 1:
            flank = GPIO_RISING;
            break;
        case 2:
            flank = GPIO_BOTH;
            break;
        default:
            printf("wrong flank setting.\n");
            return 0;
    }
    if (argc >= 5) {
        pull = parse_pull(argv[4]);
        if (pull < 0) {
            return 1;
        }
    }
    else {
        pull = GPIO_NOPULL;
    }
    if (gpio_init_int(GPIO_PIN(port, pin), pull, flank, cb, (void *)pin) < 0) {
        printf("Error while initializing  PORT_%i.%02i as external interrupt\n",
               port, pin);
        return 1;
    }
    printf("PORT_%i.%02i initialized successful as external interrupt\n",
           port, pin);

    return 0;
}

static int read(int argc, char **argv)
{
    int port, pin;

    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }
    port = atoi(argv[1]);
    pin = atoi(argv[2]);
    if (gpio_read(GPIO_PIN(port, pin))) {
        printf("PORT_%i.%02i is HIGH\n", port, pin);
    }
    else {
        printf("PORT_%i.%02i is LOW\n", port, pin);
    }
    return 0;
}

static int set(int argc, char **argv)
{
    int port, pin;

    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }
    port = atoi(argv[1]);
    pin = atoi(argv[2]);

    gpio_set(GPIO_PIN(port, pin));
    return 0;
}

static int clear(int argc, char **argv)
{
    int port, pin;

    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }
    port = atoi(argv[1]);
    pin = atoi(argv[2]);
    gpio_clear(GPIO_PIN(port, pin));
    return 0;
}

static int toggle(int argc, char **argv)
{
    int port, pin;

    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }
    port = atoi(argv[1]);
    pin = atoi(argv[2]);
    gpio_toggle(GPIO_PIN(port, pin));
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init_in", "initialize pin as output", init_in },
    { "init_out", "initialize pin as input", init_out },
    { "init_int", "initialize pin as EXTI", init_int },
    { "read", "read pin status", read },
    { "set", "set pin to HIGH", set },
    { "clear", "set pin to LOW", clear },
    { "toggle", "toggle pin", toggle },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("GPIO peripheral driver test\n");
    puts("In this test, pins are specified by integer port and pin numbers.\n"
         "So if your platform has a pin PA01, it will be port=0 and pin=1,\n"
         "PC14 would be port=2 and pin=14 etc.\n\n"
         "NOTE: make sure the values you use exist on your platform! The\n"
         "      behavior for not existing ports/pins is not defined!");

    /* start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
