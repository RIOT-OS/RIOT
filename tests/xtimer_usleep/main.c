/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       xtimer_usleep test application
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "timex.h"

#define ONE_SEC_SLEEP     (1 * US_PER_SEC)
#define FIVE_SEC_SLEEP    (5 * US_PER_SEC)
#define TEN_SEC_SLEEP     (10 * US_PER_SEC)

int main(void)
{
    int t = 9;

    printf("This test will print \"Slept for X sec...\" every 1, 5 and 10 seconds.\n");
    printf("\n<======== If using pyterm, this is the time when started.\n\n");

    while (t) {
        xtimer_usleep(ONE_SEC_SLEEP);
        printf("Slept for 1 sec...\n");
        xtimer_usleep(FIVE_SEC_SLEEP);
        printf("Slept for 5 sec...\n");
        xtimer_usleep(TEN_SEC_SLEEP);
        printf("Slept for 10 sec...\n");
        t--;
    }
    printf("Test end.\n");

    return 0;
}
