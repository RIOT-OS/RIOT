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

#include "byteorder.h"
#include "net/ieee802154.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "net/sixlowpan.h"
#include "utlist.h"

#include "net/gnrc/sixlowpan/iphc.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* dispatch byte definitions */
#define IPHC1_IDX                   (0U)
#define IPHC2_IDX                   (1U)
#define CID_EXT_IDX                 (2U)

/* compression values for traffic class and flow label */
#define IPHC_TF_ECN_DSCP_FL         (0x00)
#define IPHC_TF_ECN_FL              (0x08)
#define IPHC_TF_ECN_DSCP            (0x10)
#define IPHC_TF_ECN_ELIDE           (0x18)

/* compression values for hop limit */
#define IPHC_HL_INLINE              (0x00)
#define IPHC_HL_1                   (0x01)
#define IPHC_HL_64                  (0x02)
#define IPHC_HL_255                 (0x03)

/* compression values for source address */
#define IPHC_SAC_SAM_FULL           (0x00)
#define IPHC_SAC_SAM_64             (0x10)
#define IPHC_SAC_SAM_16             (0x20)
#define IPHC_SAC_SAM_L2             (0x30)
#define IPHC_SAC_SAM_UNSPEC         (0x40)
#define IPHC_SAC_SAM_CTX_64         (0x50)
#define IPHC_SAC_SAM_CTX_16         (0x60)
#define IPHC_SAC_SAM_CTX_L2         (0x70)

/* compression values for destination address */
#define IPHC_M_DAC_DAM_U_FULL       (0x00)
#define IPHC_M_DAC_DAM_U_64         (0x01)
#define IPHC_M_DAC_DAM_U_16         (0x02)
#define IPHC_M_DAC_DAM_U_L2         (0x03)
#define IPHC_M_DAC_DAM_U_UNSPEC     (0x04)
#define IPHC_M_DAC_DAM_U_CTX_64     (0x05)
#define IPHC_M_DAC_DAM_U_CTX_16     (0x06)
#define IPHC_M_DAC_DAM_U_CTX_L2     (0x07)
#define IPHC_M_DAC_DAM_M_FULL       (0x08)
#define IPHC_M_DAC_DAM_M_48         (0x09)
#define IPHC_M_DAC_DAM_M_32         (0x0a)
#define IPHC_M_DAC_DAM_M_8          (0x0b)
#define IPHC_M_DAC_DAM_M_UC_PREFIX  (0x0c)

static inline bool _context_overlaps_iid(gnrc_sixlowpan_ctx_t *ctx,
                                         ipv6_addr_t *addr,
                                         eui64_t *iid)
{
    uint8_t byte_mask[] = {0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01};

    if ((ctx == NULL) || (ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_COMP)) {
        return false;
    }

    return ((ctx->prefix_len == 128) || /* Full-length prefix overlaps IID in any case */
            ((ctx->prefix_len > 64) &&  /* otherwise, if bigger than 64-bit */
             /* compare bytes until prefix length with IID */
             (memcmp(&(addr->u8[(ctx->prefix_len / 8) + 1]),
                     &(iid->uint8[(ctx->prefix_len / 8) - 7]),
                     sizeof(network_uint64_t) - ((ctx->prefix_len / 8) - 7)) == 0) &&
             /* compare bits at prefix length with IID */
             (addr->u8[(ctx->prefix_len / 8)] & byte_mask[ctx->prefix_len % 8]) ==
             (iid->uint8[(ctx->prefix_len / 8) - 8] & byte_mask[ctx->prefix_len % 8])));
}

