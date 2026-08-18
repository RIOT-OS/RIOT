/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for AT86RF215 IEEE 802.15.4 device driver
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "event.h"
#include "event/thread.h"
#include "thread.h"
#include "init_dev.h"
#include "sys/bus.h"
#include "od.h"

#include "at86rf215.h"
#include "at86rf215_internal.h"
#include "at86rf215_params.h"

#include "net/netdev/ieee802154_submac.h"

#include "test_utils/netdev_ieee802154_minimal.h"

static at86rf215_t at86rf215_devs[NETDEV_IEEE802154_MINIMAL_NUMOF];
static at86rf215_bhp_ev_t at86rf215_bhp[NETDEV_IEEE802154_MINIMAL_NUMOF];
static netdev_ieee802154_submac_t at86rf215_netdev[NETDEV_IEEE802154_MINIMAL_NUMOF];
static char batmon_stack[THREAD_STACKSIZE_MAIN];

void *batmon_thread(void *arg)
{
    (void) arg;

    msg_t msg;
    msg_bus_entry_t sub;
    msg_bus_t *bus = sys_bus_get(SYS_BUS_POWER);

    msg_bus_attach(bus, &sub);
    msg_bus_subscribe(&sub, SYS_BUS_POWER_EVENT_LOW_VOLTAGE);

    while (1) {
        msg_receive(&msg);
        puts("NA NA NA NA NA NA NA NA NA NA NA NA NA BATMON");
    }
}

static int cmd_enable_batmon(int argc, char **argv)
{
    int res;
    uint16_t voltage;
    netdev_t *netdev = &(at86rf215_netdev[0].dev.netdev);

    if (argc < 2) {
        printf("usage: %s <treshold_mV>\n", argv[0]);
        return -1;
    }

    voltage = atoi(argv[1]);
    res = netdev->driver->set(netdev, NETOPT_BATMON, &voltage, sizeof(voltage));

    if (res != sizeof(voltage)) {
        puts("value out of range");
    }

    return res;
}

static int cmd_set_trim(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <trim>\n", argv[0]);
        return 1;
    }

    uint8_t trim = atoi(argv[1]);
    at86rf215_t *dev = &at86rf215_devs[0];

    if (trim > 0xF) {
        puts("Trim value out of range");
        return 1;
    }

    printf("setting trim to %u fF\n", 300U * trim);
    at86rf215_set_trim(dev, trim);

    return 0;
}

static int cmd_set_clock_out(int argc, char **argv)
{
    const char *keys[] = {
        [AT86RF215_CLKO_OFF]    = "off",
        [AT86RF215_CLKO_26_MHz] = "26",
        [AT86RF215_CLKO_32_MHz] = "32",
        [AT86RF215_CLKO_16_MHz] = "16",
        [AT86RF215_CLKO_8_MHz]  = "8",
        [AT86RF215_CLKO_4_MHz]  = "4",
        [AT86RF215_CLKO_2_MHz]  = "2",
        [AT86RF215_CLKO_1_MHz]  = "1",
    };

    at86rf215_clko_freq_t freq = AT86RF215_CLKO_26_MHz;
    at86rf215_t *dev = &at86rf215_devs[0];

    if (argc > 1) {
        unsigned tmp = 0xFF;
        for (unsigned i = 0; i < ARRAY_SIZE(keys); ++i) {
            if (strcmp(argv[1], keys[i]) == 0) {
                tmp = i;
                break;
            }
        }

        if (tmp == 0xFF) {
            printf("usage: %s [freq in MHz | off]\n", argv[0]);
            printf("valid frequencies: off");
            for (unsigned i = 1; i < ARRAY_SIZE(keys); ++i) {
                printf(", %s MHz", keys[i]);
            }
            puts("");
            return 1;
        }

        freq = tmp;
    }

    printf("Clock output set to %s %s\n", keys[freq], freq ? "MHz" : "");
    at86rf215_set_clock_output(dev, AT86RF215_CLKO_4mA, freq);

    return 0;
}

