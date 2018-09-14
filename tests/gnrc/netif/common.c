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

#include <errno.h>
#include <stdio.h>

#include "common.h"
#include "net/ethernet.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pktdump.h"
#include "net/ipv6.h"
#include "net/netdev_test.h"
#include "od.h"

static netdev_test_t _devs[GNRC_NETIF_NUMOF];

netdev_t *ethernet_dev = (netdev_t *)&_devs[0];
netdev_t *ieee802154_dev = (netdev_t *)&_devs[1];
netdev_t *devs[DEFAULT_DEVS_NUMOF];

#define MSG_QUEUE_SIZE  (8)

static gnrc_netreg_entry_t dumper_undef, dumper_ipv6;
static msg_t _main_msg_queue[MSG_QUEUE_SIZE];
static uint8_t tmp_buffer[ETHERNET_DATA_LEN];
static size_t tmp_buffer_bytes = 0;

static int _dump_send_packet(netdev_t *netdev, const iolist_t *iolist)
{
    int res;

    tmp_buffer_bytes = 0;

    printf("Sending data from ");
    if (netdev == ethernet_dev) {
        printf("Ethernet ");
    }
    else if (netdev == ieee802154_dev) {
        printf("IEEE 802.15.4 ");
    }
    else {
        printf("unknown ");
    }
    puts("device:");
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        size_t len = iol->iol_len;
        if ((tmp_buffer_bytes + len) > ETHERNET_DATA_LEN) {
            return -ENOBUFS;
        }
        memcpy(&tmp_buffer[tmp_buffer_bytes], iol->iol_base, len);
        tmp_buffer_bytes += len;
    }
    od_hex_dump(tmp_buffer, tmp_buffer_bytes, OD_WIDTH_DEFAULT);
    res = (int)tmp_buffer_bytes;
    return res;
}

void _test_trigger_recv(gnrc_netif_t *netif, const uint8_t *data,
                        size_t data_len)
{
    netdev_t *dev = netif->dev;

    assert(data_len <= ETHERNET_DATA_LEN);
    if ((data != NULL) || (data_len > 0)) {
        tmp_buffer_bytes = data_len;
        memcpy(tmp_buffer, data, data_len);
    }
    else {
        tmp_buffer_bytes = 0;
    }
    assert(dev->event_callback);
    dev->event_callback(dev, NETDEV_EVENT_ISR);
}

static int _netdev_recv(netdev_t *dev, char *buf, int len, void *info)
{
    int res;

    (void)dev;
    (void)info;
    res = (int)tmp_buffer_bytes;
    if (buf == NULL) {
        if (len > 0) {
            tmp_buffer_bytes = 0;
        }
        return res;
    }
    if (((unsigned)len) < tmp_buffer_bytes) {
        return -ENOBUFS;
    }
    memcpy(buf, tmp_buffer, tmp_buffer_bytes);
    return res;
}

static void _netdev_isr(netdev_t *dev)
{
    assert(dev->event_callback);
    dev->event_callback(dev, NETDEV_EVENT_RX_COMPLETE);
}

static int _get_netdev_device_type(netdev_t *netdev, void *value, size_t max_len)
{
    assert(max_len == sizeof(uint16_t));
    (void)max_len;

    netdev_test_t *dev = (netdev_test_t *)netdev;

    if (dev->state == 0x0) {
        *((uint16_t *)value) = NETDEV_TYPE_ETHERNET;
    }
    else if (dev->state == (void *)0x1) {
        *((uint16_t *)value) = NETDEV_TYPE_IEEE802154;
    }
    else {
        *((uint16_t *)value) = NETDEV_TYPE_UNKNOWN;
    }
    return sizeof(uint16_t);
}

static int _get_netdev_max_packet_size(netdev_t *netdev, void *value, size_t max_len)
{
    assert(max_len == sizeof(uint16_t));
    (void)max_len;

    netdev_test_t *dev = (netdev_test_t *)netdev;

    if (dev->state == 0x0) {
        *((uint16_t *)value) = ETHERNET_DATA_LEN;
    }
    else if (dev->state == ((void *)0x1)) {
        *((uint16_t *)value) = TEST_IEEE802154_MAX_FRAG_SIZE;
    }
    else {
        *((uint16_t *)value) = IPV6_MIN_MTU;
    }
    return sizeof(uint16_t);
}


void _tests_init(void)
{
    msg_init_queue(_main_msg_queue, MSG_QUEUE_SIZE);
    netdev_test_setup((netdev_test_t *)ethernet_dev, 0);
    netdev_test_set_send_cb((netdev_test_t *)ethernet_dev, _dump_send_packet);
    netdev_test_set_recv_cb((netdev_test_t *)ethernet_dev, _netdev_recv);
    netdev_test_set_isr_cb((netdev_test_t *)ethernet_dev, _netdev_isr);
    netdev_test_set_get_cb((netdev_test_t *)ethernet_dev, NETOPT_DEVICE_TYPE,
                           _get_netdev_device_type);
    netdev_test_set_get_cb((netdev_test_t *)ethernet_dev, NETOPT_MAX_PACKET_SIZE,
                           _get_netdev_max_packet_size);
    netdev_test_setup((netdev_test_t *)ieee802154_dev, (void *)1);
    netdev_test_set_send_cb((netdev_test_t *)ieee802154_dev, _dump_send_packet);
    netdev_test_set_recv_cb((netdev_test_t *)ieee802154_dev, _netdev_recv);
    netdev_test_set_isr_cb((netdev_test_t *)ieee802154_dev, _netdev_isr);
    netdev_test_set_get_cb((netdev_test_t *)ieee802154_dev, NETOPT_DEVICE_TYPE,
                           _get_netdev_device_type);
    netdev_test_set_get_cb((netdev_test_t *)ieee802154_dev,
                           NETOPT_MAX_PACKET_SIZE, _get_netdev_max_packet_size);
    for (intptr_t i = SPECIAL_DEVS; i < GNRC_NETIF_NUMOF; i++) {
        devs[i - SPECIAL_DEVS] = (netdev_t *)&_devs[i];
        netdev_test_setup(&_devs[i], (void *)i);
        netdev_test_set_get_cb(&_devs[i], NETOPT_DEVICE_TYPE,
                               _get_netdev_device_type);
        netdev_test_set_get_cb(&_devs[i], NETOPT_MAX_PACKET_SIZE,
                               _get_netdev_max_packet_size);
    }
    gnrc_netreg_entry_init_pid(&dumper_undef, GNRC_NETREG_DEMUX_CTX_ALL,
                               gnrc_pktdump_pid);
    gnrc_netreg_entry_init_pid(&dumper_ipv6, GNRC_NETREG_DEMUX_CTX_ALL,
                               gnrc_pktdump_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dumper_undef);
    gnrc_netreg_register(GNRC_NETTYPE_IPV6, &dumper_ipv6);
}

/** @} */
