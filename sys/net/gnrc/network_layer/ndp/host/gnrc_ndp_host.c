/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include <inttypes.h>
#include "random.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ndp.h"
#include "net/gnrc/ndp/internal.h"
#include "xtimer.h"

#include "net/gnrc/ndp/host.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static inline void _reschedule_rtr_sol(gnrc_ipv6_netif_t *iface, timex_t delay)
{
    xtimer_remove(&iface->rtr_sol_timer);
    iface->rtr_sol_msg.type = GNRC_NDP_MSG_RTR_SOL_RETRANS;
    iface->rtr_sol_msg.content.ptr = (char *) iface;
    xtimer_set_msg(&iface->rtr_sol_timer, (uint32_t) timex_uint64(delay), &iface->rtr_sol_msg,
                   gnrc_ipv6_pid);
}

void gnrc_ndp_host_init(gnrc_ipv6_netif_t *iface)
{
    uint32_t interval = genrand_uint32_range(0, GNRC_NDP_MAX_RTR_SOL_DELAY * SEC_IN_USEC);
    mutex_lock(&iface->mutex);
    iface->rtr_sol_count = GNRC_NDP_MAX_RTR_SOL_NUMOF;
    DEBUG("ndp host: delayed initial router solicitation by %" PRIu32 " usec.\n", interval);
    _reschedule_rtr_sol(iface, timex_set(0, interval));
    mutex_unlock(&iface->mutex);
}

void gnrc_ndp_host_retrans_rtr_sol(gnrc_ipv6_netif_t *iface)
{
    mutex_lock(&iface->mutex);
    if (iface->rtr_sol_count > 1) { /* regard off-by-one error */
        DEBUG("ndp hst: retransmit rtr sol in %d sec\n", GNRC_NDP_MAX_RTR_SOL_INT);
        iface->rtr_sol_count--;
        _reschedule_rtr_sol(iface, timex_set(GNRC_NDP_MAX_RTR_SOL_INT, 0));
    }
    mutex_unlock(&iface->mutex);
    gnrc_ndp_internal_send_rtr_sol(iface->pid, NULL);
}

/** @} */
