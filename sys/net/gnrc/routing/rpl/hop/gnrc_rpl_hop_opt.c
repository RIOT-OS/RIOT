/*
 * Copyright (C) 2017 Martin Landsmann  <martin.landsmann@haw-hamburg.de>
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
#include "net/gnrc/rpl/dodag.h"
#include "net/gnrc/rpl/hop.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

int gnrc_rpl_hop_opt_process(gnrc_rpl_hop_opt_t *hop)
{
    /* excluding the Option Type and Opt Data Len fields */
    if (hop->len >= (sizeof(gnrc_rpl_hop_opt_t) - 2)) {
        return HOP_OPT_ERR_HEADER_LENGTH;
    }

    if (hop->ORF_flags & GNRC_RPL_HOP_OPT_FLAG_F) {
        /* we received a forwarding incosistency */
        return HOP_OPT_ERR_INCONSISTENCY;
    }

    for (uint8_t i = 0; i < GNRC_RPL_INSTANCES_NUMOF; ++i) {
        /* check if the option is for us */
        if (gnrc_rpl_instances[i].id == hop->instance_id) {
            /* check if the packet traversed in the expected direction */
            if ((gnrc_rpl_instances[i].dodag.my_rank < hop->sender_rank)
                && (hop->ORF_flags & GNRC_RPL_HOP_OPT_FLAG_O)) {
                /* everything worked out as expectes so we store our rank */
                hop->sender_rank = gnrc_rpl_instances[i].dodag.my_rank;
                /* and push the paket further towards its destination */
                return HOP_OPT_SUCCESS;
            }
            else {
                /* it didn't */
                if (hop->ORF_flags & GNRC_RPL_HOP_OPT_FLAG_R) {
                    /* its not the first time so we set error flag F */
                    hop->ORF_flags |= GNRC_RPL_HOP_OPT_FLAG_F;
                    /* start local repair and return */
                    gnrc_rpl_local_repair(&gnrc_rpl_instances[i].dodag);

                    return HOP_OPT_ERR_FLAG_F_SET;
                }
                /* set error flag R and return */
                hop->ORF_flags |= GNRC_RPL_HOP_OPT_FLAG_R;
                return HOP_OPT_ERR_FLAG_R_SET;
            }
        }
    }
    /* the option is not related to us, we just forward the packet further */
    return HOP_OPT_ERR_NOT_FOR_ME;
}

/** @} */
