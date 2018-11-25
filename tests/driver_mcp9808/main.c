/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the MCP9808 pressure and temperature sensor
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "mcp9808.h"
#include "mcp9808_params.h"
#include "xtimer.h"
#include "board.h"


static mcp9808_t dev;

int main(void)
{
    int result;

    puts("MCP9808 test application\n");

    result = mcp9808_init(&dev, &mcp9808_params[0]);
    if (result == -MCP9808_ERR_NODEV) {
        puts("[Error] The sensor did not answer correctly on the given address");
        return 1;
    }

    puts("Initialization successful\n");

    int16_t temperature = 0;
    while (1) {
        if (mcp9808_wakeup(&dev) != MCP9808_OK) {
            puts("[Error] The sensor cannot be wake up");
            return 1;
        }

        /* 250ms delay is required after device wake up */
        xtimer_usleep(250 * US_PER_MS);

        /* Read temperature in d°C */
        if (mcp9808_read_temperature(&dev, &temperature) != MCP9808_OK) {
            puts("[Error] Cannot read ambient temperature");
            return 1;
        }

        if (mcp9808_shutdown(&dev) != MCP9808_OK) {
            puts("[Error] The sensor cannot be shutdown");
            return 1;
        }

        printf("Temperature [d°C]: %i\n", (int)temperature);
        xtimer_sleep(2);
    }

    return 0;
}
