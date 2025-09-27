/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_nrf5x_nrfmin_gnrc
 * @{
 *
 * @file
 * @brief       GNRC adapter for the nrfmin radio driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>

#include "net/gnrc.h"
#include "thread.h"
#include "net/gnrc/netif.h"

#include "nrfmin_gnrc.h"

#define ENABLE_DEBUG                0
#include "debug.h"

/**
 * @brief   Definition of default thread priority and stacksize
 * @{
 */
#ifndef NRFMIN_GNRC_THREAD_PRIO
#define NRFMIN_GNRC_THREAD_PRIO     GNRC_NETIF_PRIO
#endif

#ifndef NRFMIN_GNRC_STACKSIZE
#define NRFMIN_GNRC_STACKSIZE       THREAD_STACKSIZE_DEFAULT
#endif
/** @} */

/**
 * @brief
 */
#define BCAST   (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)

/**
 * @brief   Allocate the stack for the GNRC netdev thread to run in
 */
static char stack[NRFMIN_GNRC_STACKSIZE];
static gnrc_netif_t _netif;

static int hdr_netif_to_nrfmin(nrfmin_hdr_t *nrfmin, gnrc_pktsnip_t *pkt)
{
    gnrc_netif_hdr_t *netif = (gnrc_netif_hdr_t *)pkt->data;

    if (!(netif->flags & BCAST) && (netif->dst_l2addr_len != 2)) {
        return -EINVAL;
    }

    nrfmin->len = gnrc_pkt_len(pkt->next) + NRFMIN_HDR_LEN;
    if (netif->flags & BCAST) {
        nrfmin->dst_addr = NRFMIN_ADDR_BCAST;
    }
    else {
        memcpy(&nrfmin->dst_addr, gnrc_netif_hdr_get_dst_addr(netif), 2);
    }
    nrfmin->src_addr = nrfmin_get_addr();
    if (pkt->next) {
        nrfmin->proto = (uint8_t)pkt->next->type;
    }
    else {
        nrfmin->proto = 0;
    }

    return 0;
}

static int gnrc_nrfmin_send(gnrc_netif_t *dev, gnrc_pktsnip_t *pkt)
{
    int res;
    nrfmin_hdr_t nrfmin_hdr;

    assert(pkt);

    if (pkt->type != GNRC_NETTYPE_NETIF) {
        DEBUG("[nrfmin_gnrc] send: first header is not generic netif header\n");
        return -EBADMSG;
    }

    /* build the nrfmin header from the generic netif header */
    res = hdr_netif_to_nrfmin(&nrfmin_hdr, pkt);
    if (res < 0) {
        DEBUG("[nrfmin_gnrc] send: failed to build nrfmin header\n");
        goto out;
    }

    /* link first entry after netif hdr of the pkt to the nrfmin header */
    iolist_t iolist = {
        .iol_next = (iolist_t *)pkt->next,
        .iol_base = &nrfmin_hdr,
        .iol_len = NRFMIN_HDR_LEN
    };

    /* and finally send out the data and release the packet */
    res = dev->dev->driver->send(dev->dev, &iolist);

out:
    gnrc_pktbuf_release(pkt);

    return res;
}

static gnrc_pktsnip_t *gnrc_nrfmin_recv(gnrc_netif_t *dev)
{
    int pktsize;
    nrfmin_hdr_t *nrfmin;
    gnrc_netif_hdr_t *netif;
    gnrc_pktsnip_t *pkt_snip;
    gnrc_pktsnip_t *hdr_snip;
    gnrc_pktsnip_t *netif_snip;

    /* get the size of the new packet */
    pktsize = nrfmin_dev.driver->recv(NULL, NULL, 0, NULL);
    if (pktsize <= 0) {
        DEBUG("[nrfmin_gnrc] recv: error: tried to read empty packet\n");
        return NULL;
    }

    /* allocate space in the packet buffer */
    pkt_snip = gnrc_pktbuf_add(NULL, NULL, pktsize, GNRC_NETTYPE_UNDEF);
    if (pkt_snip == NULL) {
        DEBUG("[nrfmin_gnrc] recv: unable to allocate pktsnip\n");
        return NULL;
    }

    /* read the incoming data into the packet buffer */
    nrfmin_dev.driver->recv(NULL, pkt_snip->data, pktsize, NULL);

    /* now we mark the nrfmin header */
    hdr_snip = gnrc_pktbuf_mark(pkt_snip, NRFMIN_HDR_LEN, GNRC_NETTYPE_UNDEF);
    if (hdr_snip == NULL) {
        DEBUG("[nrfmin_gnrc] recv: unable to mark the nrfmin header\n");
        gnrc_pktbuf_release(pkt_snip);
        return NULL;
    }

    /* allocate the generic netif header and populate it with data from the
       nrfmin header */
    nrfmin = (nrfmin_hdr_t *)hdr_snip->data;
    netif_snip = gnrc_netif_hdr_build((uint8_t *)&nrfmin->src_addr, 2,
                                      (uint8_t *)&nrfmin->dst_addr, 2);
    if (netif_snip == NULL) {
        DEBUG("[nrfmin_gnrc] recv: unable to allocate netif header\n");
        gnrc_pktbuf_release(pkt_snip);
        return NULL;
    }

    netif = (gnrc_netif_hdr_t *)netif_snip->data;
    if (nrfmin->dst_addr == NRFMIN_ADDR_BCAST) {
        netif->flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
    }
    gnrc_netif_hdr_set_netif(netif, dev);
    pkt_snip->type = nrfmin->proto;

    /* finally: remove the nrfmin header and append the netif header */
    gnrc_pktbuf_remove_snip(pkt_snip, hdr_snip);
    return gnrc_pkt_append(pkt_snip, netif_snip);
}

static const gnrc_netif_ops_t gnrc_nrfmin_ops = {
    .init = gnrc_netif_default_init,
    .send = gnrc_nrfmin_send,
    .recv = gnrc_nrfmin_recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
};

void gnrc_nrfmin_init(void)
{
    /* setup the NRFMIN driver */
    nrfmin_setup();
    gnrc_netif_create(&_netif, stack, sizeof(stack), NRFMIN_GNRC_THREAD_PRIO, "nrfmin",
                      &nrfmin_dev, &gnrc_nrfmin_ops);
}
