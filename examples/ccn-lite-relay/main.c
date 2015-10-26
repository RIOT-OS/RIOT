/*
 * Copyright (C) 2015 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Basic ccn-lite relay example (produce and consumer via shell)
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "msg.h"
#include "timex.h"
#include "shell_commands.h"
#include "net/packet.h"
#include "net/netopt.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netapi.h"
#include "ccn-lite-riot.h"
#include "ccnl-core.h"
#include "ccnl-headers.h"
#include "ccnl-pkt-ndntlv.h"
#include "ccnl-defs.h"
#include "net/gnrc/nettype.h"

/* main thread's message queue */
#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* check for one-time initialization */
static bool started = false;

/* shell command functions */
static int _ccnl_open(int argc, char **argv);
extern int _ccnl_content(int argc, char **argv);
extern int _ccnl_interest(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "open", "opens an interface or socket", _ccnl_open},
    { "interest", "sends an interest", _ccnl_interest},
    { "content", "create content and populated it", _ccnl_content},
    { NULL, NULL, NULL }
};

/* usage for open command */
static void _usage(void)
{
    puts("ccnl <interface>");
}

static int _ccnl_open(int argc, char **argv)
{
    /* check if already running */
    if (started) {
        puts("Already opened an interface for CCN!");
        return -1;
    }

    /* check if parameter is given */
    if (argc != 2) {
        _usage();
        return -1;
    }

    /* check if given number is a valid netif PID */
    int pid = atoi(argv[1]);
    if (!gnrc_netif_exist(pid)) {
        printf("%i is not a valid interface!\n", pid);
        return -1;
    }

    ccnl_start();

    /* set the relay's PID, configure the interface to interface to use CCN
     * nettype */
    if (ccnl_open_netif(pid, GNRC_NETTYPE_CCN) < 0) {
        puts("Error registering at network interface!");
        return -1;
    }

    started = true;

    return 0;
}

int main(void)
{
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Basic CCN-Lite example");

    ccnl_core_init();

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
