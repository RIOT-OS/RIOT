/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Test application for the NRF51822 minimal radio driver (nrfmin)
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "nrfmin.h"
#include "net/gnrc.h"
#include "net/gnrc/nomac.h"
#include "net/gnrc/pktdump.h"

static char nomac_stack[THREAD_STACKSIZE_DEFAULT];

int main(void)
{
    gnrc_netdev_t dev;
    gnrc_netreg_entry_t netobj;

    puts("\nManual test for the minimal NRF51822 radio driver\n");
    puts("Use the 'ifconfig' and 'txtsnd' shell commands to verify the driver");

    /* initialize network device */
    nrfmin_init(&dev);
    gnrc_nomac_init(nomac_stack, sizeof(nomac_stack), 5, "nomac", &dev);

    /* initialize packet dumper */
    netobj.pid = gnrc_pktdump_pid;
    netobj.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &netobj);

    /* initialize and run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
