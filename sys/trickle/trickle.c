/*
 * Trickle implementation
 *
 * Copyright (C) 2013, 2014  INRIA.
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */

#include "inttypes.h"
#include "random.h"
#include "trickle.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

void trickle_callback(trickle_t *trickle)
{
    /* Handle k=0 like k=infinity (according to RFC6206, section 6.5) */
    if ((trickle->c < trickle->k) || (trickle->k == 0)) {
        (*trickle->callback.func)(trickle->callback.args);
    }

    trickle_interval(trickle);
}

void trickle_interval(trickle_t *trickle)
{
    assert(trickle->I > 0);

    uint32_t old_interval = trickle->I;
    uint32_t max_interval = trickle->Imin << trickle->Imax;
    uint32_t diff = old_interval - trickle->t;

    trickle->I *= 2;
    if (trickle->I > max_interval) {
        trickle->I = max_interval;
        old_interval = max_interval / 2;
    }

    DEBUG("trickle: I == %" PRIu32 ", diff == %" PRIu32 "\n", trickle->I, diff);

    trickle->c = 0;
    /* old_interval == trickle->I / 2 */
    trickle->t = random_uint32_range(old_interval, trickle->I);

    uint64_t msg_time = (trickle->t + diff) * US_PER_MS;
    xtimer_set_msg64(&trickle->msg_timer, msg_time, &trickle->msg,
                     trickle->pid);
}

void trickle_reset_timer(trickle_t *trickle)
{
    assert(trickle->I > trickle->Imin);

    trickle_stop(trickle);
    trickle->I = trickle->t = trickle->Imin;
    trickle_interval(trickle);
}

void trickle_start(kernel_pid_t pid, trickle_t *trickle, uint16_t msg_type,
                   uint32_t Imin, uint8_t Imax, uint8_t k)
{
    assert(Imin > 0);
    assert((Imin << Imax) < (UINT32_MAX / 2));

    trickle->pid = pid;

    trickle->c = 0;
    trickle->k = k;
    trickle->Imin = Imin;
    trickle->Imax = Imax;
    trickle->I = trickle->t = random_uint32_range(trickle->Imin,
                                                  4 * trickle->Imin);
    trickle->pid = pid;
    trickle->msg.content.ptr = trickle;
    trickle->msg.type = msg_type;

    trickle_interval(trickle);
}

void trickle_stop(trickle_t *trickle)
{
    xtimer_remove(&trickle->msg_timer);
}

void trickle_increment_counter(trickle_t *trickle)
{
    trickle->c++;
}
