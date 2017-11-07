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

#include "net/eui64.h"
#include "net/icmpv6.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ndp2.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/sixlowpan.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "random.h"

#include "net/gnrc/sixlowpan/nd.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

gnrc_pktsnip_t *gnrc_sixlowpan_nd_opt_ar_build(uint8_t status, uint16_t ltime, eui64_t *eui64,
                                               gnrc_pktsnip_t *next)
{
    gnrc_pktsnip_t *pkt = gnrc_ndp2_opt_build(NDP_OPT_AR, sizeof(sixlowpan_nd_opt_ar_t), next);

    if (pkt != NULL) {
        sixlowpan_nd_opt_ar_t *ar_opt = pkt->data;
        ar_opt->status = status;
        ar_opt->resv[0] = ar_opt->resv[1] = ar_opt->resv[2] = 0;
        ar_opt->ltime = byteorder_htons(ltime);
        memcpy(&ar_opt->eui64, eui64, sizeof(eui64_t));
    }

    return pkt;
}

/* gnrc_sixlowpan_nd_opt_abr_handle etc. implemented in gnrc_sixlowpan_nd_router */

/** @} */
