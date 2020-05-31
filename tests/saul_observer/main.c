/*
 * Copyright (C) 2020 Juergen Fitchen <me@jue.yt>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 *
 * @file
 * @brief       Test the SAUL observer
 *
 * @author      Juergen Fitchen <me@jue.yt>
 *
 */

#include <stdio.h>

#include "thread.h"
#include "phydat.h"
#include "saul_reg.h"
#include "saul_observer.h"
#include "shell.h"

static void *_notify(void *arg)
{
    msg_t msg, msgq[4];
    msg_bus_entry_t sub;
    saul_reg_t *ptr;
    phydat_t res;
    int rc;

    (void) arg;
    msg_init_queue(msgq, 4);
    msg_bus_attach(&saul_observer_bus, &sub);

    for (ptr = saul_reg; ptr; ptr = ptr->next) {
        saul_observer_subscribe(&sub, ptr);
    }

    while (1) {
        msg_receive(&msg);
        saul_reg_t *dev = msg.content.ptr;
        rc = saul_reg_read(dev, &res);
        printf("SAUL DEV %d (%s) CHANGED - ", dev->id, dev->name);
        phydat_dump(&res, rc);
    }

    return NULL;
}

char _stack[1024];

int main(void)
{
    puts("SAUL observer test application");
    thread_create(_stack, sizeof(_stack), 8,
                  THREAD_CREATE_STACKTEST,
                  _notify, NULL, "saul_observer");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
