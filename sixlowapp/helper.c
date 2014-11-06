/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 * @brief       6LoWPAN example application helper functions
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include "msg.h"
#include "sixlowpan/ip.h"

#include "sixlowapp.h"

#define ENABLE_DEBUG            (0)
#include "debug.h"

void sixlowapp_ndp_workaround(ipv6_addr_t *dest)
{
    /* add the destination to the neighbor cache if is not already in it */
    if (!ndp_neighbor_cache_search(dest)) {
        DEBUGF("XXX: Adding %s to neighbor cache.\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, dest));
        ndp_neighbor_cache_add(IF_ID, dest, &(dest->uint16[7]), 2, 0,
                               NDP_NCE_STATUS_REACHABLE,
                               NDP_NCE_TYPE_TENTATIVE, 0xffff);
    }
}

uint64_t sixlowapp_wait_for_msg_type(msg_t *m, timex_t timeout, uint16_t mtype)
{
    timex_t t1, t2, delta;
    delta = timex_set(0, 0);
    vtimer_now(&t1);
    while (timex_cmp(delta, timeout) < 0) {
        if (vtimer_msg_receive_timeout(m, timeout) < 0) {
            return 0;
        }
        vtimer_now(&t2);
        delta = timex_sub(t2, t1);
        if (m->type == mtype) {
            return timex_uint64(delta);
        }
        timeout = timex_sub(timeout, delta);
    }
    return 0;
}
