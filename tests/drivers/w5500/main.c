/*
 * SPDX-FileCopyrightText: 2023 Stefan Schmidt
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for W5500 ethernet device driver
 *
 * @author      Stefan Schmidt <stemschmidt@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "test_utils/expect.h"
#include "test_utils/netdev_eth_minimal.h"
#include "init_dev.h"
#include "w5500.h"
#include "w5500_params.h"

static w5500_t w5500[W5500_NUM];

int netdev_eth_minimal_init_devs(netdev_event_cb_t cb) {
    for (unsigned i = 0; i < W5500_NUM; i++) {
        netdev_t *device = &w5500[i].netdev;

        /* setup the specific driver */
        w5500_setup(&w5500[i], &w5500_params[i], i);

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
    puts("Test application for W5500 ethernet device driver");

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
