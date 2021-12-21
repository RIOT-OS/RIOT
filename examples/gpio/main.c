/*
 * Copyright (C) 2021 Geovane Fedrecheski
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
 * @brief       GPIO toggle application
 *
 * @author      Geovane Fedrecheski <geonnave@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "periph/gpio.h"

static int _toggle_handler(int argc, char **argv)
{
    /* These parameters are not used, avoid a warning during build */
    (void)argc;
    (void)argv;

    uint8_t pin_number;

    if (argc == 2) {
        pin_number = atoi(argv[1]);
        printf("dbg> pin_number: %d\n", pin_number);
        gpio_t pin = GPIO_PIN(1, pin_number);
        gpio_init(pin, GPIO_OUT);
        gpio_toggle(pin);
        return 0;
    }
    else {
        printf("usage: %s <pin_number>\n", argv[0]);
        return 1;
    }
}

static const shell_command_t shell_commands[] = {
    { "toggle", "Toggle gpio value at specified pin", _toggle_handler },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Example GPIO application");
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
