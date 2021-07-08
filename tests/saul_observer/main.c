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

saul_observer_t observers[32];
size_t observers_cnt = 0;

static void _notify(saul_reg_t *dev, void *arg)
{
    phydat_t res;
    int rc;
    (void) arg;

    rc = saul_reg_read(dev, &res);
    printf("SAUL DEV %s CHANGED\n", dev->name);
    phydat_dump(&res, rc);
}

static void _register_observers(void)
{
    for (size_t i = 0; i < sizeof(observers); i++) {
        saul_reg_t *dev = saul_reg_find_nth(i);
        if (dev == NULL) {
            break;
        }
        observers[i].callback = _notify;
        saul_observer_add(dev, &observers[i]);
        printf("Register SAUL device %s\n", dev->name);
    }
}

char _stack[1024];

int main(void)
{
    puts("SAUL observer test application");
    _register_observers();

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
