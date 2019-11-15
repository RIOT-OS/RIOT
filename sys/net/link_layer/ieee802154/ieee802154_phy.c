#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

#include "net/netbuf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* This function is the same for all IEEE802154 devices! */
void ieee802154_rf_rx_done(netdev_t *dev)
{
    netdev_ieee802154_rx_info_t rx_info;
    int bytes_expected = dev->driver->recv(dev, NULL, 0, NULL);
    void *psdu;
    netbuf_ctx_t *ctx = netbuf_alloc(bytes_expected, &psdu);

    if (ctx == NULL) {
        DEBUG("_recv_ieee802154: cannot allocate netbuf.\n");

        /* Discard packet on netdev device */
        dev->driver->recv(dev, NULL, bytes_expected, NULL);
        return;
    }

    int nread = dev->driver->recv(dev, psdu, bytes_expected, &rx_info);

    if (nread <= 0) {
        netbuf_free(ctx);
        return;
    }

    ieee802154_phy_ind_t ind;
    ind.lqi = rx_info.lqi;
    ind.rssi = rx_info.rssi;
    ind.psdu = psdu;
    ind.psdu_len = nread;
    ind.ctx = ctx;

    /* Interface here? */
    ieee802154_phy_ind(dev, &ind);
}

