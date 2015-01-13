/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @addtogroup  net_sixlowpan_hc
 * @{
 *
 * @file        hc.c
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <string.h>

#include "byteorder.h"
#include "pktbuf.h"
#include "timex.h"
#include "vtimer.h"
#include "sixlowpan.h"
#include "sixlowpan/iphc_cbuf.h"

#include "sixlowpan/hc.h"

#define ENABLE_DEBUG    (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include "debug.h"

/* Dispatch byte definitions */
#define _IPHC1          (0)
#define _IPHC2          (1)
#define _IPHC_CID_EXT   (2)

/* Values for traffic class and flow label compression */
#define _IPHC_TF_ECN_DSCP_FL    (0)
#define _IPHC_TF_ECN_FL         (1)
#define _IPHC_TF_ECN_DSCP       (2)
#define _IPHC_TF_NONE           (3)

/* Values for hop limit compression */
#define _IPHC_HL_INLINE (0)
#define _IPHC_HL_1      (1)
#define _IPHC_HL_64     (2)
#define _IPHC_HL_255    (3)

#define _IPHC_SAC_SAM_128_INLINE            (0x0)
#define _IPHC_SAC_SAM_64_INLINE             (0x1)
#define _IPHC_SAC_SAM_16_INLINE             (0x2)
#define _IPHC_SAC_SAM_FROM_L2_SRC           (0x3)
#define _IPHC_SAC_SAM_UNSPECIFIED           (0x4)
#define _IPHC_SAC_SAM_CTX_BASED_64_INLINE   (0x5)
#define _IPHC_SAC_SAM_CTX_BASED_16_INLINE   (0x6)
#define _IPHC_SAC_SAM_CTX_BASED_FROM_L2_SRC (0x7)

#define _IPHC_M_DAC_DAM_128_INLINE              (0x0)
#define _IPHC_M_DAC_DAM_64_INLINE               (0x1)
#define _IPHC_M_DAC_DAM_16_INLINE               (0x2)
#define _IPHC_M_DAC_DAM_FROM_L2_DEST            (0x3)
#define _IPHC_M_DAC_DAM_CTX_BASED_64_INLINE     (0x5)
#define _IPHC_M_DAC_DAM_CTX_BASED_16_INLINE     (0x6)
#define _IPHC_M_DAC_DAM_CTX_BASED_FROM_L2_DEST  (0x7)
#define _IPHC_M_DAC_DAM_MC_128_INLINE           (0x8)
#define _IPHC_M_DAC_DAM_MC_48_INLINE            (0x9)
#define _IPHC_M_DAC_DAM_MC_32_INLINE            (0xa)
#define _IPHC_M_DAC_DAM_MC_8_INLINE             (0xb)
#define _IPHC_M_DAC_DAM_MC_UNICAST_PREFIX_BASED (0xc)

/* helper functions for field decodings */
static inline int _iphc_has_cid_extension(uint8_t *iphc_dispatch);

/* field decoding functions */
static uint8_t _decode_trafficclass_flowlabel(ipv6_hdr_t *ipv6_hdr,
        uint8_t *iphc_dispatch,
        uint8_t *data);
static uint8_t _decode_nextheader(ipv6_hdr_t *ipv6_hdr, uint8_t *iphc_dispatch,
                                  uint8_t *data);
static uint8_t _decode_hoplimit(ipv6_hdr_t *ipv6_hdr, uint8_t *iphc_dispatch,
                                uint8_t *data);
static int16_t _decode_src_addr(ipv6_hdr_t *ipv6_hdr, uint8_t *iphc_dispatch,
                                void *l2_src, size_t l2_src_len,
                                uint8_t *data);
static int16_t _decode_dest_addr(ipv6_hdr_t *ipv6_hdr, uint8_t *iphc_dispatch,
                                 void *l2_dest, size_t l2_dest_len,
                                 uint8_t *data);
static inline void _decode_length(ipv6_hdr_t *ipv6_hdr, size_t lowpan_data_len,
                                  size_t payload_offset);
static inline int _decode_data(uint8_t *ipv6_data, uint8_t *lowpan_data,
                               size_t data_len);

/* helper functions for encoding */
static ipv6_hdr_t *_iphc_encoding_get_ipv6_hdr(netdev_hlist_t **ulh);

/* field encoding functions */
static uint16_t _encode_trafficclass_flowlabel(uint8_t *iphc_dispatch,
        uint16_t inline_pos,
        ipv6_hdr_t *ipv6_hdr);
static uint16_t _encode_nextheader(uint8_t *iphc_dispatch, uint16_t inline_pos,
                                   ipv6_hdr_t *ipv6_hdr);
static uint16_t _encode_hoplimit(uint8_t *iphc_dispatch, uint16_t inline_pos,
                                 ipv6_hdr_t *ipv6_hdr);
static uint16_t _encode_src_addr(uint8_t *iphc_dispatch, ipv6_hdr_t *ipv6_hdr,
                                 uint16_t inline_pos, void *l2_src,
                                 size_t l2_src_len, sixlowpan_iphc_cbuf_t *context);
static uint16_t _encode_dest_addr(uint8_t *iphc_dispatch, ipv6_hdr_t *ipv6_hdr,
                                  uint16_t inline_pos, void *l2_dest,
                                  size_t l2_dest_len, sixlowpan_iphc_cbuf_t *context);

/* API implementation */
int sixlowpan_hc_decode(netapi_rcv_pkt_t *rcv, void *src, size_t src_len,
                        void *dest, size_t dest_len, uint8_t *data,
                        size_t data_len)
{
    int res;
    uint8_t payload_offset;
    uint8_t iphc_dispatch[SIXLOWPAN_IPHC_HDR_LEN + SIXLOWPAN_IPHC_CONTEXT_HDR_LEN] =
        SIXLOWPAN_IPHC1_ZERO_INIT;
    uint8_t no_nhc = 1;

#ifdef DEVELHELP

    if (rcv == NULL || src == NULL || dest == NULL || data == NULL) {
        return -EFAULT;
    }

#endif

    if ((data[_IPHC1] & SIXLOWPAN_IPHC1_HDR_MASK) != SIXLOWPAN_IPHC1_DISPATCH) {
        DEBUG("Not an IPHC packet\n");
        return -EINVAL;
    }

    if (data_len < 2) {
        return -EMSGSIZE;
    }

    rcv->src = src;
    rcv->src_len = src_len;
    rcv->dest = dest;
    rcv->dest_len = dest_len;

    iphc_dispatch[_IPHC1] = data[_IPHC1];
    iphc_dispatch[_IPHC2] = data[_IPHC2];
    payload_offset = 2;

    if (_iphc_has_cid_extension(iphc_dispatch)) {
        if (data_len < SIXLOWPAN_IPHC_HDR_LEN + SIXLOWPAN_IPHC_CONTEXT_HDR_LEN) {
            return -EMSGSIZE;
        }

        iphc_dispatch[_IPHC_CID_EXT] = data[_IPHC_CID_EXT];
        payload_offset++;
    }

    rcv->data = pktbuf_alloc(sizeof(ipv6_hdr_t) + data_len);

    ipv6_hdr_set_version(rcv->data);
    payload_offset += _decode_trafficclass_flowlabel(rcv->data, iphc_dispatch,
                      &(data[payload_offset]));

    no_nhc = _decode_nextheader(rcv->data, iphc_dispatch, &(data[payload_offset]));

    payload_offset += no_nhc;
    payload_offset += _decode_hoplimit(rcv->data, iphc_dispatch,
                                       &(data[payload_offset]));

    if ((res = _decode_src_addr(rcv->data, iphc_dispatch, src, src_len,
                                &(data[payload_offset]))) < 0) {
        pktbuf_release(rcv->data);
        return res;
    };

    payload_offset += (uint8_t)res;

    if ((res = _decode_dest_addr(rcv->data, iphc_dispatch, dest, dest_len,
                                 &(data[payload_offset]))) < 0) {
        pktbuf_release(rcv->data);
        return res;
    }

    payload_offset += (uint8_t)res;

    if (!no_nhc) {
        /* TODO */
        pktbuf_release(rcv->data);
        return -ENOTSUP;
    }

    if ((res = _decode_data(rcv->data, &(data[payload_offset]), data_len - payload_offset)) < 0) {
        pktbuf_release(rcv->data);
        return res;
    }

    _decode_length(rcv->data, data_len, payload_offset);

    rcv->data_len = sizeof(ipv6_hdr_t) + data_len - payload_offset;
    pktbuf_realloc(rcv->data, rcv->data_len);   /* reduce needed memory */
    pktbuf_release(data);

    return data_len - payload_offset;
}

int sixlowpan_hc_encode(netdev_hlist_t *dispatch, netdev_hlist_t **ulh,
                        void *src, size_t src_len, void *dest, size_t dest_len)
{
    ipv6_hdr_t *ipv6_hdr;
    sixlowpan_iphc_cbuf_t *src_context = NULL;
    sixlowpan_iphc_cbuf_t *dest_context = NULL;
    uint8_t *iphc_dispatch;
    uint16_t inline_pos = SIXLOWPAN_IPHC_HDR_LEN;
    uint8_t no_nhc = 1;

#ifdef DEVELHELP

    if (dispatch == NULL || src == NULL || dest == NULL || ulh == NULL) {
        return -EFAULT;
    }

#endif

    iphc_dispatch = dispatch->header;

    if (dest_len == 0 || (dest_len > 2 && dest_len != 8) ||
        src_len == 0 || (src_len > 2 && src_len != 8)) {
        return -EAFNOSUPPORT;
    }

    ipv6_hdr = _iphc_encoding_get_ipv6_hdr(ulh);

    if (ipv6_hdr == NULL) {
        return -EINVAL;
    }

    iphc_dispatch[_IPHC1] = SIXLOWPAN_IPHC1_DISPATCH;
    iphc_dispatch[_IPHC2] = 0;

    src_context = sixlowpan_iphc_cbuf_lookup_addr(&(ipv6_hdr->srcaddr));

    if (!ipv6_addr_is_multicast(&(ipv6_hdr->destaddr))) {
        dest_context = sixlowpan_iphc_cbuf_lookup_addr(&(ipv6_hdr->destaddr));
    }

    if ((src_context != NULL && src_context->cid != 0) ||
        (dest_context != NULL && dest_context->cid != 0)) {
        if (dispatch->header_len < SIXLOWPAN_IPHC_HDR_LEN + SIXLOWPAN_IPHC_CONTEXT_HDR_LEN) {
            /* if dispatch has no space for the context extension */
            return -ENOMEM;
        }
        else {
            iphc_dispatch[_IPHC2] |= SIXLOWPAN_IPHC2_CID;
            iphc_dispatch[_IPHC_CID_EXT] = 0;
            inline_pos += SIXLOWPAN_IPHC_CONTEXT_HDR_LEN;
        }
    }

    inline_pos = _encode_trafficclass_flowlabel(iphc_dispatch, inline_pos, ipv6_hdr);

    no_nhc = _encode_nextheader(iphc_dispatch, inline_pos, ipv6_hdr);

    if (inline_pos == no_nhc) {
        no_nhc = 0;
    }
    else {
        inline_pos = no_nhc;
    }

    inline_pos = _encode_hoplimit(iphc_dispatch, inline_pos, ipv6_hdr);
    inline_pos = _encode_src_addr(iphc_dispatch, ipv6_hdr, inline_pos, src,
                                  src_len, src_context);
    inline_pos = _encode_dest_addr(iphc_dispatch, ipv6_hdr, inline_pos, dest,
                                   dest_len, dest_context);

    /* if (!no_nhc) { */
    /*     TODO */
    /* } */

    dispatch->header_len = inline_pos;
    dispatch->protocol = NETDEV_PROTO_6LOWPAN;

    pktbuf_release(ipv6_hdr);

    return 0;
}

/* internal function implementations */

static inline uint8_t _iphc_tf_value(uint8_t *iphc_dispatch)
{
    return ((iphc_dispatch[_IPHC1] & SIXLOWPAN_IPHC1_TF) >> 3);
}

static inline uint8_t _iphc_set_tf_value(uint8_t tf_value)
{
    return ((tf_value << 3) & SIXLOWPAN_IPHC1_TF);
}

static inline uint32_t _iphc_tf_flowlabel_from_inline(uint8_t *inl)
{
    return (uint32_t)(((inl[0] & 0x0f) << 16) | (inl[1] << 8) | inl[2]);
}

static inline int _iphc_has_cid_extension(uint8_t *iphc_dispatch)
{
    return (iphc_dispatch[_IPHC2] & SIXLOWPAN_IPHC2_CID);
}

static inline uint8_t _iphc_sac_sam_value(uint8_t *iphc_dispatch)
{
    return ((iphc_dispatch[_IPHC2] & (SIXLOWPAN_IPHC2_SAC | SIXLOWPAN_IPHC2_SAM)) >> 4);
}

static inline uint8_t _iphc_set_sac_sam_value(uint8_t sac_sam_value)
{
    return ((sac_sam_value) << 4) & (SIXLOWPAN_IPHC2_SAC | SIXLOWPAN_IPHC2_SAM);
}

static inline uint8_t _iphc_m_dac_dam_value(uint8_t *iphc_dispatch)
{
    return (iphc_dispatch[_IPHC2] & (SIXLOWPAN_IPHC2_M | SIXLOWPAN_IPHC2_DAC |
                                     SIXLOWPAN_IPHC2_DAM));
}

static inline uint8_t _iphc_set_m_dac_dam_value(uint8_t m_dac_dam_value)
{
    return (m_dac_dam_value & (SIXLOWPAN_IPHC2_M | SIXLOWPAN_IPHC2_DAC |
                               SIXLOWPAN_IPHC2_DAM));
}

static inline uint8_t _iphc_get_sci(uint8_t cid_ext)
{
    return (cid_ext >> 4);
}

static inline uint8_t _iphc_get_dci(uint8_t cid_ext)
{
    return (cid_ext & 0x0f);
}

static inline uint8_t _iphc_set_sci(uint8_t sci)
{
    return ((sci & 0x0f) << 4);
}

static inline uint8_t _iphc_set_dci(uint8_t dci)
{
    return (dci & 0x0f);
}

static void _overwrite_prefix_with_context(uint8_t *addr,
        sixlowpan_iphc_cbuf_t *context)
{
    if (context->prefix_len > 128) {
        context->prefix_len = 128;
    }

    uint8_t bytes = context->prefix_len / 8, mask;

    if (context->prefix_len % 8) {
        mask = 0xff << (8 - (context->prefix_len - (bytes * 8)));
    }
    else {
        mask = 0x00;
    }

    memcpy(addr, &(context->prefix), bytes);
    addr[bytes] &= ~mask;
    addr[bytes] |= context->prefix.u8[bytes] & mask;
}

static int _iid_from_l2_addr(ipv6_addr_t *addr, uint8_t *l2_addr, size_t l2_addr_len)
{
    if (l2_addr_len <= 2) {
        addr->u16[5] = byteorder_htons(0x00ff);
        addr->u16[6] = byteorder_htons(0xfe00);
    }

    switch (l2_addr_len) {
        case 1:
            addr->u8[15] = *l2_addr;
            break;

        case 2:
            memcpy(&(addr->u8[14]), l2_addr, l2_addr_len);
            break;

        case 8:
            memcpy(&(addr->u8[8]), l2_addr, l2_addr_len);
            addr->u8[8] ^= 0x02; /* invert universal/local bit */
            break;

        default:
            return -EAFNOSUPPORT;
    }

    return 0;
}

static uint8_t _decode_trafficclass_flowlabel(ipv6_hdr_t *ipv6_hdr,
        uint8_t *iphc_dispatch,
        uint8_t *data)
{
    uint8_t payload_offset = 0;
    uint8_t ecn = 0;
    uint8_t dscp = 0;
    uint32_t flowlabel = 0;

    switch (_iphc_tf_value(iphc_dispatch)) {
        case _IPHC_TF_ECN_DSCP_FL:
            ecn = data[0] >> 6;
            dscp = data[0] & 0x3f;
            flowlabel = _iphc_tf_flowlabel_from_inline(&(data[1]));
            payload_offset += 4;
            break;

        case _IPHC_TF_ECN_FL:
            ecn = data[0] >> 6;
            flowlabel = _iphc_tf_flowlabel_from_inline(data);
            payload_offset += 3;
            break;

        case _IPHC_TF_ECN_DSCP:
            ecn = data[0] >> 6;
            dscp = data[0] & 0x3f;
            payload_offset += 1;
            break;

        case _IPHC_TF_NONE:

            /* all elided => all zero, nothing to do */
        default:
            /* should not happen, since _iphc_tf_value only has 2-bit result */
            break;
    }

    ipv6_hdr_set_trafficclass_ecn(ipv6_hdr, ecn);
    ipv6_hdr_set_trafficclass_dscp(ipv6_hdr, dscp);
    ipv6_hdr_set_flowlabel(ipv6_hdr, flowlabel);

    return payload_offset;
}

static uint8_t _decode_nextheader(ipv6_hdr_t *ipv6_hdr, uint8_t *iphc_dispatch,
                                  uint8_t *data)
{
    uint8_t payload_offset = 0;

    if (!(iphc_dispatch[_IPHC1] & SIXLOWPAN_IPHC1_NH)) {
        ipv6_hdr->nextheader = *data;
        payload_offset++;
    }

    return payload_offset;
}

static uint8_t _decode_hoplimit(ipv6_hdr_t *ipv6_hdr, uint8_t *iphc_dispatch,
                                uint8_t *data)
{
    switch (iphc_dispatch[_IPHC1] & SIXLOWPAN_IPHC1_HL) {
        case _IPHC_HL_INLINE:
            ipv6_hdr->hoplimit = *data;
            return 1;

        case _IPHC_HL_1:
            ipv6_hdr->hoplimit = 1;
            break;

        case _IPHC_HL_64:
            ipv6_hdr->hoplimit = 64;
            break;

        case _IPHC_HL_255:
            ipv6_hdr->hoplimit = 255;

        default:
            /* should not happen */
            break;
    }

    return 0;
}

static int16_t _decode_src_addr(ipv6_hdr_t *ipv6_hdr, uint8_t *iphc_dispatch,
                                void *l2_src, size_t l2_src_len,
                                uint8_t *data)
{
    uint8_t ll_prefix[2] = {0xfe, 0x80};
    sixlowpan_iphc_cbuf_t *context = NULL;
    uint8_t payload_offset = 0;

    if (iphc_dispatch[_IPHC2] & SIXLOWPAN_IPHC2_SAC) {   /* Stateful source
                                                          * address compression */
        uint8_t sci = 0;

        if (_iphc_has_cid_extension(iphc_dispatch)) {
            sci = _iphc_get_sci(iphc_dispatch[_IPHC_CID_EXT]);
        }

        if (iphc_dispatch[_IPHC2] & SIXLOWPAN_IPHC2_SAM) {
            context = sixlowpan_iphc_cbuf_lookup_cid(sci);

            if (context == NULL) {
                DEBUG("ERROR: context not found\n");
                return -EADDRNOTAVAIL;
            }
        }
    }

    ipv6_hdr->srcaddr.u32[0].u32 = 0;
    ipv6_hdr->srcaddr.u32[1].u32 = 0;
    ipv6_hdr->srcaddr.u32[2].u32 = 0;
    ipv6_hdr->srcaddr.u32[3].u32 = 0;

    switch (_iphc_sac_sam_value(iphc_dispatch)) {
        case _IPHC_SAC_SAM_128_INLINE:
            memcpy(&(ipv6_hdr->srcaddr), data, 16); /* take 128 bits from inline */
            payload_offset += 16;
            break;

        case _IPHC_SAC_SAM_64_INLINE:
            memcpy(&(ipv6_hdr->srcaddr.u8[0]), &ll_prefix[0], 2);
            memcpy(&(ipv6_hdr->srcaddr.u8[8]), data, 8);
            payload_offset += 8;
            break;

        case _IPHC_SAC_SAM_16_INLINE:
            memcpy(&(ipv6_hdr->srcaddr.u8[0]), &ll_prefix[0], 2);
            ipv6_hdr->srcaddr.u16[5] = byteorder_htons(0x00ff);
            ipv6_hdr->srcaddr.u16[6] = byteorder_htons(0xfe00);
            memcpy(&(ipv6_hdr->srcaddr.u8[14]), data, 2);
            payload_offset += 2;
            break;

        case _IPHC_SAC_SAM_FROM_L2_SRC:
            memcpy(&(ipv6_hdr->srcaddr.u8[0]), &ll_prefix[0], 2);
            _iid_from_l2_addr(&(ipv6_hdr->srcaddr), l2_src, l2_src_len);
            break;

        case _IPHC_SAC_SAM_UNSPECIFIED:
            /* already set to 0 */
            break;

        case _IPHC_SAC_SAM_CTX_BASED_64_INLINE:
            memcpy(&(ipv6_hdr->srcaddr.u8[8]), data, 8); /* take 64 bits from inline */
            _overwrite_prefix_with_context(&(ipv6_hdr->srcaddr.u8[0]), context);
                    /* always take complete context bits */
            payload_offset += 8;
            break;

        case _IPHC_SAC_SAM_CTX_BASED_16_INLINE:
            ipv6_hdr->srcaddr.u16[5] = byteorder_htons(0x00ff);
            ipv6_hdr->srcaddr.u16[6] = byteorder_htons(0xfe00);
            memcpy(&(ipv6_hdr->srcaddr.u8[14]), data, 2); /* take 16 bits from inline */
            _overwrite_prefix_with_context(&(ipv6_hdr->srcaddr.u8[0]), context);
                    /* always take complete context bits */
            payload_offset += 2;
            break;

        case _IPHC_SAC_SAM_CTX_BASED_FROM_L2_SRC:
            _iid_from_l2_addr(&(ipv6_hdr->srcaddr), l2_src, l2_src_len);
                    /* take IID from encapsulating header */
            _overwrite_prefix_with_context(&(ipv6_hdr->srcaddr.u8[0]), context);
                    /* always take complete context bits */
            break;

        default:
            /* should not happen */
            break;
    }

    return payload_offset;
}

static int16_t _decode_dest_addr(ipv6_hdr_t *ipv6_hdr, uint8_t *iphc_dispatch,
                                 void *l2_dest, size_t l2_dest_len,
                                 uint8_t *data)
{
    uint8_t ll_prefix[2] = {0xfe, 0x80};
    sixlowpan_iphc_cbuf_t *context = NULL;
    uint8_t payload_offset = 0;

    if (iphc_dispatch[_IPHC2] & SIXLOWPAN_IPHC2_DAC) {   /* Stateful source address compression */
        uint8_t dci = 0;

        if (_iphc_has_cid_extension(iphc_dispatch)) {
            dci = _iphc_get_dci(iphc_dispatch[_IPHC_CID_EXT]);
        }

        if ((iphc_dispatch[_IPHC2] & SIXLOWPAN_IPHC2_DAM) ||
            (iphc_dispatch[_IPHC2] & SIXLOWPAN_IPHC2_M)) {
            context = sixlowpan_iphc_cbuf_lookup_cid(dci);

            if (context == NULL) {
                DEBUG("ERROR: context not found\n");
                return -EADDRNOTAVAIL;
            }
        }
    }

    ipv6_hdr->destaddr.u32[0].u32 = 0;
    ipv6_hdr->destaddr.u32[1].u32 = 0;
    ipv6_hdr->destaddr.u32[2].u32 = 0;
    ipv6_hdr->destaddr.u32[3].u32 = 0;

    switch (_iphc_m_dac_dam_value(iphc_dispatch)) {
        case _IPHC_M_DAC_DAM_128_INLINE:
        case _IPHC_M_DAC_DAM_MC_128_INLINE:
            memcpy(&(ipv6_hdr->destaddr.u8), data, 16); /* take 128 bits from inline */
            payload_offset += 16;
            break;

        case _IPHC_M_DAC_DAM_64_INLINE:
            memcpy(&(ipv6_hdr->destaddr.u8[0]), &ll_prefix[0], 2);
            memcpy(&(ipv6_hdr->destaddr.u8[8]), data, 8);
            payload_offset += 8;
            break;

        case _IPHC_M_DAC_DAM_16_INLINE:
            memcpy(&(ipv6_hdr->destaddr.u8[0]), &ll_prefix[0], 2);
            ipv6_hdr->destaddr.u16[5] = byteorder_htons(0x00ff);
            ipv6_hdr->destaddr.u16[6] = byteorder_htons(0xfe00);
            memcpy(&(ipv6_hdr->destaddr.u8[14]), data, 2);
            payload_offset += 2;
            break;

        case _IPHC_M_DAC_DAM_FROM_L2_DEST:
            memcpy(&(ipv6_hdr->destaddr.u8[0]), &ll_prefix[0], 2);
            _iid_from_l2_addr(&(ipv6_hdr->destaddr), l2_dest, l2_dest_len);
            break;

        case _IPHC_M_DAC_DAM_CTX_BASED_64_INLINE:
            memcpy(&(ipv6_hdr->destaddr.u8[8]), data, 8); /* take 64 bits from inline */
            _overwrite_prefix_with_context(&(ipv6_hdr->destaddr.u8[0]), context);
                    /* always take complete context bits */
            payload_offset += 8;
            break;

        case _IPHC_M_DAC_DAM_CTX_BASED_16_INLINE:
            ipv6_hdr->destaddr.u16[5] = byteorder_htons(0x00ff);
            ipv6_hdr->destaddr.u16[6] = byteorder_htons(0xfe00);
            memcpy(&(ipv6_hdr->destaddr.u8[14]), data, 2); /* take 16 bits from inline */
            _overwrite_prefix_with_context(&(ipv6_hdr->destaddr.u8[0]), context);
                    /* always take complete context bits */
            payload_offset += 2;
            break;

        case _IPHC_M_DAC_DAM_CTX_BASED_FROM_L2_DEST:
            _iid_from_l2_addr(&(ipv6_hdr->destaddr), l2_dest, l2_dest_len);
                    /* take IID from encapsulating header */
            _overwrite_prefix_with_context(&(ipv6_hdr->destaddr.u8[0]), context);
                    /* always take complete context bits */
            break;

        case _IPHC_M_DAC_DAM_MC_48_INLINE:
            /* ffXX::00XX:XXXX:XXXX */
            ipv6_hdr->destaddr.u8[0] = 0xff;
            ipv6_hdr->destaddr.u8[1] = data[0];
            ipv6_hdr->destaddr.u8[11] = data[1];
            ipv6_hdr->destaddr.u8[12] = data[2];
            ipv6_hdr->destaddr.u8[13] = data[3];
            ipv6_hdr->destaddr.u8[14] = data[4];
            ipv6_hdr->destaddr.u8[15] = data[5];
            payload_offset += 6;
            break;

        case _IPHC_M_DAC_DAM_MC_32_INLINE:
            /* ffXX::0000:00XX:XXXX */
            ipv6_hdr->destaddr.u8[0] = 0xff;
            ipv6_hdr->destaddr.u8[1] = data[0];
            ipv6_hdr->destaddr.u8[13] = data[1];
            ipv6_hdr->destaddr.u8[14] = data[2];
            ipv6_hdr->destaddr.u8[15] = data[3];
            payload_offset += 4;
            break;

        case _IPHC_M_DAC_DAM_MC_8_INLINE:
            /* ff02::XX */
            ipv6_hdr->destaddr.u8[0] = 0xff;
            ipv6_hdr->destaddr.u8[1] = 0x02;
            ipv6_hdr->destaddr.u8[15] = data[0];
            payload_offset++;
            break;

        case _IPHC_M_DAC_DAM_MC_UNICAST_PREFIX_BASED:
            do {
                uint8_t orig_context_len = context->prefix_len;

                if (context->prefix_len > 64) {
                    context->prefix_len = 64;
                }

                ipv6_hdr->destaddr.u8[0] = 0xff;
                ipv6_hdr->destaddr.u8[1] = data[0];
                ipv6_hdr->destaddr.u8[2] = data[1];
                ipv6_hdr->destaddr.u8[3] = context->prefix_len;
                _overwrite_prefix_with_context(&(ipv6_hdr->destaddr.u8[4]),
                                               context);
                memcpy(&(ipv6_hdr->destaddr.u8[12]), &(data[2]), 4);

                context->prefix_len = orig_context_len;
            } while (0);

            payload_offset += 6;

            break;

        default:
            DEBUG("ERROR: Unspecified M, DAC, and DAM combination\n");
            return -EDESTADDRREQ;

    }

    return payload_offset;
}

static inline int _decode_data(uint8_t *ipv6_data, uint8_t *lowpan_data,
                               size_t data_len)
{
    return pktbuf_copy(&(ipv6_data[sizeof(ipv6_hdr_t)]), lowpan_data, data_len);
}

static inline void _decode_length(ipv6_hdr_t *ipv6_hdr, size_t lowpan_data_len,
                                  size_t payload_offset)
{
    ipv6_hdr->length = byteorder_htons(lowpan_data_len - payload_offset);
}

static netdev_hlist_t *_deep_copy_ulh(netdev_hlist_t *ulh)
{
    netdev_hlist_t *ptr = ulh;
    netdev_hlist_t *new = NULL;

    if (ptr == NULL) {
        return NULL;
    }

    do {
        netdev_hlist_t *prev = ptr;

        clist_add((clist_node_t **)(&new),
                  (clist_node_t *)pktbuf_alloc(sizeof(netdev_hlist_t)));
        netdev_hlist_advance(&new);

        new->protocol = ptr->protocol;
        new->header = pktbuf_insert(ptr->header, ptr->header_len);
        new->header_len = ptr->header_len;

        netdev_hlist_advance(&ptr);

        pktbuf_release(prev->header);
        pktbuf_release(prev);
    } while (ulh != ptr);

    return new->next;
}

static ipv6_hdr_t *_iphc_encoding_get_ipv6_hdr(netdev_hlist_t **ulh)
{
    ipv6_hdr_t *ipv6_hdr = NULL;

    if (ulh != NULL) {
        *ulh = _deep_copy_ulh(*ulh);
    }

    if (ulh != NULL && *ulh != NULL && (*ulh)->protocol == NETDEV_PROTO_IPV6 &&
        (*ulh)->header_len == 40 && ipv6_hdr_is_ipv6_hdr((*ulh)->header)) { /* better save than sorry */
        netdev_hlist_t *ipv6_hdr_node = *ulh;

        ipv6_hdr = (ipv6_hdr_t *)ipv6_hdr_node->header;
        netdev_hlist_remove(ulh, ipv6_hdr_node);
        pktbuf_release(ipv6_hdr_node);
    }

    return ipv6_hdr;
}

static network_uint64_t _iphc_init_iid(void *addr, size_t addr_len)
{
    union {
        network_uint64_t u64[1];
        network_uint16_t u16[4];
        uint8_t  u8[8];
    } iid;

    if (addr_len <= 2) {
        iid.u64[0] = byteorder_htonll(0x000000fffe000000);

        if (addr_len == 1) {
            uint8_t *src_u8 = (uint8_t *)addr;
            iid.u8[7] = *src_u8;
        }
        else {
            network_uint16_t *src_u16 = (network_uint16_t *)addr;
            iid.u16[3] = *src_u16;
        }
    }
    else if (addr_len == 8) {
        network_uint64_t *src_u64 = (network_uint64_t *)addr;
        iid.u64[0] = *src_u64;
        iid.u8[0] ^= 0x02;
    }
    else {
        return (network_uint64_t) { 0 };
    }

    return iid.u64[0];
}

static inline uint8_t _ipv6_addr_from_16_bit(ipv6_addr_t *addr)
{
    return ((byteorder_ntohl(addr->u32[2]) == 0x000000ff) &&
            (byteorder_ntohs(addr->u16[6]) == 0xfe00));
}

static inline uint8_t _context_overlaps_iid(sixlowpan_iphc_cbuf_t *context,
        ipv6_addr_t *addr,
        network_uint64_t iid)
{
    uint8_t byte_mask[] = {0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01};

    return (((context != NULL) && (context->prefix_len == 128)) ||
            ((context != NULL) && (context->prefix_len > 64) &&
             (memcmp(&(addr->u8[(context->prefix_len / 8) + 1]),
                     &(iid.u8[(context->prefix_len / 8) - 7]),
                     sizeof(network_uint64_t) - ((context->prefix_len / 8) - 7)) == 0) &&
             ((addr->u8[(context->prefix_len / 8)] & byte_mask[context->prefix_len % 8]) ==
              (iid.u8[(context->prefix_len / 8) - 8] & byte_mask[context->prefix_len % 8]))));
}

static uint16_t _encode_trafficclass_flowlabel(uint8_t *iphc_dispatch,
        uint16_t inline_pos,
        ipv6_hdr_t *ipv6_hdr)
{
    if (ipv6_hdr_get_flowlabel(ipv6_hdr) == 0) {
        if (ipv6_hdr_get_trafficclass(ipv6_hdr) == 0) {
            /* elide traffic class and flow label */
            iphc_dispatch[_IPHC1] |= _iphc_set_tf_value(_IPHC_TF_NONE);
        }
        else {
            /* elide flow label, traffic class (DSCP + ECN) inline (1 byte) */
            iphc_dispatch[_IPHC1] |= _iphc_set_tf_value(_IPHC_TF_ECN_DSCP);
            iphc_dispatch[inline_pos++] = ipv6_hdr_get_trafficclass(ipv6_hdr);
        }
    }
    else {
        if (ipv6_hdr_get_trafficclass_dscp(ipv6_hdr) == 0) {
            iphc_dispatch[_IPHC1] |= _iphc_set_tf_value(_IPHC_TF_ECN_FL);

            /* elide DSCP, ECN + 2-bit pad + Flow Label inline (3 byte) */
            iphc_dispatch[inline_pos++] = (uint8_t)((ipv6_hdr_get_trafficclass_ecn(ipv6_hdr) << 6) |
                                                    (ipv6_hdr_get_flowlabel(ipv6_hdr) & 0x000f0000) >> 16);
        }
        else {
            /* _IPHC_TF_ECN_DSCP_FL is 0 so nothing todo with iphc_dispatch */
            /*  ECN + DSCP + 4-bit Pad + Flow Label (4 bytes)*/
            iphc_dispatch[inline_pos++] = ipv6_hdr_get_trafficclass(ipv6_hdr);
            iphc_dispatch[inline_pos++] = (uint8_t)((ipv6_hdr_get_flowlabel(ipv6_hdr) & 0x000f0000) >> 16);
        }

        iphc_dispatch[inline_pos++] = (uint8_t)((ipv6_hdr_get_flowlabel(ipv6_hdr) & 0x0000ff00) >> 8);
        iphc_dispatch[inline_pos++] = (uint8_t) (ipv6_hdr_get_flowlabel(ipv6_hdr) & 0x000000ff);
    }

    return inline_pos;
}

static uint16_t _encode_nextheader(uint8_t *iphc_dispatch, uint16_t inline_pos,
                                   ipv6_hdr_t *ipv6_hdr)
{
    switch (ipv6_hdr->nextheader) {
        default:
            iphc_dispatch[inline_pos++] = ipv6_hdr->nextheader;
            break;
    }

    return inline_pos;
}

static uint16_t _encode_hoplimit(uint8_t *iphc_dispatch, uint16_t inline_pos,
                                 ipv6_hdr_t *ipv6_hdr)
{
    switch (ipv6_hdr->hoplimit) {
        case (1):
            /* The Hop Limit field is compressed and the hop limit is 1. */
            iphc_dispatch[_IPHC1] |= _IPHC_HL_1;

            break;

        case (64):
            /* The Hop Limit field is compressed and the hop limit is 64. */
            iphc_dispatch[_IPHC1] |= _IPHC_HL_64;

            break;

        case (255):
            /* The Hop Limit field is compressed and the hop limit is 255. */
            iphc_dispatch[_IPHC1] |= _IPHC_HL_255;

            break;

        default:
            iphc_dispatch[inline_pos++] = ipv6_hdr->hoplimit;
            break;
    }

    return inline_pos;
}

static uint16_t _encode_src_addr(uint8_t *iphc_dispatch, ipv6_hdr_t *ipv6_hdr,
                                 uint16_t inline_pos, void *l2_src,
                                 size_t l2_src_len, sixlowpan_iphc_cbuf_t *context)
{
    if (ipv6_addr_is_unspecified(&(ipv6_hdr->srcaddr))) {
        iphc_dispatch[_IPHC2] |= _iphc_set_sac_sam_value(_IPHC_SAC_SAM_UNSPECIFIED);
    }
    else {
        if (context != NULL) {
            /* 1: Source address compression uses stateful, context-based
             *    compression. */
            iphc_dispatch[_IPHC2] |= SIXLOWPAN_IPHC2_SAC;

            if (context->cid != 0) {    /* if context is 0, it's default and
                                         * can be elided */
                iphc_dispatch[_IPHC_CID_EXT] |= _iphc_set_sci(context->cid);
            }
        }

        if (context != NULL || ipv6_addr_is_link_local(&ipv6_hdr->srcaddr)) {
            network_uint64_t iid = _iphc_init_iid(l2_src, l2_src_len);

            if ((memcmp(&(ipv6_hdr->srcaddr.u8[8]), &iid, sizeof(uint64_t)) == 0) ||
                _context_overlaps_iid(context, &(ipv6_hdr->srcaddr), iid)) {
                /* 0 bits. The address is derived using the link-layer address */
                iphc_dispatch[_IPHC2] |= _iphc_set_sac_sam_value(_IPHC_SAC_SAM_FROM_L2_SRC);
            }
            else if (_ipv6_addr_from_16_bit(&(ipv6_hdr->srcaddr))) {
                /* 16 bits. The address is derived using 16 bits carried inline */
                iphc_dispatch[_IPHC2] |= _iphc_set_sac_sam_value(_IPHC_SAC_SAM_16_INLINE);
                memcpy(&(iphc_dispatch[inline_pos]), &(ipv6_hdr->srcaddr.u16[7]), 2);
                inline_pos += 2;
            }
            else {
                /* 64 bits. The address is derived using 64 bits carried inline */
                iphc_dispatch[_IPHC2] |= _iphc_set_sac_sam_value(_IPHC_SAC_SAM_64_INLINE);
                memcpy(&(iphc_dispatch[inline_pos]), &(ipv6_hdr->srcaddr.u8[8]), 8);
                inline_pos += 8;
            }
        }
        else {
            /* full address carried inline */
            memcpy(&(iphc_dispatch[inline_pos]), &(ipv6_hdr->srcaddr), 16);
            inline_pos += 16;
        }
    }

    return inline_pos;
}

static uint16_t _encode_dest_addr(uint8_t *iphc_dispatch, ipv6_hdr_t *ipv6_hdr,
                                  uint16_t inline_pos, void *l2_dest,
                                  size_t l2_dest_len, sixlowpan_iphc_cbuf_t *context)
{
    uint8_t compressed = 0;

    /* M: Multicast compression */
    if (ipv6_addr_is_multicast(&(ipv6_hdr->destaddr))) {
        iphc_dispatch[_IPHC2] |= SIXLOWPAN_IPHC2_M;

        /* if multicast address of form ffXX::XXXX:XXXX:XXXX */
        if ((ipv6_hdr->destaddr.u16[1].u16 == 0) &&
            (ipv6_hdr->destaddr.u32[1].u32 == 0) &&
            (ipv6_hdr->destaddr.u16[4].u16 == 0)) {
            /* if multicast address of form ff02::XX */
            if ((ipv6_hdr->destaddr.u8[1] == 2) &&
                (ipv6_hdr->destaddr.u32[2].u32 == 0) &&
                (ipv6_hdr->destaddr.u16[6].u16 == 0) &&
                (ipv6_hdr->destaddr.u8[14] == 0)) {
                iphc_dispatch[_IPHC2] |= _IPHC_M_DAC_DAM_MC_8_INLINE;
                iphc_dispatch[inline_pos++] = ipv6_hdr->destaddr.u8[15];
                compressed = 1;
            }
            /* if multicast address of form ffXX::XX:XXXX */
            else if ((ipv6_hdr->destaddr.u16[5].u16 == 0) &&
                     (ipv6_hdr->destaddr.u8[12] == 0)) {
                iphc_dispatch[_IPHC2] |= _IPHC_M_DAC_DAM_MC_32_INLINE;
                iphc_dispatch[inline_pos++] = ipv6_hdr->destaddr.u8[1];
                memcpy(&(iphc_dispatch[inline_pos]), &(ipv6_hdr->destaddr.u8[13]), 3);
                inline_pos += 3;
                compressed = 1;
            }
            /* if multicast address of form ffXX::XX:XXXX:XXXX */
            else if (ipv6_hdr->destaddr.u8[10] == 0) {
                iphc_dispatch[_IPHC2] |= _IPHC_M_DAC_DAM_MC_48_INLINE;
                iphc_dispatch[inline_pos++] = ipv6_hdr->destaddr.u8[1];
                memcpy(&(iphc_dispatch[inline_pos]), &(ipv6_hdr->destaddr.u8[11]), 5);
                inline_pos += 5;
                compressed = 1;
            }
        }
        else {
            /* unicast prefix based compression */
            ipv6_addr_t unicast_prefix;
            unicast_prefix.u16[0] = ipv6_hdr->destaddr.u16[2];
            unicast_prefix.u16[1] = ipv6_hdr->destaddr.u16[3];
            unicast_prefix.u16[2] = ipv6_hdr->destaddr.u16[4];
            unicast_prefix.u16[3] = ipv6_hdr->destaddr.u16[5];

            context = sixlowpan_iphc_cbuf_lookup_addr(&unicast_prefix);

            if (context != NULL && context->prefix_len == ipv6_hdr->destaddr.u8[3]) {
                /* Unicast-Prefix based IPv6 multicast address with given context
                 * for unicast prefix -> context based compression */
                iphc_dispatch[_IPHC2] |= SIXLOWPAN_IPHC2_DAC;
                iphc_dispatch[inline_pos++] = ipv6_hdr->destaddr.u8[1];
                iphc_dispatch[inline_pos++] = ipv6_hdr->destaddr.u8[2];
                memcpy(&(iphc_dispatch[inline_pos]), &(ipv6_hdr->destaddr.u16[6]), 4);
                inline_pos += 4;
                compressed = 1;
            }
        }
    }
    else {
        if (context != NULL) {
            /* 1: Destination address compression uses stateful, context-based
             *    compression. */
            iphc_dispatch[_IPHC2] |= SIXLOWPAN_IPHC2_DAC;

            if (context->cid != 0) {    /* if context is 0, it's default and
                                         * can be elided */
                iphc_dispatch[_IPHC_CID_EXT] |= _iphc_set_dci(context->cid);
            }
        }

        if (context != NULL || ipv6_addr_is_link_local(&ipv6_hdr->destaddr)) {
            network_uint64_t iid = _iphc_init_iid(l2_dest, l2_dest_len);

            if ((memcmp(&(ipv6_hdr->destaddr.u8[8]), &iid, sizeof(uint64_t)) == 0) ||
                _context_overlaps_iid(context, &(ipv6_hdr->destaddr), iid)) {
                /* 0 bits. The address is derived using the link-layer address */
                iphc_dispatch[_IPHC2] |= _iphc_set_m_dac_dam_value(_IPHC_M_DAC_DAM_FROM_L2_DEST);
                compressed = 1;
            }
            else if (_ipv6_addr_from_16_bit(&(ipv6_hdr->destaddr))) {
                /* 16 bits. The address is derived using 16 bits carried inline */
                iphc_dispatch[_IPHC2] |= _iphc_set_m_dac_dam_value(_IPHC_M_DAC_DAM_16_INLINE);
                memcpy(&(iphc_dispatch[inline_pos]), &(ipv6_hdr->destaddr.u16[7]), 2);
                inline_pos += 2;
                compressed = 1;
            }
            else {
                /* 64 bits. The address is derived using 64 bits carried inline */
                iphc_dispatch[_IPHC2] |= _iphc_set_m_dac_dam_value(_IPHC_M_DAC_DAM_64_INLINE);
                memcpy(&(iphc_dispatch[inline_pos]), &(ipv6_hdr->destaddr.u8[8]), 8);
                inline_pos += 8;
                compressed = 1;
            }
        }
    }

    if (!compressed) {
        /* full address carried inline */
        memcpy(&(iphc_dispatch[inline_pos]), &(ipv6_hdr->destaddr), 16);
        inline_pos += 16;
    }

    return inline_pos;
}

/**
 * @}
 */
