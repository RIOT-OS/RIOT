/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
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
 * @brief       Test application for at86rf215 driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <stdio.h>

#include "at86rf215.h"
#include "at86rf215_internal.h"
#include "thread.h"
#include "shell.h"
#include "shell_commands.h"
#include "sys/bus.h"

#include "net/gnrc/pktdump.h"
#include "net/gnrc/netif.h"
#include "net/gnrc.h"
#include "od.h"

static char batmon_stack[THREAD_STACKSIZE_MAIN];
static at86rf215_t *dev;

void netdev_register_signal(netdev_t *netdev, netdev_type_t type, uint8_t index)
{
   (void) index;
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    if (type == NETDEV_AT86RF215 && !dev) {
        dev = container_of(netdev_ieee802154, at86rf215_t, netdev);
    }
}

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
    gnrc_netif_t* netif = gnrc_netif_iter(NULL);

    if (argc < 2) {
        printf("usage: %s <treshold_mV>\n", argv[0]);
        return -1;
    }

    if (netif == NULL) {
        puts("no netif found");
        return -1;
    }

    voltage = atoi(argv[1]);
    res     = gnrc_netapi_set(netif->pid, NETOPT_BATMON, 0,
                              &voltage, sizeof(voltage));

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

    if (trim > 0xF) {
        puts("Trim value out of range");
        return 1;
    }

    if (dev == NULL) {
        puts("No at86rf215 radio found");
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

    if (dev == NULL) {
        puts("No at86rf215 radio found");
        return 1;
    }

    printf("Clock output set to %s %s\n", keys[freq], freq ? "MHz" : "");
    at86rf215_set_clock_output(dev, AT86RF215_CLKO_4mA, freq);

    return 0;
}

static int cmd_get_random(int argc, char **argv)
{
    uint8_t values;
    uint8_t buffer[256];

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

    if (dev == NULL) {
        puts("No at86rf215 radio found");
        return 1;
    }

    at86rf215_get_random(dev, buffer, values);

    od_hex_dump(buffer, values, 0);

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
    /* enable pktdump output */
    gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                          gnrc_pktdump_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);

    /* create battery monitor thread */
    thread_create(batmon_stack, sizeof(batmon_stack), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, batmon_thread, NULL, "batmon");

    /* start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
