/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdio.h>

#include "net/gnrc/sixlowpan/frag/stats.h"
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS
#include "net/gnrc/sixlowpan/frag/sfr.h"
#endif

int _gnrc_6lo_frag_stats(int argc, char **argv)
{
    gnrc_sixlowpan_frag_stats_t *stats = gnrc_sixlowpan_frag_stats_get();

    (void)argc;
    (void)argv;
    printf("rbuf full: %u\n", stats->rbuf_full);
    printf("frag full: %u\n", stats->frag_full);
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_VRB
    printf("VRB full: %u\n", stats->vrb_full);
#endif
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS
    gnrc_sixlowpan_frag_sfr_stats_t sfr;

    gnrc_sixlowpan_frag_sfr_stats_get(&sfr);
    printf("DG resends: %lu\n", (long unsigned)sfr.datagram_resends);
    printf("frags sent: usual: %lu, aborts: %lu, forwarded: %lu\n",
           (long unsigned)sfr.fragments_sent.usual,
           (long unsigned)sfr.fragments_sent.aborts,
           (long unsigned)sfr.fragments_sent.forwarded);
    printf("frag resends: NACK: %lu, timeout: %lu\n",
           (long unsigned)sfr.fragment_resends.by_nack,
           (long unsigned)sfr.fragment_resends.by_timeout);
    printf("ACKs: full: %lu, partly: %lu, aborts: %lu, forwarded: %lu\n",
           (long unsigned)sfr.acks.full,
           (long unsigned)sfr.acks.partly,
           (long unsigned)sfr.acks.aborts,
           (long unsigned)sfr.acks.forwarded);
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS */
    printf("frags complete: %u\n", stats->fragments);
    printf("dgs complete: %u\n", stats->datagrams);
    return 0;
}

/** @} */
