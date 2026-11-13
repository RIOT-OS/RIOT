/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell command for sending raw text on network
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "net/gnrc/netif/hdr.h"
#include "shell.h"
#include "container.h"

static int _gnrc_netif_send(int argc, char **argv)
{
    netif_t *iface;
    uint8_t addr[GNRC_NETIF_L2ADDR_MAXLEN];
    size_t addr_len;
    gnrc_pktsnip_t *pkt;
    gnrc_pktsnip_t *hdr;
    gnrc_netif_hdr_t *nethdr;
    uint8_t flags = 0x00;
    bool indirect = false;
    int argpos = 1;

    if ((argc > 1) && (strcmp(argv[1], "--indirect") == 0)) {
        indirect = true;
        argpos++;
    }

    if ((argc - argpos) < 3) {
        printf("usage: %s [--indirect] <if> [<L2 addr>|bcast] <data>\n", argv[0]);
        return 1;
    }

    iface = netif_get_by_name(argv[argpos++]);
    if (!iface) {
        printf("error: invalid interface given\n");
        return 1;
    }

    /* parse address */
    addr_len = gnrc_netif_addr_from_str(argv[argpos], addr);

    if (addr_len == 0) {
        if (strcmp(argv[argpos], "bcast") == 0) {
            flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
        }
        else {
            printf("error: invalid address given\n");
            return 1;
        }
    }
    argpos++;

    /* put packet together */
    pkt = gnrc_pktbuf_add(NULL, argv[argpos], strlen(argv[argpos]), GNRC_NETTYPE_UNDEF);
    if (pkt == NULL) {
        printf("error: packet buffer full\n");
        return 1;
    }
    hdr = gnrc_netif_hdr_build(NULL, 0, addr, addr_len);
    if (hdr == NULL) {
        printf("error: packet buffer full\n");
        gnrc_pktbuf_release(pkt);
        return 1;
    }
    pkt = gnrc_pkt_prepend(pkt, hdr);
    nethdr = (gnrc_netif_hdr_t *)hdr->data;
    nethdr->flags = flags;
    if (indirect) {
        nethdr->flags |= GNRC_NETIF_HDR_FLAGS_TX_INDIRECT;
    }
    /* and send it */
    if (gnrc_netif_send(container_of(iface, gnrc_netif_t, netif), pkt) < 1) {
        printf("error: unable to send\n");
        gnrc_pktbuf_release(pkt);
        return 1;
    }

    return 0;
}

SHELL_COMMAND(txtsnd, "Sends a custom string as is over the link layer", _gnrc_netif_send);
