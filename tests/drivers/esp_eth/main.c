/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for ESP ethernet peripheral
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
#include "esp_eth_netdev.h"
#include "esp_eth_params.h"

extern void esp_eth_setup(esp_eth_netdev_t* dev);
extern esp_eth_netdev_t _esp_eth_dev;

int netdev_eth_minimal_init_devs(netdev_event_cb_t cb) {
    netdev_t *device = &_esp_eth_dev.netdev;

    /* setup the specific driver */
    esp_eth_setup(&_esp_eth_dev);

    /* set the application-provided callback */
    device->event_callback = cb;

    /* initialize the device driver */
    int res = device->driver->init(device);
    expect(!res);

    return 0;
}

int main(void)
{
    puts("Test application for ESP ethernet peripheral");

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
