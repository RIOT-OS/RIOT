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
#include <vtimer.h>
#include <periph/random.h>
#include <periph_conf.h>

#define LIMIT       (20U)

int main(void)
{
    char buf[LIMIT];
    int count = 0;

    puts("\nRandom number generator low-level driver test\n");
    printf("This test will print from 1 to %i random bytes about every second\n\n", LIMIT);

    puts("Initializing Random Number Generator driver.\n");
    random_init();

    while (1) {
        /* zero out buffer */
        memset(buf, 0, sizeof(buf));

        /* create random numbers */
        for (int i = 1; i <= LIMIT; i++) {
            printf("generating %i random byte(s)\n", i);
            count = random_read(buf, i);

            if (count != i) {
                printf("Error generating random bytes, got %i instead of %i", count, i);
                return 0;
            }

            printf("Got:");
            for (int j = 0; j < i; j++) {
                printf(" 0x%02x", buf[j]);
            }
            printf("\n");
        }

        vtimer_usleep(1000 * 1000);
    }

    return 0;
}