size_t gnrc_sixlowpan_iphc_decode(gnrc_pktsnip_t *ipv6, gnrc_pktsnip_t *pkt, size_t datagram_size,
                                  size_t offset)
{
    gnrc_netif_hdr_t *netif_hdr = pkt->next->data;
    ipv6_hdr_t *ipv6_hdr;
    uint8_t *iphc_hdr = pkt->data;
    size_t payload_offset = SIXLOWPAN_IPHC_HDR_LEN;
    gnrc_sixlowpan_ctx_t *ctx = NULL;

    assert(ipv6 != NULL);
    assert(ipv6->size >= sizeof(ipv6_hdr_t));

    ipv6_hdr = ipv6->data;
    iphc_hdr += offset;

    if (iphc_hdr[IPHC2_IDX] & SIXLOWPAN_IPHC2_CID_EXT) {
        payload_offset++;
    }

    ipv6_hdr_set_version(ipv6_hdr);

    switch (iphc_hdr[IPHC1_IDX] & SIXLOWPAN_IPHC1_TF) {
        case IPHC_TF_ECN_DSCP_FL:
            ipv6_hdr_set_tc(ipv6_hdr, iphc_hdr[payload_offset++]);
            ipv6_hdr->v_tc_fl.u8[1] |= iphc_hdr[payload_offset++] & 0x0f;
            ipv6_hdr->v_tc_fl.u8[2] |= iphc_hdr[payload_offset++];
            ipv6_hdr->v_tc_fl.u8[3] |= iphc_hdr[payload_offset++];
            break;

        case IPHC_TF_ECN_FL:
            ipv6_hdr_set_tc_ecn(ipv6_hdr, iphc_hdr[payload_offset] >> 6);
            ipv6_hdr_set_tc_dscp(ipv6_hdr, 0);
            ipv6_hdr->v_tc_fl.u8[1] |= iphc_hdr[payload_offset++] & 0x0f;
            ipv6_hdr->v_tc_fl.u8[2] |= iphc_hdr[payload_offset++];
            ipv6_hdr->v_tc_fl.u8[3] |= iphc_hdr[payload_offset++];
            break;

        case IPHC_TF_ECN_DSCP:
            ipv6_hdr_set_tc(ipv6_hdr, iphc_hdr[payload_offset++]);
            ipv6_hdr_set_fl(ipv6_hdr, 0);
            break;

        case IPHC_TF_ECN_ELIDE:
            ipv6_hdr_set_tc(ipv6_hdr, 0);
            ipv6_hdr_set_fl(ipv6_hdr, 0);
            break;
    }

    if (!(iphc_hdr[IPHC1_IDX] & SIXLOWPAN_IPHC1_NH)) {
        ipv6_hdr->nh = iphc_hdr[payload_offset++];
    }

    switch (iphc_hdr[IPHC1_IDX] & SIXLOWPAN_IPHC1_HL) {
        case IPHC_HL_INLINE:
            ipv6_hdr->hl = iphc_hdr[payload_offset++];
            break;

        case IPHC_HL_1:
            ipv6_hdr->hl = 1;
            break;

        case IPHC_HL_64:
            ipv6_hdr->hl = 64;
            break;

        case IPHC_HL_255:
            ipv6_hdr->hl = 255;
            break;
    }

    if (iphc_hdr[IPHC2_IDX] & SIXLOWPAN_IPHC2_SAC) {
        uint8_t sci = 0;

        if (iphc_hdr[IPHC2_IDX] & SIXLOWPAN_IPHC2_CID_EXT) {
            sci = iphc_hdr[CID_EXT_IDX] >> 4;
        }

        if (iphc_hdr[IPHC2_IDX] & SIXLOWPAN_IPHC2_SAM) {
            ctx = gnrc_sixlowpan_ctx_lookup_id(sci);

            if (ctx == NULL) {
                DEBUG("6lo iphc: could not find source context\n");
                return 0;
            }
        }
    }

    switch (iphc_hdr[IPHC2_IDX] & (SIXLOWPAN_IPHC2_SAC | SIXLOWPAN_IPHC2_SAM)) {

        case IPHC_SAC_SAM_FULL:
            /* take full 128 from inline */
            memcpy(&(ipv6_hdr->src), iphc_hdr + payload_offset, 16);
            payload_offset += 16;
            break;

        case IPHC_SAC_SAM_64:
            ipv6_addr_set_link_local_prefix(&ipv6_hdr->src);
            memcpy(ipv6_hdr->src.u8 + 8, iphc_hdr + payload_offset, 8);
            payload_offset += 8;
            break;

        case IPHC_SAC_SAM_16:
            ipv6_addr_set_link_local_prefix(&ipv6_hdr->src);
            ipv6_hdr->src.u32[2] = byteorder_htonl(0x000000ff);
            ipv6_hdr->src.u16[6] = byteorder_htons(0xfe00);
            memcpy(ipv6_hdr->src.u8 + 14, iphc_hdr + payload_offset, 2);
            payload_offset += 2;
            break;

        case IPHC_SAC_SAM_L2:
            ieee802154_get_iid((eui64_t *)(&ipv6_hdr->src.u64[1]),
                               gnrc_netif_hdr_get_src_addr(netif_hdr),
                               netif_hdr->src_l2addr_len);
            ipv6_addr_set_link_local_prefix(&ipv6_hdr->src);
            break;

        case IPHC_SAC_SAM_UNSPEC:
            ipv6_addr_set_unspecified(&ipv6_hdr->src);
            break;

        case IPHC_SAC_SAM_CTX_64:
            assert(ctx != NULL);
            memcpy(ipv6_hdr->src.u8 + 8, iphc_hdr + payload_offset, 8);
            ipv6_addr_init_prefix(&ipv6_hdr->src, &ctx->prefix,
                                  ctx->prefix_len);
            payload_offset += 8;
            break;

        case IPHC_SAC_SAM_CTX_16:
            assert(ctx != NULL);
            ipv6_hdr->src.u32[2] = byteorder_htonl(0x000000ff);
            ipv6_hdr->src.u16[6] = byteorder_htons(0xfe00);
            memcpy(ipv6_hdr->src.u8 + 14, iphc_hdr + payload_offset, 2);
            ipv6_addr_init_prefix(&ipv6_hdr->src, &ctx->prefix,
                                  ctx->prefix_len);
            payload_offset += 2;
            break;

        case IPHC_SAC_SAM_CTX_L2:
            assert(ctx != NULL);
            ieee802154_get_iid((eui64_t *)(&ipv6_hdr->src.u64[1]),
                               gnrc_netif_hdr_get_src_addr(netif_hdr),
                               netif_hdr->src_l2addr_len);
            ipv6_addr_init_prefix(&ipv6_hdr->src, &ctx->prefix,
                                  ctx->prefix_len);
            break;
    }

    if (iphc_hdr[IPHC2_IDX] & SIXLOWPAN_IPHC2_DAC) {
        uint8_t dci = 0;

        if (iphc_hdr[IPHC2_IDX] & SIXLOWPAN_IPHC2_CID_EXT) {
            dci = iphc_hdr[CID_EXT_IDX] & 0x0f;
        }

        if (iphc_hdr[IPHC2_IDX] & SIXLOWPAN_IPHC2_DAM) {
            ctx = gnrc_sixlowpan_ctx_lookup_id(dci);

            if (ctx == NULL) {
                DEBUG("6lo iphc: could not find destination context\n");
                return 0;
            }
        }
    }

    switch (iphc_hdr[IPHC2_IDX] & (SIXLOWPAN_IPHC2_M | SIXLOWPAN_IPHC2_DAC |
                                   SIXLOWPAN_IPHC2_DAM)) {
        case IPHC_M_DAC_DAM_U_FULL:
        case IPHC_M_DAC_DAM_M_FULL:
            memcpy(&(ipv6_hdr->dst.u8), iphc_hdr + payload_offset, 16);
            payload_offset += 16;
            break;

        case IPHC_M_DAC_DAM_U_64:
            ipv6_addr_set_link_local_prefix(&ipv6_hdr->dst);
            memcpy(ipv6_hdr->dst.u8 + 8, iphc_hdr + payload_offset, 8);
            payload_offset += 8;
            break;

        case IPHC_M_DAC_DAM_U_16:
            ipv6_addr_set_link_local_prefix(&ipv6_hdr->dst);
            ipv6_hdr->dst.u32[2] = byteorder_htonl(0x000000ff);
            ipv6_hdr->dst.u16[6] = byteorder_htons(0xfe00);
            memcpy(ipv6_hdr->dst.u8 + 14, iphc_hdr + payload_offset, 2);
            payload_offset += 2;
            break;

        case IPHC_M_DAC_DAM_U_L2:
            ieee802154_get_iid((eui64_t *)(&ipv6_hdr->dst.u64[1]),
                               gnrc_netif_hdr_get_dst_addr(netif_hdr),
                               netif_hdr->dst_l2addr_len);
            ipv6_addr_set_link_local_prefix(&ipv6_hdr->dst);
            break;

        case IPHC_M_DAC_DAM_U_CTX_64:
            memcpy(ipv6_hdr->dst.u8 + 8, iphc_hdr + payload_offset, 8);
            ipv6_addr_init_prefix(&ipv6_hdr->dst, &ctx->prefix,
                                  ctx->prefix_len);
            payload_offset += 8;
            break;

        case IPHC_M_DAC_DAM_U_CTX_16:
            ipv6_hdr->dst.u32[2] = byteorder_htonl(0x000000ff);
            ipv6_hdr->dst.u16[6] = byteorder_htons(0xfe00);
            memcpy(ipv6_hdr->dst.u8 + 14, iphc_hdr + payload_offset, 2);
            ipv6_addr_init_prefix(&ipv6_hdr->dst, &ctx->prefix,
                                  ctx->prefix_len);
            payload_offset += 2;
            break;

        case IPHC_M_DAC_DAM_U_CTX_L2:
            ieee802154_get_iid((eui64_t *)(&ipv6_hdr->dst.u64[1]),
                               gnrc_netif_hdr_get_dst_addr(netif_hdr),
                               netif_hdr->dst_l2addr_len);
            ipv6_addr_init_prefix(&ipv6_hdr->dst, &ctx->prefix,
                                  ctx->prefix_len);
            break;

        case IPHC_M_DAC_DAM_M_48:
            /* ffXX::00XX:XXXX:XXXX */
            ipv6_addr_set_unspecified(&ipv6_hdr->dst);
            ipv6_hdr->dst.u8[0] = 0xff;
            ipv6_hdr->dst.u8[1] = iphc_hdr[payload_offset++];
            ipv6_hdr->dst.u8[11] = iphc_hdr[payload_offset++];
            ipv6_hdr->dst.u8[12] = iphc_hdr[payload_offset++];
            ipv6_hdr->dst.u8[13] = iphc_hdr[payload_offset++];
            ipv6_hdr->dst.u8[14] = iphc_hdr[payload_offset++];
            ipv6_hdr->dst.u8[15] = iphc_hdr[payload_offset++];
            break;

        case IPHC_M_DAC_DAM_M_32:
            /* ffXX::00XX:XXXX */
            ipv6_addr_set_unspecified(&ipv6_hdr->dst);
            ipv6_hdr->dst.u8[0] = 0xff;
            ipv6_hdr->dst.u8[1] = iphc_hdr[payload_offset++];
            ipv6_hdr->dst.u8[13] = iphc_hdr[payload_offset++];
            ipv6_hdr->dst.u8[14] = iphc_hdr[payload_offset++];
            ipv6_hdr->dst.u8[15] = iphc_hdr[payload_offset++];
            break;

        case IPHC_M_DAC_DAM_M_8:
            /* ff02::XX: */
            ipv6_addr_set_unspecified(&ipv6_hdr->dst);
            ipv6_hdr->dst.u8[0] = 0xff;
            ipv6_hdr->dst.u8[1] = 0x02;
            ipv6_hdr->dst.u8[15] = iphc_hdr[payload_offset++];
            break;

        case IPHC_M_DAC_DAM_M_UC_PREFIX:
            do {
                uint8_t orig_ctx_len = ctx->prefix_len;

                ipv6_addr_set_unspecified(&ipv6_hdr->dst);

                if (ctx->prefix_len > 64) {
                    ctx->prefix_len = 64;
                }

                ipv6_hdr->dst.u8[0] = 0xff;
                ipv6_hdr->dst.u8[1] = iphc_hdr[payload_offset++];
                ipv6_hdr->dst.u8[2] = iphc_hdr[payload_offset++];
                ipv6_hdr->dst.u8[3] = ctx->prefix_len;
                ipv6_addr_init_prefix((ipv6_addr_t *)ipv6_hdr->dst.u8 + 4,
                                      &ctx->prefix, ctx->prefix_len);
                memcpy(ipv6_hdr->dst.u8 + 12, iphc_hdr + payload_offset + 2, 4);

                payload_offset += 4;

                ctx->prefix_len = orig_ctx_len;
            } while (0);    /* ANSI-C compatible block creation for orig_ctx_len allocation */
            break;

        default:
            DEBUG("6lo iphc: unspecified or reserved M, DAC, DAM combination\n");
            return 0;

    }

    /* TODO: add next header decoding */

    /* set IPv6 header payload length field to the length of whatever is left
     * after removing the 6LoWPAN header */
    if (datagram_size == 0) {
        ipv6_hdr->len = byteorder_htons((uint16_t)(pkt->size - payload_offset));
    }
    else {
        ipv6_hdr->len = byteorder_htons((uint16_t)(datagram_size - sizeof(ipv6_hdr_t)));
    }


    return payload_offset;
}

