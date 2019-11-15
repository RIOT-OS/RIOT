#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "net/netif.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

void ieee802154_mac_ind(netdev_ieee802154_t *netdev, ieee802154_mac_ind_t *ind, ieee802154_phy_ind_t *phy_ind)
{
    netif_pkt_t pkt = {0};

    /* set broadcast flag for broadcast destination */
    if ((ind->dst_len == 2) && (ind->dst[0] == 0xff) && (ind->dst[1] == 0xff)) {
        pkt.flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
    }

    /* set flags for pending frames */
    if (ind->frame_pending & IEEE802154_FCF_FRAME_PEND) {
        pkt.flags |= GNRC_NETIF_HDR_FLAGS_MORE_DATA;
    }

    pkt.lqi = phy_ind->lqi;
    pkt.rssi = phy_ind->rssi;
    pkt.ctx = phy_ind->ctx;
    pkt.msdu = ind->msdu;
    pkt.src_l2addr = ind->src;
    pkt.src_l2addr_len = ind->src_len;
    pkt.dst_l2addr = ind->dst;
    pkt.dst_l2addr_len = ind->dst_len;

#if ENABLE_DEBUG
    DEBUG("_recv_ieee802154: received packet from %s of length %u\n",
          gnrc_netif_addr_to_str(src,
                                 src_len,
                                 src_str),
          pkt.msdu.iol_len);
#if defined(MODULE_OD)
    od_hex_dump(mhr, pkt.msdu.iol_len, OD_WIDTH_DEFAULT);
#endif
#endif

    netif_t *netif = ((netdev_t*) netdev)->context;
    if(!netif_recv(netif, &pkt)) {
        netbuf_free(phy_ind->ctx);
    }
}
