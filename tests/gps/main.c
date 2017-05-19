/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     test
 * @{
 *
 * @file
 * @brief       RIOT GPS test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */


#include <stdio.h>

#include "gps.h"
#include "gps/serial.h"
#include "periph/uart.h"

static gps_serial_t gps;
static const gps_serial_params_t gps_serial_params = { .uart=UART_DEV(1), .baudrate=9600 };

int main(void)
{
    gps_serial_init(&gps, &gps_serial_params);

    char lat[16];
    char lon[16];

    while(1) {
        int res = gps_get_loc((gps_t*)&gps, lat, lon, 2000000LLU);
        if (!res) {
            printf("lat=%s lon=%s\n", lat, lon);
        }
        else {
            printf("res=%i\n", res);
        }
    }

    return 0;
}
