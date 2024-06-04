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
 * @brief       Low-level random number generator driver test
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "xtimer.h"
#include "periph/hwrng.h"

#define NUM_BYTES   (20U)

int main(void)
{
    uint8_t buf[NUM_BYTES];

    puts("\nHWRNG peripheral driver test\n");
    printf("This test will print from 1 to %u random bytes about every "
           "second\n\n", NUM_BYTES);

    while (1) {
        /* zero out buffer */
        memset(buf, 0, sizeof(buf));

        /* create random numbers */
        for (unsigned i = 1; i <= NUM_BYTES; i++) {
            printf("generating %u random byte(s)\n", i);
            hwrng_read(buf, i);

            printf("Got:");
            for (unsigned j = 0; j < i; j++) {
                printf(" 0x%02x", (unsigned char)buf[j]);
            }
            printf("\n");
        }

        xtimer_usleep(1000 * 1000);
    }

    return 0;
}
