/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the Gaisler GRETH Ethernet device driver
 *
 * @author      Matvii Ivashchenko
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "greth.h"
#include "greth_params.h"
#include "test_utils/expect.h"
#include "test_utils/netdev_eth_minimal.h"
#include "init_dev.h"

static greth_t greth[GRETH_NUM];

int netdev_eth_minimal_init_devs(netdev_event_cb_t cb)
{
    for (unsigned i = 0; i < GRETH_NUM; i++) {
        netdev_t *device = &greth[i].netdev;

        /* setup the specific driver */
        greth_setup(&greth[i], &greth_params[i], i);

        /* set the application-provided callback */
        device->event_callback = cb;

        /* initialize the device driver */
        int res = device->driver->init(device);
        expect(!res);
    }

    return 0;
}

int main(void)
{
    puts("Test application for GRETH Ethernet device driver");

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
