/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ng_at86rf2xx
 * @{
 *
 * @file
 * @brief       Netdev adaption for the AT86RF2xx drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "net/eui64.h"
#include "net/ng_ieee802154.h"
#include "net/ng_netbase.h"
#include "ng_at86rf2xx.h"
#include "ng_at86rf2xx_netdev.h"
#include "ng_at86rf2xx_internal.h"
#include "ng_at86rf2xx_registers.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define _MAX_MHR_OVERHEAD   (25)

/* TODO: generalize and move to ng_ieee802154 */
static size_t _make_data_frame_hdr(ng_at86rf2xx_t *dev, uint8_t *buf,
                                   ng_netif_hdr_t *hdr)
{
    int pos = 0;

    /* we are building a data frame here */
    buf[0] = NG_IEEE802154_FCF_TYPE_DATA;
    buf[1] = NG_IEEE802154_FCF_VERS_V1;

    /* if AUTOACK is enabled, then we also expect ACKs for this packet */
    if (!(hdr->flags & NG_NETIF_HDR_FLAGS_BROADCAST) &&
        !(hdr->flags & NG_NETIF_HDR_FLAGS_MULTICAST) &&
        (dev->options & NG_AT86RF2XX_OPT_AUTOACK)) {
        buf[0] |= NG_IEEE802154_FCF_ACK_REQ;
    }

    /* fill in destination PAN ID */
    pos = 3;
    buf[pos++] = (uint8_t)((dev->pan) & 0xff);
    buf[pos++] = (uint8_t)((dev->pan) >> 8);

    /* fill in destination address */
    if (hdr->flags &
        (NG_NETIF_HDR_FLAGS_BROADCAST | NG_NETIF_HDR_FLAGS_MULTICAST)) {
        buf[1] |= NG_IEEE802154_FCF_DST_ADDR_SHORT;
        buf[pos++] = 0xff;
        buf[pos++] = 0xff;
    }
    else if (hdr->dst_l2addr_len == 2) {
        uint8_t *dst_addr = ng_netif_hdr_get_dst_addr(hdr);
        buf[1] |= NG_IEEE802154_FCF_DST_ADDR_SHORT;
        buf[pos++] = dst_addr[1];
        buf[pos++] = dst_addr[0];
    }
    else if (hdr->dst_l2addr_len == 8) {
        buf[1] |= NG_IEEE802154_FCF_DST_ADDR_LONG;
        uint8_t *dst_addr = ng_netif_hdr_get_dst_addr(hdr);
        for (int i = 7;  i >= 0; i--) {
            buf[pos++] = dst_addr[i];
        }
    }
    else {
        /* unsupported address length */
        return 0;
    }

    /* fill in source PAN ID (if applicable */
    if (dev->options & NG_AT86RF2XX_OPT_USE_SRC_PAN) {
        buf[pos++] = (uint8_t)((dev->pan) & 0xff);
        buf[pos++] = (uint8_t)((dev->pan) >> 8);
    } else {
        buf[0] |= NG_IEEE802154_FCF_PAN_COMP;
    }

    /* fill in source address */
    if (dev->options & NG_AT86RF2XX_OPT_SRC_ADDR_LONG) {
        buf[1] |= NG_IEEE802154_FCF_SRC_ADDR_LONG;
        memcpy(&(buf[pos]), dev->addr_long, 8);
        pos += 8;
    }
    else {
        buf[1] |= NG_IEEE802154_FCF_SRC_ADDR_SHORT;
        buf[pos++] = dev->addr_short[0];
        buf[pos++] = dev->addr_short[1];
    }

    /* set sequence number */
    buf[2] = dev->seq_nr++;
    /* return actual header length */
    return pos;
}

/* TODO: generalize and move to ng_ieee802154 */
/* TODO: include security header implications */
static size_t _get_frame_hdr_len(uint8_t *mhr)
{
    uint8_t tmp;
    size_t len = 3;

    /* figure out address sizes */
    tmp = (mhr[1] & NG_IEEE802154_FCF_DST_ADDR_MASK);
    if (tmp == NG_IEEE802154_FCF_DST_ADDR_SHORT) {
        len += 4;
    }
    else if (tmp == NG_IEEE802154_FCF_DST_ADDR_LONG) {
        len += 10;
    }
    else if (tmp != NG_IEEE802154_FCF_DST_ADDR_VOID) {
        return 0;
    }
    tmp = (mhr[1] & NG_IEEE802154_FCF_SRC_ADDR_MASK);
    if (tmp == NG_IEEE802154_FCF_SRC_ADDR_VOID) {
        return len;
    }
    else {
        if (!(mhr[0] & NG_IEEE802154_FCF_PAN_COMP)) {
            len += 2;
        }
        if (tmp == NG_IEEE802154_FCF_SRC_ADDR_SHORT) {
            return (len + 2);
        }
        else if (tmp == NG_IEEE802154_FCF_SRC_ADDR_LONG) {
            return (len + 8);
        }
    }
    return 0;
}

/* TODO: generalize and move to ng_ieee802154 */
static ng_pktsnip_t *_make_netif_hdr(uint8_t *mhr)
{
    uint8_t tmp;
    uint8_t *addr;
    uint8_t src_len, dst_len;
    ng_pktsnip_t *snip;
    ng_netif_hdr_t *hdr;

    /* figure out address sizes */
    tmp = mhr[1] & NG_IEEE802154_FCF_SRC_ADDR_MASK;
    if (tmp == NG_IEEE802154_FCF_SRC_ADDR_SHORT) {
        src_len = 2;
    }
    else if (tmp == NG_IEEE802154_FCF_SRC_ADDR_LONG) {
        src_len = 8;
    }
    else if (tmp == NG_IEEE802154_FCF_SRC_ADDR_VOID) {
        src_len = 0;
    }
    else {
        return NULL;
    }
    tmp = mhr[1] & NG_IEEE802154_FCF_DST_ADDR_MASK;
    if (tmp == NG_IEEE802154_FCF_DST_ADDR_SHORT) {
        dst_len = 2;
    }
    else if (tmp == NG_IEEE802154_FCF_DST_ADDR_LONG) {
        dst_len = 8;
    }
    else if (tmp == NG_IEEE802154_FCF_DST_ADDR_VOID) {
        dst_len = 0;
    }
    else {
        return NULL;
    }
    /* allocate space for header */
    snip = ng_pktbuf_add(NULL, NULL, sizeof(ng_netif_hdr_t) + src_len + dst_len,
                         NG_NETTYPE_NETIF);
    if (snip == NULL) {
        return NULL;
    }
    /* fill header */
    hdr = (ng_netif_hdr_t *)snip->data;
    ng_netif_hdr_init(hdr, src_len, dst_len);
    if (dst_len > 0) {
        tmp = 5 + dst_len;
        addr = ng_netif_hdr_get_dst_addr(hdr);
        for (int i = 0; i < dst_len; i++) {
            addr[i] = mhr[5 + (dst_len - i) - 1];
        }
    }
    else {
        tmp = 3;
    }
    if (!(mhr[0] & NG_IEEE802154_FCF_PAN_COMP)) {
        tmp += 2;
    }
    if (src_len > 0) {
        addr = ng_netif_hdr_get_src_addr(hdr);
        for (int i = 0; i < src_len; i++) {
            addr[i] = mhr[tmp + (src_len - i) - 1];
        }
    }
    return snip;
}


static int _send(ng_netdev_t *netdev, ng_pktsnip_t *pkt)
{
    ng_at86rf2xx_t *dev = (ng_at86rf2xx_t *)netdev;
    ng_pktsnip_t *snip;
    uint8_t mhr[NG_IEEE802154_MAX_HDR_LEN];
    size_t len;

    if (pkt == NULL) {
        return -ENOMSG;
    }
    if (dev == NULL) {
        ng_pktbuf_release(pkt);
        return -ENODEV;
    }

    /* create 802.15.4 header */
    len = _make_data_frame_hdr(dev, mhr, (ng_netif_hdr_t *)pkt->data);
    if (len == 0) {
        DEBUG("[ng_at86rf2xx] error: unable to create 802.15.4 header\n");
        ng_pktbuf_release(pkt);
        return -ENOMSG;
    }
    /* check if packet (header + payload + FCS) fits into FIFO */
    snip = pkt->next;
    if ((ng_pkt_len(snip) + len + 2) > NG_AT86RF2XX_MAX_PKT_LENGTH) {
        printf("[ng_at86rf2xx] error: packet too large (%u byte) to be send\n",
               ng_pkt_len(snip) + len + 2);
        ng_pktbuf_release(pkt);
        return -EOVERFLOW;
    }

    ng_at86rf2xx_tx_prepare(dev);
    /* put header into FIFO */
    len = ng_at86rf2xx_tx_load(dev, mhr, len, 0);
    /* load packet data into FIFO */
    while (snip) {
        len = ng_at86rf2xx_tx_load(dev, snip->data, snip->size, len);
        snip = snip->next;
    }
    /* send data out directly if pre-loading id disabled */
    if (!(dev->options & NG_AT86RF2XX_OPT_PRELOADING)) {
        ng_at86rf2xx_tx_exec(dev);
    }
    /* release packet */
    ng_pktbuf_release(pkt);
    /* return the number of bytes that were actually send out */
    return (int)len;
}

static void _receive_data(ng_at86rf2xx_t *dev)
{
    uint8_t mhr[NG_IEEE802154_MAX_HDR_LEN];
    size_t pkt_len, hdr_len;
    ng_pktsnip_t *hdr, *payload = NULL;
    ng_netif_hdr_t *netif;

    /* get the size of the received packet (unlocks frame buffer protection) */
    pkt_len = ng_at86rf2xx_rx_len(dev);

    /* abort here already if no event callback is registered */
    if (!dev->event_cb) {
        return;
    }

    /* in raw mode, just read the binary dump into the packet buffer */
    if (dev->options & NG_AT86RF2XX_OPT_RAWDUMP) {
        payload = ng_pktbuf_add(NULL, NULL, pkt_len, NG_NETTYPE_UNDEF);
        if (payload == NULL ) {
            DEBUG("[ng_at86rf2xx] error: unable to allocate RAW data\n");
            return;
        }
        ng_at86rf2xx_rx_read(dev, payload->data, pkt_len, 0);
        dev->event_cb(NETDEV_EVENT_RX_COMPLETE, payload);
        return;
    }

    /* get FCF field and compute 802.15.4 header length */
    ng_at86rf2xx_rx_read(dev, mhr, 2, 0);
    hdr_len = _get_frame_hdr_len(mhr);
    if (hdr_len == 0) {
        DEBUG("[ng_at86rf2xx] error: unable parse incoming frame header\n");
        return;
    }
    /* read the rest of the header and parse the netif header from it */
    ng_at86rf2xx_rx_read(dev, &(mhr[2]), hdr_len - 2, 2);
    hdr = _make_netif_hdr(mhr);
    if (hdr == NULL) {
        DEBUG("[ng_at86rf2xx] error: unable to allocate netif header\n");
        return;
    }
    /* fill missing fields in netif header */
    netif = (ng_netif_hdr_t *)hdr->data;
    netif->if_pid = dev->mac_pid;
    ng_at86rf2xx_rx_read(dev, &(netif->lqi), 1, pkt_len);
    netif->rssi = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__PHY_ED_LEVEL);

    /* allocate payload */
    payload = ng_pktbuf_add(hdr, NULL, (pkt_len - hdr_len), dev->proto);
    if (payload == NULL) {
        DEBUG("[ng_at86rf2xx] error: unable to allocate incoming payload\n");
        ng_pktbuf_release(hdr);
        return;
    }
    /* copy payload */
    ng_at86rf2xx_rx_read(dev, payload->data, payload->size, hdr_len);
    /* finish up and send data to upper layers */
    dev->event_cb(NETDEV_EVENT_RX_COMPLETE, payload);
}

