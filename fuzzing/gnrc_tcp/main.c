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

#include "net/af.h"
#include "net/gnrc/tcp.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/pkt.h"

static uint32_t demux = GNRC_NETREG_DEMUX_CTX_ALL;
static gnrc_nettype_t ntype = GNRC_NETTYPE_TCP;

static void *tcploop(void *arg)
{
    mutex_t *tcpmtx = arg;
    gnrc_tcp_tcb_t tcb;
    gnrc_tcp_ep_t ep;

    if (gnrc_tcp_ep_from_str(&ep, "[" SERVER_ADDR "]")) {
        errx(EXIT_FAILURE, "gnrc_tcp_ep_from_str failed");
    }
    ep.port = SERVER_PORT;

    for (;;) {
        gnrc_tcp_tcb_init(&tcb);
        mutex_unlock(tcpmtx);

        int ret = gnrc_tcp_open_passive(&tcb, &ep);
        if (!ret) {
            errx(EXIT_FAILURE, "gnrc_tcp_open_passive failed: %d\n", ret);
        }
    }

    return NULL;
}

static void inittcp(void)
{
    static char tcpthr[THREAD_STACKSIZE_DEFAULT];
    static mutex_t tcpmtx = MUTEX_INIT_LOCKED;
    kernel_pid_t pid;

    pid = thread_create(tcpthr, sizeof(tcpthr), THREAD_PRIORITY_MAIN,
                        0, tcploop, &tcpmtx, "gnrc_tcp fuzzing");
    if (pid < 0) {
        errx(EXIT_FAILURE, "thread_create failed: %d\n", pid);
    }

    mutex_lock(&tcpmtx); /* wait until tcp is initialized */
}

void initialize(ipv6_addr_t *addr)
{
    if (ipv6_addr_from_str(addr, SERVER_ADDR) == NULL) {
        errx(EXIT_FAILURE, "ipv6_addr_from_str failed");
    }
    if (fuzzing_init(addr, SERVER_ADDR_PREFIX)) {
        errx(EXIT_FAILURE, "fuzzing_init failed");
    }

    inittcp();
}

int main(void)
{
    ipv6_addr_t myaddr;
    gnrc_pktsnip_t *ipkt, *tpkt;

    initialize(&myaddr);
    if (!(ipkt = gnrc_ipv6_hdr_build(NULL, NULL, &myaddr))) {
        errx(EXIT_FAILURE, "gnrc_ipv6_hdr_build failed");
    }
    if (!(tpkt = gnrc_pktbuf_add(ipkt, NULL, 0, GNRC_NETTYPE_TCP))) {
        errx(EXIT_FAILURE, "gnrc_pktbuf_add failed");
    }

    if (fuzzing_read_packet(STDIN_FILENO, tpkt)) {
        errx(EXIT_FAILURE, "fuzzing_read_packet failed");
    }
    if (!gnrc_netapi_dispatch_receive(ntype, demux, tpkt)) {
        errx(EXIT_FAILURE, "couldn't find any subscriber");
    }

    return EXIT_SUCCESS;
}
