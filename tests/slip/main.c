/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
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
 * @brief       Test application for gnrc_tapnet network device driver
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "shell_commands.h"
#include "net/gnrc.h"
#include "net/gnrc/pktdump.h"

/**
 * @brief   Maybe you are a golfer?!
 */
int main(void)
{
    gnrc_netreg_entry_t dump;

    puts("SLIP test");

    /* initialize and register pktdump */
    dump.pid = gnrc_pktdump_getpid();
    dump.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;

    if (dump.pid <= KERNEL_PID_UNDEF) {
        puts("Error starting pktdump thread");
        return -1;
    }

    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);

    /* start the shell */
    puts("Initialization OK, starting shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