static int _set_state(ng_at86rf2xx_t *dev, ng_netopt_state_t state)
{
    switch (state) {
        case NG_NETOPT_STATE_SLEEP:
            ng_at86rf2xx_set_state(dev, NG_AT86RF2XX_STATE_TRX_OFF);
            break;
        case NG_NETOPT_STATE_IDLE:
            ng_at86rf2xx_set_state(dev, NG_AT86RF2XX_STATE_RX_AACK_ON);
            break;
        case NG_NETOPT_STATE_TX:
            if (dev->options & NG_AT86RF2XX_OPT_PRELOADING) {
                ng_at86rf2xx_tx_exec(dev);
            }
            break;
        case NG_NETOPT_STATE_RESET:
            ng_at86rf2xx_reset(dev);
            break;
        default:
            return -ENOTSUP;
    }
    return sizeof(ng_netopt_state_t);
}

ng_netopt_state_t _get_state(ng_at86rf2xx_t *dev)
{
    switch (ng_at86rf2xx_get_status(dev)) {
        case NG_AT86RF2XX_STATE_SLEEP:
            return NG_NETOPT_STATE_SLEEP;
        case NG_AT86RF2XX_STATE_BUSY_RX_AACK:
            return NG_NETOPT_STATE_RX;
        case NG_AT86RF2XX_STATE_BUSY_TX_ARET:
        case NG_AT86RF2XX_STATE_TX_ARET_ON:
            return NG_NETOPT_STATE_TX;
        case NG_AT86RF2XX_STATE_RX_AACK_ON:
        default:
            return NG_NETOPT_STATE_IDLE;
    }
}

