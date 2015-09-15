/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     tests
 * @file
 * @brief       Test application for KW2x network device driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "shell_commands.h"
#include "net/gnrc.h"
#include "net/gnrc/pktdump.h"

int main(void)
{
    gnrc_netreg_entry_t dump;

    puts("KW2XRF device driver test");

    /* register the pktdump thread */
    puts("Register the packet dump thread for GNRC_NETTYPE_UNDEF packets");
    dump.pid = gnrc_pktdump_getpid();
    dump.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);

    /* start the shell */
    puts("Initialization successful - starting the shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
