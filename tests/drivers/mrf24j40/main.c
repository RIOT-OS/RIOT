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
#include "net/netdev/ieee802154_submac.h"
#include "common.h"

static netdev_ieee802154_submac_t mrf24j40_netdev[MRF24J40_NUM];

static ieee802154_dev_t *_reg_callback(ieee802154_dev_type_t type, void *opaque)
{
    if (type != IEEE802154_DEV_TYPE_MRF24J40) {
        assert(false);
    }
    int *c = opaque;
    return &mrf24j40_netdev[(*(c))++].submac.dev;
}

int netdev_ieee802154_minimal_init_devs(netdev_event_cb_t cb) {
    puts("Initializing MRF24J40 devices");

    int c = 0;
    /* This function will iterate through all kw2xrf radios */
    ieee802154_hal_test_init_devs(_reg_callback, &c);

    for (unsigned i = 0; i < MRF24J40_NUM; i++) {
        printf("%d out of %u\n", i + 1, (unsigned)MRF24J40_NUM);
        netdev_register(&mrf24j40_netdev[i].dev.netdev, NETDEV_MRF24J40, 0);
        netdev_ieee802154_submac_init(&mrf24j40_netdev[i]);

        /* set the application-provided callback */
        mrf24j40_netdev[i].dev.netdev.event_callback = cb;

        /* initialize the device driver */
        int res = mrf24j40_netdev[i].dev.netdev.driver->init(&mrf24j40_netdev[i].dev.netdev);
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
