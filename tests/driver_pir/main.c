/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the PIR motion sensor driver
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#ifndef PIR_GPIO
#error "PIR_GPIO not defined"
#endif

#include <stdio.h>

#include "thread.h"
#include "xtimer.h"
#include "pir.h"

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
            case PIR_STATUS_HI:
                puts("something started moving.");
                break;
            case PIR_STATUS_LO:
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
    printf("Initializing PIR sensor at GPIO_%ld... ", (long)PIR_GPIO);
    if (pir_init(&dev, PIR_GPIO) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

#if TEST_PIR_POLLING
    puts("Printing sensor state every second.");
    while (1) {
        printf("Status: %s\n", pir_get_status(&dev) == PIR_STATUS_LO ? "lo" : "hi");
        xtimer_usleep(1000 * 1000);
    }
#else
   thread_create(
           pir_handler_stack, sizeof(pir_handler_stack), THREAD_PRIORITY_MAIN - 1,
           THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
           pir_handler, NULL, "pir_handler");
#endif
    return 0;
}
