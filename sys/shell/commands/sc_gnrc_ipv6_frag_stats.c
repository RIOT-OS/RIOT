/*
 * Copyright (C) 2020 Freie Universität Berlin
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
#include "net/gnrc/ipv6/ext/frag.h"

int _gnrc_ipv6_frag_stats(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (IS_USED(MODULE_GNRC_IPV6_EXT_FRAG_STATS)) {
        gnrc_ipv6_ext_frag_stats_t *stats = gnrc_ipv6_ext_frag_stats();

        printf("rbuf full: %u\n", stats->rbuf_full);
        printf("frag full: %u\n", stats->frag_full);
        printf("frags complete: %u\n", stats->fragments);
        printf("dgs complete: %u\n", stats->datagrams);
    }
    return 0;
}

/** @} */