static int cmd_get_random(int argc, char **argv)
{
    uint8_t values;
    uint8_t buffer[256];
    at86rf215_t *dev = at86rf215_bhp[0].hal_24->priv;

    if (argc > 1) {
        values = atoi(argv[1]);
    }
    else {
        values = 16;
    }

    if (values == 0) {
        printf("usage: %s [num]\n", argv[0]);
        return 1;
    }

    at86rf215_get_random(dev, buffer, values);

    od_hex_dump(buffer, values, 0);

    return 0;
}

int test_init(void)
{
    /* create battery monitor thread */
    thread_create(batmon_stack, sizeof(batmon_stack), THREAD_PRIORITY_MAIN - 1,
                  0, batmon_thread, NULL, "batmon");
    return 0;
}

static int _init_driver(netdev_t *netdev, netdev_event_cb_t cb)
{
    /* set the application-provided callback */
    netdev->event_callback = cb;

    /* initialize the device driver */
    return netdev->driver->init(netdev);
}

int netdev_ieee802154_minimal_init_devs(netdev_event_cb_t cb) {
    unsigned idx = 0;

    puts("Initializing AT86RF215 at86rf215");

    for (unsigned i = 0; i < AT86RF215_NUM; i++) {
        at86rf215_t *dev_09 = NULL;
        at86rf215_t *dev_24 = NULL;
        ieee802154_dev_t *hal_09 = NULL;
        ieee802154_dev_t *hal_24 = NULL;
        netdev_ieee802154_submac_t *netdev_09 = NULL;
        netdev_ieee802154_submac_t *netdev_24 = NULL;

        printf("%d out of %u\n", i + 1, (unsigned)AT86RF215_NUM);

        if (IS_USED(MODULE_AT86RF215_SUBGHZ)) {
            puts("Sub-GHz");
            dev_09    = &at86rf215_devs[idx];
            hal_09    = &at86rf215_netdev[idx].submac.dev;
            netdev_09 = &at86rf215_netdev[idx];
            idx++;
        }

        if (IS_USED(MODULE_AT86RF215_24GHZ)) {
            puts("2.4 GHz");
            dev_24    = &at86rf215_devs[idx];
            hal_24    = &at86rf215_netdev[idx].submac.dev;
            netdev_24 = &at86rf215_netdev[idx];
            idx++;
        }

        /* setup the specific driver */
        at86rf215_init_event(&at86rf215_bhp[i], hal_09, hal_24, EVENT_PRIO_HIGHEST);
        at86rf215_init(dev_09, dev_24, hal_09, hal_24, &at86rf215_params[i], &at86rf215_bhp[i]);

        int index = 2 * i;
        if (IS_USED(MODULE_AT86RF215_SUBGHZ)) {
            netdev_register(&netdev_09->dev.netdev, NETDEV_AT86RF215, index);
            netdev_ieee802154_submac_init(netdev_09);
            ++index;
        }

        if (IS_USED(MODULE_AT86RF215_24GHZ)) {
            netdev_register(&netdev_24->dev.netdev, NETDEV_AT86RF215, index);
            netdev_ieee802154_submac_init(netdev_24);
        }

        int res;
        if (dev_09) {
            res = _init_driver(&netdev_09->dev.netdev, cb);
            if (res) {
                return res;
            }
        }

        if (dev_24) {
            res = _init_driver(&netdev_24->dev.netdev, cb);
            if (res) {
                return res;
            }
        }
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "batmon", "Enable the battery monitor", cmd_enable_batmon },
    { "set_trim", "at86rf215: Set the trim value of the crystal oscillator", cmd_set_trim },
    { "set_clko", "at86rf215: Configure the Clock Output pin", cmd_set_clock_out },
    { "get_random", "at86rf215: Get random values from the radio", cmd_get_random },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Test application for AT86RF215 IEEE 802.15.4 device driver");

    int res = netdev_ieee802154_minimal_init();
    if (res) {
        puts("Error initializing at86rf215");
        return 1;
    }

    /* create battery monitor thread */
    thread_create(batmon_stack, sizeof(batmon_stack), THREAD_PRIORITY_MAIN - 1,
                  0, batmon_thread, NULL, "batmon");

    /* start the shell */
    puts("Initialization successful - starting the shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
