/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @author      Attilio Dona'
 *
 */

#include "throw.h"
#include "thread.h"
#include "msg.h"
#include "xtimer.h"
#include "periph/gpio.h"


#define ERROR_TASK_PRIORITY             (5)


/**
 * the error signaling thread
 */
static kernel_pid_t errors_pid;
char errors_stack[512];


static void* errors_task(void* arg);


void throw(short err_type, long err_value) {
    msg_t m;

    m.type = err_type;
    m.content.value = err_value;

    msg_send_receive(&m, &m, errors_pid);
}


void error_bip(void* arg) {
    msg_t m = { 0, LOST_IN_PANIC, { 0 } };

    msg_send(&m, errors_pid);
}


void errors_thread_init(void) {

    errors_pid = thread_create(errors_stack,
            sizeof(errors_stack), ERROR_TASK_PRIORITY,
			THREAD_CREATE_STACKTEST, errors_task, NULL, "errtrap");

}

void* errors_task(void* arg) {
    msg_t m, msg_queue[2];
    xtimer_t timer;

    timer.callback = error_bip;

    msg_init_queue(msg_queue, 2);

    while(1) {
        msg_receive(&m);

        switch (m.type) {
            case LOST_IN_PANIC:
                LED_RED_TOGGLE;
                xtimer_set(&timer, MSEC_TO_TICKS(100));
                break;
            default:
                xtimer_set(&timer, MSEC_TO_TICKS(100));
                puts(sl_err_descr[m.type]);
        }
    }
    return NULL;
}
