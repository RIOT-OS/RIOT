#include <stdio.h>
#include <time.h>

#include <thread.h>
#include <msg.h>
#include <rtc.h>
#include <display.h>

#include "alarm_app.h"
#include "clock_app.h"
#include "watch.h"

static char alarm_stack[KERNEL_CONF_STACKSIZE_DEFAULT];

static void alarm_thread(void) {
    msg_t m;

    struct tm time;

    time.tm_sec = 1;
    time.tm_min = 2;
    time.tm_hour = 3;

    int active = 0;

    while(1) {
        msg_receive(&m);
        switch (m.type) {
            case MSG_ACTIVATE:
                {
                    time_print(&time);
                    if (active) {
                    } else {
                    }
                    break;
                }
            case MSG_DEACTIVATE:
                  {
                      break;
                  }
            case MSG_BUTTON_HASH:
                  {
                      if (active) {
                          active = 0;
                          display_symbol(LCD_ICON_ALARM, SEG_OFF);
                      } else {
                          active = 1;
                          display_symbol(LCD_ICON_ALARM, SEG_ON);
                      }
                      break;
                  }
            default: 
                  {
                      printf("def alarm\n");
                  }
        }
    }
}

int alarm_app_init() {
   return thread_create(alarm_stack, sizeof(alarm_stack), PRIORITY_MAIN-1, CREATE_STACKTEST, alarm_thread, "alarm");
}
