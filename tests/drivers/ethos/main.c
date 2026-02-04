/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for ethernet-over-serial driver
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
#include "ethos.h"
#include "ethos_params.h"

static ethos_t ethos[ETHOS_NUM];
static uint8_t inbuf[ETHOS_NUM][2048];

int netdev_eth_minimal_init_devs(netdev_event_cb_t cb) {
    for (unsigned i = 0; i < ETHOS_NUM; i++) {
        netdev_t *device = &ethos[i].netdev;

        /* setup the specific driver */
        ethos_setup(&ethos[i], &ethos_params[i], i, inbuf[i], sizeof(inbuf[i]));

        /* set the application-provided callback */
        device->event_callback = cb;

        /* initialize the device driver */
        int res = device->driver->init(device);
        expect(!res);
    }

    return 0;
}

int main(void)
{
    puts("Test application for ETHOS driver");

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
