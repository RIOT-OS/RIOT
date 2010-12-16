#include <stdio.h>
#include <time.h>

#include <thread.h>
#include <msg.h>
#include <rtc.h>

#include "clock_app.h"
#include "watch.h"

void time_print(struct tm *time) {
    printf("%02i %02i%02i\n", time->tm_sec, time->tm_hour, time->tm_min);
}

static char clock_stack[KERNEL_CONF_STACKSIZE_DEFAULT];

static void clock_thread(void) {
    msg m;
   
    int active = 0;
    rtc_second_pid = thread_getpid();

    while(1) {
        msg_receive(&m);
        switch (m.type) {
            case RTC_SECOND: 
                {
                    if (! active) break;
                }
            case MSG_ACTIVATE:
                {
                    active = 1;
                    struct tm now;
                    rtc_get_localtime(&now);
                    time_print(&now);
                    break;
                }
            case MSG_DEACTIVATE:
                  {
                      active = 0;
                      break;
                  }
            case MSG_BUTTON_HASH:
                  {
                      printf("hashclock\n");
                      break;
                  }
            default: 
                  {
                      printf("def clock\n");
                  }
        }
    }
}

int clock_app_init() {
   return thread_create(clock_stack, sizeof(clock_stack), PRIORITY_MAIN-1, CREATE_STACKTEST, clock_thread, "clock");
}
