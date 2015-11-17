/*
 * Copyright (C) 2015 Matt Poppe<matt@poppe.me>
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
 * @brief       Short test for the KSZ8851SNL Driver on Samr21-Xpro
 *
 * @author      Matt Poppe<matt@poppe.me>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "shell.h"
#include "shell_commands.h"

#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"

#define MSG_QUEUE_SIZE 1
msg_t msgq[MSG_QUEUE_SIZE];

int main(void)
{


//    gnrc_netreg_entry_t dump;

//    dump.pid = gnrc_pktdump_getpid();
//    dump.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
//    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);

    msg_init_queue(msgq, MSG_QUEUE_SIZE);

    (void) puts("RIOT KSZ8851SNL Driver Test!");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
