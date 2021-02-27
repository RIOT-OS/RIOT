/*
 * Copyright (C) 2017 Freie Universität Berlin
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

#include "net/gnrc.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/netdev/ieee802154.h"

#ifdef MODULE_GNRC_IPV6
#include "net/ipv6/hdr.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#include "od.h"

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);
static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif);

static const gnrc_netif_ops_t ieee802154_ops = {
    .init = gnrc_netif_default_init,
    .send = _send,
    .recv = _recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
};

int gnrc_netif_ieee802154_create(gnrc_netif_t *netif, char *stack, int stacksize,
                                 char priority, const char *name, netdev_t *dev)
{
    return gnrc_netif_create(netif, stack, stacksize, priority, name, dev,
                             &ieee802154_ops);
}

static gnrc_pktsnip_t *_make_netif_hdr(uint8_t *mhr)
{
    gnrc_netif_hdr_t *hdr;
    gnrc_pktsnip_t *snip;
    uint8_t src[IEEE802154_LONG_ADDRESS_LEN], dst[IEEE802154_LONG_ADDRESS_LEN];
    int src_len, dst_len;
    le_uint16_t _pan_tmp;   /* TODO: hand-up PAN IDs to GNRC? */

    dst_len = ieee802154_get_dst(mhr, dst, &_pan_tmp);
    src_len = ieee802154_get_src(mhr, src, &_pan_tmp);
    if ((dst_len < 0) || (src_len < 0)) {
        DEBUG("_make_netif_hdr: unable to get addresses\n");
        return NULL;
    }
    /* allocate space for header */
    snip = gnrc_netif_hdr_build(src, (size_t)src_len, dst, (size_t)dst_len);
    if (snip == NULL) {
        DEBUG("_make_netif_hdr: no space left in packet buffer\n");
        return NULL;
    }
    hdr = snip->data;
    /* set broadcast flag for broadcast destination */
    if ((dst_len == 2) && (dst[0] == 0xff) && (dst[1] == 0xff)) {
        hdr->flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
    }
    /* set flags for pending frames */
    if (mhr[0] & IEEE802154_FCF_FRAME_PEND) {
        hdr->flags |= GNRC_NETIF_HDR_FLAGS_MORE_DATA;
    }
    return snip;
}

#if MODULE_GNRC_NETIF_DEDUP
static inline bool _already_received(gnrc_netif_t *netif,
                                     gnrc_netif_hdr_t *netif_hdr,
                                     uint8_t *mhr)
{
    const uint8_t seq = ieee802154_get_seq(mhr);

    return  (netif->last_pkt.seq == seq) &&
            (netif->last_pkt.src_len == netif_hdr->src_l2addr_len) &&
            (memcmp(netif->last_pkt.src, gnrc_netif_hdr_get_src_addr(netif_hdr),
                    netif_hdr->src_l2addr_len) == 0);
}
#endif /* MODULE_GNRC_NETIF_DEDUP */

