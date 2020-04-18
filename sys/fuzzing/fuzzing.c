/*
 * Copyright (C) 2019 Sören Tempel <tempel@uni-bremen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include "assert.h"
#include "fuzzing.h"

#include "net/ipv6/addr.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/pkt.h"

extern int fuzzing_netdev(gnrc_netif_t *);
extern void fuzzing_netdev_wait(void);

/* used by gnrc_pktbuf_malloc to exit on free */
gnrc_pktsnip_t *gnrc_pktbuf_fuzzptr = NULL;

/* buffer sizes for reading from an fd */
#define FUZZING_BSIZE 1024
#define FUZZING_BSTEP 128

int
fuzzing_init(ipv6_addr_t *addr, unsigned pfx_len)
{
    static gnrc_netif_t netif;

    if (fuzzing_netdev(&netif)) {
        return -ENXIO;
    }

    fuzzing_netdev_wait();
    if (addr != NULL) {
        if (gnrc_netif_ipv6_addr_add(&netif, addr, pfx_len, 0) != sizeof(ipv6_addr_t)) {
            return -EINVAL;
        }
    }

    return 0;
}

int
fuzzing_read_packet(int fd, gnrc_pktsnip_t *pkt)
{
    ssize_t r;
    size_t csiz, rsiz;

    /* can only be called once currently */
    assert(gnrc_pktbuf_fuzzptr == NULL);

    csiz = 0;
    rsiz = FUZZING_BSIZE;
    if (gnrc_pktbuf_realloc_data(pkt, rsiz)) {
        return -ENOMEM;
    }

    while ((r = read(fd, &((char *)pkt->data)[csiz], rsiz)) > 0) {
        assert((size_t)r <= rsiz);

        csiz += r;
        rsiz -= r;

        if (rsiz == 0) {
             if (gnrc_pktbuf_realloc_data(pkt, csiz + FUZZING_BSTEP)) {
                 return -ENOMEM;
             }
             rsiz += FUZZING_BSTEP;
        }
    }
    if (r == -1) {
        return -errno;
    }

    /* shrink packet to actual size */
    if (gnrc_pktbuf_realloc_data(pkt, csiz)) {
        return -ENOMEM;
    }

    gnrc_pktbuf_fuzzptr = pkt;
    return 0;
}
