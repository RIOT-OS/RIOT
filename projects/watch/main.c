#include <stdio.h>
#include <string.h>
#include <time.h>
#include <thread.h>
#include <msg.h>

#include <gpioint.h>
#include <buttons.h>
#include <buzzer.h>

#include <posix_io.h>
#include <board_uart0.h>

#include "watch.h"
#include "alarm_app.h"
#include "clock_app.h"

#define NUM_APPS 2
int napps = NUM_APPS;
int apps[NUM_APPS];

int button_thread = 0;
void button_star(void) {
    msg m;

    if (button_thread) {
        m.type = MSG_BUTTON_STAR;
        msg_send(&m, button_thread, false);
    }
}

int main(void)
{
    memset(apps, '\0', sizeof(apps));
    apps[0] = clock_app_init();
    apps[1] = alarm_app_init();

    gpioint_set(2, BUTTON_STAR_PIN, (GPIOINT_RISING_EDGE | GPIOINT_DEBOUNCE), button_star);
    button_thread = thread_getpid();

    int active_app = 0;
    
    msg m;

    //buzzer_beep(15, 5000);
    
    printf("ukleos\n");

    m.type = MSG_ACTIVATE;
    msg_send(&m, apps[active_app], true);

    while(1) {
        msg_receive(&m);

        switch (m.type) {
            case MSG_BUTTON_STAR: { 
                    m.type = MSG_DEACTIVATE;
                    msg_send(&m, apps[active_app], true);
                    
                    active_app++;

                    if (active_app == (NUM_APPS)) active_app = 0;

                    m.type = MSG_ACTIVATE;
                    msg_send(&m, apps[active_app], true);
                    
      //              buzzer_beep(15, 5000);

                    break;
                }
            case MSG_BUTTON_HASH: 
                {
                    m.type = MSG_BUTTON_HASH;
                    msg_send(&m, apps[active_app], true);
                    break;
                }
            default:
                {
                    printf("msg\n");
                }
        }
    }
}
