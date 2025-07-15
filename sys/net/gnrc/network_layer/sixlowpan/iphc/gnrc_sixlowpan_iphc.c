/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de> (nhc udp encoding)
 */

#include <stdbool.h>

#include "byteorder.h"
#include "net/ipv6/hdr.h"
#include "net/ipv6/ext.h"
#include "net/gnrc.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/sixlowpan.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "net/gnrc/sixlowpan/frag/rb.h"
#include "net/gnrc/sixlowpan/frag/minfwd.h"
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_SFR
#include "net/gnrc/sixlowpan/frag/sfr.h"
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_SFR */
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_VRB
#include "net/gnrc/sixlowpan/frag/vrb.h"
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_VRB */
#include "net/gnrc/sixlowpan/internal.h"
#include "net/sixlowpan.h"
#include "utlist.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/udp.h"
#include "od.h"

#include "net/gnrc/sixlowpan/iphc.h"

#define ENABLE_DEBUG 0
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

#define NHC_ID_MASK                 (0xF8)
#define NHC_UDP_ID                  (0xF0)
#define NHC_UDP_PP_MASK             (0x03)
#define NHC_UDP_SD_INLINE           (0x00)
#define NHC_UDP_S_INLINE            (0x01)
#define NHC_UDP_D_INLINE            (0x02)
#define NHC_UDP_SD_ELIDED           (0x03)
#define NHC_UDP_C_ELIDED            (0x04)

#define NHC_UDP_4BIT_PORT           (0xF0B0)
#define NHC_UDP_4BIT_MASK           (0xFFF0)
#define NHC_UDP_8BIT_PORT           (0xF000)
#define NHC_UDP_8BIT_MASK           (0xFF00)

#define NHC_IPV6_EXT_ID             (0xE0)
#define NHC_IPV6_EXT_ID_ALT         (0xE8)  /* first bit of EID 1 */
#define NHC_IPV6_EXT_EID_MASK       (0x0E)
#define NHC_IPV6_EXT_NH             (0x01)

#define NHC_IPV6_EXT_EID_HOPOPT     (0x00 << 1)
#define NHC_IPV6_EXT_EID_RH         (0x01 << 1)
#define NHC_IPV6_EXT_EID_FRAG       (0x02 << 1)
#define NHC_IPV6_EXT_EID_DST        (0x03 << 1)
#define NHC_IPV6_EXT_EID_MOB        (0x04 << 1)
#define NHC_IPV6_EXT_EID_IPV6       (0x07 << 1)

#define SIXLOWPAN_IPHC_PREFIX_LEN   (64)    /**< minimum prefix length for IPHC */

/* currently only used with forwarding output, remove guard if more debug info
 * is added */
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_VRB
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_VRB */

static inline bool _is_rfrag(gnrc_pktsnip_t *sixlo)
{
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_SFR
    assert((sixlo->next != NULL) &&
           (sixlo->next->type == GNRC_NETTYPE_SIXLOWPAN));
    return sixlowpan_sfr_rfrag_is(sixlo->next->data);
#else   /* MODULE_GNRC_SIXLOWPAN_FRAG_SFR */
    (void)sixlo;
    return false;
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_SFR */
}

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

static gnrc_pktsnip_t *_iphc_encode(gnrc_pktsnip_t *pkt,
                                    const gnrc_netif_hdr_t *netif_hdr,
                                    gnrc_netif_t *netif);

#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_VRB
static gnrc_pktsnip_t *_encode_frag_for_forwarding(gnrc_pktsnip_t *decoded_pkt,
                                                   gnrc_sixlowpan_frag_vrb_t *vrbe);
static int _forward_frag(gnrc_pktsnip_t *pkt, gnrc_pktsnip_t *frag_hdr,
                         gnrc_sixlowpan_frag_vrb_t *vrbe, unsigned page);
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_VRB */

static size_t _iphc_ipv6_decode(const uint8_t *iphc_hdr,
                                const gnrc_netif_hdr_t *netif_hdr,
                                gnrc_netif_t *iface, ipv6_hdr_t *ipv6_hdr)
{
    gnrc_sixlowpan_ctx_t *ctx = NULL;
    size_t payload_offset = SIXLOWPAN_IPHC_HDR_LEN;

    if (iphc_hdr[IPHC2_IDX] & SIXLOWPAN_IPHC2_CID_EXT) {
        payload_offset++;
    }

    /* bits of the uncompressed address might not be written in decompression,
     * so zero the whole header first */
    memset(ipv6_hdr, 0, sizeof(*ipv6_hdr));
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

    iface = gnrc_netif_hdr_get_netif(netif_hdr);
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
            if (gnrc_netif_hdr_ipv6_iid_from_src(
                        iface, netif_hdr, (eui64_t *)(&ipv6_hdr->src.u64[1])
                    ) < 0) {
                DEBUG("6lo iphc: could not get source's IID\n");
                return 0;
            }
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
            if (gnrc_netif_hdr_ipv6_iid_from_src(
                        iface, netif_hdr, (eui64_t *)(&ipv6_hdr->src.u64[1])
                    ) < 0) {
                DEBUG("6lo iphc: could not get source's IID\n");
                return 0;
            }
            ipv6_addr_init_prefix(&ipv6_hdr->src, &ctx->prefix,
                                  ctx->prefix_len);
            break;
    }

    if (iphc_hdr[IPHC2_IDX] & SIXLOWPAN_IPHC2_DAC) {
        uint8_t dci = 0;

        if (iphc_hdr[IPHC2_IDX] & SIXLOWPAN_IPHC2_CID_EXT) {
            dci = iphc_hdr[CID_EXT_IDX] & 0x0f;
        }

        if (iphc_hdr[IPHC2_IDX] & (SIXLOWPAN_IPHC2_M | SIXLOWPAN_IPHC2_DAM)) {
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
            if (gnrc_netif_hdr_ipv6_iid_from_dst(
                        iface, netif_hdr, (eui64_t *)(&ipv6_hdr->dst.u64[1])
                    ) < 0) {
                DEBUG("6lo iphc: could not get destination's IID\n");
                return 0;
            }
            ipv6_addr_set_link_local_prefix(&ipv6_hdr->dst);
            break;

        case IPHC_M_DAC_DAM_U_CTX_64:
            assert(ctx != NULL);
            memcpy(ipv6_hdr->dst.u8 + 8, iphc_hdr + payload_offset, 8);
            ipv6_addr_init_prefix(&ipv6_hdr->dst, &ctx->prefix,
                                  ctx->prefix_len);
            payload_offset += 8;
            break;

        case IPHC_M_DAC_DAM_U_CTX_16:
            ipv6_hdr->dst.u32[2] = byteorder_htonl(0x000000ff);
            ipv6_hdr->dst.u16[6] = byteorder_htons(0xfe00);
            memcpy(ipv6_hdr->dst.u8 + 14, iphc_hdr + payload_offset, 2);
            assert(ctx != NULL);
            ipv6_addr_init_prefix(&ipv6_hdr->dst, &ctx->prefix,
                                  ctx->prefix_len);
            payload_offset += 2;
            break;

        case IPHC_M_DAC_DAM_U_CTX_L2:
            if (gnrc_netif_hdr_ipv6_iid_from_dst(
                        iface, netif_hdr, (eui64_t *)(&ipv6_hdr->dst.u64[1])
                    ) < 0) {
                DEBUG("6lo iphc: could not get destination's IID\n");
                return 0;
            }
            assert(ctx != NULL);
            ipv6_addr_init_prefix(&ipv6_hdr->dst, &ctx->prefix,
                                  ctx->prefix_len);
            break;

        case IPHC_M_DAC_DAM_M_48:
            /* ffXX::00XX:XXXX:XXXX */
            ipv6_addr_set_unspecified(&ipv6_hdr->dst);
            ipv6_hdr->dst.u8[0] = 0xff;
            ipv6_hdr->dst.u8[1] = iphc_hdr[payload_offset++];
            memcpy(ipv6_hdr->dst.u8 + 11, iphc_hdr + payload_offset, 5);
            payload_offset += 5;
            break;

        case IPHC_M_DAC_DAM_M_32:
            /* ffXX::00XX:XXXX */
            ipv6_addr_set_unspecified(&ipv6_hdr->dst);
            ipv6_hdr->dst.u8[0] = 0xff;
            ipv6_hdr->dst.u8[1] = iphc_hdr[payload_offset++];
            memcpy(ipv6_hdr->dst.u8 + 13, iphc_hdr + payload_offset, 3);
            payload_offset += 3;
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
                assert(ctx != NULL);
                uint8_t orig_ctx_len = ctx->prefix_len;

                ipv6_addr_set_unspecified(&ipv6_hdr->dst);

                if (ctx->prefix_len > 64) {
                    ctx->prefix_len = 64;
                }

                ipv6_hdr->dst.u8[0] = 0xff;
                ipv6_hdr->dst.u8[1] = iphc_hdr[payload_offset++];
                ipv6_hdr->dst.u8[2] = iphc_hdr[payload_offset++];
                ipv6_hdr->dst.u8[3] = ctx->prefix_len;
                ipv6_addr_init_prefix((ipv6_addr_t *)(ipv6_hdr->dst.u8 + 4),
                                      &ctx->prefix, ctx->prefix_len);
                memcpy(ipv6_hdr->dst.u8 + 12, iphc_hdr + payload_offset + 2, 4);

                payload_offset += 4;
                ctx->prefix_len = orig_ctx_len;
            } while (0);    /* ANSI-C compatible block creation for orig_ctx_len allocation */
            break;

        default:
            DEBUG("6lo iphc: unspecified or reserved M, DAC, DAM combination\n");
            break;
    }
    return payload_offset;
}

