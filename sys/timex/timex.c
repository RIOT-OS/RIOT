/**
 * Timex implementation
 *
 * Copyright (C) 2009, 2010, 2013, 2014 Freie Universitaet Berlin (FUB).
 * Copyright (C) 2013, 2014 INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @author Oliver Hahm <oliver.hahm@inria.fr>
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author Daniel Jentsch <d.jentsch@fu-berlin.de>
 *
 */

#include <stdio.h>
#include <inttypes.h>

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "timex.h"

void timex_add(const timex_t *a, const timex_t *b, timex_t *r)
{
#if ENABLE_DEBUG
    if (!timex_isnormalized(&a) || !timex_isnormalized(&b)) {
        puts("timex_add on denormalized value");
    }
#endif

    r->seconds = a->seconds + b->seconds;
    r->microseconds = a->microseconds + b->microseconds;

    if (r->microseconds > SEC_IN_USEC) {
        r->microseconds -= SEC_IN_USEC;
        r->seconds++;
    }

    return;
}

timex_t timex_set(uint32_t seconds, uint32_t microseconds)
{
    timex_t result;
    result.seconds = seconds;
    result.microseconds = microseconds;

#if ENABLE_DEBUG
    if (!timex_isnormalized(&result)) {
        puts("timex_set on denormalized value");
    }
#endif

    return result;
}

void timex_sub(const timex_t *a, const timex_t *b, timex_t *r)
{
#if ENABLE_DEBUG
    if (!timex_isnormalized(&a) || !timex_isnormalized(&b)) {
        puts("timex_sub on denormalized value");
    }
#endif

    if (a->microseconds >= b->microseconds) {
        r->seconds = a->seconds - b->seconds;
        r->microseconds = a->microseconds - b->microseconds;
    }
    else {
        r->seconds = a->seconds - b->seconds - 1;
        r->microseconds = a->microseconds + SEC_IN_USEC - b->microseconds;
    }

    return;
}

int timex_cmp(const timex_t a, const timex_t b)
{
#if ENABLE_DEBUG
    if (!timex_isnormalized(&a) || !timex_isnormalized(&b)) {
        puts("timex_cmp on denormalized value");
    }
#endif

    if (a.seconds < b.seconds) {
        return -1;
    }

    if (a.seconds == b.seconds) {
        if (a.microseconds < b.microseconds) {
            return -1;
        }

        if (a.microseconds == b.microseconds) {
            return 0;
        }
    }

    return 1;
}
