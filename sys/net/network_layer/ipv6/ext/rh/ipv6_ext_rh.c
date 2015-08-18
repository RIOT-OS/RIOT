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

#include <stdbool.h>

#include "net/protnum.h"
#include "net/ipv6/ext/rh.h"
#include "net/gnrc/rpl/srh.h"

ipv6_addr_t *ipv6_ext_rh_next_hop(ipv6_hdr_t *ipv6)
{
    ipv6_ext_rh_t *ext = (ipv6_ext_rh_t *)(ipv6 + 1);
    bool c = true;

    while (c) {
        switch (ext->type) {
            case PROTNUM_IPV6_EXT_HOPOPT:
            case PROTNUM_IPV6_EXT_DST:
            case PROTNUM_IPV6_EXT_FRAG:
            case PROTNUM_IPV6_EXT_AH:
            case PROTNUM_IPV6_EXT_ESP:
            case PROTNUM_IPV6_EXT_MOB:
                ext = (ipv6_ext_rh_t *)ipv6_ext_get_next((ipv6_ext_t *)ext);
                break;

            case PROTNUM_IPV6_EXT_RH:
                c = false;
                break;

            default:
                c = false;
                break;
        }
    }

    if (ipv6->nh == PROTNUM_IPV6_EXT_RH) {
        switch (ext->type) {
#ifdef MODULE_GNRC_RPL_SRH
            case GNRC_RPL_SRH_TYPE:
                return gnrc_rpl_srh_next_hop((gnrc_rpl_srh_t *)ext);
#endif

            default:
                break;
        }
    }

    return NULL;
}

/** @} */
