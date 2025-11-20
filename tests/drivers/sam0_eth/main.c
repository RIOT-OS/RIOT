/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "test_utils/expect.h"
#include "test_utils/netdev_eth_minimal.h"
#include "init_dev.h"
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
    expect(!res);

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