#ifdef MODULE_GNRC_SIXLOWPAN_IPHC_NHC
static size_t _iphc_nhc_ipv6_ext_decode(gnrc_pktsnip_t *sixlo, size_t offset,
                                        size_t *prev_nh_offset,
                                        gnrc_pktsnip_t *ipv6,
                                        size_t *uncomp_hdr_len)
{
    uint8_t *payload = sixlo->data;
    ipv6_ext_t *ext_hdr;
    uint8_t ipv6_ext_nhc = payload[offset++];
    uint8_t protnum;
    uint8_t ext_len = (ipv6_ext_nhc & NHC_IPV6_EXT_NH)
                    ? payload[offset]
                    : payload[offset + 1];

    /* realloc size for uncompressed snip, if too small */
    if (ipv6->size < (*uncomp_hdr_len + sizeof(ipv6_ext_t) + ext_len)) {
        if (gnrc_pktbuf_realloc_data(ipv6,
                                     *uncomp_hdr_len + sizeof(ipv6_ext_t) +
                                     ext_len)) {
            DEBUG("6lo iphc: unable to decode IPv6 Extension header NHC "
                  "(not enough buffer space)\n");
            return 0;
        }
    }
    ext_hdr = (ipv6_ext_t *)((uint8_t *)ipv6->data + *uncomp_hdr_len);
    switch (ipv6_ext_nhc & NHC_IPV6_EXT_EID_MASK) {
        case NHC_IPV6_EXT_EID_HOPOPT:
            protnum = PROTNUM_IPV6_EXT_HOPOPT;
            break;
#ifndef MODULE_GNRC_RPL_SR
        case NHC_IPV6_EXT_EID_RH:
            protnum = PROTNUM_IPV6_EXT_RH;
            break;
#endif
        case NHC_IPV6_EXT_EID_FRAG:
            protnum = PROTNUM_IPV6_EXT_FRAG;
            break;
        case NHC_IPV6_EXT_EID_DST:
            protnum = PROTNUM_IPV6_EXT_DST;
            break;
        case NHC_IPV6_EXT_EID_MOB:
            protnum = PROTNUM_IPV6_EXT_MOB;
            break;
        default:
            DEBUG("6lo iphc: unexpected extension header EID %u\n",
                  (ipv6_ext_nhc & NHC_IPV6_EXT_EID_MASK) >> 1U);
            return 0;
    }
    ((uint8_t *)ipv6->data)[*prev_nh_offset] = protnum;
    if (!(ipv6_ext_nhc & NHC_IPV6_EXT_NH)) {
        ext_hdr->nh = payload[offset++];
        /* signal end of next header compression to caller */
        *prev_nh_offset = 0;
    }
    else {
        *prev_nh_offset = (&ext_hdr->nh) - ((uint8_t *)ipv6->data);
    }
    /* skip already fetched length field */
    offset++;
    ext_hdr->len = ((sizeof(ipv6_ext_t) + ext_len) - IPV6_EXT_LEN_UNIT) /
                   IPV6_EXT_LEN_UNIT;
    memcpy(ext_hdr + 1, &payload[offset], ext_len);
    offset += ext_len;
    *uncomp_hdr_len += sizeof(ipv6_ext_t) + ext_len;
    return offset;
}

static size_t _iphc_nhc_ipv6_decode(gnrc_pktsnip_t *sixlo, size_t offset,
                                    const gnrc_sixlowpan_frag_rb_t *rbuf,
                                    size_t *prev_nh_offset,
                                    gnrc_pktsnip_t *ipv6,
                                    size_t *uncomp_hdr_len)
{
    uint8_t *payload = sixlo->data;
    uint8_t ipv6_nhc = payload[offset];

    switch (ipv6_nhc & NHC_IPV6_EXT_EID_MASK) {
        case NHC_IPV6_EXT_EID_HOPOPT:
        #ifndef MODULE_GNRC_RPL_SR
        case NHC_IPV6_EXT_EID_RH:
        #endif
        case NHC_IPV6_EXT_EID_FRAG:
        case NHC_IPV6_EXT_EID_DST:
        case NHC_IPV6_EXT_EID_MOB: {
            size_t tmp;
            tmp = _iphc_nhc_ipv6_ext_decode(sixlo, offset, prev_nh_offset,
                                            ipv6, uncomp_hdr_len);
            if (tmp == 0) {
                /* unable to parse IPHC header */
                return 0;
            }
            offset = tmp;
            break;
        }
        case NHC_IPV6_EXT_EID_IPV6: {
            gnrc_pktsnip_t *netif = gnrc_pktsnip_search_type(sixlo,
                                                             GNRC_NETTYPE_NETIF);
            ipv6_hdr_t *ipv6_hdr;
            uint16_t payload_len;
            size_t tmp;

            if (netif == NULL) {
                DEBUG("6lo iphc: unable to find NETIF snip\n");
                return 0;
            }

            offset++;   /* move over NHC header */
            /* realloc size for uncompressed snip, if too small */
            if (ipv6->size < (*uncomp_hdr_len + sizeof(ipv6_hdr_t))) {
                if (gnrc_pktbuf_realloc_data(ipv6,
                                             *uncomp_hdr_len +
                                             sizeof(ipv6_hdr_t))) {
                    DEBUG("6lo iphc: unable to decode IPv6 encapsulated header "
                          "NHC (not enough buffer space)\n");
                    return 0;
                }
            }
            ipv6_hdr = (ipv6_hdr_t *)(((uint8_t *)ipv6->data) + *uncomp_hdr_len);
            tmp = _iphc_ipv6_decode(&payload[offset], netif->data,
                                    gnrc_netif_hdr_get_netif(netif->data),
                                    ipv6_hdr);
            if (tmp == 0) {
                /* unable to parse IPHC header */
                return 0;
            }
            ((uint8_t *)ipv6->data)[*prev_nh_offset] = PROTNUM_IPV6;
            if (payload[offset + IPHC1_IDX] & SIXLOWPAN_IPHC1_NH) {
                *prev_nh_offset = (&ipv6_hdr->nh) - ((uint8_t *)ipv6->data);
            }
            else {
                /* signal end of next header compression to caller */
                *prev_nh_offset = 0;
            }
            offset += tmp;
            /* might be needed to be overwritten by IPv6 reassembly after the IPv6
             * packet was reassembled to get complete length */
            if (rbuf != NULL) {
                if (_is_rfrag(sixlo)) {
                    payload_len = (rbuf->super.datagram_size + *uncomp_hdr_len) -
                                  (sizeof(ipv6_hdr_t) - offset);
                }
                else {
                    payload_len = rbuf->super.datagram_size - *uncomp_hdr_len -
                                  sizeof(ipv6_hdr_t);
                }
            }
            else {
                payload_len = (sixlo->size + *uncomp_hdr_len) -
                              sizeof(ipv6_hdr_t) - offset;
            }
            ipv6_hdr->len = byteorder_htons(payload_len);
            *uncomp_hdr_len += sizeof(ipv6_hdr_t);
            break;
        }
        default:
            DEBUG("6lo iphc: unknown IPv6 extension header EID\n");
            break;
    }
    return offset;
}

