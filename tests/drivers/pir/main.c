/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2018 UC Berkeley
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the PIR motion sensor driver
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "timex.h"
#include "ztimer.h"
#include "pir.h"
#include "pir_params.h"

static char pir_handler_stack[THREAD_STACKSIZE_MAIN];
static pir_t dev;

void* pir_handler(void *arg)
{
    (void)arg;
    msg_t msg_q[1];
    msg_init_queue(msg_q, 1);

    printf("Registering PIR handler thread...     %s\n",
           pir_register_thread(&dev) == 0 ? "[OK]" : "[Failed]");

    msg_t m;
    while (msg_receive(&m)) {
        printf("PIR handler got a message: ");
        switch (m.type) {
            case PIR_STATUS_ACTIVE:
                puts("something started moving.");
                break;
            case PIR_STATUS_INACTIVE:
                puts("the movement has ceased.");
                break;
            default:
                puts("stray message.");
                break;
        }
    }
    puts("PIR handler: this should not have happened!");

    return NULL;
}

int main(void)
{
    puts("PIR motion sensor test application\n");
    printf("Initializing PIR sensor at GPIO_%ld... ", (long)PIR_PARAM_GPIO);
    if (pir_init(&dev, &pir_params[0]) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

#if TEST_PIR_POLLING
    puts("Printing sensor state every second.");
    while (1) {
        printf("Status: %s\n", pir_get_status(&dev) == PIR_STATUS_INACTIVE ?
               "inactive" : "active");
        ztimer_sleep(ZTIMER_USEC, 1 * US_PER_SEC);
    }
#else
   thread_create(
           pir_handler_stack, sizeof(pir_handler_stack), THREAD_PRIORITY_MAIN - 1,
           THREAD_CREATE_WOUT_YIELD,
           pir_handler, NULL, "pir_handler");
#endif
    return 0;
}
