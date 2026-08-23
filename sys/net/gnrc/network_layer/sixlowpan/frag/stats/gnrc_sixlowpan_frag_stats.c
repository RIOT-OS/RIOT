/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "net/gnrc/sixlowpan/frag/stats.h"

static gnrc_sixlowpan_frag_stats_t _stats;

gnrc_sixlowpan_frag_stats_t *gnrc_sixlowpan_frag_stats_get(void)
{
    return &_stats;
}

/** @} */
