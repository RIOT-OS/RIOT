/*
 * Copyright (C) 2018 Timo Rothenpieler
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common_esp_now
 * @{
 *
 * @file
 * @brief       Netif interface for the ESP-NOW WiFi P2P protocol
 *
 * @author Timo Rothenpieler <timo.rothenpieler@uni-bremen.de>
 */

#include <assert.h>
#include <stdlib.h>

#include <sys/uio.h>

#include "cpu_conf.h"
#include "net/netdev.h"
#include "net/gnrc.h"
#include "esp_common_log.h"
#include "esp_now_params.h"
#include "esp_now_netdev.h"
#include "esp_now_gnrc.h"
#include "net/gnrc/netif.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    uint8_t mac[ESP_NOW_ADDR_LEN];
    esp_now_pkt_hdr_t esp_hdr;
    netdev_t *dev = netif->dev;

    assert(pkt != NULL);

    if (pkt->type != GNRC_NETTYPE_NETIF) {
        DEBUG("gnrc_esp_now: First header was not generic netif header\n");
        gnrc_pktbuf_release(pkt);
        return -EBADMSG;
    }

    gnrc_netif_hdr_t *netif_hdr = (gnrc_netif_hdr_t*)pkt->data;
    gnrc_pktsnip_t *payload = pkt->next;

    if (netif_hdr->flags & (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        /* ESP-NOW does not support multicast, always broadcast */
        memset(mac, 0xff, ESP_NOW_ADDR_LEN);
    } else if (netif_hdr->dst_l2addr_len == ESP_NOW_ADDR_LEN) {
        memcpy(mac, gnrc_netif_hdr_get_dst_addr(netif_hdr), ESP_NOW_ADDR_LEN);
    } else {
        DEBUG("gnrc_esp_now: destination address had unexpected format"
              "(flags=%d, dst_l2addr_len=%d)\n", netif_hdr->flags, netif_hdr->dst_l2addr_len);
        gnrc_pktbuf_release(pkt);
        return -EBADMSG;
    }

    iolist_t esp_hdr_iolist = {
        .iol_base = &esp_hdr,
        .iol_len = sizeof(esp_hdr),
        .iol_next = (iolist_t*)payload
    };

    iolist_t iolist = {
        .iol_base = mac,
        .iol_len = sizeof(mac),
        .iol_next = &esp_hdr_iolist
    };

    switch (payload->type) {
#ifdef MODULE_GNRC_SIXLOWPAN
        case GNRC_NETTYPE_SIXLOWPAN:
            esp_hdr.flags = ESP_NOW_PKT_HDR_FLAG_SIXLO;
            break;
#endif
        default:
            esp_hdr.flags = 0;
    }

    DEBUG("gnrc_esp_now: sending packet to %02x:%02x:%02x:%02x:%02x:%02x with size %u\n",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], (unsigned)payload->size);

    int res = dev->driver->send(dev, &iolist);

    gnrc_pktbuf_release(pkt);

    return res;
}

static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;
    esp_now_netdev_t *esp_now = (esp_now_netdev_t*)dev;

    int bytes_expected = dev->driver->recv(dev, NULL, 0, NULL);
    if (bytes_expected <= 0) {
        DEBUG("gnrc_esp_now: failed receiving packet: %d\n", bytes_expected);
        return NULL;
    }

    gnrc_pktsnip_t *pkt;
    pkt = gnrc_pktbuf_add(NULL, NULL,
                          bytes_expected,
                          GNRC_NETTYPE_UNDEF);
    if (!pkt) {
        DEBUG("gnrc_esp_now: cannot allocate pktsnip.\n");

        /* drop the packet */
        dev->driver->recv(dev, NULL, bytes_expected, NULL);

        return NULL;
    }

    int nread = dev->driver->recv(dev, pkt->data, bytes_expected, NULL);
    if (nread <= 0) {
        DEBUG("gnrc_esp_now: read error %d\n", nread);
        goto err;
    }

    if (nread < bytes_expected) {
        DEBUG("gnrc_esp_now: reallocating.\n");
        gnrc_pktbuf_realloc_data(pkt, nread);
    }

    gnrc_pktsnip_t *mac_hdr;
    mac_hdr = gnrc_pktbuf_mark(pkt, ESP_NOW_ADDR_LEN, GNRC_NETTYPE_UNDEF);
    if (!mac_hdr) {
        DEBUG("gnrc_esp_now: no space left in packet buffer\n");
        goto err;
    }

    gnrc_pktsnip_t *esp_hdr;
    esp_hdr = gnrc_pktbuf_mark(pkt, sizeof(esp_now_pkt_hdr_t), GNRC_NETTYPE_UNDEF);
    if (!esp_hdr) {
        DEBUG("gnrc_esp_now: no space left in packet buffer\n");
        pkt = mac_hdr;
        goto err;
    }

#ifdef MODULE_L2FILTER
    if (!l2filter_pass(dev->filter, mac_hdr->data, ESP_NOW_ADDR_LEN)) {
        DEBUG("gnrc_esp_now: incoming packet filtered by l2filter\n");
        pkt = mac_hdr;
        goto err;
    }
#endif

    pkt->type = GNRC_NETTYPE_UNDEF;

#ifdef MODULE_GNRC_SIXLOWPAN
    esp_now_pkt_hdr_t *hdr = (esp_now_pkt_hdr_t*)esp_hdr->data;

    if (hdr->flags & ESP_NOW_PKT_HDR_FLAG_SIXLO) {
        pkt->type = GNRC_NETTYPE_SIXLOWPAN;
    }
#endif

    gnrc_pktsnip_t *netif_hdr = gnrc_netif_hdr_build(mac_hdr->data, ESP_NOW_ADDR_LEN,
                                                     esp_now->addr, ESP_NOW_ADDR_LEN);
    if (!netif_hdr) {
        DEBUG("gnrc_esp_now: no space left in packet buffer\n");
        pkt = mac_hdr;
        goto err;
    }

    gnrc_netif_hdr_set_netif((gnrc_netif_hdr_t *)netif_hdr->data, netif);

    uint8_t *mac = mac_hdr->data;
    DEBUG("gnrc_esp_now: received packet from %02x:%02x:%02x:%02x:%02x:%02x of length %u\n",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], pkt->size);

    gnrc_pktbuf_remove_snip(pkt, mac_hdr);
    gnrc_pktbuf_remove_snip(pkt, esp_hdr);
    LL_APPEND(pkt, netif_hdr);

    return pkt;

err:
    gnrc_pktbuf_release(pkt);
    return NULL;
}

static const gnrc_netif_ops_t _esp_now_ops = {
    .init = gnrc_netif_default_init,
    .send = _send,
    .recv = _recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
};

gnrc_netif_t *gnrc_netif_esp_now_create(char *stack, int stacksize, char priority,
                                        char *name, netdev_t *dev)
{
    return gnrc_netif_create(stack, stacksize, priority, name, dev, &_esp_now_ops);
}

/* device thread stack */
static char _esp_now_stack[ESP_NOW_STACKSIZE];

void auto_init_esp_now(void)
{
    LOG_TAG_INFO("esp_now", "initializing ESP-NOW device\n");

    esp_now_netdev_t *esp_now_dev = netdev_esp_now_setup();
    if (!esp_now_dev) {
        LOG_ERROR("[auto_init_netif] error initializing esp_now\n");
    } else {
        gnrc_netif_esp_now_create(_esp_now_stack, sizeof(_esp_now_stack),
                                  ESP_NOW_PRIO,
                                  "esp-now",
                                  &esp_now_dev->netdev);
    }
}

/** @} */
