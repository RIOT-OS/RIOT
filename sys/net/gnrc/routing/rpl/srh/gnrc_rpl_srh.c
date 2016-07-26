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

#include <string.h>
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/rpl/srh.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

#define GNRC_RPL_SRH_PADDING(X)     ((X & 0xF0) >> 4)
#define GNRC_RPL_SRH_COMPRE(X)      (X & 0x0F)
#define GNRC_RPL_SRH_COMPRI(X)      ((X & 0xF0) >> 4)

int gnrc_rpl_srh_process(ipv6_hdr_t *ipv6, gnrc_rpl_srh_t *rh)
{
    if (rh->seg_left == 0) {
        return EXT_RH_CODE_OK;
    }

    uint8_t n = (((rh->len * 8) - GNRC_RPL_SRH_PADDING(rh->pad_resv) -
                 (16 - GNRC_RPL_SRH_COMPRE(rh->compr))) /
                 (16 - GNRC_RPL_SRH_COMPRI(rh->compr))) + 1;
    ipv6_addr_t addr = ipv6->dst, tmp;
    uint8_t i, pref_elided, tmp_pref_elided, addr_len, compri_addr_len, tmp_addr_len, found_pos = 0;
    uint8_t *addr_vec = (uint8_t *) (rh + 1);
    bool found = false;

    DEBUG("RPL SRH: %u addresses in the routing header\n", (unsigned) n);

    if (rh->seg_left > n) {
        DEBUG("RPL SRH: number of segments left > number of addresses - discard\n");
        /* TODO ICMP Parameter Problem - Code 0 */
        return EXT_RH_CODE_ERROR;
    }

    rh->seg_left--;
    i = n - rh->seg_left;
    pref_elided = rh->seg_left ? GNRC_RPL_SRH_COMPRI(rh->compr) : GNRC_RPL_SRH_COMPRE(rh->compr);
    compri_addr_len = sizeof(ipv6_addr_t) - GNRC_RPL_SRH_COMPRI(rh->compr);
    addr_len = sizeof(ipv6_addr_t) - pref_elided;
    memcpy(&addr.u8[pref_elided], &addr_vec[(i - 1) * compri_addr_len], addr_len);

    if (ipv6_addr_is_multicast(&ipv6->dst) || ipv6_addr_is_multicast(&addr)) {
        DEBUG("RPL SRH: found a multicast address - discard\n");
        /* TODO discard the packet */
        return EXT_RH_CODE_ERROR;
    }

    /* check if multiple addresses of my interface exist */
    tmp_pref_elided = GNRC_RPL_SRH_COMPRI(rh->compr);
    tmp_addr_len = sizeof(ipv6_addr_t) - tmp_pref_elided;
    tmp = ipv6->dst;
    for (uint8_t k = 0; k < n; k++) {
        if (k == n - 1) {
            tmp_pref_elided = GNRC_RPL_SRH_COMPRE(rh->compr);
            tmp_addr_len = sizeof(ipv6_addr_t) - tmp_pref_elided;
        }
        memcpy(&tmp.u8[tmp_pref_elided], &addr_vec[k * compri_addr_len], tmp_addr_len);
        if (gnrc_ipv6_netif_find_by_addr(NULL, &tmp) != KERNEL_PID_UNDEF) {
            if (found && ((k - found_pos) > 1)) {
                DEBUG("RPL SRH: found multiple addresses that belong to me - discard\n");
                /* TODO send an ICMP Parameter Problem (Code 0) and discard the packet */
                return EXT_RH_CODE_ERROR;
            }
            found_pos = k;
            found = true;
        }
    }

    memcpy(&addr_vec[(i - 1) * compri_addr_len], &ipv6->dst.u8[pref_elided], addr_len);

    DEBUG("RPL SRH: Next hop: %s at position %d\n",
          ipv6_addr_to_str(addr_str, &addr, sizeof(addr_str)), i);

    ipv6->dst = addr;

    return EXT_RH_CODE_FORWARD;
}

/** @} */
