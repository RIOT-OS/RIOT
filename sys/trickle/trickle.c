/**
 * Trickle implementation
 *
 * Copyright (C) 2013, 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Cenk Gündoğan <cnkgndgn@gmail.com>
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "inttypes.h"
#include "trickle.h"
#include "rpl.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static void *trickle_interval_over(void *arg);

void reset_trickletimer(trickle_t *trickle)
{
    trickle->I = trickle->Imin + (rand() % (4 * trickle->Imin)) ;
    trickle->c = 0;
    trickle->reset = 1;
    thread_wakeup(trickle->thread_pid);
}

void start_trickle(trickle_t *trickle, uint8_t DIOIntMin, uint8_t DIOIntDoubl,
                   uint8_t DIORedundancyConstant)
{
    trickle->c = 0;
    trickle->k = DIORedundancyConstant;
    trickle->Imin = (1 << DIOIntMin);
    trickle->Imax = DIOIntDoubl;
    trickle->I = trickle->Imin + (rand() % (4 * trickle->Imin)) ;

    trickle->thread_pid = thread_create(trickle->thread_buf, TRICKLE_INTERVAL_STACKSIZE,
                                      PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                      trickle_interval_over, (void *) trickle, "trickle");
}

void stop_trickle(trickle_t *trickle) {
    trickle->done = 1;
    thread_wakeup(trickle->thread_pid);
}

void trickle_increment_counter(trickle_t *trickle)
{
    trickle->c++;
}

static void *trickle_interval_over(void *arg)
{
    trickle_t *trickle = (trickle_t *) arg;

    while (!trickle->done) {
        trickle->I = trickle->I * 2;
        DEBUG("TRICKLE new Interval %" PRIu32 "\n", trickle->I);

        if (trickle->I == 0) {
            DEBUGF("[WARNING] Interval was 0\n");

            if (trickle->Imax == 0) {
                DEBUGF("[WARNING] Imax == 0\n");
            }

            trickle->I = (trickle->Imin << trickle->Imax);
        }

        if (trickle->I > (trickle->Imin << trickle->Imax)) {
            trickle->I = (trickle->Imin << trickle->Imax);
        }

        trickle->c = 0;
        trickle->t = (trickle->I / 2) + (rand() % ((trickle->I / 2) + 1));

        trickle->wakeup_time = timex_set(0, trickle->t * 1000);
        vtimer_set_wakeup(&trickle->wakeup_timer, trickle->wakeup_time, thread_getpid());
        thread_sleep();
        vtimer_remove(&trickle->wakeup_timer);
        /* Handle k=0 like k=infinity (according to RFC6206, section 6.5) */
        if ((trickle->c < trickle->k) || (trickle->k == 0)) {
            (*trickle->callback.func)(trickle->callback.args);
        }

        if (trickle->reset || trickle->done) {
            trickle->reset = 0;
            continue;
        }

        trickle->wakeup_time = timex_set(0, trickle->I * 1000);
        vtimer_set_wakeup(&trickle->wakeup_timer, trickle->wakeup_time, thread_getpid());
        thread_sleep();
        vtimer_remove(&trickle->wakeup_timer);
    }

    vtimer_remove(&trickle->wakeup_timer);
    vtimer_remove(&trickle->wakeup_timer);

    return NULL;
}