/**
 * @brief   Decodes UDP NHC
 *
 * @param[in] sixlo                 The IPHC encoded packet
 * @param[in] offset                The offset of the NHC encoded header
 * @param[in] rbuf                  Reassembly buffer entry if @p ipv6 is a
 *                                  fragmented datagram. May be NULL, if @p ipv6
 *                                  is not fragmented
 * @param[out] prev_nh_offset       Offset to previous nh field in
 *                                  gnrc_pktsnip_t::data of @p ipv6
 * @param[out] ipv6                 The packet to write the decoded data to
 * @param[in,out] uncomp_hdr_len    Number of bytes already decoded into @p ipv6
 *                                  by IPHC and other NHC. Adds size of @ref
 *                                  udp_hdr_t after successful UDP header
 *                                  decompression
 *
 * @return  The offset after UDP NHC header on success.
 * @return  0 on error.
 */
static size_t _iphc_nhc_udp_decode(gnrc_pktsnip_t *sixlo, size_t offset,
                                   const gnrc_sixlowpan_frag_rb_t *rbuf,
                                   size_t prev_nh_offset, gnrc_pktsnip_t *ipv6,
                                   size_t *uncomp_hdr_len)
{
    uint8_t *payload = sixlo->data;
    udp_hdr_t *udp_hdr;
    uint16_t payload_len;
    uint8_t udp_nhc = payload[offset++];
    uint8_t tmp;

    /* realloc size for uncompressed snip, if too small */
    if (ipv6->size < (*uncomp_hdr_len + sizeof(udp_hdr_t))) {
        if (gnrc_pktbuf_realloc_data(ipv6,
                                     *uncomp_hdr_len + sizeof(udp_hdr_t))) {
            DEBUG("6lo: unable to decode UDP NHC (not enough buffer space)\n");
            return 0;
        }
    }
    udp_hdr = (udp_hdr_t *)((uint8_t *)ipv6->data + *uncomp_hdr_len);
    network_uint16_t *src_port = &(udp_hdr->src_port);
    network_uint16_t *dst_port = &(udp_hdr->dst_port);

    switch (udp_nhc & NHC_UDP_PP_MASK) {

        case NHC_UDP_SD_INLINE:
            DEBUG("6lo iphc nhc: SD_INLINE\n");
            src_port->u8[0] = payload[offset++];
            src_port->u8[1] = payload[offset++];
            dst_port->u8[0] = payload[offset++];
            dst_port->u8[1] = payload[offset++];
            break;

        case NHC_UDP_S_INLINE:
            DEBUG("6lo iphc nhc: S_INLINE\n");
            src_port->u8[0] = payload[offset++];
            src_port->u8[1] = payload[offset++];
            *dst_port = byteorder_htons(payload[offset++] + NHC_UDP_8BIT_PORT);
            break;

        case NHC_UDP_D_INLINE:
            DEBUG("6lo iphc nhc: D_INLINE\n");
            *src_port = byteorder_htons(payload[offset++] + NHC_UDP_8BIT_PORT);
            dst_port->u8[0] = payload[offset++];
            dst_port->u8[1] = payload[offset++];
            break;

        case NHC_UDP_SD_ELIDED:
            DEBUG("6lo iphc nhc: SD_ELIDED\n");
            tmp = payload[offset++];
            *src_port = byteorder_htons((tmp >> 4) + NHC_UDP_4BIT_PORT);
            *dst_port = byteorder_htons((tmp & 0xf) + NHC_UDP_4BIT_PORT);
            break;

        default:
            break;
    }

    if ((udp_nhc & NHC_UDP_C_ELIDED) != 0) {
        DEBUG("6lo iphc nhc: unsupported elided checksum\n");
        return 0;
    }
    else {
        udp_hdr->checksum.u8[0] = payload[offset++];
        udp_hdr->checksum.u8[1] = payload[offset++];
    }

    /* might be needed to be overwritten by IPv6 reassembly after the IPv6
     * packet was reassembled to get complete length */
    if (rbuf != NULL) {
        if (_is_rfrag(sixlo)) {
            payload_len = rbuf->super.datagram_size + sizeof(udp_hdr_t) -
                          offset;
        }
        else {
            payload_len = rbuf->super.datagram_size - *uncomp_hdr_len;
        }
    }
    else {
        payload_len = sixlo->size + sizeof(udp_hdr_t) - offset;
    }
    udp_hdr->length = byteorder_htons(payload_len);
    *uncomp_hdr_len += sizeof(udp_hdr_t);
    ((uint8_t *)ipv6->data)[prev_nh_offset] = PROTNUM_UDP;

    return offset;
}
#endif

static inline void _recv_error_release(gnrc_pktsnip_t *sixlo,
                                       gnrc_pktsnip_t *ipv6,
                                       gnrc_sixlowpan_frag_rb_t *rbuf) {
    if (rbuf != NULL) {
        gnrc_sixlowpan_frag_rb_remove(rbuf);
    }
    gnrc_pktbuf_release(ipv6);
    gnrc_pktbuf_release(sixlo);
}

