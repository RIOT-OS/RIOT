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

kernel_pid_t trickle_pid = KERNEL_PID_UNDEF;
char trickle_buf[TRICKLE_STACKSIZE];

void reset_trickletimer(trickle_t *trickle)
{
    vtimer_remove(&trickle->wakeup_timer);
    start_trickle(trickle, trickle->Imin, trickle->Imax, trickle->k);
}

void start_trickle(trickle_t *trickle, uint32_t Imin, uint8_t Imax, uint8_t k)
{
    msg_t m_send;
    m_send.content.ptr = (char *) trickle;
    trickle->code = TRICKLE_CODE_INTERVAL;

    trickle->c = 0;
    trickle->k = k;
    trickle->Imin = Imin;
    trickle->Imax = Imax;
    trickle->I = trickle->Imin + (rand() % (4 * trickle->Imin));

    msg_send(&m_send, trickle_pid);
}

void stop_trickle(trickle_t *trickle)
{
    msg_t m_send;
    m_send.content.ptr = (char *) trickle;
    trickle->code = TRICKLE_CODE_STOP;
    msg_send(&m_send, trickle_pid);
}

void trickle_increment_counter(trickle_t *trickle)
{
    trickle->c++;
}

void trickle_set_interval(trickle_t *trickle, uint8_t code)
{
    trickle->code = code;
    vtimer_set_msg(&trickle->wakeup_timer, trickle->wakeup_time, trickle_pid, trickle);
}

void trickle_callback(trickle_t *trickle)
{
    /* Handle k=0 like k=infinity (according to RFC6206, section 6.5) */
    if ((trickle->c < trickle->k) || (trickle->k == 0)) {
        (*trickle->callback.func)(trickle->callback.args);
    }

    trickle->wakeup_time = timex_set(0, trickle->I * 1000);
    trickle_set_interval(trickle, TRICKLE_CODE_INTERVAL);
}

void trickle_interval(trickle_t *trickle)
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

    trickle->wakeup_time = timex_set(0, trickle->t * 1000);
    trickle_set_interval(trickle, TRICKLE_CODE_CALLBACK);
}

static void *trickle_thread(void *arg)
{
    (void) arg;

    msg_t m_recv;
    msg_init_queue(rpl_msg_queue, TRICKLE_PKT_RECV_BUF_SIZE);

    while (1) {
        msg_receive(&m_recv);
        trickle_t *trickle = (trickle_t *) m_recv.content.ptr;

        switch (trickle->code) {
            case TRICKLE_CODE_INTERVAL:
                trickle_interval(trickle);
                break;
            case TRICKLE_CODE_CALLBACK:
                trickle_callback(trickle);
                break;
            case TRICKLE_CODE_STOP:
                vtimer_remove(&trickle->wakeup_timer);
                break;
            default:
                break;
        }
    }

    return NULL;
}

int trickle_init(void)
{
    trickle_pid = thread_create(trickle_buf, TRICKLE_STACKSIZE, PRIORITY_MAIN - 1, CREATE_STACKTEST, trickle_thread, NULL, "trickle_thread");
    return 0;
}
