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

void trickle_callback(trickle_t *trickle)
{
    /* Handle k=0 like k=infinity (according to RFC6206, section 6.5) */
    if ((trickle->c < trickle->k) || (trickle->k == 0)) {
        (*trickle->callback.func)(trickle->callback.args);
    }
}

void trickle_interval(trickle_t *trickle, void *msg_interval, timex_t *msg_interval_time, vtimer_t *msg_interval_timer,
        void *msg_callback, timex_t *msg_callback_time, vtimer_t *msg_callback_timer)
{
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

    vtimer_remove(msg_callback_timer);
    (*msg_callback_time) = timex_set(0, trickle->t * 1000);
    vtimer_set_msg(msg_callback_timer, (*msg_callback_time), trickle->pid, msg_callback);

    vtimer_remove(msg_interval_timer);
    (*msg_interval_time) = timex_set(0, trickle->I * 1000);
    vtimer_set_msg(msg_interval_timer, (*msg_interval_time), trickle->pid, msg_interval);
}

void reset_trickletimer(trickle_t *trickle, void *msg_interval, timex_t *msg_interval_time, vtimer_t *msg_interval_timer,
        void *msg_callback, timex_t *msg_callback_time, vtimer_t *msg_callback_timer)
{
    vtimer_remove(msg_interval_timer);
    vtimer_remove(msg_callback_timer);
    start_trickle(trickle->pid, trickle, msg_interval, msg_interval_time, msg_interval_timer,
            msg_callback, msg_callback_time, msg_callback_timer, trickle->Imin, trickle->Imax, trickle->k);
}

void start_trickle(kernel_pid_t pid, trickle_t *trickle, void *msg_interval, timex_t *msg_interval_time, vtimer_t *msg_interval_timer,
        void *msg_callback, timex_t *msg_callback_time, vtimer_t *msg_callback_timer, uint32_t Imin, uint8_t Imax, uint8_t k)
{
    trickle->pid = pid;

    trickle->c = 0;
    trickle->k = k;
    trickle->Imin = Imin;
    trickle->Imax = Imax;
    trickle->I = trickle->Imin + (rand() % (4 * trickle->Imin));

    trickle_interval(trickle, msg_interval, msg_interval_time, msg_interval_timer, msg_callback, msg_callback_time, msg_callback_timer);
}

void stop_trickle(vtimer_t *msg_interval_timer, vtimer_t *msg_callback_timer)
{
    vtimer_remove(msg_interval_timer);
    vtimer_remove(msg_callback_timer);
}

void trickle_increment_counter(trickle_t *trickle)
{
    trickle->c++;
}
