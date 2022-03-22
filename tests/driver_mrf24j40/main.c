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
 * @brief       Test application for MRF24J40 IEEE 802.15.4 device driver
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "init_dev.h"
#include "mrf24j40.h"
#include "mrf24j40_params.h"
#include "shell.h"
#include "test_utils/netdev_ieee802154_minimal.h"

static mrf24j40_t mrf24j40[MRF24J40_NUM];

int netdev_ieee802154_minimal_init_devs(netdev_event_cb_t cb) {
    puts("Initializing MRF24J40 devices");

    for (unsigned i = 0; i < MRF24J40_NUM; i++) {
        printf("%d out of %d\n", i + 1, MRF24J40_NUM);
        /* setup the specific driver */
        mrf24j40_setup(&mrf24j40[i], &mrf24j40_params[i], i);

        /* set the application-provided callback */
        mrf24j40[i].netdev.netdev.event_callback = cb;

        /* initialize the device driver */
        int res = mrf24j40[i].netdev.netdev.driver->init(&mrf24j40[i].netdev.netdev);
        if (res != 0) {
            return -1;
        }
    }

    return 0;
}

int main(void)
{
    puts("Test application for MRF24J40 IEEE 802.15.4 device driver");

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
