#include "Arduino.h"
#include "ztimer.h"

unsigned long millis(void){
    return ztimer_now(ZTIMER_MSEC);
}

unsigned long micros(void){
    return ztimer_now(ZTIMER_USEC);
}


