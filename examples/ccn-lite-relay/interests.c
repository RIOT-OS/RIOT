/*
 * Copyright (C) 2013-15, Christian Tschudin, University of Basel
 * Copyright (C) 2015, Oliver Hahm, Inria
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * File history:
 * 2015-11-09  created (based on ccn-lite-peek.c)
 */


#include <unistd.h>
#include "random.h"
#include "xtimer.h"
#include "timex.h"
#include "arpa/inet.h"
#include "net/packet.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/netapi.h"

#include "ccn-lite-riot.h"
#include "ccnl-core.h"
#include "ccnl-headers.h"
#include "ccnl-pkt-ndntlv.h"
#include "ccnl-defs.h"
#include "ccnl-ext.h"
#include "ccnl-pkt-ccntlv.h"

/**
 * Maximum number of Interest retransmissions
 */
#define CCNL_INTEREST_RETRIES   (3)

#define MAX_ADDR_LEN            (8U)

#define BUF_SIZE    (128)
static unsigned char _int_buf[BUF_SIZE];
static unsigned char _cont_buf[BUF_SIZE];

static void _usage(char *arg)
{
    printf("usage: %s <URI> [relay]\n"
            "%% %s /riot/peter/schmerzl             (classic lookup)\n",
            arg, arg);
}

int _ccnl_interest(int argc, char **argv)
{
    if (argc < 2) {
        _usage(argv[0]);
        return -1;
    }

    /* XXX: https://xkcd.com/221/ */
    genrand_init(0x4);

    /* initialize address with 0xFF for broadcast */
    size_t addr_len = MAX_ADDR_LEN;
    uint8_t relay_addr[MAX_ADDR_LEN];
    memset(relay_addr, UINT8_MAX, MAX_ADDR_LEN);
    if (argc > 2) {
        addr_len = gnrc_netif_addr_from_str(relay_addr, sizeof(relay_addr), argv[2]);
    }

    for (int cnt = 0; cnt < CCNL_INTEREST_RETRIES; cnt++) {
        ccnl_send_interest(CCNL_SUITE_NDNTLV, argv[1], relay_addr, addr_len, NULL, _int_buf, BUF_SIZE);
        ccnl_wait_for_chunk(_cont_buf, BUF_SIZE);
    }

    return 0;
}
