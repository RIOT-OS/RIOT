/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "assert.h"
#include "net/gnrc/netif/lora.h"
#include "net/lora.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifdef MODULE_NETDEV_LORA

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);
static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif);

static const gnrc_netif_ops_t lora_ops = {
    .send = _send,
    .recv = _recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
};

gnrc_netif_t *gnrc_netif_lora_create(char *stack, int stacksize, char priority,
                                     char *name, netdev_t *dev)
{
    return gnrc_netif_create(stack, stacksize, priority, name, dev, &lora_ops);
}

static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;

    int pktlen;
    gnrc_pktsnip_t *payload;

    assert(dev);

    /* see how much data there is to process */
    pktlen = dev->driver->recv(dev, NULL, 0, NULL);
    if (pktlen <= 0) {
        DEBUG("[lora-gnrc] recv: no data available to process\n");
        return NULL;
    }

    /* allocate space for the packet in the pktbuf */
    payload = gnrc_pktbuf_add(NULL, NULL, pktlen, GNRC_NETTYPE_UNDEF);
    if (payload == NULL) {
        DEBUG("[lora-gnrc] recv: unable to allocate space in the pktbuf\n");
        /* tell the driver to drop the packet */
        dev->driver->recv(dev, NULL, 1, NULL);
        return NULL;
    }

    /* copy the complete payload into the packet buffer */
    dev->driver->recv(dev, payload->data, pktlen, NULL);

    return payload;
}

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    netdev_t *dev = netif->dev;

    int res = 0;
    size_t size;
    size_t count;
    gnrc_pktsnip_t *vec;

    /* check device descriptor and packet */
    assert(dev && pkt);

    /* get the payload size */
    size = gnrc_pkt_len(pkt);
    DEBUG("[lora-gnrc] send: payload of packet is %i\n", (int)size);

    /* now let's extract the iovector and send out the stuff */
    vec = gnrc_pktbuf_get_iovec(pkt, &count);
    if (vec != NULL) {
        pkt = vec;
        struct iovec *vector = (struct iovec *)pkt->data;
        vector[0].iov_base = pkt->next->data;
        vector[0].iov_len = size;
        DEBUG("[lora-gnrc] send: triggering the drivers send function\n");
        res = dev->driver->send(dev, vector, count);
        if (res == -ENOTSUP) {
            DEBUG("[lora-gnrc] send: cannot send, radio is still transmitting");
        }
    }
    else {
        DEBUG("[lora-gnrc] send: unable to create iovec\n");
    }

    gnrc_pktbuf_release(pkt);

    return res;
}
#else   /* MODULE_NETDEV_LORA */
typedef int dont_be_pedantic;
#endif  /* MODULE_NETDEV_LORA */
