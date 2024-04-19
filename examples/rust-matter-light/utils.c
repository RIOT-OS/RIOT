#include <stdint.h>
#include "modules.h"

#if IS_USED(MODULE_PERIPH_RTC)
#include "periph/rtc.h"
#else
#include "ztimer.h"
#endif

// RIOT_EPOCH(2020) in seconds since UNIX Epoch
#define RIOT_EPOCH_SECS 1577833200;

uint32_t get_seconds_since_unix_epoch(void) {
#if IS_USED(MODULE_PERIPH_RTC)
    // Get seconds since RIOT_EPOCH from RTC
    struct tm now;
    rtc_get_time(&now);
    // Add to RIOT_EPOCH to get seconds since UNIX epoch
    return RIOT_EPOCH_SECS + rtc_mktime(&now);
#else
    // Get elapsed seconds from ztimer
    return RIOT_EPOCH_SECS + ztimer_now(ZTIMER_SEC);
#endif
}
