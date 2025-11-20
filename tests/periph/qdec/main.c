/*
 * SPDX-FileCopyrightText: 2017 Gilles DOFFE <gdoffe@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for low-level QDEC drivers
 *
 * This test initializes all declared QDEC devices.
 * It displays QDEC counters value each second.
 *
 * @author      Gilles DOFFE <gdoffe@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "periph/qdec.h"
#include "xtimer.h"

static void handler(void *arg)
{
    qdec_t qdec = (qdec_t)(uintptr_t)arg;
    printf("QDEC %u counter overflow : reset counter\n", qdec);
    qdec_read_and_reset(QDEC_DEV(qdec));
}

int main(void)
{
    unsigned i;
    int32_t value;
    puts("Welcome into Quadrature Decoder (QDEC) test program.");
    puts("This program will count pulses on all available QDEC channels");
    puts("Written for nucleo-f401re, you have to plug signals A and B as follow :");
    puts("  QDEC0 : signal A on PA6 and signal B on PA7");
    puts("  QDEC1 : signal A on PB6 and signal B on PB7");
    puts("Quadrature decoding mode is set to X4 : counting on all edges on both signals");

    for (i = 0; i < QDEC_NUMOF; i++) {
        int32_t error = qdec_init(QDEC_DEV(i), QDEC_X4, handler, (void *)(uintptr_t)i);
        if (error) {
            fprintf(stderr,"Not supported mode !\n");
            return error;
        }
    }

    while (1) {
        for (i = 0; i < QDEC_NUMOF; i++) {
            value = qdec_read_and_reset(QDEC_DEV(i));
            printf("QDEC %lu = %ld\n", (unsigned long int)i, (long int)value);
        }
        xtimer_sleep(1);
    }

    return 0;
}
