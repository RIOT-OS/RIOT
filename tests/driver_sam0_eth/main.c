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
 * @brief       Test application for SAM0 ethernet peripheral
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
#include "net/netdev.h"
#include "sam0_eth_netdev.h"

static netdev_t sam0_eth;

int netdev_eth_minimal_init_devs(netdev_event_cb_t cb) {

    /* setup the specific driver */
    sam0_eth_setup(&sam0_eth);

    /* set the application-provided callback */
    sam0_eth.event_callback = cb;

    /* initialize the device driver */
    int res = sam0_eth.driver->init(&sam0_eth);
    assert(!res);

    return 0;
}

int main(void)
{
    puts("Test application for SAM0 ethernet peripheral");

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
