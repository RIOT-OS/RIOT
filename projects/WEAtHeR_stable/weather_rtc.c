#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef MODULE_RTC
#include <lpc2387-rtc.h>


void _gettime_handler(void) {
    struct tm now;
    rtc_get_localtime(&now);

    printf("%s", asctime(&now));
}

void _settime_handler(char* c) {
    time_t now;
    int res;

    res = sscanf(c, "date %lu", &now);
    
    if (res < 1) {
        printf("Usage: date <UNIX timestamp>\n");
        return;
    }
    else {
        puts("OK");
    }
    
    rtc_set(now);
}

void _date_handler(char* c) {
	if (strlen(c) == 4) {
		_gettime_handler();	
	}
	else {
		_settime_handler(c);
	}
}

#endif
