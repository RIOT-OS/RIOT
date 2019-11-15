#include "net/netdev/ieee802154.h"
#include "net/netbuf.h"
#include "net/netif.h"
#include "net/gnrc/netif.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* Note all IEEE802.15.4 MAC layers will have this signature.
 * There's probably no need for an interface here, unless there are co-existent
 * IEEE802.15.4 MAC layers. */
void ieee802154_mac_recv(netdev_ieee802154_t *dev, ieee802154_phy_ind_t *ind)
{
    ieee802154_mac_ind_t mac_ind;
    if (ind->psdu_len < (int)IEEE802154_MIN_FRAME_LEN) {
        DEBUG("_recv_ieee802154: received frame is too short\n");
        netbuf_free(ind->ctx);
        return;
    }

    /* Normal mode, try to parse the frame according to IEEE 802.15.4 */
#if ENABLE_DEBUG
    char src_str[GNRC_NETIF_HDR_L2ADDR_PRINT_LEN];
#endif
    uint8_t *mhr = ind->psdu;
    size_t mhr_len = ieee802154_get_frame_hdr_len(mhr);

    /* nread was checked for <= 0 before so we can safely cast it to
     * unsigned */
    if ((mhr_len == 0) || ((size_t)ind->psdu_len < mhr_len)) {
        DEBUG("_recv_ieee802154: illegally formatted frame received\n");
        netbuf_free(ind->ctx);
        return;
    }
    /* mark IEEE 802.15.4 header */
    mac_ind.msdu.iol_base = mhr + mhr_len;
    mac_ind.msdu.iol_len = ind->psdu_len-mhr_len;

    uint8_t src[IEEE802154_LONG_ADDRESS_LEN], dst[IEEE802154_LONG_ADDRESS_LEN];
    int src_len, dst_len;
    le_uint16_t _pan_tmp;   /* TODO: hand-up PAN IDs to GNRC? */

    dst_len = ieee802154_get_dst(mhr, dst, &_pan_tmp);
    src_len = ieee802154_get_src(mhr, src, &_pan_tmp);

    if ((dst_len < 0) || (src_len < 0)) {
        DEBUG("_make_netif_hdr: unable to get addresses\n");
        netbuf_free(ind->ctx);
        return;
    }
    mac_ind.src = src;
    mac_ind.src_len = src_len;
    mac_ind.dst = dst;
    mac_ind.dst_len = dst_len;

#ifdef MODULE_L2FILTER
    if (!l2filter_pass(dev->filter, src,
                       src_len)) {
        netbuf_free(ind->ctx);
        DEBUG("_recv_ieee802154: packet dropped by l2filter\n");
        return NULL;
    }
#endif
#ifdef MODULE_GNRC_NETIF_DEDUP
    /* NOTE!!: This should be implemented by the IEEE802.15.4 layer! */
    const uint8_t seq = ieee802154_get_seq(mhr);

    int received = (netif->last_pkt.seq == seq) &&
            (netif->last_pkt.src_len == src_len) &&
            (memcmp(netif->last_pkt.src, src,
            src_len) == 0);

    if (received) {
        netbuf_free(ind->ctx);
        DEBUG("_recv_ieee802154: packet dropped by deduplication\n");
        return NULL;
    }

    memcpy(netif->last_pkt.src, src,
           src_len);
    netif->last_pkt.src_len = src_len;
    netif->last_pkt.seq = ieee802154_get_seq(mhr);
#endif /* MODULE_GNRC_NETIF_DEDUP */
    ieee802154_mac_ind(dev, &mac_ind, ind);
}

/* Interface here? */
void ieee802154_phy_ind(netdev_t *netdev, ieee802154_phy_ind_t *ind)
{
#ifdef MODULE_NETSTATS_L2
    netif->stats.rx_count++;
    netif->stats.rx_bytes += nread;
#endif

    if (((gnrc_netif_t*) netdev->context)->flags & GNRC_NETIF_FLAGS_RAWMODE) {
        /* Raw mode, skip packet processing, but provide rx_info via
         * GNRC_NETTYPE_NETIF */
        netif_pkt_t pkt = {0};
        pkt.lqi = ind->lqi;
        pkt.rssi = ind->rssi;
        pkt.ctx = ind->ctx;
        pkt.msdu.iol_base = ind->psdu;
        pkt.msdu.iol_len = ind->psdu_len;
        if(!netif_recv((netif_t*) netdev->context, &pkt)) {
            netbuf_free(ind->ctx);
        }
    }
    else {
        ieee802154_mac_recv((netdev_ieee802154_t*) netdev, ind);
    }
}

int ieee802154_mac_send(netdev_ieee802154_t *netdev, iolist_t *msdu, int frame_pending,
        uint8_t *src, size_t src_len, uint8_t *dst, size_t dst_len)
{
    netdev_ieee802154_t *state = netdev;
    netdev_t *dev = &netdev->netdev;

    int res = 0;
    uint8_t mhr[IEEE802154_MAX_HDR_LEN];
    le_uint16_t dev_pan = byteorder_btols(byteorder_htons(state->pan));
    uint8_t flags = (uint8_t)(state->flags & NETDEV_IEEE802154_SEND_MASK);

    flags |= IEEE802154_FCF_TYPE_DATA;

    if (frame_pending) {
        /* Set frame pending field */
        flags |= IEEE802154_FCF_FRAME_PEND;
    }
    /* fill MAC header, seq should be set by device */
    if ((res = ieee802154_set_frame_hdr(mhr, src, src_len,
                                        dst, dst_len, dev_pan,
                                        dev_pan, flags, state->seq++)) == 0) {
        DEBUG("_send_ieee802154: Error preperaring frame\n");
        return -EINVAL;
    }

    /* prepare iolist for netdev / mac layer */
    iolist_t iolist = {
        .iol_next = msdu,
        .iol_base = mhr,
        .iol_len = (size_t)res
    };

#ifdef MODULE_GNRC_MAC
    if (netif->mac.mac_info & GNRC_NETIF_MAC_INFO_CSMA_ENABLED) {
        res = csma_sender_csma_ca_send(dev, &iolist, &netif->mac.csma_conf);
    }
    else {
        res = dev->driver->send(dev, &iolist);
    }
#else
    res = dev->driver->send(dev, &iolist);
#endif

    return res;
}

