/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for AT86RF2XX IEEE 802.15.4 device driver
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#include "at86rf2xx_internal.h"
#include "event/thread.h"
#include "init_dev.h"
#include "net/ieee802154.h"
#include "net/ieee802154/radio.h"
#include "net/netdev/ieee802154_submac.h"
#include "shell.h"
#include "test_utils/netdev_ieee802154_minimal.h"
#include "test_utils/expect.h"

static at86rf2xx_bhp_ev_t at86rf2xx_bhp[AT86RF2XX_NUM];
static netdev_ieee802154_submac_t at86rf2xx_netdev[AT86RF2XX_NUM];

#if AT86RF2XX_RANDOM_NUMBER_GENERATOR
int random_by_at86rf2xx(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    for (unsigned int i = 0; i < AT86RF2XX_NUM; i++) {
        uint32_t test = 0;
        at86rf2xx_get_random(&at86rf2xx_bhp[i].dev, (uint8_t *)&test, sizeof(test));
        printf("Random number for device %u via native API: %" PRIx32 "\n", i, test);
    }
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "random", "Get a value from Random Number Generator", random_by_at86rf2xx },
    { NULL, NULL, NULL }
};
#endif

int netdev_ieee802154_minimal_init_devs(netdev_event_cb_t cb) {

    puts("Initializing AT86RF2XX devices");

    for (unsigned i = 0; i < AT86RF2XX_NUM; i++) {
        printf("%d out of %u\n", i + 1, (unsigned)AT86RF2XX_NUM);
        /* setup the specific driver */
        at86rf2xx_init_event(&at86rf2xx_bhp[i], &at86rf2xx_params[i], &at86rf2xx_netdev[i].submac.dev, EVENT_PRIO_HIGHEST);

        netdev_register(&at86rf2xx_netdev[i].dev.netdev, NETDEV_AT86RF2XX, i);
        netdev_ieee802154_submac_init(&at86rf2xx_netdev[i]);
        /* set the application-provided callback */
        at86rf2xx_netdev[i].dev.netdev.event_callback = cb;

        /* initialize the device driver */
        int res = at86rf2xx_netdev[i].dev.netdev.driver->init(&at86rf2xx_netdev[i].dev.netdev);
        if (res != 0) {
            return -1;
        }
    }

    return 0;
}

int main(void)
{
    puts("Test application for AT86RF2XX IEEE 802.15.4 device driver");

    int res = netdev_ieee802154_minimal_init();
    if (res) {
        puts("Error initializing devices");
        return 1;
    }

    /* start the shell */
    puts("Initialization successful - starting the shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
#if AT86RF2XX_RANDOM_NUMBER_GENERATOR
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
#else
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
#endif

    return 0;
}