static int _get(ng_netdev_t *device, ng_netopt_t opt, void *val, size_t max_len)
{
    if (device == NULL) {
        return -ENODEV;
    }
    ng_at86rf2xx_t *dev = (ng_at86rf2xx_t *) device;

    switch (opt) {

        case NG_NETOPT_ADDRESS:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)val) = ng_at86rf2xx_get_addr_short(dev);
            return sizeof(uint16_t);

        case NG_NETOPT_ADDRESS_LONG:
            if (max_len < sizeof(uint64_t)) {
                return -EOVERFLOW;
            }
            *((uint64_t *)val) = ng_at86rf2xx_get_addr_long(dev);
            return sizeof(uint64_t);

        case NG_NETOPT_ADDR_LEN:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)val) = 2;
            return sizeof(uint16_t);

        case NG_NETOPT_SRC_LEN:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            if (dev->options & NG_AT86RF2XX_OPT_SRC_ADDR_LONG) {
                *((uint16_t *)val) = 8;
            }
            else {
                *((uint16_t *)val) = 2;
            }
            return sizeof(uint16_t);

        case NG_NETOPT_NID:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)val) = dev->pan;
            return sizeof(uint16_t);

        case NG_NETOPT_IPV6_IID:
            if (max_len < sizeof(eui64_t)) {
                return -EOVERFLOW;
            }
            if (dev->options & NG_AT86RF2XX_OPT_SRC_ADDR_LONG) {
                uint64_t addr = ng_at86rf2xx_get_addr_long(dev);
                ng_ieee802154_get_iid(val, (uint8_t *)&addr, 8);
            }
            else {
                uint16_t addr = ng_at86rf2xx_get_addr_short(dev);
                ng_ieee802154_get_iid(val, (uint8_t *)&addr, 2);
            }
            return sizeof(eui64_t);

        case NG_NETOPT_PROTO:
            if (max_len < sizeof(ng_nettype_t)) {
                return -EOVERFLOW;
            }
            *((ng_nettype_t *)val) = dev->proto;
            return sizeof(ng_nettype_t);

        case NG_NETOPT_CHANNEL:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            ((uint8_t *)val)[1] = 0;
            ((uint8_t *)val)[0] = ng_at86rf2xx_get_chan(dev);
            return sizeof(uint16_t);

        case NG_NETOPT_TX_POWER:
            if (max_len < sizeof(int16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)val) = ng_at86rf2xx_get_txpower(dev);
            return sizeof(uint16_t);

        case NG_NETOPT_MAX_PACKET_SIZE:
            if (max_len < sizeof(int16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)val) = NG_AT86RF2XX_MAX_PKT_LENGTH - _MAX_MHR_OVERHEAD;
            return sizeof(uint16_t);

        case NG_NETOPT_STATE:
            if (max_len < sizeof(ng_netopt_state_t)) {
                return -EOVERFLOW;
            }
            *((ng_netopt_state_t*)val) = _get_state(dev);
            break;

        case NG_NETOPT_PRELOADING:
            if (dev->options & NG_AT86RF2XX_OPT_PRELOADING) {
                *((ng_netopt_enable_t *)val) = NG_NETOPT_ENABLE;
            }
            else {
                *((ng_netopt_enable_t *)val) = NG_NETOPT_DISABLE;
            }
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_AUTOACK:
            if (dev->options & NG_AT86RF2XX_OPT_AUTOACK) {
                *((ng_netopt_enable_t *)val) = NG_NETOPT_ENABLE;
            }
            else {
                *((ng_netopt_enable_t *)val) = NG_NETOPT_DISABLE;
            }
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_RETRANS:
            if (max_len < sizeof(uint8_t)) {
                return -EOVERFLOW;
            }
            *((uint8_t *)val) = ng_at86rf2xx_get_max_retries(dev);
            return sizeof(uint8_t);

        case NG_NETOPT_PROMISCUOUSMODE:
            if (dev->options & NG_AT86RF2XX_OPT_PROMISCUOUS) {
                *((ng_netopt_enable_t *)val) = NG_NETOPT_ENABLE;
            }
            else {
                *((ng_netopt_enable_t *)val) = NG_NETOPT_DISABLE;
            }
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_RAWMODE:
            if (dev->options & NG_AT86RF2XX_OPT_RAWDUMP) {
                *((ng_netopt_enable_t *)val) = NG_NETOPT_ENABLE;
            }
            else {
                *((ng_netopt_enable_t *)val) = NG_NETOPT_DISABLE;
            }
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_IS_CHANNEL_CLR:
            if (ng_at86rf2xx_cca(dev)) {
                *((ng_netopt_enable_t *)val) = NG_NETOPT_ENABLE;
            }
            else {
                *((ng_netopt_enable_t *)val) = NG_NETOPT_DISABLE;
            }
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_RX_START_IRQ:
            *((ng_netopt_enable_t *)val) =
                !!(dev->options & NG_AT86RF2XX_OPT_TELL_RX_START);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_RX_END_IRQ:
            *((ng_netopt_enable_t *)val) =
                !!(dev->options & NG_AT86RF2XX_OPT_TELL_RX_END);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_TX_START_IRQ:
            *((ng_netopt_enable_t *)val) =
                !!(dev->options & NG_AT86RF2XX_OPT_TELL_TX_START);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_TX_END_IRQ:
            *((ng_netopt_enable_t *)val) =
                !!(dev->options & NG_AT86RF2XX_OPT_TELL_TX_END);
            return sizeof(ng_netopt_enable_t);

        default:
            return -ENOTSUP;
    }

    return 0;
}