void gnrc_sixlowpan_iphc_recv(gnrc_pktsnip_t *sixlo, void *rbuf_ptr,
                              unsigned page)
{
    assert(sixlo != NULL);
    gnrc_pktsnip_t *ipv6, *netif;
    gnrc_netif_t *iface;
    ipv6_hdr_t *ipv6_hdr;
    uint8_t *iphc_hdr = sixlo->data;
    size_t payload_offset;
    size_t uncomp_hdr_len = sizeof(ipv6_hdr_t);
    gnrc_sixlowpan_frag_rb_t *rbuf = rbuf_ptr;
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_VRB
    gnrc_sixlowpan_frag_vrb_t *vrbe = NULL;
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_VRB */

    if (sixlo->size < 2U) {
        DEBUG("6lo iphc: IPHC header truncated\n");
        if (rbuf != NULL) {
            gnrc_sixlowpan_frag_rb_remove(rbuf);
        }
        gnrc_pktbuf_release(sixlo);
        return;
    }
    if (rbuf != NULL) {
        ipv6 = rbuf->pkt;
        assert(ipv6 != NULL);
        if ((ipv6->size < sizeof(ipv6_hdr_t)) &&
            (gnrc_pktbuf_realloc_data(ipv6, sizeof(ipv6_hdr_t)) != 0)) {
            DEBUG("6lo iphc: no space to decompress IPHC\n");
            _recv_error_release(sixlo, ipv6, rbuf);
            return;
        }
    }
    else {
        ipv6 = gnrc_pktbuf_add(NULL, NULL, sizeof(ipv6_hdr_t),
                               GNRC_NETTYPE_IPV6);
        if (ipv6 == NULL) {
            gnrc_pktbuf_release(sixlo);
            return;
        }
    }

    assert(ipv6->size >= sizeof(ipv6_hdr_t));

    netif = gnrc_pktsnip_search_type(sixlo, GNRC_NETTYPE_NETIF);
    assert(netif != NULL);
    iface = gnrc_netif_hdr_get_netif(netif->data);
    payload_offset = _iphc_ipv6_decode(iphc_hdr, netif->data, iface,
                                       ipv6->data);
    if ((payload_offset == 0) || (payload_offset > sixlo->size)) {
        /* unable to parse IPHC header or malicious packet */
        DEBUG("6lo iphc: malformed IPHC header\n");
        _recv_error_release(sixlo, ipv6, rbuf);
        return;
    }
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC_NHC
    if (iphc_hdr[IPHC1_IDX] & SIXLOWPAN_IPHC1_NH) {
        bool nhc_header = true;
        ipv6_hdr = ipv6->data;
        size_t prev_nh_offset = (&ipv6_hdr->nh) - ((uint8_t *)ipv6->data);

        while (nhc_header) {
            switch (iphc_hdr[payload_offset] & NHC_ID_MASK) {
                case NHC_IPV6_EXT_ID:
                case NHC_IPV6_EXT_ID_ALT:
                    payload_offset = _iphc_nhc_ipv6_decode(sixlo,
                                                           payload_offset,
                                                           rbuf,
                                                           &prev_nh_offset,
                                                           ipv6,
                                                           &uncomp_hdr_len);
                    if ((payload_offset == 0) || (payload_offset > sixlo->size)) {
                        /* unable to parse IPHC header or malicious packet */
                        DEBUG("6lo iphc: malformed IPHC NHC IPv6 header\n");
                        _recv_error_release(sixlo, ipv6, rbuf);
                        return;
                    }
                    /* prev_nh_offset is set to 0 if next header is not
                     * compressed (== NH flag in compression header not set) */
                    nhc_header = (prev_nh_offset > 0);
                    break;
                case NHC_UDP_ID: {
                    payload_offset = _iphc_nhc_udp_decode(sixlo,
                                                          payload_offset,
                                                          rbuf,
                                                          prev_nh_offset,
                                                          ipv6,
                                                          &uncomp_hdr_len);
                    if ((payload_offset == 0) || (payload_offset > sixlo->size)) {
                        /* unable to parse IPHC header or malicious packet */
                        DEBUG("6lo iphc: malformed IPHC NHC IPv6 header\n");
                        _recv_error_release(sixlo, ipv6, rbuf);
                        return;
                    }
                    /* no NHC after UDP header */
                    nhc_header = false;
                    break;
                }
                default:
                    nhc_header = false;
                    break;
            }
        }
    }
#endif
    uint16_t payload_len;
    if (rbuf != NULL) {
        /* for a fragmented datagram we know the overall length already */
        if (_is_rfrag(sixlo)) {
            DEBUG("6lo iphc: calculating payload length for SFR\n");
            DEBUG(" - rbuf->super.datagram_size: %u\n",
                  rbuf->super.datagram_size);
            DEBUG(" - payload_offset: %" PRIuSIZE "\n", payload_offset);
            DEBUG(" - uncomp_hdr_len: %" PRIuSIZE "\n", uncomp_hdr_len);
            /* set IPv6 header payload length field to the length of whatever is
             * left after removing the 6LoWPAN header and adding uncompressed
             * headers */
            payload_len = (rbuf->super.datagram_size - payload_offset) +
                          (uncomp_hdr_len - sizeof(ipv6_hdr_t));
            DEBUG("   => %u\n", payload_len);
            /* adapt datagram size for uncompressed datagram */
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_SFR
            /* guard required because SFR-specific field of vrbe is accessed */
            rbuf->offset_diff += (uncomp_hdr_len - payload_offset);
            rbuf->super.datagram_size += rbuf->offset_diff;
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_VRB */
        }
        else {
            /* for a fragmented datagram we know the overall length already */
            payload_len = (uint16_t)(rbuf->super.datagram_size - sizeof(ipv6_hdr_t));
        }
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_VRB
        /* re-assign IPv6 header in case realloc changed the address */
        ipv6_hdr = ipv6->data;
        DEBUG("6lo iphc: VRB present, trying to create entry for dst %s\n",
              ipv6_addr_to_str(addr_str, &ipv6_hdr->dst, sizeof(addr_str)));
        /* only create virtual reassembly buffer entry from IPv6 destination if
         * the current first fragment is the only received fragment in the
         * reassembly buffer so far and the hop-limit is larger than 1
         */
        if ((rbuf->super.current_size <= sixlo->size) && (ipv6_hdr->hl > 1U) &&
            /* and there is enough slack for changing compression */
            (rbuf->super.current_size <= iface->sixlo.max_frag_size) &&
            (vrbe = gnrc_sixlowpan_frag_vrb_from_route(&rbuf->super, iface,
                                                       ipv6))) {
            /* add netif header to `ipv6` so its flags can be used when
             * forwarding the fragment */
            sixlo = gnrc_pkt_delete(sixlo, netif);
            ipv6 = gnrc_pkt_append(ipv6, netif);
            /* provide space to copy remaining payload */
            if (gnrc_pktbuf_realloc_data(ipv6, uncomp_hdr_len + sixlo->size -
                                         payload_offset) != 0) {
                DEBUG("6lo iphc: no space left to copy payload\n");
                gnrc_sixlowpan_frag_vrb_rm(vrbe);
                _recv_error_release(sixlo, ipv6, rbuf);
                return;
            }
        }
        /* reallocate to copy complete payload */
        else if (gnrc_pktbuf_realloc_data(ipv6, rbuf->super.datagram_size) != 0) {
            DEBUG("6lo iphc: no space left to reassemble payload\n");
            _recv_error_release(sixlo, ipv6, rbuf);
            return;
        }
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_VRB */
    }
    else {
        /* set IPv6 header payload length field to the length of whatever is left
         * after removing the 6LoWPAN header and adding uncompressed headers */
        payload_len = (sixlo->size + uncomp_hdr_len -
                       payload_offset - sizeof(ipv6_hdr_t));
    }
    if (rbuf == NULL) {
        /* (rbuf == NULL) => forwarding is not affected by this */
        if (gnrc_pktbuf_realloc_data(ipv6, uncomp_hdr_len + payload_len) != 0) {
            DEBUG("6lo iphc: no space left to copy payload\n");
            _recv_error_release(sixlo, ipv6, rbuf);
            return;
        }
    }
    else {
        if (ipv6->size < (uncomp_hdr_len + (sixlo->size - payload_offset))) {
            DEBUG("6lo iphc: not enough space to copy payload.\n");
            DEBUG("6lo iphc: potentially malicious datagram size received.\n");
            _recv_error_release(sixlo, ipv6, rbuf);
            return;
        }
    }

    /* re-assign IPv6 header in case realloc changed the address */
    ipv6_hdr = ipv6->data;
    ipv6_hdr->len = byteorder_htons(payload_len);
    if (sixlo->size > payload_offset) {
        memcpy(((uint8_t *)ipv6->data) + uncomp_hdr_len,
               ((uint8_t *)sixlo->data) + payload_offset,
               sixlo->size - payload_offset);
    }
    if (rbuf != NULL) {
        rbuf->super.current_size += (uncomp_hdr_len - payload_offset);
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_VRB
        if (vrbe != NULL) {
            int res = -1;
            DEBUG("6lo iphc: found route, trying to forward\n");
            ipv6_hdr->hl--;
            vrbe->super.current_size = rbuf->super.current_size;
            if ((ipv6 = _encode_frag_for_forwarding(ipv6, vrbe))) {
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_SFR
                /* guard required because SFR-specific field of vrbe is
                 * accessed */
                if (_is_rfrag(sixlo)) {
                    vrbe->in_netif = iface;
                    /* calculate offset difference due to compression */
                    vrbe->offset_diff = ((int)gnrc_pkt_len(ipv6->next)) -
                                        sixlo->size;
                }
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_SFR */
                if ((res = _forward_frag(ipv6, sixlo->next, vrbe, page)) == 0) {
                    DEBUG("6lo iphc: successfully recompressed and forwarded "
                          "1st fragment\n");
                    /* empty list, as it should be in VRB now */
                    rbuf->super.ints = NULL;
                }
            }
            if ((ipv6 == NULL) || (res < 0)) {
                /* TODO: There is a potential to fall-back to classic reassembly
                 * when ipv6 != NULL. However, since `ipv6` was reversed in
                 * `_encode_frag_for_forwarding`, that step needs to be reversed
                 * or a version of the old ipv6 needs to be held in the buffer.
                 * For now, just drop the packet all together in an error case
                 */
                gnrc_sixlowpan_frag_vrb_rm(vrbe);
            }
            gnrc_pktbuf_release(sixlo);
            /* don't remove `rbuf->pkt` (aka ipv6) as it was forwarded */
            gnrc_sixlowpan_frag_rb_remove(rbuf);
            return;
        }
        DEBUG("6lo iphc: no route found, reassemble datagram normally\n");
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_VRB */
    }
    else {
        sixlo = gnrc_pkt_delete(sixlo, netif);
        ipv6 = gnrc_pkt_append(ipv6, netif);
        gnrc_sixlowpan_dispatch_recv(ipv6, NULL, page);
    }
    gnrc_pktbuf_release(sixlo);
    return;
}

