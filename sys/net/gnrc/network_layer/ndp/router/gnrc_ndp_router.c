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

#include "net/gnrc/ipv6.h"
#include "net/gnrc/ndp.h"
#include "net/gnrc/ndp/internal.h"
#include "random.h"
#include "xtimer.h"

#include "net/gnrc/ndp/router.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void _send_rtr_adv(gnrc_ipv6_netif_t *iface, ipv6_addr_t *dst);

void gnrc_ndp_router_set_router(gnrc_ipv6_netif_t *iface, bool enable)
{
    if (enable && !(iface->flags & GNRC_IPV6_NETIF_FLAGS_ROUTER)) {
        gnrc_ipv6_netif_add_addr(iface->pid, &ipv6_addr_all_routers_link_local,
                                 128, GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST);
        mutex_lock(&iface->mutex);
        iface->flags |= GNRC_IPV6_NETIF_FLAGS_ROUTER;
        iface->max_adv_int = GNRC_IPV6_NETIF_DEFAULT_MAX_ADV_INT;
        iface->min_adv_int = GNRC_IPV6_NETIF_DEFAULT_MIN_ADV_INT;
        iface->adv_ltime = GNRC_IPV6_NETIF_DEFAULT_ROUTER_LTIME;
        mutex_unlock(&iface->mutex);
        gnrc_ndp_router_set_rtr_adv(iface, enable);
    }
    else if (!enable && (iface->flags & GNRC_IPV6_NETIF_FLAGS_ROUTER)) {
        gnrc_ipv6_netif_remove_addr(iface->pid, (ipv6_addr_t *)&ipv6_addr_all_routers_link_local);
        gnrc_ndp_router_set_rtr_adv(iface, enable);
    }
}

void gnrc_ndp_router_set_rtr_adv(gnrc_ipv6_netif_t *iface, bool enable)
{
    if (enable && !(iface->flags & GNRC_IPV6_NETIF_FLAGS_RTR_ADV)) {
        mutex_lock(&iface->mutex);
        iface->flags |= GNRC_IPV6_NETIF_FLAGS_RTR_ADV;
        iface->rtr_adv_count = GNRC_NDP_MAX_INIT_RTR_ADV_NUMOF;
        mutex_unlock(&iface->mutex);
        _send_rtr_adv(iface, NULL);
    }
    else if (!enable && (iface->flags & GNRC_IPV6_NETIF_FLAGS_RTR_ADV)) {
        mutex_lock(&iface->mutex);
        iface->rtr_adv_count = GNRC_NDP_MAX_FIN_RTR_ADV_NUMOF;
        iface->flags &= ~GNRC_IPV6_NETIF_FLAGS_RTR_ADV;
        iface->adv_ltime = 0;
#ifdef MODULE_GNRC_NDP_HOST
        iface->rtr_sol_count = GNRC_NDP_MAX_RTR_SOL_NUMOF;
#endif
        mutex_unlock(&iface->mutex);
        _send_rtr_adv(iface, NULL);
#ifdef MODULE_GNRC_NDP_HOST
        gnrc_ndp_host_retrans_rtr_sol(iface);
#endif
    }
}

void gnrc_ndp_router_retrans_rtr_adv(gnrc_ipv6_netif_t *iface)
{
    _send_rtr_adv(iface, NULL);
}

void gnrc_ndp_router_send_rtr_adv(gnrc_ipv6_nc_t *neighbor)
{
    gnrc_ipv6_netif_t *iface = gnrc_ipv6_netif_get(neighbor->iface);
    /* TODO: send one router advertisement per ABR: https://tools.ietf.org/html/rfc6775#section-6.3 */
    _send_rtr_adv(iface, &neighbor->ipv6_addr);
}

static void _send_rtr_adv(gnrc_ipv6_netif_t *iface, ipv6_addr_t *dst)
{
    bool fin;
    uint32_t interval;

    mutex_lock(&iface->mutex);
    fin = (iface->adv_ltime == 0);
    assert((iface->min_adv_int != 0) && (iface->max_adv_int != 0));
    interval = random_uint32_range(iface->min_adv_int, iface->max_adv_int);
    if (!fin && !((iface->flags | GNRC_IPV6_NETIF_FLAGS_ROUTER) &&
                  (iface->flags | GNRC_IPV6_NETIF_FLAGS_RTR_ADV))) {
        DEBUG("ndp rtr: interface %" PRIkernel_pid " is not an advertising interface\n",
              iface->pid);
        return;
    }
    if (iface->rtr_adv_count > 1) { /* regard for off-by-one error */
        iface->rtr_adv_count--;
        if (!fin && (interval > GNRC_NDP_MAX_INIT_RTR_ADV_INT)) {
            interval = GNRC_NDP_MAX_INIT_RTR_ADV_INT;
        }
    }
    if (!fin || (iface->rtr_adv_count > 1)) {   /* regard for off-by-one-error */
        /* reset timer for next router advertisement */
        xtimer_remove(&iface->rtr_adv_timer);
        iface->rtr_adv_msg.type = GNRC_NDP_MSG_RTR_ADV_RETRANS;
        iface->rtr_adv_msg.content.ptr = iface;
        xtimer_set_msg(&iface->rtr_adv_timer, interval * US_PER_SEC, &iface->rtr_adv_msg,
                       gnrc_ipv6_pid);
    }
    mutex_unlock(&iface->mutex);
    for (int i = 0; i < GNRC_IPV6_NETIF_ADDR_NUMOF; i++) {
        ipv6_addr_t *src = &iface->addrs[i].addr;

        if (!ipv6_addr_is_unspecified(src) && ipv6_addr_is_link_local(src) &&
            !gnrc_ipv6_netif_addr_is_non_unicast(src)) {
            /* send one for every link local address (ideally there is only one) */
            gnrc_ndp_internal_send_rtr_adv(iface->pid, src, dst, fin);
        }
    }
}

/** @} */
