/*
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @brief Hwtimer test application
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "hwtimer.h"
#include "thread.h"

#define DELAY           (1000UL * 1000UL)           /* 1 second delay */
#define MSGLEN          (12U)                       /* == strlen("callback %2i") */

static char msg[MSGLEN * HWTIMER_MAXTIMERS];        /* == [callback  1\0callback  2\0...] */

void callback(void *ptr)
{
    puts((char *) ptr);
}

int main(void)
{
    char *msgn = msg;

    puts("\nhwtimer test application...\n");

    puts("This test will set all available channels of the hardware timer concurrently. Each");
    puts("channel will time out 1 second after the previous one.\n");
    puts("NOTE: this test assumes, that your hwtimer can cope with delays in the magnitude of");
    puts("      some seconds.\n");
    puts("On timeout, each channel will print \"callback xx\", you should see the following");
    puts("sequence:");
    puts("  callback  n");
    puts("  callback  1");
    puts("  callback  2");
    puts("  ...");
    puts("  callback  n-1");
    printf("where n equals %i on your platform\n\n\n", HWTIMER_MAXTIMERS);

    puts("Setting timers:");
    for (int i = 1; i < HWTIMER_MAXTIMERS; i++) {
        printf("  Timer %2i set\n", i);
        snprintf(msgn, MSGLEN, "callback %2x", i);
        hwtimer_set((i + 1) * DELAY, callback, (void*)msgn);
        msgn = msg + (i * MSGLEN);
    }

    printf("  Timer %2i set\n\n\n", HWTIMER_MAXTIMERS);
    hwtimer_wait(HWTIMER_TICKS(DELAY));
    printf("callback %2i\n", HWTIMER_MAXTIMERS);

    return 0;
}
