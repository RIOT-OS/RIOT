#include <stdio.h>
#include <inttypes.h>

#include "timex.h"

#define SEC_IN_USEC 1000000

timex_t timex_add(const timex_t a, const timex_t b)
{
    timex_t result;
    result.seconds = a.seconds + b.seconds;
    result.microseconds = a.microseconds + b.microseconds;

    if (result.microseconds < a.microseconds) {
        result.seconds++;
    }

    /*    if (result.microseconds > SEC_IN_USEC) {
            result.microseconds -= SEC_IN_USEC;
            result.seconds++;
        }
    */
    return result;
}

void timex_normalize(timex_t *time)
{
    time->seconds += (time->microseconds / SEC_IN_USEC);
    time->microseconds %= SEC_IN_USEC;
}

timex_t timex_set(uint32_t seconds, uint32_t microseconds)
{
    timex_t result;
    result.seconds = seconds;
    result.microseconds = microseconds;

    return result;
}

timex_t timex_sub(const timex_t a, const timex_t b)
{
    timex_t result;
    result.seconds = a.seconds - b.seconds;
    result.microseconds = a.microseconds - b.microseconds;

    return result;
}

int timex_cmp(const timex_t a, const timex_t b)
{
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

void timex_print(const timex_t t)
{
    printf("Seconds: %"PRIu32" - Microseconds: %"PRIu32"\n", t.seconds, t.microseconds);
}
