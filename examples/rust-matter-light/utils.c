#include <stdint.h>
#include "modules.h"
#include <stdio.h>
#include "ztimer.h"
#if IS_USED(MODULE_PERIPH_RTC)
#include "periph/rtc.h"
#endif

uint32_t get_seconds_since_riot_epoch(void) {
#if IS_USED(MODULE_PERIPH_RTC)
    // Get seconds since RIOT_EPOCH from RTC
    struct tm now;
    rtc_get_time(&now);
    rtc_tm_normalize(&now);
    return rtc_mktime(&now);
#else
    // Get elapsed seconds from ztimer
    return ztimer_now(ZTIMER_SEC);
#endif
}

uint16_t get_milliseconds(void) {
    return ztimer_now(ZTIMER_MSEC) % 1000;
}