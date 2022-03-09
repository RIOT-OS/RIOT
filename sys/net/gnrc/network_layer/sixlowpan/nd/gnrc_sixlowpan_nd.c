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

#include "net/eui64.h"
#include "net/icmpv6.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ndp.h"
#include "net/gnrc/sixlowpan.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "random.h"

#include "net/gnrc/sixlowpan/nd.h"

#define ENABLE_DEBUG 0
#include "debug.h"

gnrc_pktsnip_t *gnrc_sixlowpan_nd_opt_ar_build(uint8_t status, uint16_t ltime, eui64_t *eui64,
                                               gnrc_pktsnip_t *next)
{
    gnrc_pktsnip_t *pkt = gnrc_ndp_opt_build(NDP_OPT_AR, sizeof(sixlowpan_nd_opt_ar_t), next);

    if (pkt != NULL) {
        sixlowpan_nd_opt_ar_t *ar_opt = pkt->data;
        ar_opt->status = status;
        ar_opt->resv[0] = ar_opt->resv[1] = ar_opt->resv[2] = 0;
        ar_opt->ltime = byteorder_htons(ltime);
        memcpy(&ar_opt->eui64, eui64, sizeof(eui64_t));
    }

    return pkt;
}

gnrc_pktsnip_t *gnrc_sixlowpan_nd_opt_6ctx_build(uint8_t prefix_len, uint8_t flags, uint16_t ltime,
                                                 ipv6_addr_t *prefix, gnrc_pktsnip_t *next)
{
    gnrc_pktsnip_t *pkt = gnrc_ndp_opt_build(NDP_OPT_6CTX,
                                             sizeof(sixlowpan_nd_opt_6ctx_t) + (prefix_len / 8),
                                             next);

    if (pkt != NULL) {
        sixlowpan_nd_opt_6ctx_t *ctx_opt = pkt->data;
        ctx_opt->ctx_len = prefix_len;
        ctx_opt->resv_c_cid = flags;
        ctx_opt->resv.u16 = 0;
        ctx_opt->ltime = byteorder_htons(ltime);
        /* Bits beyond prefix_len MUST be 0 */
        memset(ctx_opt + 1, 0, pkt->size - sizeof(sixlowpan_nd_opt_6ctx_t));
        ipv6_addr_init_prefix((ipv6_addr_t *)(ctx_opt + 1), prefix, prefix_len);
    }

    return pkt;
}

gnrc_pktsnip_t *gnrc_sixlowpan_nd_opt_abr_build(uint32_t version, uint16_t ltime,
                                                ipv6_addr_t *braddr, gnrc_pktsnip_t *next)
{
    gnrc_pktsnip_t *pkt = gnrc_ndp_opt_build(NDP_OPT_ABR, sizeof(sixlowpan_nd_opt_abr_t), next);

    if (pkt != NULL) {
        sixlowpan_nd_opt_abr_t *abr_opt = pkt->data;
        abr_opt->vlow = byteorder_htons(version & 0xffff);
        abr_opt->vhigh = byteorder_htons(version >> 16);
        abr_opt->ltime = byteorder_htons(ltime);
        abr_opt->braddr.u64[0] = braddr->u64[0];
        abr_opt->braddr.u64[1] = braddr->u64[1];
    }

    return pkt;
}

/** @} */