static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;
    netdev_ieee802154_rx_info_t rx_info;
    gnrc_pktsnip_t *pkt = NULL;
    int bytes_expected = dev->driver->recv(dev, NULL, 0, NULL);

    if (bytes_expected >= (int)IEEE802154_MIN_FRAME_LEN) {
        int nread;

        pkt = gnrc_pktbuf_add(NULL, NULL, bytes_expected, GNRC_NETTYPE_UNDEF);
        if (pkt == NULL) {
            DEBUG("_recv_ieee802154: cannot allocate pktsnip.\n");
            /* Discard packet on netdev device */
            dev->driver->recv(dev, NULL, bytes_expected, NULL);
            return NULL;
        }
        nread = dev->driver->recv(dev, pkt->data, bytes_expected, &rx_info);
        if (nread <= 0) {
            gnrc_pktbuf_release(pkt);
            return NULL;
        }
#ifdef MODULE_NETSTATS_L2
        netif->stats.rx_count++;
        netif->stats.rx_bytes += nread;
#endif

        if (netif->flags & GNRC_NETIF_FLAGS_RAWMODE) {
            /* Raw mode, skip packet processing, but provide rx_info via
             * GNRC_NETTYPE_NETIF */
            gnrc_pktsnip_t *netif_snip = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
            if (netif_snip == NULL) {
                DEBUG("_recv_ieee802154: no space left in packet buffer\n");
                gnrc_pktbuf_release(pkt);
                return NULL;
            }
            gnrc_netif_hdr_t *hdr = netif_snip->data;
            hdr->lqi = rx_info.lqi;
            hdr->rssi = rx_info.rssi;
            gnrc_netif_hdr_set_netif(hdr, netif);
            pkt = gnrc_pkt_append(pkt, netif_snip);
        }
        else {
            /* Normal mode, try to parse the frame according to IEEE 802.15.4 */
            gnrc_pktsnip_t *ieee802154_hdr, *netif_hdr;
            gnrc_netif_hdr_t *hdr;
            size_t mhr_len = ieee802154_get_frame_hdr_len(pkt->data);
            uint8_t *mhr = pkt->data;
            /* nread was checked for <= 0 before so we can safely cast it to
             * unsigned */
            if ((mhr_len == 0) || ((size_t)nread < mhr_len)) {
                DEBUG("_recv_ieee802154: illegally formatted frame received\n");
                gnrc_pktbuf_release(pkt);
                return NULL;
            }
            netif_hdr = _make_netif_hdr(mhr);
            if (netif_hdr == NULL) {
                DEBUG("_recv_ieee802154: no space left in packet buffer\n");
                gnrc_pktbuf_release(pkt);
                return NULL;
            }
            hdr = netif_hdr->data;

#ifdef MODULE_L2FILTER
            if (!l2filter_pass(dev->filter, gnrc_netif_hdr_get_src_addr(hdr),
                               hdr->src_l2addr_len)) {
                gnrc_pktbuf_release(pkt);
                gnrc_pktbuf_release(netif_hdr);
                DEBUG("_recv_ieee802154: packet dropped by l2filter\n");
                return NULL;
            }
#endif
#ifdef MODULE_GNRC_NETIF_DEDUP
            if (_already_received(netif, hdr, mhr)) {
                gnrc_pktbuf_release(pkt);
                gnrc_pktbuf_release(netif_hdr);
                DEBUG("_recv_ieee802154: packet dropped by deduplication\n");
                return NULL;
            }
            memcpy(netif->last_pkt.src, gnrc_netif_hdr_get_src_addr(hdr),
                   hdr->src_l2addr_len);
            netif->last_pkt.src_len = hdr->src_l2addr_len;
            netif->last_pkt.seq = ieee802154_get_seq(mhr);
#endif /* MODULE_GNRC_NETIF_DEDUP */
#if IS_USED(MODULE_IEEE802154_SECURITY)
            {
                uint8_t *payload = NULL;
                uint16_t payload_size = 0;
                uint8_t *mic = NULL;
                uint8_t mic_size = 0;
                if (mhr[0] & NETDEV_IEEE802154_SECURITY_EN) {
                    if (ieee802154_sec_decrypt_frame(&((netdev_ieee802154_t *)dev)->sec_ctx,
                                                     nread,
                                                     mhr, (uint8_t *)&mhr_len,
                                                     &payload, &payload_size,
                                                     &mic, &mic_size,
                                                     gnrc_netif_hdr_get_src_addr(hdr)) != 0) {
                        DEBUG("_recv_ieee802154: packet dropped by security check\n");
                        gnrc_pktbuf_release(pkt);
                        gnrc_pktbuf_release(netif_hdr);
                        return NULL;
                    }
                }
                nread -= mic_size;
            }
#endif
            hdr->lqi = rx_info.lqi;
            hdr->rssi = rx_info.rssi;
            gnrc_netif_hdr_set_netif(hdr, netif);
            dev->driver->get(dev, NETOPT_PROTO, &pkt->type, sizeof(pkt->type));
            if (IS_ACTIVE(ENABLE_DEBUG)) {
                char src_str[GNRC_NETIF_HDR_L2ADDR_PRINT_LEN];

                DEBUG("_recv_ieee802154: received packet from %s of length %u\n",
                    gnrc_netif_addr_to_str(gnrc_netif_hdr_get_src_addr(hdr),
                                            hdr->src_l2addr_len,
                                            src_str),
                    nread);
                if (IS_USED(MODULE_OD)) {
                    od_hex_dump(pkt->data, nread, OD_WIDTH_DEFAULT);
                }
            }
            /* mark IEEE 802.15.4 header */
            ieee802154_hdr = gnrc_pktbuf_mark(pkt, mhr_len, GNRC_NETTYPE_UNDEF);
            if (ieee802154_hdr == NULL) {
                DEBUG("_recv_ieee802154: no space left in packet buffer\n");
                gnrc_pktbuf_release(pkt);
                gnrc_pktbuf_release(netif_hdr);
                return NULL;
            }
            nread -= ieee802154_hdr->size;
            gnrc_pktbuf_remove_snip(pkt, ieee802154_hdr);
            pkt = gnrc_pkt_append(pkt, netif_hdr);
        }

        DEBUG("_recv_ieee802154: reallocating MAC payload for upper layer.\n");
        gnrc_pktbuf_realloc_data(pkt, nread);
    } else if (bytes_expected > 0) {
        DEBUG("_recv_ieee802154: received frame is too short\n");
        dev->driver->recv(dev, NULL, bytes_expected, NULL);
    }

    return pkt;
}

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    netdev_t *dev = netif->dev;
    netdev_ieee802154_t *state = (netdev_ieee802154_t *)netif->dev;
    gnrc_netif_hdr_t *netif_hdr;
    const uint8_t *src, *dst = NULL;
    int res = 0;
    size_t src_len, dst_len;
    uint8_t mhr_len;
#if IS_USED(MODULE_IEEE802154_SECURITY)
    uint8_t mhr[IEEE802154_MAX_HDR_LEN + IEEE802154_SEC_MAX_AUX_HDR_LEN];
#else
    uint8_t mhr[IEEE802154_MAX_HDR_LEN];
