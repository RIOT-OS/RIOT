/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       example application for setting a periodic wakeup
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include "ztimer.h"
#include "timex.h"

/* set interval to 1 second */
#define INTERVAL_MS (1U * MS_PER_SEC)

int main(void)
{
    uint32_t last_wakeup = ztimer_now(ZTIMER_MSEC);

    while (1) {
        ztimer_periodic_wakeup(ZTIMER_MSEC, &last_wakeup, INTERVAL_MS);
        printf("slept until %" PRIu32 "\n", ztimer_now(ZTIMER_MSEC));
    }

    return 0;
}
