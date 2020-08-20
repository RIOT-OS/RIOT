/*
 * Copyright (C) 2020 Université Grenoble Alpes
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for the STTS751 I2C Temperature Sensor
 * @author      Pierre Millot
 * @file
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "shell.h"

#include "stts751.h"
#include "stts751_params.h"

stts751_t stts751;

int get_temp(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    double temp = stts751_get_temperature(&stts751);
    printf("Temperature: %d.%d C\n", (uint8_t)temp, (uint8_t)(temp * 10) % 10);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "temp", "Get the temperature (Celsius)", get_temp },
    { NULL, NULL, NULL }
};

int main(void)
{
    if (stts751_init(&stts751, &stts751_params[0]) != 0) {
        puts("Initialization failed!");
        return 1;
    }

    puts("STTS751 initialization done");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
