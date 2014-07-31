/**
 * Timex implementation
 *
 * Copyright (C) 2009, 2010, 2013, 2014 Freie Universitaet Berlin (FUB).
 * Copyright (C) 2013, INRIA.
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

#define SEC_IN_USEC 1000000

timex_t timex_add(const timex_t a, const timex_t b)
{
#if ENABLE_DEBUG
    if (!timex_isnormalized(a) || !timex_isnormalized(b)) {
        puts("timex_add on denormalized value");
    }
#endif

    timex_t result;
    result.seconds = a.seconds + b.seconds;
    result.microseconds = a.microseconds + b.microseconds;

    if (result.microseconds > SEC_IN_USEC) {
        result.microseconds -= SEC_IN_USEC;
        result.seconds++;
    }

    return result;
}

void timex_normalize(timex_t *time)
{
    time->seconds += (time->microseconds / SEC_IN_USEC);
    time->microseconds %= SEC_IN_USEC;
}

int timex_isnormalized(timex_t *time)
{
    return (time->microseconds < SEC_IN_USEC);
}

timex_t timex_set(uint32_t seconds, uint32_t microseconds)
{
    timex_t result;
    result.seconds = seconds;
    result.microseconds = microseconds;

#if ENABLE_DEBUG
    if (!timex_isnormalized(result)) {
        puts("timex_set on denormalized value");
    }
#endif

    return result;
}

timex_t timex_sub(const timex_t a, const timex_t b)
{
#if ENABLE_DEBUG
    if (!timex_isnormalized(a) || !timex_isnormalized(b)) {
        puts("timex_sub on denormalized value");
    }
#endif

    timex_t result;

    if (a.microseconds >= b.microseconds) {
        result.seconds = a.seconds - b.seconds;
        result.microseconds = a.microseconds - b.microseconds;
    }
    else {
        result.seconds = a.seconds - b.seconds - 1;
        result.microseconds = a.microseconds + SEC_IN_USEC - b.microseconds;
    }

    return result;
}

int timex_cmp(const timex_t a, const timex_t b)
{
#if ENABLE_DEBUG
    if (!timex_isnormalized(a) || !timex_isnormalized(b)) {
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

uint64_t timex_uint64(const timex_t a)
{
    return (uint64_t) a.seconds * SEC_IN_USEC + a.microseconds;
}

timex_t timex_from_uint64(const uint64_t timestamp)
{
    return timex_set(timestamp / SEC_IN_USEC, timestamp % SEC_IN_USEC);
}

void timex_print(const timex_t t)
{
    printf("Seconds: %" PRIu32 " - Microseconds: %" PRIu32 "\n", t.seconds, t.microseconds);
}
