/*
 * Copyright (C) 2016 Kees Bakker, SODAQ
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
 * @brief       Test application for the SHT2x temperature and humidity sensor
 *
 * @author      Kees Bakker <kees@sodaq.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "sht2x_params.h"
#include "sht2x.h"
#include "ztimer.h"

#define SLEEP_2S   (2)  /* 2 seconds delay between printf */

static void dump_buffer(const char* txt, uint8_t* buffer, size_t len);

int main(void)
{
    sht2x_t dev;
    uint8_t ident[8];
    int result;
    int ident_size;

    memset(ident, 0, sizeof(ident));

    puts("SHT2X test application\n");

    printf("+------------Initializing------------+\n");
    /* Use the first default parameter set */
    result = sht2x_init(&dev, &sht2x_params[0]);
    if (result != SHT2X_OK) {
        puts("[Error] The given i2c is not enabled");
        return 1;
    }
    else {
        printf("Initialization successful\n\n");
    }

    printf("\n+--------Identifier--------+\n");
    ident_size = sht2x_read_ident(&dev, ident, sizeof(ident));
    if (ident_size > 0) {
        dump_buffer("Identifier Code:", ident, ident_size);
    }

    printf("\n+--------Starting Measurements--------+\n");
    while (1) {
        int16_t temperature;

        {
            /* Print the identification every so often */
            static int count = 0;
            if (++count >= 100) {
                ident_size = sht2x_read_ident(&dev, ident, sizeof(ident));
                if (ident_size > 0) {
                    dump_buffer("Identifier Code:", ident, ident_size);
                }
                count = 0;
            }
        }

        /* Get temperature in centi degrees celsius */
        temperature = sht2x_read_temperature(&dev);
        if (temperature == INT16_MIN) {
            printf("\n+--------Soft Reset--------+\n");
            sht2x_reset(&dev);
        } else {
            uint16_t humidity;

            /* Get humidity in %RH */
            humidity = sht2x_read_humidity(&dev);

            printf("Temperature [°C]: %d.%d\n"
                   "Humidity [%%RH]: %u.%02u\n"
                   "\n+-------------------------------------+\n",
                   temperature / 100, abs(temperature % 100) / 10,
                   (unsigned int)(humidity / 100), (unsigned int)(humidity % 100)
                   );
        }

        ztimer_sleep(ZTIMER_SEC, SLEEP_2S);
    }

    return 0;
}

static void dump_buffer(const char* txt, uint8_t* buffer, size_t len)
{
    size_t ix;
    printf("%s\n", txt);
    for (ix = 0; ix < len; ++ix) {
        if (ix > 0) {
            if ((ix % 8) == 0) {
                printf("\n");
            }
            else {
                printf(" ");
            }
        }
        printf("%02x", buffer[ix]);
    }
    if (len > 0) {
        printf("\n");
    }
}
