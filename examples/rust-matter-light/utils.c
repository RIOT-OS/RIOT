#include "periph/rtc.h"
#include <time.h>

// RIOT_EPOCH(2020) in seconds since UNIX Epoch
#define RIOT_EPOCH_SECS 1577833200;

uint32_t get_seconds_since_unix_epoch(void) {
    // Get time since RIOT_EPOCH from RTC
    struct tm now;
    rtc_get_time(&now);
    // Add to RIOT_EPOCH to get current seconds since UNIX epoch
    return rtc_mktime(&now) + RIOT_EPOCH_SECS;
}
