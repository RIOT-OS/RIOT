/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for DOSE driver
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "test_utils/netdev_eth_minimal.h"
#include "init_dev.h"
#include "assert.h"
#include "dose.h"
#include "dose_params.h"

static dose_t dose[DOSE_NUM];

int netdev_eth_minimal_init_devs(netdev_event_cb_t cb) {
    for (unsigned i = 0; i < DOSE_NUM; i ++) {
        netdev_t *device = &dose[i].netdev;

        /* setup the specific driver */
        dose_setup(&dose[i], &dose_params[i], i);

        /* set the application-provided callback */
        device->event_callback = cb;

        /* initialize the device driver */
        int res = device->driver->init(device);
        assert(!res);
    }

    return 0;
}

int main(void)
{
    puts("Test application for DOSE driver");

    int res = netdev_eth_minimal_init();
    if (res) {
        puts("Error initializing devices");
        return 1;
    }

    /* start the shell */
    puts("Initialization successful - starting the shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
