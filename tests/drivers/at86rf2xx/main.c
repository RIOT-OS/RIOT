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
#include "init_dev.h"
#include "net/ieee802154.h"
#include "net/netdev/ieee802154.h"
#include "shell.h"
#include "test_utils/netdev_ieee802154_minimal.h"
#include "test_utils/expect.h"

static at86rf2xx_t at86rf2xx[AT86RF2XX_NUM];

#if AT86RF2XX_RANDOM_NUMBER_GENERATOR
void random_net_api(uint8_t idx, uint32_t *value)
{
    netdev_ieee802154_t *dev = &at86rf2xx[idx].netdev;
    dev->netdev.driver->get(&dev->netdev, NETOPT_RANDOM, value, sizeof(uint32_t));
    int retval = dev->netdev.driver->get(&dev->netdev, NETOPT_RANDOM,
                                         value, sizeof(uint32_t));
    if (retval < 0) {
        printf("get(NETOPT_RANDOM) failed: %s\n", strerror(-retval));
    }
    else {
        expect(retval == sizeof(*value));
    }
}

int random_by_at86rf2xx(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    for (unsigned int i = 0; i < AT86RF2XX_NUM; i++) {
        uint32_t test = 0;
        at86rf2xx_get_random(&at86rf2xx[i], (uint8_t *)&test, sizeof(test));
        printf("Random number for device %u via native API: %" PRIx32 "\n", i, test);
        test = 0;
        random_net_api(i, &test);
        printf("Random number for device %u via netopt: %" PRIx32 "\n", i, test);
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
        at86rf2xx_setup(&at86rf2xx[i], &at86rf2xx_params[i], i);

        /* set the application-provided callback */
        at86rf2xx[i].netdev.netdev.event_callback = cb;

        /* initialize the device driver */
        int res = at86rf2xx[i].netdev.netdev.driver->init(&at86rf2xx[i].netdev.netdev);
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
