/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for EFM32 ethernet peripheral
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "cpu.h"
#include "efm32_eth_netdev.h"
#include "net/netdev.h"
#include "shell.h"
#include "test_utils/expect.h"
#include "test_utils/netdev_eth_minimal.h"

#include "init_dev.h"

static netdev_t efm32_eth_netdev;

int netdev_eth_minimal_init_devs(netdev_event_cb_t cb)
{
    /* setup the specific driver */
    efm32_eth_netdev_setup(&efm32_eth_netdev);

    /* set the application-provided callback */
    efm32_eth_netdev.event_callback = cb;

    /* initialize the device driver */
    int res = efm32_eth_netdev.driver->init(&efm32_eth_netdev);
    expect(!res);

    return 0;
}

int main(void)
{
    puts("Test application for EFM32 ethernet peripheral");

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
