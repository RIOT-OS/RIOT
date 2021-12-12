/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>

#include "net/ipv6.h"
#include "net/ipv6/ext.h"
#include "net/ipv6/ext/opt.h"
#include "net/gnrc/icmpv6/error.h"
#include "net/gnrc/pktbuf.h"

#include "net/gnrc/ipv6/ext/opt.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Determine what action to do, when option is not recognized
 *
 * @see     https://tools.ietf.org/html/rfc8200#section-4.2
 *
 * @param[in] type  Type of the option
 */
static inline uint8_t _unrec_action(uint8_t type)
{
    return (type & IPV6_EXT_OPT_ACTION_MASK);
}

static bool _multicast_dst(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *ipv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    ipv6_hdr_t *ipv6_hdr;

    assert(ipv6 != NULL);
    ipv6_hdr = ipv6->data;
    return ipv6_addr_is_multicast(&ipv6_hdr->dst);
}

gnrc_pktsnip_t *gnrc_ipv6_ext_opt_process(gnrc_pktsnip_t *pkt,
                                          uint8_t protnum)
{
    (void)protnum;
    assert(pkt != NULL);
    assert((protnum == PROTNUM_IPV6_EXT_HOPOPT) ||
           (protnum == PROTNUM_IPV6_EXT_DST));
    gnrc_pktsnip_t *hdr;
    ipv6_ext_t *opt_hdr = pkt->data;
    uint8_t *opts;
    size_t hdr_len;

    if (pkt->size < sizeof(ipv6_ext_t)) {
        DEBUG("gnrc_ipv6_ext_opt: packet of invalid size\n");
        goto error;
    }
    hdr_len = ((opt_hdr->len * IPV6_EXT_LEN_UNIT) + IPV6_EXT_LEN_UNIT);
    hdr = gnrc_pktbuf_mark(pkt, hdr_len, GNRC_NETTYPE_IPV6_EXT);
    if (hdr == NULL) {
        DEBUG("gnrc_ipv6_ext_opt: unable to mark option header\n");
        goto error;
    }
    opts = hdr->data;
    for (unsigned offset = sizeof(ipv6_ext_t); offset < hdr_len;) {
        uint8_t opt_type = opts[offset++];
        uint8_t opt_len;

        if (opt_type == IPV6_EXT_OPT_PAD1) {
            /* nothing more to do */
            continue;
        }
        opt_len = opts[offset++];
        if (opt_len > (hdr_len - offset)) {
            DEBUG("gnrc_ipv6_ext_opt: invalid option size\n");
            goto error;
        }
        switch (opt_type) {
            /* IPV6_EXT_OPT_PAD1 already handled before length check due
             * to special format */
            case IPV6_EXT_OPT_PADN:
                /* nothing to do, offset will be progressed below */
                break;
            default: {
                bool send_error = false;

                switch (_unrec_action(opt_type)) {
                    case IPV6_EXT_OPT_ACTION_SKIP:
                        DEBUG("gnrc_ipv6_ext_opt: skipping unknown "
                              "option %02x\n", opt_type);
                        /* skip here already, as we don't reach the
                         * incrementation of offset below */
                        offset += opt_len;
                        continue;
                    case IPV6_EXT_OPT_ACTION_DISC:
                        break;
                    case IPV6_EXT_OPT_ACTION_DISC_ERR_MCAST:
                        send_error = IS_USED(MODULE_GNRC_ICMPV6_ERROR);
                        break;
                    case IPV6_EXT_OPT_ACTION_DISC_ERR:
                        send_error = IS_USED(MODULE_GNRC_ICMPV6_ERROR) &&
                                     !_multicast_dst(pkt);
                        break;
                }
                DEBUG("gnrc_ipv6_ext_opt: discarding packet with unknown "
                      "option %02x\n", opt_type);
                if (send_error) {
                    DEBUG("gnrc_ipv6_ext_opt: reporting parameter problem "
                          "for option %02x (pos at %02x)\n", opts[offset - 2U],
                          opt_type);
                    gnrc_icmpv6_error_param_prob_send(
                            ICMPV6_ERROR_PARAM_PROB_OPT,
                            /* offset was already progressed to opt data*/
                            &opts[offset - 2U], pkt);
                }
                goto error;
            }
        }
        offset += opt_len;
    }
    return pkt;
error:
    gnrc_pktbuf_release_error(pkt, EINVAL);
    return NULL;
}

/** @} */
