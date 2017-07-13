/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc
 * @{
 *
 * @file
 * @brief       GNRC to netdev adapter for LoRa devices
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "assert.h"
#include "net/gnrc.h"
#include "net/gnrc/netdev/lora.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static gnrc_pktsnip_t *lora_adpt_recv(gnrc_netdev_t *gnrc_netdev)
{
    netdev_t *dev = gnrc_netdev->dev;
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

static int lora_adpt_send(gnrc_netdev_t *dev, gnrc_pktsnip_t *pkt)
{
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
        res = dev->dev->driver->send(dev->dev, vector, count);
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

void gnrc_netdev_lora_init(gnrc_netdev_t *gnrc_netdev, netdev_t *dev)
{
    assert(gnrc_netdev && dev);

    gnrc_netdev->send = lora_adpt_send;
    gnrc_netdev->recv = lora_adpt_recv;
    gnrc_netdev->dev = dev;
}
