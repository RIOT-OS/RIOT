/*
 * Copyright (C) 2019 Sören Tempel <tempel@uni-bremen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <err.h>
#include <stdlib.h>

#include "thread.h"
#include "fuzzing.h"

#include "net/gcoap.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/pkt.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/ipv6/hdr.h"

static uint32_t demux = COAP_PORT;
static gnrc_nettype_t ntype = GNRC_NETTYPE_UDP;

void initialize(void)
{
    if (fuzzing_init(NULL, 0)) {
        errx(EXIT_FAILURE, "fuzzing_init failed");
    }

    gcoap_init();
}

int main(void)
{
    gnrc_pktsnip_t *ipkt, *upkt, *cpkt;

    initialize();
    if (!(ipkt = gnrc_ipv6_hdr_build(NULL, NULL, &ipv6_addr_loopback))) {
        errx(EXIT_FAILURE, "gnrc_ipv6_hdr_build failed");
    }
    if (!(upkt = gnrc_udp_hdr_build(ipkt, 2342, COAP_PORT))) {
        errx(EXIT_FAILURE, "gnrc_udp_hdr_build failed");
    }

    if (!(cpkt = gnrc_pktbuf_add(upkt, NULL, 0, GNRC_NETTYPE_UNDEF))) {
        errx(EXIT_FAILURE, "gnrc_pktbuf_add failed");
    }
    if (fuzzing_read_packet(STDIN_FILENO, cpkt)) {
        errx(EXIT_FAILURE, "fuzzing_read_packet failed");
    }

    if (!gnrc_netapi_dispatch_receive(ntype, demux, cpkt)) {
        errx(EXIT_FAILURE, "couldn't find any subscriber");
    }

    return EXIT_SUCCESS;
}
