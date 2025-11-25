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
 * @brief       Test application for CC2420 IEEE 802.15.4 device driver
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "cc2420.h"
#include "init_dev.h"
#include "shell.h"
#include "test_utils/netdev_ieee802154_minimal.h"

static cc2420_t cc2420[CC2420_NUM];

int netdev_ieee802154_minimal_init_devs(netdev_event_cb_t cb) {
    puts("Initializing CC2420 devices");

    for (unsigned i = 0; i < CC2420_NUM; i++) {
        printf("%d out of %u\n", i + 1, (unsigned)CC2420_NUM);
        netdev_t *netdev = &cc2420[i].netdev.netdev;

        /* setup the specific driver */
        cc2420_setup(&cc2420[i], &cc2420_params[i], i);

        /* set the application-provided callback */
        netdev->event_callback = cb;

        /* initialize the device driver */
        int res = netdev->driver->init(netdev);
        if (res) {
            return res;
        }
    }
    return 0;
}

int main(void)
{
    puts("Test application for CC2420 IEEE 802.15.4 device driver");

    int res = netdev_ieee802154_minimal_init();
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
