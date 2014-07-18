/*
 * @f ccnl-platform.c
 * @b routines for uniform time handling
 *
 * Copyright (C) 2011-13, Christian Tschudin, University of Basel
 * Copyright (C) 2013, Christian Mehlis, Freie Universität Berlin
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * File history:
 * 2012-11 created
 * 2013-03-18 updated (ms): removed omnet related code, and moved struct
 *   ccnl_timer_s to the '#ifndef CCNL_LINUXKERNEL' section
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ccnl-core.h"
#include "ccnl-platform.h"

#include "vtimer.h"

long
timevaldelta(struct timeval *a, struct timeval *b)
{
    return 1000000 * (a->tv_sec - b->tv_sec) + a->tv_usec - b->tv_usec;
}

// ----------------------------------------------------------------------

struct ccnl_timer_s *eventqueue;

void
ccnl_get_timeval(struct timeval *tv)
{
    timex_t now;

    vtimer_now(&now);

    tv->tv_sec = now.seconds;
    tv->tv_usec = now.microseconds;
}

void *
ccnl_set_timer(int usec, void (*fct)(void *aux1, void *aux2),
               void *aux1, void *aux2)
{
    struct ccnl_timer_s *t, **pp;
    static int handlercnt;

    t = (struct ccnl_timer_s *) ccnl_calloc(1, sizeof(*t));

    if (!t) {
        return 0;
    }

    t->fct2 = fct;
    ccnl_get_timeval(&t->timeout);
    usec += t->timeout.tv_usec;
    t->timeout.tv_sec += usec / 1000000;
    t->timeout.tv_usec = usec % 1000000;
    t->aux1 = aux1;
    t->aux2 = aux2;

    for (pp = &eventqueue; ; pp = &((*pp)->next)) {
        if (!*pp || (*pp)->timeout.tv_sec > t->timeout.tv_sec ||
            ((*pp)->timeout.tv_sec == t->timeout.tv_sec &&
             (*pp)->timeout.tv_usec > t->timeout.tv_usec)) {
            t->next = *pp;
            t->handler = handlercnt++;
            *pp = t;
            return t;
        }
    }

    return NULL; // ?
}

void *
ccnl_set_absolute_timer(struct timeval abstime, void (*fct)(void *aux1, void *aux2),
                        void *aux1, void *aux2)
{
    struct ccnl_timer_s *t, **pp;
    static int handlercnt;

    t = (struct ccnl_timer_s *) ccnl_calloc(1, sizeof(*t));

    if (!t) {
        return 0;
    }

    t->fct2 = fct;
    t->timeout = abstime;
    t->aux1 = aux1;
    t->aux2 = aux2;

    for (pp = &eventqueue; ; pp = &((*pp)->next)) {
        if (!*pp || (*pp)->timeout.tv_sec > t->timeout.tv_sec ||
            ((*pp)->timeout.tv_sec == t->timeout.tv_sec &&
             (*pp)->timeout.tv_usec > t->timeout.tv_usec)) {
            t->next = *pp;
            t->handler = handlercnt++;
            *pp = t;
            return t;
        }
    }

    return NULL; // ?
}

void
ccnl_rem_timer(void *h)
{
    struct ccnl_timer_s **pp;

    DEBUGMSG(99, "removing time handler %p\n", h);

    for (pp = &eventqueue; *pp; pp = &((*pp)->next)) {
        if ((void *)*pp == h) {
            struct ccnl_timer_s *e = *pp;
            *pp = e->next;
            ccnl_free(e);
            break;
        }
    }
}

char *
timestamp(void)
{
    static char ts[30], *cp;
    struct timeval now;
    ccnl_get_timeval(&now);

    sprintf(ts, "%.4lu", (time_t) 100000 * now.tv_sec + now.tv_usec);
    cp = strchr(ts, '.');

    if (!cp) {
        strcat(ts, ".0000");
    }
    else if (strlen(cp) > 5) {
        cp[5] = '\0';
    }
    else while (strlen(cp) < 5) {
            strcat(cp, "0");
        }

    return ts;
}

// eof
