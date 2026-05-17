/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for LPC1768 ethernet peripheral
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "cpu.h"
#include "lpc1768_eth_netdev.h"
#include "net/netdev.h"
#include "shell.h"
#include "test_utils/expect.h"
#include "test_utils/netdev_eth_minimal.h"

#include "init_dev.h"

static netdev_t lpc1768_eth_netdev;

int netdev_eth_minimal_init_devs(netdev_event_cb_t cb)
{
    /* setup the specific driver */
    lpc1768_eth_netdev_setup(&lpc1768_eth_netdev);

    /* set the application-provided callback */
    lpc1768_eth_netdev.event_callback = cb;

    /* initialize the device driver */
    int res = lpc1768_eth_netdev.driver->init(&lpc1768_eth_netdev);
    expect(!res);

    return 0;
}

int main(void)
{
    puts("Test application for LPC1768 ethernet peripheral");

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