bool gnrc_sixlowpan_iphc_encode(gnrc_pktsnip_t *pkt)
{
    gnrc_netif_hdr_t *netif_hdr = pkt->data;
    ipv6_hdr_t *ipv6_hdr = pkt->next->data;
    uint8_t *iphc_hdr;
    uint16_t inline_pos = SIXLOWPAN_IPHC_HDR_LEN;
    bool addr_comp = false;
    gnrc_sixlowpan_ctx_t *src_ctx = NULL, *dst_ctx = NULL;
    gnrc_pktsnip_t *dispatch = gnrc_pktbuf_add(NULL, NULL, pkt->next->size,
                                               GNRC_NETTYPE_SIXLOWPAN);

    if (dispatch == NULL) {
        DEBUG("6lo iphc: error allocating dispatch space\n");
        return false;
    }

    iphc_hdr = dispatch->data;

    /* set initial dispatch value*/
    iphc_hdr[IPHC1_IDX] = SIXLOWPAN_IPHC1_DISP;
    iphc_hdr[IPHC2_IDX] = 0;

    /* check for available contexts */
    if (!ipv6_addr_is_unspecified(&(ipv6_hdr->src))) {
        src_ctx = gnrc_sixlowpan_ctx_lookup_addr(&(ipv6_hdr->src));
    }

    if (!ipv6_addr_is_multicast(&ipv6_hdr->dst)) {
        dst_ctx = gnrc_sixlowpan_ctx_lookup_addr(&(ipv6_hdr->dst));
    }

    /* if contexts available and both != 0 */
    /* since this moves inline_pos we have to do this ahead*/
    if (((src_ctx != NULL) &&
         ((src_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK) != 0) &&
         (src_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_COMP)) ||
        ((dst_ctx != NULL) &&
         ((dst_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK) != 0) &&
         (dst_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_COMP))) {
        /* add context identifier extension */
        iphc_hdr[IPHC2_IDX] |= SIXLOWPAN_IPHC2_CID_EXT;
        iphc_hdr[CID_EXT_IDX] = 0;

        /* move position to behind CID extension */
        inline_pos += SIXLOWPAN_IPHC_CID_EXT_LEN;
    }

    /* compress flow label and traffic class */
    if (ipv6_hdr_get_fl(ipv6_hdr) == 0) {
        if (ipv6_hdr_get_tc(ipv6_hdr) == 0) {
            /* elide both traffic class and flow label */
            iphc_hdr[IPHC1_IDX] |= IPHC_TF_ECN_ELIDE;
        }
        else {
            /* elide flow label, traffic class (ECN + DSCP) inline (1 byte) */
            iphc_hdr[IPHC1_IDX] |= IPHC_TF_ECN_DSCP;
            iphc_hdr[inline_pos++] = ipv6_hdr_get_tc(ipv6_hdr);
        }
    }
    else {
        if (ipv6_hdr_get_tc_dscp(ipv6_hdr) == 0) {
            /* elide DSCP, ECN + 2-bit pad + flow label inline (3 byte) */
            iphc_hdr[IPHC1_IDX] |= IPHC_TF_ECN_FL;
            iphc_hdr[inline_pos++] = (uint8_t)((ipv6_hdr_get_tc_ecn(ipv6_hdr) << 6) |
                                               ((ipv6_hdr_get_fl(ipv6_hdr) & 0x000f0000) >> 16));
        }
        else {
            /* ECN + DSCP + 4-bit pad + flow label (4 bytes) */
            iphc_hdr[IPHC1_IDX] |= IPHC_TF_ECN_DSCP_FL;
            iphc_hdr[inline_pos++] = ipv6_hdr_get_tc(ipv6_hdr);
            iphc_hdr[inline_pos++] = (uint8_t)((ipv6_hdr_get_fl(ipv6_hdr) & 0x000f0000) >> 16);
        }

        /* copy remaining byteos of flow label */
        iphc_hdr[inline_pos++] = (uint8_t)((ipv6_hdr_get_fl(ipv6_hdr) & 0x0000ff00) >> 8);
        iphc_hdr[inline_pos++] = (uint8_t)((ipv6_hdr_get_fl(ipv6_hdr) & 0x000000ff) >> 8);
    }

    /* compress next header */
    switch (ipv6_hdr->nh) {
        /* TODO: add next header compression and set NH bit */
        default:
            iphc_hdr[inline_pos++] = ipv6_hdr->nh;
            break;
    }

    /* compress hop limit */
    switch (ipv6_hdr->hl) {
        case 1:
            iphc_hdr[IPHC1_IDX] |= IPHC_HL_1;
            break;

        case 64:
            iphc_hdr[IPHC1_IDX] |= IPHC_HL_64;
            break;

        case 255:
            iphc_hdr[IPHC1_IDX] |= IPHC_HL_255;
            break;

        default:
            iphc_hdr[IPHC1_IDX] |= IPHC_HL_INLINE;
            iphc_hdr[inline_pos++] = ipv6_hdr->hl;
            break;
    }

    if (ipv6_addr_is_unspecified(&(ipv6_hdr->src))) {
        iphc_hdr[IPHC2_IDX] |= IPHC_SAC_SAM_UNSPEC;
    }
    else {
        if ((src_ctx != NULL) && (src_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_COMP)) {
            /* stateful source address compression */
            iphc_hdr[IPHC2_IDX] |= SIXLOWPAN_IPHC2_SAC;

            if (((src_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK) != 0)) {
                iphc_hdr[CID_EXT_IDX] |= ((src_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK) << 4);
            }
        }

        if ((src_ctx != NULL) || ipv6_addr_is_link_local(&(ipv6_hdr->src))) {
            eui64_t iid;
            iid.uint64.u64 = 0;

            if ((netif_hdr->src_l2addr_len == 2) ||
                (netif_hdr->src_l2addr_len == 4) ||
                (netif_hdr->src_l2addr_len == 8)) {
                /* prefer to create IID from netif header if available */
                ieee802154_get_iid(&iid, gnrc_netif_hdr_get_src_addr(netif_hdr),
                                   netif_hdr->src_l2addr_len);
            }
            else {
                /* but take from driver otherwise */
                gnrc_netapi_get(netif_hdr->if_pid, NETOPT_IPV6_IID, 0, &iid,
                                sizeof(eui64_t));
            }

            if ((ipv6_hdr->src.u64[1].u64 == iid.uint64.u64) ||
                _context_overlaps_iid(src_ctx, &ipv6_hdr->src, &iid)) {
                /* 0 bits. The address is derived from link-layer address */
                iphc_hdr[IPHC2_IDX] |= IPHC_SAC_SAM_L2;
                addr_comp = true;
            }
            else if ((byteorder_ntohl(ipv6_hdr->src.u32[2]) == 0x000000ff) &&
                     (byteorder_ntohs(ipv6_hdr->src.u16[6]) == 0xfe00)) {
                /* 16 bits. The address is derived using 16 bits carried inline */
                iphc_hdr[IPHC2_IDX] |= IPHC_SAC_SAM_16;
                memcpy(iphc_hdr + inline_pos, ipv6_hdr->src.u16 + 7, 2);
                inline_pos += 2;
                addr_comp = true;
            }
            else {
                /* 64 bits. The address is derived using 64 bits carried inline */
                iphc_hdr[IPHC2_IDX] |= IPHC_SAC_SAM_64;
                memcpy(iphc_hdr + inline_pos, ipv6_hdr->src.u64 + 1, 8);
                inline_pos += 8;
                addr_comp = true;
            }
        }

        if (!addr_comp) {
            /* full address is carried inline */
            iphc_hdr[IPHC2_IDX] |= IPHC_SAC_SAM_FULL;
            memcpy(iphc_hdr + inline_pos, &ipv6_hdr->src, 16);
            inline_pos += 16;
        }
    }

    addr_comp = false;

    /* M: Multicast compression */
    if (ipv6_addr_is_multicast(&(ipv6_hdr->dst))) {
        iphc_hdr[IPHC2_IDX] |= SIXLOWPAN_IPHC2_M;

        /* if multicast address is of format ffXX::XXXX:XXXX:XXXX */
        if ((ipv6_hdr->dst.u16[1].u16 == 0) &&
            (ipv6_hdr->dst.u32[1].u32 == 0) &&
            (ipv6_hdr->dst.u16[4].u16 == 0)) {
            /* if multicast address is of format ff02::XX */
            if ((ipv6_hdr->dst.u8[1] == 0x02) &&
                (ipv6_hdr->dst.u32[2].u32 == 0) &&
                (ipv6_hdr->dst.u16[6].u16 == 0) &&
                (ipv6_hdr->dst.u8[14] == 0)) {
                /* 8 bits. The address is derived using 8 bits carried inline */
                iphc_hdr[IPHC2_IDX] |= IPHC_M_DAC_DAM_M_8;
                iphc_hdr[inline_pos++] = ipv6_hdr->dst.u8[15];
                addr_comp = true;
            }
            /* if multicast address is of format ffXX::XX:XXXX */
            else if ((ipv6_hdr->dst.u16[5].u16 == 0) &&
                     (ipv6_hdr->dst.u8[12] == 0)) {
                /* 32 bits. The address is derived using 32 bits carried inline */
                iphc_hdr[IPHC2_IDX] |= IPHC_M_DAC_DAM_M_32;
                iphc_hdr[inline_pos++] = ipv6_hdr->dst.u8[1];
                memcpy(iphc_hdr + inline_pos, ipv6_hdr->dst.u8 + 13, 3);
                inline_pos += 3;
                addr_comp = true;
            }
            /* if multicast address is of format ffXX::XX:XXXX:XXXX */
            else if (ipv6_hdr->dst.u8[10] == 0) {
                /* 48 bits. The address is derived using 48 bits carried inline */
                iphc_hdr[IPHC2_IDX] |= IPHC_M_DAC_DAM_M_48;
                iphc_hdr[inline_pos++] = ipv6_hdr->dst.u8[1];
                memcpy(iphc_hdr + inline_pos, ipv6_hdr->dst.u8 + 11, 5);
                inline_pos += 5;
                addr_comp = true;
            }
        }
        /* try unicast prefix based compression */
        else {
            gnrc_sixlowpan_ctx_t *ctx;
            ipv6_addr_t unicast_prefix;
            unicast_prefix.u16[0] = ipv6_hdr->dst.u16[2];
            unicast_prefix.u16[1] = ipv6_hdr->dst.u16[3];
            unicast_prefix.u16[2] = ipv6_hdr->dst.u16[4];
            unicast_prefix.u16[3] = ipv6_hdr->dst.u16[5];

            ctx = gnrc_sixlowpan_ctx_lookup_addr(&unicast_prefix);

            if ((ctx != NULL) && (ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_COMP) &&
                (ctx->prefix_len == ipv6_hdr->dst.u8[3])) {
                /* Unicast prefix based IPv6 multicast address
                 * (https://tools.ietf.org/html/rfc3306) with given context
                 * for unicast prefix -> context based compression */
                iphc_hdr[IPHC2_IDX] |= SIXLOWPAN_IPHC2_DAC;
                iphc_hdr[inline_pos++] = ipv6_hdr->dst.u8[1];
                iphc_hdr[inline_pos++] = ipv6_hdr->dst.u8[2];
                memcpy(iphc_hdr + inline_pos, ipv6_hdr->dst.u16 + 6, 4);
                inline_pos += 4;
                addr_comp = true;
            }
        }
    }
    else if ((((dst_ctx != NULL) && (dst_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_COMP)) ||
              ipv6_addr_is_link_local(&ipv6_hdr->dst)) && (netif_hdr->dst_l2addr_len > 0)) {
        eui64_t iid;

        ieee802154_get_iid(&iid, gnrc_netif_hdr_get_dst_addr(netif_hdr),
                           netif_hdr->dst_l2addr_len);

        if ((ipv6_hdr->dst.u64[1].u64 == iid.uint64.u64) ||
            _context_overlaps_iid(dst_ctx, &(ipv6_hdr->dst), &iid)) {
            /* 0 bits. The address is derived using the link-layer address */
            iphc_hdr[IPHC2_IDX] |= IPHC_M_DAC_DAM_U_L2;
            addr_comp = true;
        }
        else if ((byteorder_ntohl(ipv6_hdr->dst.u32[2]) == 0x000000ff) &&
                 (byteorder_ntohs(ipv6_hdr->dst.u16[6]) == 0xfe00)) {
            /* 16 bits. The address is derived using 16 bits carried inline */
            iphc_hdr[IPHC2_IDX] |= IPHC_M_DAC_DAM_U_16;
            memcpy(&(iphc_hdr[inline_pos]), &(ipv6_hdr->dst.u16[7]), 2);
            inline_pos += 2;
            addr_comp = true;
        }
        else {
            /* 64 bits. The address is derived using 64 bits carried inline */
            iphc_hdr[IPHC2_IDX] |= IPHC_M_DAC_DAM_U_64;
            memcpy(&(iphc_hdr[inline_pos]), &(ipv6_hdr->dst.u8[8]), 8);
            inline_pos += 8;
            addr_comp = true;
        }
    }

    if (!addr_comp) {
        /* full destination address is carried inline */
        iphc_hdr[IPHC2_IDX] |= IPHC_SAC_SAM_FULL;
        memcpy(iphc_hdr + inline_pos, &ipv6_hdr->dst, 16);
        inline_pos += 16;
    }

    /* shrink dispatch allocation to final size */
    /* NOTE: Since this only shrinks the data nothing bad SHOULD happen ;-) */
    gnrc_pktbuf_realloc_data(dispatch, (size_t)inline_pos);

    /* remove IPv6 header */
    pkt = gnrc_pktbuf_remove_snip(pkt, pkt->next);

    /* insert dispatch into packet */
    dispatch->next = pkt->next;
    pkt->next = dispatch;

    return true;
}

/** @} */