static int _set(ng_netdev_t *device, ng_netopt_t opt, void *val, size_t len)
{
    ng_at86rf2xx_t *dev = (ng_at86rf2xx_t *) device;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NG_NETOPT_ADDRESS:
            if (len > sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            ng_at86rf2xx_set_addr_short(dev, *((uint16_t*)val));
            return sizeof(uint16_t);

        case NG_NETOPT_ADDRESS_LONG:
            if (len > sizeof(uint64_t)) {
                return -EOVERFLOW;
            }
            ng_at86rf2xx_set_addr_long(dev, *((uint64_t*)val));
            return sizeof(uint64_t);

        case NG_NETOPT_SRC_LEN:
            if (len > sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            if (*((uint16_t *)val) == 2) {
                ng_at86rf2xx_set_option(dev, NG_AT86RF2XX_OPT_SRC_ADDR_LONG,
                                        false);
            }
            else if (*((uint16_t *)val) == 8) {
                ng_at86rf2xx_set_option(dev, NG_AT86RF2XX_OPT_SRC_ADDR_LONG,
                                        true);
            }
            else {
                return -ENOTSUP;
            }
            return sizeof(uint16_t);

        case NG_NETOPT_NID:
            if (len > sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            ng_at86rf2xx_set_pan(dev, *((uint16_t *)val));
            return sizeof(uint16_t);

        case NG_NETOPT_CHANNEL:
            if (len != sizeof(uint16_t)) {
                return -EINVAL;
            }
            uint8_t chan = ((uint8_t *)val)[0];
            if (chan < NG_AT86RF2XX_MIN_CHANNEL ||
                chan > NG_AT86RF2XX_MAX_CHANNEL) {
                return -ENOTSUP;
            }
            ng_at86rf2xx_set_chan(dev, chan);
            return sizeof(uint16_t);

        case NG_NETOPT_TX_POWER:
            if (len > sizeof(int16_t)) {
                return -EOVERFLOW;
            }
            ng_at86rf2xx_set_txpower(dev, *((int16_t *)val));
            return sizeof(uint16_t);

        case NG_NETOPT_STATE:
            if (len > sizeof(ng_netopt_state_t)) {
                return -EOVERFLOW;
            }
            return _set_state(dev, *((ng_netopt_state_t *)val));

        case NG_NETOPT_AUTOACK:
            ng_at86rf2xx_set_option(dev, NG_AT86RF2XX_OPT_AUTOACK,
                                    ((bool *)val)[0]);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_RETRANS:
            if (len > sizeof(uint8_t)) {
                return -EOVERFLOW;
            }
            ng_at86rf2xx_set_max_retries(dev, *((uint8_t *)val));
            return sizeof(uint8_t);

        case NG_NETOPT_PRELOADING:
            ng_at86rf2xx_set_option(dev, NG_AT86RF2XX_OPT_PRELOADING,
                                    ((bool *)val)[0]);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_PROMISCUOUSMODE:
            ng_at86rf2xx_set_option(dev, NG_AT86RF2XX_OPT_PROMISCUOUS,
                                    ((bool *)val)[0]);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_RAWMODE:
            ng_at86rf2xx_set_option(dev, NG_AT86RF2XX_OPT_RAWDUMP,
                                    ((bool *)val)[0]);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_RX_START_IRQ:
            ng_at86rf2xx_set_option(dev, NG_AT86RF2XX_OPT_TELL_RX_START,
                                    ((bool *)val)[0]);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_RX_END_IRQ:
            ng_at86rf2xx_set_option(dev, NG_AT86RF2XX_OPT_TELL_RX_END,
                                    ((bool *)val)[0]);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_TX_START_IRQ:
            ng_at86rf2xx_set_option(dev, NG_AT86RF2XX_OPT_TELL_TX_START,
                                    ((bool *)val)[0]);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_TX_END_IRQ:
            ng_at86rf2xx_set_option(dev, NG_AT86RF2XX_OPT_TELL_TX_END,
                                    ((bool *)val)[0]);
            return sizeof(ng_netopt_enable_t);

        default:
            return -ENOTSUP;
    }

    return 0;
}

static int _add_event_cb(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
{
    if (dev == NULL) {
        return -ENODEV;
    }
    if (dev->event_cb) {
        return -ENOBUFS;
    }

    dev->event_cb = cb;
    return 0;
}

static int _rem_event_cb(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
{
    if (dev == NULL) {
        return -ENODEV;
    }
    if (dev->event_cb != cb) {
        return -ENOENT;
    }

    dev->event_cb = NULL;
    return 0;
}

static void _isr_event(ng_netdev_t *device, uint32_t event_type)
{
    ng_at86rf2xx_t *dev = (ng_at86rf2xx_t *) device;
    uint8_t irq_mask;
    uint8_t state;

    /* read (consume) device status */
    irq_mask = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__IRQ_STATUS);

    state = ng_at86rf2xx_get_status(dev);

    if (irq_mask & NG_AT86RF2XX_IRQ_STATUS_MASK__RX_START) {
        dev->event_cb(NETDEV_EVENT_RX_STARTED, NULL);
        DEBUG("[ng_at86rf2xx] EVT - RX_START\n");
    }
    if (irq_mask & NG_AT86RF2XX_IRQ_STATUS_MASK__TRX_END) {
        if (state == NG_AT86RF2XX_STATE_RX_AACK_ON || state == NG_AT86RF2XX_STATE_BUSY_RX_AACK) {
            DEBUG("[ng_at86rf2xx] EVT - RX_END\n");
            if (!(dev->options & NG_AT86RF2XX_OPT_TELL_RX_END)) {
                return;
            }
            _receive_data(dev);
        }
        else if (state == NG_AT86RF2XX_STATE_TX_ARET_ON) {
            ng_at86rf2xx_set_state(dev, dev->idle_state);
            if (dev->event_cb && (dev->options & NG_AT86RF2XX_OPT_TELL_TX_END)) {
                dev->event_cb(NETDEV_EVENT_TX_COMPLETE, NULL);
            }
            DEBUG("[ng_at86rf2xx] EVT - TX_END\n");
        }
    }
}

const ng_netdev_driver_t ng_at86rf2xx_driver = {
    .send_data = _send,
    .add_event_callback = _add_event_cb,
    .rem_event_callback = _rem_event_cb,
    .get = _get,
    .set = _set,
    .isr_event = _isr_event,
};
