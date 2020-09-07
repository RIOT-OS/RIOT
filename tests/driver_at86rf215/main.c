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
#include "thread.h"
#include "shell.h"
#include "shell_commands.h"
#include "sys/bus.h"

#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"

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

static const shell_command_t shell_commands[] = {
    { "batmon", "Enable the battery monitor", cmd_enable_batmon },
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
