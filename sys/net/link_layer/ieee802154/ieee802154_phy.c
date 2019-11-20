#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

#include "net/netbuf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* The transceiver HAL will call this function when there's a packet
 * in the framebuffer of the transceiver. The role of the PHY layer
 * is to use the transceiver HAL and the netbuf API to:
 * - Allocate a buffer from the network stack
 * - Read L1 data (LQI, RSSI, etc).
 * - Generate a PHY indication with the packet + L1 data.
 *
 * Here we use some components of `netdev` as our transceiver
 * HAL.
 * Note this function is the same for all IEEE802154 devices,
 * as seen, we reuse the PHY layer for those :)
 */
void ieee802154_rf_rx_done(netdev_t *dev)
{
    netdev_ieee802154_rx_info_t rx_info;
    int bytes_expected = dev->driver->recv(dev, NULL, 0, NULL);
    ieee802154_phy_ind_t ind = {0};

    void *psdu = netbuf_get(bytes_expected, &ind.ctx);

    if (psdu == NULL) {
        DEBUG("_recv_ieee802154: cannot allocate netbuf.\n");

        /* Discard packet on netdev device */
        dev->driver->recv(dev, NULL, bytes_expected, NULL);
        return;
    }

    int nread = dev->driver->recv(dev, psdu, bytes_expected, &rx_info);

    if (nread <= 0) {
        netbuf_free(ind.ctx);
        return;
    }

    /* Here we populate the indication with the Physical Service
     * Data Unit (PSDU, full L2 frame), allocation context and
     * L1 info
     */
    ind.lqi = rx_info.lqi;
    ind.rssi = rx_info.rssi;
    ind.psdu = psdu;
    ind.psdu_len = nread;

    /* We have to decide if we need an interface here
     * E.g ieee802154_phy->ind(dev, &ind); */
    ieee802154_phy_ind(dev, &ind);
}

