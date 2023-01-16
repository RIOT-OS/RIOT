/*
 * Copyright (C) 2019 Sören Tempel <tempel@uni-bremen.de>
 * Copyright (C) 2022 Bennet Blischke <bennet.blischke@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "assert.h"
#include "fuzzing.h"

#include "net/ipv6/addr.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/pkt.h"

extern int fuzzing_netdev(gnrc_netif_t *);
extern void fuzzing_netdev_wait(void);

/* buffer sizes for reading from an fd */
#define FUZZING_BSIZE 1024
#define FUZZING_BSTEP 128

/* used by gnrc_pktbuf_malloc to exit on free */
gnrc_pktsnip_t *gnrc_pktbuf_fuzzptr = NULL;

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
    size_t rsiz;

    /* can only be called once currently */
    assert(gnrc_pktbuf_fuzzptr == NULL);

    uint8_t *input = fuzzing_read_bytes(fd, &rsiz);
    if (input == NULL) {
        return -errno;
    }

    if (gnrc_pktbuf_realloc_data(pkt, rsiz)) {
        return -ENOMEM;
    }

    memcpy(pkt->data, input, rsiz);

    gnrc_pktbuf_fuzzptr = pkt;
    return 0;
}

uint8_t *
fuzzing_read_bytes(int fd, size_t *size)
{
    uint8_t *buffer = NULL;
    ssize_t r;
    size_t csiz, rsiz;

    csiz = 0;
    rsiz = FUZZING_BSIZE;
    if ((buffer = realloc(buffer, rsiz)) == NULL) {
        return NULL;
    }

    while ((r = read(fd, &(buffer[csiz]), rsiz)) > 0) {
        assert((size_t)r <= rsiz);

        csiz += r;
        rsiz -= r;

        if (rsiz == 0) {
            if ((buffer = realloc(buffer, csiz + FUZZING_BSTEP)) == NULL) {
                return NULL;
            }
            rsiz += FUZZING_BSTEP;
        }
    }
    if (r == -1) {
        return NULL;
    }

    /* shrink buffer to actual size */
    if ((buffer = realloc(buffer, csiz)) == NULL) {
        return NULL;
    }

    *size = csiz;
    return buffer;
}
