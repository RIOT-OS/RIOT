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

#include "net/gnrc/sixlowpan/frag/stats.h"

static gnrc_sixlowpan_frag_stats_t _stats;

gnrc_sixlowpan_frag_stats_t *gnrc_sixlowpan_frag_stats_get(void)
{
    return &_stats;
}

/** @} */