#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_VRB
static gnrc_pktsnip_t *_encode_frag_for_forwarding(gnrc_pktsnip_t *decoded_pkt,
                                                   gnrc_sixlowpan_frag_vrb_t *vrbe)
{
    gnrc_pktsnip_t *res;
    gnrc_netif_hdr_t *netif_hdr;

    /* mark IPv6 header to allow for next header compression */
    res = gnrc_pktbuf_mark(decoded_pkt, sizeof(ipv6_hdr_t), GNRC_NETTYPE_IPV6);
    if (res == NULL) {
        DEBUG("6lo iphc: unable to mark IPv6 header for forwarding\n");
        gnrc_pktbuf_release(decoded_pkt);
        return NULL;
    }
    res = gnrc_pktbuf_reverse_snips(decoded_pkt);
    if (res == NULL) {
        DEBUG("6lo iphc: unable to reverse packet for forwarding\n");
        /* decoded_pkt is released in gnrc_pktbuf_reverse_snips() */
        return NULL;
    }
    /* set netif header from VRB for correct encoding */
    netif_hdr = res->data;
    /* _iphc_encode only checks the destination address, so leave src
     * untouched */
    netif_hdr->dst_l2addr_len = vrbe->super.dst_len;
    gnrc_netif_hdr_set_dst_addr(netif_hdr, vrbe->super.dst,
                                vrbe->super.dst_len);
    gnrc_netif_hdr_set_netif(netif_hdr, vrbe->out_netif);
    decoded_pkt = res;
    if ((res = _iphc_encode(decoded_pkt, netif_hdr, vrbe->out_netif))) {
        return res;
    }
    else {
        DEBUG("6lo iphc: unable to compress packet for forwarding\n");
        gnrc_pktbuf_release(decoded_pkt);
        return NULL;
    }
}

static int _forward_frag(gnrc_pktsnip_t *pkt, gnrc_pktsnip_t *frag_hdr,
                         gnrc_sixlowpan_frag_vrb_t *vrbe, unsigned page)
{
    /* remove rewritten netif header (forwarding implementation must do this
     * anyway) */
    pkt = gnrc_pktbuf_remove_snip(pkt, pkt);
    if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_MINFWD) &&
        sixlowpan_frag_is(frag_hdr->data)) {
        return gnrc_sixlowpan_frag_minfwd_forward(pkt, frag_hdr->data, vrbe,
                                                  page);
    }
    /* the following is just debug output for testing without any forwarding
     * scheme */
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_SFR
    if (sixlowpan_sfr_rfrag_is(frag_hdr->data)) {
        return gnrc_sixlowpan_frag_sfr_forward(pkt, frag_hdr->data, vrbe, page);
    }
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_SFR */
    DEBUG("6lo iphc: Do not know how to forward fragment from (%s, %u) ",
          gnrc_netif_addr_to_str(vrbe->super.src, vrbe->super.src_len,
                                 addr_str), vrbe->super.tag);
    DEBUG("to (%s, %u)\n",
          gnrc_netif_addr_to_str(vrbe->super.dst, vrbe->super.dst_len,
                                 addr_str), vrbe->out_tag);
    if (IS_ACTIVE(ENABLE_DEBUG) && IS_USED(MODULE_OD)) {
        DEBUG("Original fragmentation header:\n");
        od_hex_dump(frag_hdr->data, frag_hdr->size, OD_WIDTH_DEFAULT);
        DEBUG("IPHC headers + payload:\n");
        frag_hdr = pkt;
        while (frag_hdr) {
            od_hex_dump(frag_hdr->data, frag_hdr->size, OD_WIDTH_DEFAULT);
            frag_hdr = frag_hdr->next;
        }
    }
    gnrc_pktbuf_release(pkt);
    (void)frag_hdr;
    (void)page;
    return -ENOTSUP;
}
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_VRB */

static inline bool _compressible_nh(uint8_t nh)
{
    switch (nh) {
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC_NHC
        case PROTNUM_IPV6_EXT_HOPOPT:
        case PROTNUM_UDP:
        case PROTNUM_IPV6:
#ifndef MODULE_GNRC_RPL_SR
        case PROTNUM_IPV6_EXT_RH:
#endif
        case PROTNUM_IPV6_EXT_FRAG:
        case PROTNUM_IPV6_EXT_DST:
        case PROTNUM_IPV6_EXT_MOB:
            return true;
#endif
        default:
            return false;
    }
}

