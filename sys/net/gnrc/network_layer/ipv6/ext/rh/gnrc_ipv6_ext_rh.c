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
#include "net/ipv6/ext.h"
#include "net/gnrc/ipv6/ext/rh.h"
#include "net/gnrc/rpl/srh.h"

int gnrc_ipv6_ext_rh_process(ipv6_hdr_t *hdr, ipv6_ext_rh_t *ext)
{
    (void) hdr;

    switch (ext->type) {
#ifdef MODULE_GNRC_RPL_SRH
        case IPV6_EXT_RH_TYPE_RPL_SRH:
            return gnrc_rpl_srh_process(hdr, (gnrc_rpl_srh_t *)ext);
#endif

        default:
            break;
    }
    return GNRC_IPV6_EXT_RH_ERROR;
}

/** @} */
