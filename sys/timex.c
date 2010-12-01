#include <timex.h>

timex_t timex_add(const timex_t a, const timex_t b) {
    timex_t result;
    result.seconds = a.seconds + b.seconds;
    result.nanoseconds = a.nanoseconds + b.nanoseconds;

    while (result.nanoseconds > 1000*1000) {
        result.nanoseconds -= 1000*1000;
        result.seconds++;
    }

    return result;
}
