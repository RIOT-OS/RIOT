/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     test_utils_netdev_eth_minimal
 * @{
 *
 * @file
 * @brief       Implementation of netdev Eth minimal test utility module
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include <stdio.h>

#include "event.h"
#include "event/thread.h"
#include "od.h"
#include "net/ethernet.h"
#include "net/ethernet/hdr.h"
#include "net/l2util.h"
#include "net/netdev.h"
#include "net/netdev/eth.h"
#include "test_utils/netdev_eth_minimal.h"
#include "netdev_eth_minimal_internal.h"

/* provided by the test application */
#include "init_dev.h"

device_reg_entry_t _devices[NETDEV_ETH_MINIMAL_NUMOF];
static uint8_t _buffer[ETHERNET_MAX_LEN];
static char _addr_str[ETHERNET_ADDR_LEN * 3];

void _recv(netdev_t *dev)
{
    ssize_t data_len;
    netdev_eth_rx_info_t rx_info = { 0 };
    ethernet_hdr_t *header = (ethernet_hdr_t *)_buffer;
    uint8_t *payload = _buffer + sizeof(ethernet_hdr_t);

    putchar('\n');
    data_len = dev->driver->recv(dev, _buffer, sizeof(_buffer), &rx_info);
    if (data_len < 0) {
        return;
    }

    l2util_addr_to_str(header->dst, ETHERNET_ADDR_LEN, _addr_str);
    printf("Dest. addr.: %s\n", _addr_str);

    l2util_addr_to_str(header->src, ETHERNET_ADDR_LEN, _addr_str);
    printf("Src. addr.: %s\n", _addr_str);

    data_len -= sizeof(ethernet_hdr_t);
    printf("Payload (%u bytes): \n", (unsigned)data_len);
    od_hex_dump(payload, data_len, 0);
}

static void _isr_event_handler(event_t *event)
{
    /* recover the netdev from the event */
    device_reg_entry_t *netdev_event = container_of(event, device_reg_entry_t, event);
    netdev_t *netdev = netdev_event->dev;
    netdev->driver->isr(netdev);
}

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    device_reg_entry_t *device = dev->context;

    switch (event) {
    case NETDEV_EVENT_ISR:
        event_post(EVENT_PRIO_HIGHEST, &device->event);
        break;

    case NETDEV_EVENT_RX_COMPLETE:
        _recv(dev);
        break;

    default:
        break;
    }
}

/* Implement netdev_register_signal hook to associate registered devices to specific event
 * structures.
 */
void netdev_register_signal(struct netdev *dev, netdev_type_t type, uint8_t index)
{
    (void) type;

    if (index > NETDEV_ETH_MINIMAL_NUMOF) {
        return;
    }
    printf("Device %d registered (type: %d)\n", index, type);
    dev->context = &_devices[index];
    _devices[index].dev = dev;
    _devices[index].event.handler = _isr_event_handler;
}

int netdev_eth_minimal_init(void)
{
    return netdev_eth_minimal_init_devs(_event_cb);
}

/** @} */