#endif
    uint8_t flags = (uint8_t)(state->flags & NETDEV_IEEE802154_SEND_MASK);
    le_uint16_t dev_pan = byteorder_htols(state->pan);

    flags |= IEEE802154_FCF_TYPE_DATA;
    if (pkt == NULL) {
        DEBUG("_send_ieee802154: pkt was NULL\n");
        return -EINVAL;
    }
    if (pkt->type != GNRC_NETTYPE_NETIF) {
        DEBUG("_send_ieee802154: first header is not generic netif header\n");
        return -EBADMSG;
    }
    netif_hdr = pkt->data;
    if (netif_hdr->flags & GNRC_NETIF_HDR_FLAGS_MORE_DATA) {
        /* Set frame pending field */
        flags |= IEEE802154_FCF_FRAME_PEND;
    }
    /* prepare destination address */
    if (netif_hdr->flags & /* If any of these flags is set assume broadcast */
        (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        dst = ieee802154_addr_bcast;
        dst_len = IEEE802154_ADDR_BCAST_LEN;
    }
    else {
        dst = gnrc_netif_hdr_get_dst_addr(netif_hdr);
        dst_len = netif_hdr->dst_l2addr_len;
    }
    if (flags & NETDEV_IEEE802154_SECURITY_EN) {
        /* need to include long source address because the recipient
           will need it to decrypt the frame */
        src_len = IEEE802154_LONG_ADDRESS_LEN;
        src = state->long_addr;
    }
    else {
        src_len = netif_hdr->src_l2addr_len;
        if (src_len > 0) {
            src = gnrc_netif_hdr_get_src_addr(netif_hdr);
        }
        else {
            src_len = netif->l2addr_len;
            src = netif->l2addr;
        }
    }
    /* fill MAC header, seq should be set by device */
    if ((res = ieee802154_set_frame_hdr(mhr, src, src_len,
                                        dst, dst_len, dev_pan,
                                        dev_pan, flags, state->seq++)) == 0) {
        DEBUG("_send_ieee802154: Error preperaring frame\n");
        gnrc_pktbuf_release(pkt);
        return -EINVAL;
    }
    mhr_len = res;

    /* prepare iolist for netdev / mac layer */
    iolist_t iolist_header = {
        .iol_next = (iolist_t *)pkt->next,
        .iol_base = mhr,
        .iol_len = mhr_len
    };

#if IS_USED(MODULE_IEEE802154_SECURITY)
    {
        /* write protect `pkt` to set `pkt->next` */
        gnrc_pktsnip_t *tmp = gnrc_pktbuf_start_write(pkt);
        if (!tmp) {
            DEBUG("_send_ieee802154: no write access to pkt");
            gnrc_pktbuf_release(pkt);
            return -ENOMEM;
        }
        pkt = tmp;
        tmp = gnrc_pktbuf_start_write(pkt->next);
        if (!tmp) {
            DEBUG("_send_ieee802154: no write access to pkt->next");
            gnrc_pktbuf_release(pkt);
            return -ENOMEM;
        }
        pkt->next = tmp;
        /* merge snippets to store the L2 payload uniformly in one buffer */
        res = gnrc_pktbuf_merge(pkt->next);
        if (res < 0) {
            DEBUG("_send_ieee802154: failed to merge pktbuf\n");
            gnrc_pktbuf_release(pkt);
            return res;
        }

        iolist_header.iol_next = (iolist_t *)pkt->next;

        uint8_t mic[IEEE802154_SEC_MAX_MAC_SIZE];
        uint8_t mic_size = 0;

        if (flags & NETDEV_IEEE802154_SECURITY_EN) {
            res = ieee802154_sec_encrypt_frame(&state->sec_ctx,
                                               mhr, &mhr_len,
                                               pkt->next->data, pkt->next->size,
                                               mic, &mic_size,
                                               state->long_addr);
            if (res != 0) {
                DEBUG("_send_ieee802154: encryption failedf\n");
                gnrc_pktbuf_release(pkt);
                return res;
            }
        }
        if (mic_size) {
            gnrc_pktsnip_t *pktmic = gnrc_pktbuf_add(pkt->next->next,
                                                     mic, mic_size,
                                                     GNRC_NETTYPE_UNDEF);
            if (!pktmic) {
                DEBUG("_send_ieee802154: no space left in pktbuf to allocate MIC\n");
                gnrc_pktbuf_release(pkt);
                return -ENOMEM;
            }
            pkt->next->next = pktmic;
        }
        iolist_header.iol_len = mhr_len;
    }
#endif
#ifdef MODULE_NETSTATS_L2
    if (netif_hdr->flags &
            (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        netif->stats.tx_mcast_count++;
    }
    else {
        netif->stats.tx_unicast_count++;
    }
#endif
#ifdef MODULE_GNRC_MAC
    if (netif->mac.mac_info & GNRC_NETIF_MAC_INFO_CSMA_ENABLED) {
        res = csma_sender_csma_ca_send(dev, &iolist_header, &netif->mac.csma_conf);
    }
    else {
        res = dev->driver->send(dev, &iolist_header);
    }
#else
    res = dev->driver->send(dev, &iolist_header);
#endif

    /* release old data */
    gnrc_pktbuf_release(pkt);
    return res;
}
/** @} */