static size_t _iphc_ipv6_encode(gnrc_pktsnip_t *pkt,
                                const gnrc_netif_hdr_t *netif_hdr,
                                gnrc_netif_t *iface,
                                uint8_t *iphc_hdr)
{
    gnrc_sixlowpan_ctx_t *src_ctx = NULL, *dst_ctx = NULL;
    ipv6_hdr_t *ipv6_hdr;
    bool addr_comp = false;
    uint16_t inline_pos = SIXLOWPAN_IPHC_HDR_LEN;

    assert(iface != NULL);

    if (pkt->next == NULL) {
        DEBUG("6lo iphc: packet missing header\n");
        return 0;
    }
    ipv6_hdr = pkt->next->data;

    /* set initial dispatch value*/
    iphc_hdr[IPHC1_IDX] = SIXLOWPAN_IPHC1_DISP;
    iphc_hdr[IPHC2_IDX] = 0;

    /* check for available contexts */
    if (!ipv6_addr_is_unspecified(&(ipv6_hdr->src))) {
        src_ctx = gnrc_sixlowpan_ctx_lookup_addr(&(ipv6_hdr->src));
        /* do not use source context for compression if */
        /* GNRC_SIXLOWPAN_CTX_FLAGS_COMP is not set */
        if (src_ctx && !(src_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_COMP)) {
            src_ctx = NULL;
        }
        /* prefix bits not covered by context information must be zero */
        if (src_ctx &&
            ipv6_addr_match_prefix(&src_ctx->prefix, &ipv6_hdr->src) < SIXLOWPAN_IPHC_PREFIX_LEN) {
            src_ctx = NULL;
        }
    }

    if (!ipv6_addr_is_multicast(&ipv6_hdr->dst)) {
        dst_ctx = gnrc_sixlowpan_ctx_lookup_addr(&(ipv6_hdr->dst));
        /* do not use destination context for compression if */
        /* GNRC_SIXLOWPAN_CTX_FLAGS_COMP is not set */
        if (dst_ctx && !(dst_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_COMP)) {
            dst_ctx = NULL;
        }
        /* prefix bits not covered by context information must be zero */
        if (dst_ctx &&
            ipv6_addr_match_prefix(&dst_ctx->prefix, &ipv6_hdr->dst) < SIXLOWPAN_IPHC_PREFIX_LEN) {
            dst_ctx = NULL;
        }
    }

    /* if contexts available and both != 0 */
    /* since this moves inline_pos we have to do this ahead*/
    if (((src_ctx != NULL) &&
            ((src_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK) != 0)) ||
        ((dst_ctx != NULL) &&
            ((dst_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK) != 0))) {
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

        /* copy remaining bytes of flow label */
        iphc_hdr[inline_pos++] = (uint8_t)((ipv6_hdr_get_fl(ipv6_hdr) & 0x0000ff00) >> 8);
        iphc_hdr[inline_pos++] = (uint8_t)(ipv6_hdr_get_fl(ipv6_hdr) & 0x000000ff);
    }

    /* check for compressible next header */
    if (_compressible_nh(ipv6_hdr->nh)) {
        iphc_hdr[IPHC1_IDX] |= SIXLOWPAN_IPHC1_NH;
    }
    else {
        iphc_hdr[inline_pos++] = ipv6_hdr->nh;
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
        if (src_ctx != NULL) {
            /* stateful source address compression */
            iphc_hdr[IPHC2_IDX] |= SIXLOWPAN_IPHC2_SAC;

            if (((src_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK) != 0)) {
                iphc_hdr[CID_EXT_IDX] |= ((src_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK) << 4);
            }
        }

        if ((src_ctx != NULL) || ipv6_addr_is_link_local(&(ipv6_hdr->src))) {
            eui64_t iid;
            iid.uint64.u64 = 0;

            gnrc_netif_acquire(iface);
            if (gnrc_netif_ipv6_get_iid(iface, &iid) < 0) {
                DEBUG("6lo iphc: could not get interface's IID\n");
                gnrc_netif_release(iface);
                return 0;
            }
            gnrc_netif_release(iface);

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
                if ((ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK) != 0) {
                    iphc_hdr[CID_EXT_IDX] |= (ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK);
                }
                iphc_hdr[inline_pos++] = ipv6_hdr->dst.u8[1];
                iphc_hdr[inline_pos++] = ipv6_hdr->dst.u8[2];
                memcpy(iphc_hdr + inline_pos, ipv6_hdr->dst.u16 + 6, 4);
                inline_pos += 4;
                addr_comp = true;
            }
        }
    }
    else if (((dst_ctx != NULL) ||
              ipv6_addr_is_link_local(&ipv6_hdr->dst)) && (netif_hdr->dst_l2addr_len > 0)) {
        eui64_t iid;

        if (dst_ctx != NULL) {
            /* stateful destination address compression */
            iphc_hdr[IPHC2_IDX] |= SIXLOWPAN_IPHC2_DAC;

            if (((dst_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK) != 0)) {
                iphc_hdr[CID_EXT_IDX] |= (dst_ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK);
            }
        }

        if (gnrc_netif_hdr_ipv6_iid_from_dst(iface, netif_hdr, &iid) < 0) {
            DEBUG("6lo iphc: could not get destination's IID\n");
            return 0;
        }

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

    return inline_pos;
}

#ifdef MODULE_GNRC_SIXLOWPAN_IPHC_NHC
static ssize_t _iphc_nhc_ipv6_ext_encode(uint8_t *nhc_data,
                                        const gnrc_pktsnip_t *ext,
                                        uint16_t ext_len,
                                        uint8_t *protnum)
{
    const ipv6_ext_t *ext_hdr = ext->data;
    size_t nhc_len = 1; /* skip over NHC header */
    uint8_t nh = ext_hdr->nh;

    /* From https://tools.ietf.org/html/rfc6282#section-4.1:
     * > The Length field contained in a compressed IPv6 Extension Header
     * > indicates the number of octets that pertain to the (compressed)
     * > extension header following the Length field.
     *
     * ipv6_ext_t is nh + length field so subtract it
     */
    ext_len -= sizeof(ipv6_ext_t);
    if (ext_len > UINT8_MAX) {
        /* From https://tools.ietf.org/html/rfc6282#section-4.1:
         * > Note that specifying units in octets means that LOWPAN_NHC MUST NOT
         * > be used to encode IPv6 Extension Headers that have more than 255
         * > octets following the Length field after compression. */
        return 0;
    }
    /* Set IPv6 extension compression header type
     * (see https://tools.ietf.org/html/rfc6282#section-4.2). */
    nhc_data[0] = NHC_IPV6_EXT_ID;
    switch (*protnum) {
        case PROTNUM_IPV6_EXT_HOPOPT:
            nhc_data[0] |= NHC_IPV6_EXT_EID_HOPOPT;
            /* TODO: decrement ext_len by length of trailing Pad1/PadN option:
             * > IPv6 Hop-by-Hop and Destination Options Headers may use a trailing
             * > Pad1 or PadN to achieve 8-octet alignment.  When there is a single
             * > trailing Pad1 or PadN option of 7 octets or less and the containing
             * > header is a multiple of 8 octets, the trailing Pad1 or PadN option
             * > MAY be elided by the compressor. */
            break;
        #ifndef MODULE_GNRC_RPL_SR
        case PROTNUM_IPV6_EXT_RH:
            nhc_data[0] |= NHC_IPV6_EXT_EID_RH;
            break;
        #endif
        case PROTNUM_IPV6_EXT_FRAG:
            nhc_data[0] |= NHC_IPV6_EXT_EID_FRAG;
            break;
        case PROTNUM_IPV6_EXT_DST:
            nhc_data[0] |= NHC_IPV6_EXT_EID_DST;
            /* TODO: decrement ext_len by length of trailing Pad1/PadN option:
             * > IPv6 Hop-by-Hop and Destination Options Headers may use a trailing
             * > Pad1 or PadN to achieve 8-octet alignment.  When there is a single
             * > trailing Pad1 or PadN option of 7 octets or less and the containing
             * > header is a multiple of 8 octets, the trailing Pad1 or PadN option
             * > MAY be elided by the compressor. */
            break;
        case PROTNUM_IPV6_EXT_MOB:
            nhc_data[0] |= NHC_IPV6_EXT_EID_MOB;
            break;
        default:
            return -1;
    }
    if (_compressible_nh(nh) &&
        /* carry next header inline when fragment header and offset is equal to
         * 0 (which means the next header indicates the next header after the
         * fragment header in the *first fragment*) */
        ((*protnum != PROTNUM_IPV6_EXT_FRAG) ||
         (ipv6_ext_frag_get_offset((ipv6_ext_frag_t *)ext_hdr) == 0))) {
        nhc_data[0] |= NHC_IPV6_EXT_NH;
    }
    else {
        nhc_data[nhc_len++] = ext_hdr->nh;
        /* prevent next header from being encoded regardless (e.g. if not
         * first IPv6 fragment) */
        nh = PROTNUM_RESERVED;
    }
    /* integer overflow prevented by `ext_len > UINT8_MAX` check above */
    nhc_data[nhc_len++] = (uint8_t)ext_len;
    memcpy(&nhc_data[nhc_len], ext_hdr + 1, ext_len);
    *protnum = nh;
    return nhc_len + ext_len;
}

