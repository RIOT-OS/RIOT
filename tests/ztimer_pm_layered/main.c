/*
 * Copyright (C) 2020 Juergen Fitschen <me@jue.yt>
 *               2020 SSV Software Systems GmbH
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
 * @brief       ztimer test application for interacting with pm_layered
 *
 * @author      2020 SSV Software Systems GmbH
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "board.h"
#include "ztimer.h"
#include "timex.h"
#include "pm_layered.h"

#define SLEEP_SEC  (5LU)
#define SLEEP_MSEC (SLEEP_SEC * MS_PER_SEC)
#define SLEEP_USEC (SLEEP_SEC * MS_PER_SEC * MS_PER_SEC)

static void dump_time(void)
{
    static uint32_t past_msec = 0;
    static uint32_t past_usec = 0;

    uint32_t now_msec = ztimer_now(ZTIMER_MSEC);
    uint32_t now_usec = ztimer_now(ZTIMER_USEC);

    printf("Elapsed time - ZTIMER_MSEC: %4" PRIu32 "ms, ZTIMER_USEC: %4" PRIu32 "ms\n",
            (now_msec - past_msec), (now_usec - past_usec) / US_PER_MS);

    past_msec = now_msec;
    past_usec = now_usec;
}

int main(void)
{
    printf("This test application lets the CPU sleep for %" PRIu32 " seconds using the clocks \n"
           "ZTIMER_MSEC and ZTIMER_USEC alternating.\n", SLEEP_SEC);

#if (!defined(CONFIG_ZTIMER_MSEC_REQUIRED_PM_MODE) || \
    CONFIG_ZTIMER_MSEC_REQUIRED_PM_MODE == ZTIMER_CLOCK_NO_REQUIRED_PM_MODE) && \
    (!defined(CONFIG_ZTIMER_USEC_REQUIRED_PM_MODE) || \
    CONFIG_ZTIMER_USEC_REQUIRED_PM_MODE == ZTIMER_CLOCK_NO_REQUIRED_PM_MODE)
    printf("WARNING: Neither ZTIMER_MSEC nor ZTIMER_USEC is interacting with pm_layered!\n"
           "Don't expect any power saving.\n");
#else
    printf("\nUnblocking all pm modes related to ztimer clocks ...\n");
#endif

#if defined(CONFIG_ZTIMER_MSEC_REQUIRED_PM_MODE) && \
    CONFIG_ZTIMER_MSEC_REQUIRED_PM_MODE != ZTIMER_CLOCK_NO_REQUIRED_PM_MODE
    while (pm_get_blocker().val_u8[CONFIG_ZTIMER_MSEC_REQUIRED_PM_MODE]) {
        printf("pm_unblock(CONFIG_ZTIMER_MSEC_REQUIRED_PM_MODE)\n");
        pm_unblock(CONFIG_ZTIMER_MSEC_REQUIRED_PM_MODE);
    }
#endif

#if defined(CONFIG_ZTIMER_USEC_REQUIRED_PM_MODE) && \
    CONFIG_ZTIMER_USEC_REQUIRED_PM_MODE != ZTIMER_CLOCK_NO_REQUIRED_PM_MODE
    /* If ztimer_msec is based on ztimer_usec, it already blocked
     * CONFIG_ZTIMER_USEC_REQUIRED_PM_MODE once during auto_init.
     * Don't remove that ... */
    uint8_t usec_pm_mode = (ZTIMER_MSEC_BASE == ZTIMER_USEC_BASE) ? 1 : 0;
    while (pm_get_blocker().val_u8[CONFIG_ZTIMER_USEC_REQUIRED_PM_MODE] > usec_pm_mode) {
        printf("pm_unblock(CONFIG_ZTIMER_USEC_REQUIRED_PM_MODE)\n");
        pm_unblock(CONFIG_ZTIMER_USEC_REQUIRED_PM_MODE);
    }
#endif

    printf("\nEntering the sleepy loop ...\n");

    while (1) {
        dump_time();
        printf("ztimer_sleep(ZTIMER_MSEC, %" PRIu32 ")\n", SLEEP_MSEC);
        ztimer_sleep(ZTIMER_MSEC, SLEEP_MSEC);
        dump_time();
        printf("ztimer_sleep(ZTIMER_USEC, %" PRIu32 ")\n", SLEEP_USEC);
        ztimer_sleep(ZTIMER_USEC, SLEEP_USEC);
    }

    return 0;
}
