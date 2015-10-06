/*
 * leds and button example
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 *  Public License v2.1. See the file LICENSE in the top level directory
 *  for more details.
 *
 * @author      Attilio Dona'
 *
 *
 */
#include <stdio.h>

#include "periph/gpio.h"
#include "xtimer.h"
#include "thread.h"

#define GREEN  1
#define YELLOW 2

kernel_pid_t tid;

void green_pulse(void* arg) {
    msg_t msg;
    msg.type = GREEN;
    msg_send(&msg, tid);
    if (sched_context_switch_request) {
        thread_yield();
    }
}

void yellow_pulse(void* arg) {
    msg_t msg;
    msg.type = YELLOW;
    msg_send(&msg, tid);
    if (sched_context_switch_request) {
        thread_yield();
    }
}

void bounce_filter(void* arg) {
    int val;

    //printf("debounce timer\n");

    val = gpio_read(PUSH2);

    if (val == HIGH) {
        LED_RED_ON;
    } else {
        LED_RED_OFF;
    }

}

void sw2_event(void* arg) {

}

int main(void) {
    msg_t msg, msg_queue[4];
    xtimer_t green_tim, yellow_tim;

    tid = thread_getpid();

    msg_init_queue(msg_queue, 4);

    green_tim.callback = green_pulse;
    yellow_tim.callback = yellow_pulse;

    gpio_init_int(PUSH2, GPIO_NOPULL, GPIO_BOTH, bounce_filter, 0);
    gpio_init(DEBUG_PIN, GPIO_DIR_OUT, GPIO_NOPULL);

    xtimer_set(&green_tim, MSEC_TO_TICKS(100));
    xtimer_set(&yellow_tim, MSEC_TO_TICKS(200));

    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
        case GREEN:
            LED_GREEN_TOGGLE;
            xtimer_set(&green_tim, MSEC_TO_TICKS(100));
            break;
        case YELLOW:
            LED_YELLOW_TOGGLE;
            xtimer_set(&yellow_tim, MSEC_TO_TICKS(200));
            break;
        }
    }

    return 0;
}
