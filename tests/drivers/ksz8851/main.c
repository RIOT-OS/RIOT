/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for KSZ8851SNL ethernet device driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>

#include "init_dev.h"
#include "ksz8851.h"
#include "ksz8851_params.h"
#include "shell.h"
#include "test_utils/expect.h"
#include "test_utils/netdev_eth_minimal.h"

static ksz8851_t ksz8851[KSZ8851_NUM];

int netdev_eth_minimal_init_devs(netdev_event_cb_t cb)
{
    for (unsigned i = 0; i < KSZ8851_NUM; i++) {
        netdev_t *device = &ksz8851[i].netdev;

        /* setup the specific driver */
        ksz8851_setup(&ksz8851[i], &ksz8851_params[i], i);

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
    puts("Test application for KSZ8851SNL ethernet device driver");

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
