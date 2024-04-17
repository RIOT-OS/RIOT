#include "periph/rtc.h"
#include <stdio.h>
#include <time.h>

// Matter Epoch: 2000/01/01 00:00:00 UTC
static struct tm matter_epoch = {
    .tm_year = 100,  // 2000 - 1900
    .tm_mon = 0,     // January
    .tm_mday = 1,    // 1st day
    .tm_hour = 0,    // 0 hours
    .tm_min = 0,     // 0 minutes
    .tm_sec = 0     // 0 seconds
};

int get_seconds_since_matter_epoch(void) {
    // Get current time from RTC
    struct tm now;
    rtc_get_time(&now);

    // Convert both times to time_t (timestamp)
    time_t start_ts = mktime(&matter_epoch);
    time_t now_ts = mktime(&now);
    double seconds_since_matter_epoch = difftime(now_ts, start_ts);
    return (int)seconds_since_matter_epoch;
}
