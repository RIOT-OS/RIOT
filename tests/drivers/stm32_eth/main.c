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
 * @brief       Test application for STM32 ethernet peripheral driver
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "stm32_eth.h"
#include "test_utils/expect.h"
#include "test_utils/netdev_eth_minimal.h"

static netdev_t stm32_eth;

int netdev_eth_minimal_init_devs(netdev_event_cb_t cb) {

    /* setup the specific driver */
    stm32_eth_netdev_setup(&stm32_eth);

    /* set the application-provided callback */
    stm32_eth.event_callback = cb;

    /* initialize the device driver */
    int res = stm32_eth.driver->init(&stm32_eth);
    expect(!res);

    return 0;
}

int main(void)
{
    puts("Test application for STM32 ethernet peripheral driver");

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
