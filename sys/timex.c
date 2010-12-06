#include <timex.h>

timex_t timex_add(const timex_t a, const timex_t b) {
    timex_t result;
    result.seconds = a.seconds + b.seconds;
    result.nanoseconds = a.nanoseconds + b.nanoseconds;

    if (result.nanoseconds < a.nanoseconds) {
        result.seconds++;
    }

/*    if (result.nanoseconds > 1000000) {
        result.nanoseconds -= 1000000;
        result.seconds++;
    }
*/
    return result;
}

void timex_normalize(timex_t *time) {
    time->seconds += (time->nanoseconds / 1000000);
    time->nanoseconds %= 1000000;
}

timex_t timex_set(uint32_t seconds, uint32_t nanoseconds) {
    timex_t result;
    result.seconds = seconds;
    result.nanoseconds = nanoseconds;

    return result;
}

timex_t timex_sub(const timex_t a, const timex_t b) {
    timex_t result;
    result.seconds = a.seconds - b.seconds;
    result.nanoseconds = a.nanoseconds - b.nanoseconds;

    return result;
}

int timex_cmp(const timex_t a, const timex_t b) {
    if (a.seconds < b.seconds) return -1;
    if (a.seconds == b.seconds) {
        if (a.nanoseconds < b.nanoseconds) return -1;
        if (a.nanoseconds == b.nanoseconds) return 0;
    }
    return 1;
}

