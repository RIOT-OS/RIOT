/*
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

#define ENABLE_DEBUG    (0)
#include "debug.h"

void trickle_callback(trickle_t *trickle)
{
    /* Handle k=0 like k=infinity (according to RFC6206, section 6.5) */
    if ((trickle->c < trickle->k) || (trickle->k == 0)) {
        (*trickle->callback.func)(trickle->callback.args);
    }
}

void trickle_interval(trickle_t *trickle)
{
    trickle->I = trickle->I * 2;
    DEBUG("TRICKLE new Interval %" PRIu32 "\n", trickle->I);

    if (trickle->I == 0) {
        DEBUG("%s:%d in %s: [WARNING] Interval was 0\n", RIOT_FILE_RELATIVE,
              __LINE__, DEBUG_FUNC);

        if (trickle->Imax == 0) {
            DEBUG("%s:%d in %s: [WARNING] Imax == 0\n", RIOT_FILE_RELATIVE,
                  __LINE__, DEBUG_FUNC);
        }

        trickle->I = (trickle->Imin << trickle->Imax);
    }

    if (trickle->I > (trickle->Imin << trickle->Imax)) {
        trickle->I = (trickle->Imin << trickle->Imax);
    }

    trickle->c = 0;
    trickle->t = (trickle->I / 2) + (rand() % ((trickle->I / 2) + 1));

    trickle->msg_callback_time = trickle->t * SEC_IN_MS;
    xtimer_set_msg64(&trickle->msg_callback_timer, trickle->msg_callback_time,
                     &trickle->msg_callback, trickle->pid);

    trickle->msg_interval_time = trickle->I * SEC_IN_MS;
    xtimer_set_msg64(&trickle->msg_interval_timer, trickle->msg_interval_time,
                     &trickle->msg_interval, trickle->pid);
}

void trickle_reset_timer(trickle_t *trickle)
{
    trickle_stop(trickle);
    trickle_start(trickle->pid, trickle, trickle->msg_interval.type, trickle->msg_callback.type,
                  trickle->Imin, trickle->Imax, trickle->k);
}

void trickle_start(kernel_pid_t pid, trickle_t *trickle, uint16_t interval_msg_type,
                   uint16_t callback_msg_type, uint32_t Imin, uint8_t Imax, uint8_t k)
{
    trickle->pid = pid;

    trickle->c = 0;
    trickle->k = k;
    trickle->Imin = Imin;
    trickle->Imax = Imax;
    trickle->I = trickle->Imin + (rand() % (4 * trickle->Imin));
    trickle->pid = pid;
    trickle->msg_interval.content.ptr = (char *)trickle;
    trickle->msg_interval.type = interval_msg_type;
    trickle->msg_callback.content.ptr = (char *)trickle;
    trickle->msg_callback.type = callback_msg_type;

    trickle_interval(trickle);
}

void trickle_stop(trickle_t *trickle)
{
    xtimer_remove(&trickle->msg_interval_timer);
    xtimer_remove(&trickle->msg_callback_timer);
}

void trickle_increment_counter(trickle_t *trickle)
{
    trickle->c++;
}
