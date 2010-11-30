#include <stdio.h>
#include <stdint.h>
#include <lpc2387-rtc.h>
#include <sys/time.h>
#include <string.h>

void _gettime_handler(void) {
    struct tm now;
    rtc_get_localtime(&now);

    printf("%s", asctime(&now));
}

void _settime_handler(char* c) {
    struct tm now;
    int res;
    uint16_t month, epoch_year;

    res = sscanf(c, "date %hu-%hu-%u %u:%u:%u",
            &epoch_year,
            &month,
            (unsigned int*) &(now.tm_mday),
            (unsigned int*) &(now.tm_hour),
            (unsigned int*) &(now.tm_min),
            (unsigned int*) &(now.tm_sec));
    
    if (res < 6) {
        printf("Usage: date YYYY-MM-DD hh:mm:ss\n");
        return;
    }
    else {
        puts("OK");
    }
    
    now.tm_year = epoch_year - 1900;
    now.tm_mon = month - 1;
    time_t t = mktime(&now);
    rtc_set(t);
}

void _date_handler(char* c) {
	if (strlen(c) == 4) {
		_gettime_handler();	
	}
	else {
		_settime_handler(c);
	}
}