static inline size_t iphc_nhc_udp_encode(uint8_t *nhc_data,
                                         const gnrc_pktsnip_t *udp)
{
    const udp_hdr_t *udp_hdr = udp->data;
    uint16_t src_port = byteorder_ntohs(udp_hdr->src_port);
    uint16_t dst_port = byteorder_ntohs(udp_hdr->dst_port);
    size_t nhc_len = 1; /* skip over NHC header */

    /* Set UDP NHC header type
     * (see https://tools.ietf.org/html/rfc6282#section-4.3). */
    nhc_data[0] = NHC_UDP_ID;
    /* Compressing UDP ports, follow the same sequence as the linux kernel (nhc_udp module). */
    if (((src_port & NHC_UDP_4BIT_MASK) == NHC_UDP_4BIT_PORT) &&
        ((dst_port & NHC_UDP_4BIT_MASK) == NHC_UDP_4BIT_PORT)) {
        DEBUG("6lo iphc nhc: elide src and dst\n");
        nhc_data[0] |= NHC_UDP_SD_ELIDED;
        nhc_data[nhc_len++] = dst_port - NHC_UDP_4BIT_PORT +
                              ((src_port - NHC_UDP_4BIT_PORT) << 4);
    }
    else if ((dst_port & NHC_UDP_8BIT_MASK) == NHC_UDP_8BIT_PORT) {
        DEBUG("6lo iphc nhc: elide dst\n");
        nhc_data[0] |= NHC_UDP_S_INLINE;
        nhc_data[nhc_len++] = udp_hdr->src_port.u8[0];
        nhc_data[nhc_len++] = udp_hdr->src_port.u8[1];
        nhc_data[nhc_len++] = dst_port - NHC_UDP_8BIT_PORT;
    }
    else if ((src_port & NHC_UDP_8BIT_MASK) == NHC_UDP_8BIT_PORT) {
        DEBUG("6lo iphc nhc: elide src\n");
        nhc_data[0] |= NHC_UDP_D_INLINE;
        nhc_data[nhc_len++] = src_port - NHC_UDP_8BIT_PORT;
        nhc_data[nhc_len++] = udp_hdr->dst_port.u8[0];
        nhc_data[nhc_len++] = udp_hdr->dst_port.u8[1];
    }
    else {
        DEBUG("6lo iphc nhc: src and dst inline\n");
        nhc_data[0] |= NHC_UDP_SD_INLINE;
        nhc_data[nhc_len++] = udp_hdr->src_port.u8[0];
        nhc_data[nhc_len++] = udp_hdr->src_port.u8[1];
        nhc_data[nhc_len++] = udp_hdr->dst_port.u8[0];
        nhc_data[nhc_len++] = udp_hdr->dst_port.u8[1];
    }

    /* TODO: Add support for elided checksum. */
    nhc_data[nhc_len++] = udp_hdr->checksum.u8[0];
    nhc_data[nhc_len++] = udp_hdr->checksum.u8[1];

    return nhc_len;
}

static bool _remove_header(gnrc_pktsnip_t *pkt, gnrc_pktsnip_t *hdr,
                           size_t exp_hdr_size)
{
    if (hdr->size > exp_hdr_size) {
        hdr = gnrc_pktbuf_mark(hdr, exp_hdr_size,
                               GNRC_NETTYPE_UNDEF);

        if (hdr == NULL) {
            DEBUG("6lo iphc: unable to remove compressed header\n");
            return false;
        }
    }
    gnrc_pktbuf_remove_snip(pkt, hdr);
    return true;
}

static ssize_t _nhc_ipv6_encode_snip(gnrc_pktsnip_t *pkt,
                                     const gnrc_netif_hdr_t *netif_hdr,
                                     gnrc_netif_t *iface,
                                     uint8_t *nhc_data,
                                     uint8_t *nh)
{
    gnrc_pktsnip_t *hdr = pkt->next->next;
    ssize_t nhc_len = 1;    /* skip over NHC header */
    size_t tmp;
    uint8_t new_nh = ((ipv6_hdr_t *)hdr->data)->nh;

    assert(hdr->size >= sizeof(ipv6_hdr_t));
    /* Set IPv6 extension compression header type
     * (see https://tools.ietf.org/html/rfc6282#section-4.2). */
    nhc_data[0] = NHC_IPV6_EXT_ID;
    if (_compressible_nh(new_nh)) {
        nhc_data[0] |= NHC_IPV6_EXT_NH;
    }
    else {
        nhc_data[nhc_len++] = new_nh;
    }
    /* save to cast as result is max 40 */
    tmp = (ssize_t)_iphc_ipv6_encode(hdr, netif_hdr, iface, &nhc_data[nhc_len]);
    if (tmp == 0) {
        DEBUG("6lo iphc: error encoding IPv6 header\n");
        return -1;
    }
    nhc_len += tmp;
    /* remove encapsulated IPv6 header */
    if (!_remove_header(pkt, hdr, sizeof(ipv6_hdr_t))) {
        return -1;
    }
    *nh = new_nh;
    return nhc_len;
}

static ssize_t _nhc_ipv6_ext_encode_snip(gnrc_pktsnip_t *pkt, uint8_t *nhc_data,
                                         uint8_t *nh)
{
    gnrc_pktsnip_t *hdr = pkt->next->next;
    ipv6_ext_t *ext = hdr->data;
    ssize_t nhc_len;
    uint16_t ext_len = ((ext->len * IPV6_EXT_LEN_UNIT) + IPV6_EXT_LEN_UNIT);
    uint8_t new_nh = *nh;

    assert((hdr->size >= sizeof(ipv6_ext_t)) && (hdr->size >= ext_len));
    /* _iphc_nhc_ipv6_ext_encode() manipulates nh, so use `new_nh` as temporary
     * carrier in case of later errors */
    nhc_len = _iphc_nhc_ipv6_ext_encode(nhc_data, hdr, ext_len, &new_nh);
    if (nhc_len == 0) {
        /* extension header is not compressible, so don't compress it and
         * just copy it after the preceding compression headers */
        return nhc_len;
    }
    /* remove IPv6 extension header */
    if (!_remove_header(pkt, hdr, ext_len)) {
        return -1;
    }
    *nh = new_nh;
    return nhc_len;
}

