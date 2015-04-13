#include <stdio.h>
#include <string.h>
#include <time.h>

#include "kernel.h"
#include <cc430-rtc.h>
#include <gpioint.h>
#include <buttons.h>
#include <buzzer.h>

#include <thread.h>
#include <board.h>
#include <hwtimer.h>
#include <vtimer.h>
#include <msg.h>
#include <transceiver.h>
#include <cc110x_ng.h>
#include <battery.h>

#define BUTTON_EVENT_STAR       (0x01)
#define BUTTON_EVENT_NUM        (0x02)
#define BUTTON_EVENT_UP         (0x04)
#define BUTTON_EVENT_DOWN       (0x08)
#define BUTTON_EVENT_BACKLIGHT  (0x10)

#define BUTTON_HANDLER_STACK_SIZE    (256)
#define RADIO_STACK_SIZE    (512)
#define SEND_SIZE   CC1100_MAX_DATA_LENGTH

#define RCV_BUFFER_SIZE     (4)

#define SENDING_DELAY       (5 * 1000)

char radio_stack_buffer[RADIO_STACK_SIZE];
char button_handler_stack_buffer[BUTTON_HANDLER_STACK_SIZE];

uint8_t snd_buffer[SEND_SIZE];

msg_t msg_q[RCV_BUFFER_SIZE];

static msg_t mesg;
static transceiver_command_t tcmd;
static radio_packet_t p;

static int button_handler_pid;
static uint8_t button_event = 0;
static uint8_t mode = 0;

void send(radio_address_t dst, uint8_t len, uint8_t *data);

void send(radio_address_t dst, uint8_t len, uint8_t *data) {
    mesg.type = SND_PKT;
    mesg.content.ptr = (char*) &tcmd;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &p;

    p.length = len;
    p.dst = dst;

    p.data = data;
    msg_send(&mesg, transceiver_pid, 1);
}

void radio(void) {
    msg_t m;
    radio_packet_t *p;

    msg_init_queue(msg_q, RCV_BUFFER_SIZE);

    while (1) {
        msg_receive(&m);
        if (m.type == PKT_PENDING) {

            p = (radio_packet_t*) m.content.ptr;
            send(p->src, p->length, p->data);

            p->processing--;
        }
        else if (m.type == ENOBUFFER) {
        }
        else {
        }
    }
}

void button_handler(void) {
    while (1) {
        if (button_event & BUTTON_EVENT_STAR) {
            buzzer_beep(15, 5000);
            mode++;
            button_event &= ~BUTTON_EVENT_STAR;
        }
        thread_sleep();
    }
}

void change_mode(void) {
    button_event |= BUTTON_EVENT_STAR;
    thread_wakeup(button_handler_pid);
}

int main(void) {
    int radio_pid;
    uint32_t voltage;
    struct tm now;

    /*
    ps_init();
    reset_altitude_measurement();
    */
    rtc_init();
    gpioint_init();

    now.tm_hour = 3;
    now.tm_min = 59;
    now.tm_sec = 42;

    rtc_set_localtime(&now);

    now.tm_min = 6;

    rtc_set_alarm(&now, RTC_ALARM_MIN);

    button_handler_pid = thread_create(button_handler_stack_buffer, BUTTON_HANDLER_STACK_SIZE, PRIORITY_MAIN+1, CREATE_STACKTEST, button_handler, "button_handler");
    gpioint_set(2, BUTTON_STAR_PIN, (GPIOINT_RISING_EDGE | GPIOINT_DEBOUNCE), change_mode);

    radio_address_t addr = 43;
    memset(snd_buffer, 43, SEND_SIZE);
    /*
    radio_pid = thread_create(radio_stack_buffer, RADIO_STACK_SIZE, PRIORITY_MAIN-2, CREATE_STACKTEST, radio, "radio");
    transceiver_init(TRANSCEIVER_CC1100);
    transceiver_start();
    transceiver_register(TRANSCEIVER_CC1100, radio_pid);

    mesg.type = SET_ADDRESS;
    mesg.content.ptr = (char*) &tcmd;

    printf("CC430");
    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &addr;
    msg_send(&mesg, transceiver_pid, 1);

    send(12, SEND_SIZE, snd_buffer);
*/
   while (1) {
        hwtimer_wait(SENDING_DELAY);
//        rtc_get_localtime(&now);
        
        voltage = battery_get_voltage();
        switch (mode) {
            case 0:
                printf("\n%lu", voltage);
                break;
            case 1:
                printf("\n%02u:%02u", now.tm_hour, now.tm_min);
                break;
            case 2:
                /*
                start_altitude_measurement();
                printf("\n%u", sAlt.altitude);
                stop_altitude_measurement();
                */
                puts("RIOT");
                break;
            default:
                mode = 0;
                break;
        }
   }
}
