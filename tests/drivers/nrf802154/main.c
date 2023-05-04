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
 * @brief       Test application for NRF802154 IEEE 802.15.4 device driver
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "nrf802154.h"
#include "init_dev.h"
#include "net/netdev/ieee802154_submac.h"
#include "shell.h"
#include "test_utils/netdev_ieee802154_minimal.h"

static netdev_ieee802154_submac_t nrf802154;

int netdev_ieee802154_minimal_init_devs(netdev_event_cb_t cb) {
    netdev_t *netdev = &nrf802154.dev.netdev;

    puts("Initializing NRF802154 device");

    netdev_register(netdev, NETDEV_CC2538, 0);
    netdev_ieee802154_submac_init(&nrf802154);

    /* set the application-provided callback */
    netdev->event_callback = cb;

    /* setup and initialize the specific driver */
    nrf802154_hal_setup(&nrf802154.submac.dev);
    nrf802154_init();

    /* initialize the device driver */
    int res = netdev->driver->init(netdev);
    if (res != 0) {
        return -1;
    }

    return 0;
}

int main(void)
{
    puts("Test application for NRF802154 IEEE 802.15.4 device driver");

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
