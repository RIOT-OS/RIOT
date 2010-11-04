#include <stdio.h>
#include <stdint.h>
#include <lpc2387-rtc.h>
#include <sys/time.h>

void _gettime_handler(char *unused) {
    struct tm now;
    rtc_get_localtime(&now);

    printf("%s", asctime(&now));
}

void _settime_handler(char* c) {
    struct tm now;
    int res;
    uint16_t month, epoch_year;

    res = sscanf(c, "settime %hu-%hu-%i %i:%i:%i",
            &epoch_year,
            &month,
            &(now.tm_mday),
            &(now.tm_hour),
            &(now.tm_min),
            &(now.tm_sec));
    
    if (res < 6) {
        printf("Usage: settime YYYY-MM-DD hh:mm:ss\n");
        return;
    }
    else {
        printf("OK %s", asctime(&now));
    }
    
    now.tm_year = epoch_year - 1900;
    now.tm_mon = month - 1;
    time_t t = mktime(&now);
    rtc_set(t);
}


