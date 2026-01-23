/*
 * SPDX-FileCopyrightText: 2020 SSV Software Systems GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief    ztimer test application
 *
 * @author   Juergen Fitschen <jfi@ssv-embedded.de>
 *
 * @}
 */

#include <stdio.h>

#include "ztimer.h"

#ifndef TEST_ZTIMER_CLOCK
#error Please define TEST_ZTIMER_CLOCK
#endif

#define LOOPS (1000U)

#define xstr(s) str(s)
#define str(s) #s

int main(void)
{
    printf("Looping over ztimer clock %s\n", xstr(TEST_ZTIMER_CLOCK));

    for (unsigned i = 0; i < LOOPS; i++) {
        printf("Loop %u\n", i);
        ztimer_sleep(TEST_ZTIMER_CLOCK, 0);
    }

    printf("SUCCESS!\n");

    return 0;
}
