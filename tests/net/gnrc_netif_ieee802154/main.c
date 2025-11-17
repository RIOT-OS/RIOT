/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "net/gnrc.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/pktdump.h"

#if IS_USED(MODULE_SHELL)
#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "shell.h"
#endif

int main(void)
{
#if IS_USED(MODULE_SOCKET_ZEP)
    /* This is a test for native with socket_zep */
    char addr_str[GNRC_NETIF_L2ADDR_MAXLEN * 3];
    gnrc_netif_t *netif = gnrc_netif_iter(NULL);

    printf("l2_addr: %s\n", gnrc_netif_addr_to_str(netif->l2addr,
                                                   netif->l2addr_len,
                                                   addr_str));
#endif
    gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(
            GNRC_NETREG_DEMUX_CTX_ALL,
            gnrc_pktdump_pid
        );
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);

#if IS_USED(MODULE_SHELL)
    /* this is manual test for real MCU using shell module */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
#endif
    return 0;
}

/** @} */