static ssize_t _nhc_udp_encode_snip(gnrc_pktsnip_t *pkt, uint8_t *nhc_data)
{
    gnrc_pktsnip_t *hdr = pkt->next->next;
    ssize_t nhc_len;

    assert(hdr->size >= sizeof(udp_hdr_t));
    /* save to cast, as result is max 8 */
    nhc_len = (ssize_t)iphc_nhc_udp_encode(nhc_data, hdr);
    /* remove UDP header */
    if (!_remove_header(pkt, hdr, sizeof(udp_hdr_t))) {
        return -1;
    }
    return nhc_len;
}
#endif

static inline bool _compressible(gnrc_pktsnip_t *hdr)
{
    switch (hdr->type) {
        case GNRC_NETTYPE_UNDEF:    /* when forwarded */
        case GNRC_NETTYPE_IPV6:
#if defined(MODULE_GNRC_SIXLOWPAN_IPHC_NHC)
#ifndef MODULE_GNRC_RPL_SR
# if defined(MODULE_GNRC_IPV6_EXT)
        case GNRC_NETTYPE_IPV6_EXT:
# endif /* defined(MODULE_GNRC_IPV6_EXT) */
#endif
# if defined(MODULE_GNRC_UDP)
        case GNRC_NETTYPE_UDP:
# endif /* defined(MODULE_GNRC_UDP) */
#endif
            return true;
        default:
            return false;
    }
}

static gnrc_pktsnip_t *_iphc_encode(gnrc_pktsnip_t *pkt,
                                    const gnrc_netif_hdr_t *netif_hdr,
                                    gnrc_netif_t *iface)
{
    assert(pkt != NULL);
    uint8_t *iphc_hdr;
    gnrc_pktsnip_t *dispatch, *ptr = pkt->next;
    size_t dispatch_size = 0;
    uint16_t inline_pos = 0;
    uint8_t nh;

    dispatch = NULL;    /* use dispatch as temporary pointer for prev */
    /* determine maximum dispatch size and write protect all headers until
     * then because they will be removed */
    while ((ptr != NULL) && _compressible(ptr)) {
        gnrc_pktsnip_t *tmp = gnrc_pktbuf_start_write(ptr);

        if (tmp == NULL) {
            DEBUG("6lo iphc: unable to write protect compressible header\n");
            return NULL;
        }
        ptr = tmp;
        if (dispatch == NULL) {
            /* pkt was already write protected in gnrc_sixlowpan.c:_send so
             * we shouldn't do it again */
            pkt->next = ptr;    /* reset original packet */
        }
        else {
            dispatch->next = ptr;
        }
        dispatch_size += ptr->size;
        dispatch = ptr; /* use dispatch as temporary point for prev */
        ptr = ptr->next;
    }
    /* there should be at least one compressible header in `pkt`, otherwise this
     * function should not be called */
    assert(dispatch_size > 0);
    dispatch = gnrc_pktbuf_add(NULL, NULL, dispatch_size + 1,
                               GNRC_NETTYPE_SIXLOWPAN);

    if (dispatch == NULL) {
        DEBUG("6lo iphc: error allocating dispatch space\n");
        return NULL;
    }

    iphc_hdr = dispatch->data;
    inline_pos = _iphc_ipv6_encode(pkt, netif_hdr, iface, iphc_hdr);

    if (inline_pos == 0) {
        DEBUG("6lo iphc: error encoding IPv6 header\n");
        gnrc_pktbuf_release(dispatch);
        return NULL;
    }

    nh = ((ipv6_hdr_t *)pkt->next->data)->nh;
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC_NHC
    while (_compressible_nh(nh)) {
        ssize_t local_pos = 0;
        if (pkt->next->next == NULL) {
            DEBUG("6lo iphc: packet next header missing data");
            gnrc_pktbuf_release(dispatch);
            return NULL;
        }
        switch (nh) {
            case PROTNUM_UDP:
                local_pos = _nhc_udp_encode_snip(pkt, &iphc_hdr[inline_pos]);
                /* abort loop on next iteration */
                nh = PROTNUM_RESERVED;
                break;
            case PROTNUM_IPV6: {    /* encapsulated IPv6 header */
                local_pos = _nhc_ipv6_encode_snip(pkt, netif_hdr, iface,
                                                  &iphc_hdr[inline_pos], &nh);
                break;
            }
            case PROTNUM_IPV6_EXT_HOPOPT:
#ifndef MODULE_GNRC_RPL_SR
            case PROTNUM_IPV6_EXT_RH:
#endif
            case PROTNUM_IPV6_EXT_FRAG:
            case PROTNUM_IPV6_EXT_DST:
            case PROTNUM_IPV6_EXT_MOB:
                local_pos = _nhc_ipv6_ext_encode_snip(pkt,
                                                      &iphc_hdr[inline_pos],
                                                      &nh);
                if (local_pos == 0) {
                    /* abort loop, extension header is not compressible as
                     * length field is too large value */
                    nh = PROTNUM_RESERVED;
                }
                break;
            default:
                /* abort loop on next iteration */
                nh = PROTNUM_RESERVED;
                break;
        }
        if (local_pos < 0) {
            DEBUG("6lo iphc: error on compressing next header\n");
            gnrc_pktbuf_release(dispatch);
            return NULL;
        }
        inline_pos += local_pos;
    }
#endif

    /* shrink dispatch allocation to final size */
    /* NOTE: Since this only shrinks the data nothing bad SHOULD happen ;-) */
    gnrc_pktbuf_realloc_data(dispatch, (size_t)inline_pos);

    /* remove IPv6 header */
    pkt = gnrc_pktbuf_remove_snip(pkt, pkt->next);

    /* insert dispatch into packet */
    dispatch->next = pkt->next;
    pkt->next = dispatch;
    return pkt;
}

void gnrc_sixlowpan_iphc_send(gnrc_pktsnip_t *pkt, void *ctx, unsigned page)
{
    gnrc_netif_hdr_t *netif_hdr = pkt->data;
    gnrc_netif_t *netif = gnrc_netif_hdr_get_netif(netif_hdr);
    gnrc_pktsnip_t *tmp;
    /* datagram size before compression */
    size_t orig_datagram_size = gnrc_pkt_len(pkt->next);
    ipv6_hdr_t *ipv6_hdr = pkt->next->data;
    ipv6_addr_t dst;

    if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_MINFWD)) {
        dst = ipv6_hdr->dst;    /* copying original destination address */
    }

    if ((tmp = _iphc_encode(pkt, pkt->data, netif))) {
        if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_MINFWD) && (ctx != NULL) &&
            (gnrc_sixlowpan_frag_minfwd_frag_iphc(tmp, orig_datagram_size, &dst,
                                                  ctx) == 0)) {
            DEBUG("6lo iphc minfwd: putting slack in first fragment\n");
            return;
        }
        gnrc_sixlowpan_multiplex_by_size(tmp, orig_datagram_size, netif, page);
    }
    else {
        if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_MINFWD)) {
            gnrc_sixlowpan_frag_fb_t *fb = ctx;

            if (fb->pkt == pkt) {
                /* free provided fragmentation buffer */
                fb->pkt = NULL;
            }
        }
        gnrc_pktbuf_release(pkt);
    }
}

/** @} */
