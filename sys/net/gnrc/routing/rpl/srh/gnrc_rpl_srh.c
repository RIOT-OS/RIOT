/*
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Cenk Gündoğan <cnkgndgn@gmail.com>
 * @author Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <string.h>
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/ipv6/ext/rh.h"
#include "net/gnrc/rpl/srh.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

#define GNRC_RPL_SRH_PADDING(X)     ((X & 0xF0) >> 4)
#define GNRC_RPL_SRH_COMPRE(X)      (X & 0x0F)
#define GNRC_RPL_SRH_COMPRI(X)      ((X & 0xF0) >> 4)

/* checks if multiple addresses within the source routing header exist on my
 * interfaces */
static bool _contains_multiple_of_my_addr(const ipv6_addr_t *dst,
                                          const gnrc_rpl_srh_t *rh,
                                          unsigned num_addr,
                                          unsigned compri_addr_len)
{
    ipv6_addr_t addr;
    uint8_t *addr_vec = (uint8_t *) (rh + 1);
    bool found = false;
    uint8_t pref_elided = GNRC_RPL_SRH_COMPRI(rh->compr);
    uint8_t addr_len = compri_addr_len;
    uint8_t found_pos = 0;

    memcpy(&addr, dst, pref_elided);
    for (unsigned i = 0; i < num_addr; i++) {
        if (i == num_addr - 1) {
            pref_elided = GNRC_RPL_SRH_COMPRE(rh->compr);
            addr_len = sizeof(ipv6_addr_t) - pref_elided;
        }
        memcpy(&addr.u8[pref_elided], &addr_vec[i * compri_addr_len], addr_len);
        if (gnrc_netif_get_by_ipv6_addr(&addr) != NULL) {
            if (found && ((i - found_pos) > 1)) {
                DEBUG("RPL SRH: found multiple addresses that belong to me - "
                      "discard\n");
                /* TODO send an ICMP Parameter Problem (Code 0) and
                 * discard the packet */
                return true;
            }
            found_pos = i;
            found = true;
        }
    }
    return false;
}

int gnrc_rpl_srh_process(ipv6_hdr_t *ipv6, gnrc_rpl_srh_t *rh)
{
    ipv6_addr_t addr;
    uint8_t *addr_vec = (uint8_t *) (rh + 1);
    uint8_t num_addr;
    uint8_t current_pos, pref_elided, addr_len, compri_addr_len;
    const uint8_t new_seg_left = rh->seg_left - 1;

    assert(rh->seg_left > 0);
    num_addr = (((rh->len * 8) - GNRC_RPL_SRH_PADDING(rh->pad_resv) -
                 (16 - GNRC_RPL_SRH_COMPRE(rh->compr))) /
                (16 - GNRC_RPL_SRH_COMPRI(rh->compr))) + 1;

    DEBUG("RPL SRH: %u addresses in the routing header\n", (unsigned) num_addr);

    if (rh->seg_left > num_addr) {
        DEBUG("RPL SRH: number of segments left > number of addresses - "
              "discard\n");
        /* TODO ICMP Parameter Problem - Code 0 */
        return GNRC_IPV6_EXT_RH_ERROR;
    }

    current_pos = num_addr - new_seg_left;
    pref_elided = (new_seg_left)
                ? GNRC_RPL_SRH_COMPRI(rh->compr)
                : GNRC_RPL_SRH_COMPRE(rh->compr);
    compri_addr_len = sizeof(ipv6_addr_t) - GNRC_RPL_SRH_COMPRI(rh->compr);
    addr_len = sizeof(ipv6_addr_t) - pref_elided;
    memcpy(&addr, &ipv6->dst, pref_elided);
    memcpy(&addr.u8[pref_elided],
           &addr_vec[(current_pos - 1) * compri_addr_len],
           addr_len);

    if (ipv6_addr_is_multicast(&ipv6->dst) || ipv6_addr_is_multicast(&addr)) {
        DEBUG("RPL SRH: found a multicast address - discard\n");
        /* TODO discard the packet */
        return GNRC_IPV6_EXT_RH_ERROR;
    }

    /* check if multiple addresses of my interface exist */
    if (_contains_multiple_of_my_addr(&ipv6->dst, rh, num_addr,
                                      compri_addr_len)) {
        return GNRC_IPV6_EXT_RH_ERROR;
    }
    rh->seg_left = new_seg_left;
    memcpy(&addr_vec[(current_pos - 1) * compri_addr_len],
           &ipv6->dst.u8[pref_elided], addr_len);

    DEBUG("RPL SRH: Next hop: %s at position %d\n",
          ipv6_addr_to_str(addr_str, &addr, sizeof(addr_str)), current_pos);

    memcpy(&ipv6->dst, &addr, sizeof(ipv6->dst));

    return GNRC_IPV6_EXT_RH_FORWARDED;
}

/** @} */
