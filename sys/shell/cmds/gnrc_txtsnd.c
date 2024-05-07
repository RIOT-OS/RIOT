/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#include "net/gnrc.h"
#include "net/gnrc.h"
#include "net/gnrc/netif/hdr.h"
#include "net/ipv6/addr.h"
#include "shell.h"
#include "container.h"

static int _gnrc_netif_send(int argc, char **argv)
{
    netif_t *iface;
    uint8_t addr[GNRC_NETIF_L2ADDR_MAXLEN];
    size_t addr_len;
    gnrc_pktsnip_t *pkt, *hdr;
    gnrc_netif_hdr_t *nethdr;
    uint8_t flags = 0x00;

    if (argc < 4) {
        printf("usage: %s <if> [<L2 addr>|bcast] <data>\n", argv[0]);
        return 1;
    }

    iface = netif_get_by_name(argv[1]);
    if (!iface) {
        printf("error: invalid interface given\n");
        return 1;
    }

    /* parse address */
    addr_len = gnrc_netif_addr_from_str(argv[2], addr);

    if (addr_len == 0) {
        if (strcmp(argv[2], "bcast") == 0) {
            flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
        }
        else {
            printf("error: invalid address given\n");
            return 1;
        }
    }

    /* put packet together */
    pkt = gnrc_pktbuf_add(NULL, argv[3], strlen(argv[3]), GNRC_NETTYPE_UNDEF);
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
    /* and send it */
    if (gnrc_netif_send(container_of(iface, gnrc_netif_t, netif), pkt) < 1) {
        printf("error: unable to send\n");
        gnrc_pktbuf_release(pkt);
        return 1;
    }

    return 0;
}

SHELL_COMMAND(txtsnd, "Sends a custom string as is over the link layer", _gnrc_netif_send);
