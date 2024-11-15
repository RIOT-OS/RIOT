/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     test_utils_netdev_ieee802154_minimal
 * @{
 *
 * @file
 * @brief       Implementation of netdev IEEE 802.15.4 minimal test utility
 *              module
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */
#include <stdio.h>

#include "event.h"
#include "event/thread.h"
#include "od.h"
#include "net/ieee802154.h"
#include "net/l2util.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "test_utils/netdev_ieee802154_minimal.h"
#include "netdev_ieee802154_minimal_internal.h"

/* provided by the test application */
#include "init_dev.h"

device_reg_entry_t _devices[NETDEV_IEEE802154_MINIMAL_NUMOF];
static uint8_t _buffer[IEEE802154_FRAME_LEN_MAX];
static char _addr_str[IEEE802154_LONG_ADDRESS_LEN * 3];

struct event_pkt_desc {
    event_t event;
    struct netdev *dev;
    iolist_t *pkt;
    int res;
};

struct event_getset_desc {
    event_t event;
    struct netdev *dev;
    netopt_t opt;
    void *data;
    size_t len;
    int res;
};

static void _post_send_event(event_t *event)
{
    struct event_pkt_desc *desc = (struct event_pkt_desc*) event;
    struct netdev *dev = desc->dev;

    int res = dev->driver->send(dev, desc->pkt);
    desc->res = res;
}

static void _post_get_event(event_t *event)
{
    struct event_getset_desc *desc = (struct event_getset_desc*) event;
    struct netdev *dev = desc->dev;
    netopt_t opt = desc->opt;
    void *data = desc->data;
    size_t max_len = desc->len;

    int res = dev->driver->get(dev, opt, data, max_len);
    desc->res = res;
}

static void _post_set_event(event_t *event)
{
    struct event_getset_desc *desc = (struct event_getset_desc*) event;
    struct netdev *dev = desc->dev;
    netopt_t opt = desc->opt;
    void *data = desc->data;
    size_t len = desc->len;

    int res = dev->driver->set(dev, opt, data, len);
    desc->res = res;
}

void _recv(netdev_t *dev)
{
    uint8_t src[IEEE802154_LONG_ADDRESS_LEN], dst[IEEE802154_LONG_ADDRESS_LEN];
    int data_len, src_len, dst_len;
    size_t mhr_len;
    netdev_ieee802154_rx_info_t rx_info;
    le_uint16_t src_pan, dst_pan;

    putchar('\n');
    /* receive the frame */
    data_len = dev->driver->recv(dev, _buffer, sizeof(_buffer), &rx_info);
    mhr_len = ieee802154_get_frame_hdr_len(_buffer);
    if (mhr_len == 0) {
        puts("Unexpected MHR for incoming packet");
        return;
    }

    /* get and print addresses */
    dst_len = ieee802154_get_dst(_buffer, dst, &dst_pan);
    src_len = ieee802154_get_src(_buffer, src, &src_pan);

    l2util_addr_to_str(dst, dst_len, _addr_str);
    printf("Dest. PAN: 0x%04x\n", byteorder_ltohs(dst_pan));
    printf("Dest. addr.: %s\n", _addr_str);

    l2util_addr_to_str(src, src_len, _addr_str);
    printf("Src. PAN: 0x%04x\n", byteorder_ltohs(src_pan));
    printf("Src. addr.: %s\n", _addr_str);

    /* check frame type */
    switch (_buffer[0] & IEEE802154_FCF_TYPE_MASK) {
        case IEEE802154_FCF_TYPE_BEACON:
            puts("BEACON");
            break;
        case IEEE802154_FCF_TYPE_DATA:
            puts("DATA");
            break;
        case IEEE802154_FCF_TYPE_ACK:
            puts("ACK");
            break;
        case IEEE802154_FCF_TYPE_MACCMD:
            puts("MACCMD");
            break;
        default:
            puts("UNKNOWN");
            break;
    }

    /* print flag information */
    printf("\nSecurity: %s", _buffer[0] & IEEE802154_FCF_SECURITY_EN ? "1, " : "0, ");

    printf("Frame pend.: %s", _buffer[0] & IEEE802154_FCF_FRAME_PEND ? "1, " : "0, ");

    printf("ACK req.: %s", _buffer[0] & IEEE802154_FCF_ACK_REQ ? "1, " : "0, ");

    printf("PAN comp.:%s", _buffer[0] & IEEE802154_FCF_PAN_COMP ? "1, " : "0, ");

    printf("Version: ");
    printf("%u, ", (unsigned)((_buffer[1] & IEEE802154_FCF_VERS_MASK) >> 4));

    printf("Seq.: %u\n", (unsigned)ieee802154_get_seq(_buffer));

    printf("RSSI: %i, LQI: %u\n\n", rx_info.rssi, rx_info.lqi);

    /* dump the payload */
    od_hex_dump(_buffer + mhr_len, data_len - mhr_len, 0);
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

    case NETDEV_EVENT_TX_COMPLETE:
        puts("Tx complete");
        break;

#if IS_USED(MODULE_NETDEV_LEGACY_API)
    case NETDEV_EVENT_TX_COMPLETE_DATA_PENDING:
        puts("Tx complete (with pending data)");
        break;

    case NETDEV_EVENT_TX_MEDIUM_BUSY:
        puts("Medium busy");
        break;

    case NETDEV_EVENT_TX_NOACK:
        puts("No ACK");
        break;
#endif
    default:
        printf("Event: %d\n", event);
        break;
    }
}

/* Implement netdev_register_signal hook to associate registered devices to specific event
 * structures.
 */
void netdev_register_signal(struct netdev *dev, netdev_type_t type, uint8_t index)
{
    (void) type;

    if (index > NETDEV_IEEE802154_MINIMAL_NUMOF) {
        return;
    }
    printf("Device %d registered (type: %d)\n", index, type);
    dev->context = &_devices[index];
    _devices[index].dev = dev;
    _devices[index].event.handler = _isr_event_handler;
}

int netdev_ieee802154_minimal_init(void)
{
    return netdev_ieee802154_minimal_init_devs(_event_cb);
}

int netdev_ieee802154_minimal_send(struct netdev *dev, iolist_t *pkt)
{
    struct event_pkt_desc desc = {.event.handler=_post_send_event, .pkt = pkt,
                                  .dev = dev};
    event_post(EVENT_PRIO_HIGHEST, (event_t*) &desc);
    return desc.res;
}

int netdev_ieee802154_minimal_get(struct netdev *dev, netopt_t opt, void *data, size_t max_len)
{
    struct event_getset_desc desc = {.event.handler = _post_get_event, .opt = opt,
                                .data = data, .len = max_len, .dev = dev};
    event_post(EVENT_PRIO_HIGHEST, (event_t*) &desc);
    return desc.res;
}

int netdev_ieee802154_minimal_set(struct netdev *dev, netopt_t opt, void *data, size_t len)
{
    struct event_getset_desc desc = {.event.handler = _post_set_event, .opt = opt,
                                .data = data, .len = len, .dev = dev};
    event_post(EVENT_PRIO_HIGHEST, (event_t*) &desc);
    return desc.res;
}

/** @} */
