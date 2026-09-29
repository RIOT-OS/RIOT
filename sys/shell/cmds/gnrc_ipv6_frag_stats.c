/*
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdio.h>

#include "net/gnrc/ipv6/ext/frag.h"
#include "shell.h"

static int _gnrc_ipv6_frag_stats(int argc, char **argv)
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

SHELL_COMMAND(ip6_frag, "IPv6 fragmentation statistics", _gnrc_ipv6_frag_stats);

/** @} */
