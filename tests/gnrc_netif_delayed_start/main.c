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
 * @brief       Test application delayed startup of network interfaces.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <stdio.h>

#include "net/gnrc/netif.h"

#include "shell.h"
#include "msg.h"

#define MAIN_QUEUE_SIZE     (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static void _gnrc_netapi_set_all(netopt_state_t state)
{
    gnrc_netif_t* netif = NULL;
    while ((netif = gnrc_netif_iter(netif))) {
        /* retry if busy */
        while (gnrc_netapi_set(netif->pid, NETOPT_STATE, 0,
                               &state, sizeof(state)) == -EBUSY) {}
    }
}

static int _start_cmd(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    static bool init_called;

    if (init_called) {
        /* set all interfaces to IDLE */
        _gnrc_netapi_set_all(NETOPT_STATE_IDLE);
    } else {
        /* we must only call init once */
        gnrc_netif_init_devs();
        init_called = true;
    }

    puts("Interfaces should new be online.");

    return 0;
}

static int _shutdown_cmd(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    /* put all interfaces to SLEEP */
    _gnrc_netapi_set_all(NETOPT_STATE_SLEEP);

    puts("Interfaces should new be asleep.");

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "start", "Bring up network interfaces",  _start_cmd },
    { "shutdown", "shut down all network interfaces and the CPU",  _shutdown_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Test application for delayed startup & shutdown of network interfaces.");
    puts("\nAttach an ammeter to the supply line of your networking device.");
    puts(" - run the 'start' command to bring up your network interfaces.");
    puts("   Current draw should be on IDLE/RX level.");
    puts(" - run the 'shutdown' command to put your network interfaces to sleep.");
    puts("   Current draw should be at the lowest SLEEP level.");
    puts(" - run the 'reboot' command to confirm your interfaces remain sleeping across reboots.");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
