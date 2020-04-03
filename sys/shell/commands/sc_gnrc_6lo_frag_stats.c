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
    printf("frags complete: %u\n", stats->fragments);
    printf("dgs complete: %u\n", stats->datagrams);
    return 0;
}

/** @} */
