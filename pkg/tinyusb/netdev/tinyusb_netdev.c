/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup pkg_tinyusb
 * @{
 * @file    TinyUSB Netdev implementation for CDC Ethernet Control Model
 *
 * @author  Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include "kernel_defines.h"
#include "iolist.h"
#include "net/eui_provider.h"
#include "net/netdev/eth.h"
#include "od.h"

#include "device/usbd.h"
#include "class/net/net_device.h"

#include "tinyusb_netdev.h"

#define ENABLE_DEBUG_HEXDUMP    0
#define ENABLE_DEBUG            0
#include "debug.h"

/* The symbol is needed by the tinyUSB stack but not used and shall be removed
 * later on, see: https://github.com/hathach/tinyusb/issues/718 */
const uint8_t tud_network_mac_address[6] = { 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };

static const netdev_driver_t _tusb_driver;
tinyusb_netdev_t tinyusb_netdev;

void tinyusb_netdev_setup(tinyusb_netdev_t *dev)
{
    DEBUG("[tinyusb_netdev] %s: %p\n", __func__, dev);

    mutex_init(&dev->lock);
    dev->netdev.driver = &_tusb_driver;
    dev->rx_len = 0;
    dev->tx_len = 0;
    netdev_register(&dev->netdev, NETDEV_TINYUSB, 0);
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    DEBUG("[tinyusb_netdev] %s: %p %p\n", __func__, netdev, iolist);

    tinyusb_netdev_t *dev = (tinyusb_netdev_t *)netdev;

    assert(dev == &tinyusb_netdev);
    assert(iolist);

    if (!tud_ready()) {
        return -EBUSY;
    }

    mutex_lock(&dev->lock);

    dev->tx_len = 0;

    /* load packet data into TX buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (dev->tx_len + iol->iol_len > ETHERNET_MAX_LEN) {
            mutex_unlock(&dev->lock);
            return -EOVERFLOW;
        }
        if (iol->iol_len) {
            memcpy (dev->tx_buf + dev->tx_len, iol->iol_base, iol->iol_len);
            dev->tx_len += iol->iol_len;
        }
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        printf("[tinyusb_netdev] %s: send %d byte from %p\n",
               __func__, dev->tx_len, dev->tx_buf);
        if (IS_ACTIVE(ENABLE_DEBUG_HEXDUMP) && IS_USED(MODULE_OD)) {
            od_hex_dump(dev->tx_buf, dev->tx_len, OD_WIDTH_DEFAULT);
        }
    }

    mutex_unlock(&dev->lock);

    /* if the network driver can accept another packet, trigger the send */
    if (tud_network_can_xmit(dev->tx_len))
    {
      tud_network_xmit(dev->tx_buf, dev->tx_len);
      return 0;
    }

    return -EBUSY;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void)info;

    DEBUG("[tinyusb_netdev] %s: %p %p %u %p\n", __func__, netdev, buf, len, info);

    tinyusb_netdev_t *dev = (tinyusb_netdev_t *)netdev;
    assert(dev == &tinyusb_netdev);

    mutex_lock(&dev->lock);

    int size = dev->rx_len;

    if (!buf) {
        /* get the size of the frame; if len > 0 then also drop the frame */
        if (len > 0) {
            /* drop frame requested */
            dev->rx_len = 0;
        }
        mutex_unlock(&dev->lock);
        return size;
    }

    if (dev->rx_len > len) {
        /* buffer is smaller than the number of received bytes */
        DEBUG("[tinyusb_netdev] %s: Not enough space in receive buffer for %d bytes\n",
              __func__, dev->rx_len);
        mutex_unlock(&dev->lock);
        return -ENOBUFS;
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        printf ("[tinyusb_netdev] %s: received %d byte\n", __func__, dev->rx_len);
        if (IS_ACTIVE(ENABLE_DEBUG) && IS_USED(MODULE_OD)) {
            od_hex_dump(dev->rx_buf, dev->rx_len, OD_WIDTH_DEFAULT);
        }
    }

    /* copy received date and reset the receive length */
    memcpy(buf, dev->rx_buf, dev->rx_len);
    dev->rx_len = 0;
    mutex_unlock(&dev->lock);

    /* indicate that receiving the frame has been finished */
    tud_network_recv_renew();

    return size;
}

static int _init(netdev_t *netdev)
{
    DEBUG("[tinyusb_netdev] %s: %p\n", __func__, netdev);

    tinyusb_netdev_t *dev = (tinyusb_netdev_t *)netdev;
    assert(dev == &tinyusb_netdev);

    netdev_eui48_get(netdev, (eui48_t*)&dev->mac_addr);
    netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    DEBUG("[tinyusb_netdev] %s: %p %u %p %u\n",
          __func__, netdev, opt, value, max_len);

    tinyusb_netdev_t *dev = (tinyusb_netdev_t *)netdev;
    assert(dev == &tinyusb_netdev);

    (void)max_len;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
            memcpy(value, dev->mac_addr, ETHERNET_ADDR_LEN);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_get(netdev, opt, value, max_len);
    }
}

static int _set(netdev_t *netdev, netopt_t opt,
                const void *value, size_t len)
{
    DEBUG("[tinyusb_netdev] %s: %p %u %p %u\n",
          __func__, netdev, opt, value, len);

    tinyusb_netdev_t *dev = (tinyusb_netdev_t *)netdev;
    assert(dev == &tinyusb_netdev);

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(len == ETHERNET_ADDR_LEN);
            memcpy(dev->mac_addr, value, ETHERNET_ADDR_LEN);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_set(&dev->netdev, opt, value, len);
    }
}

static void _isr(netdev_t *netdev)
{
    tinyusb_netdev_t *dev = (tinyusb_netdev_t *)netdev;
    assert(dev == &tinyusb_netdev);

    if (dev->rx_len) {
        dev->netdev.event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
}

static const netdev_driver_t _tusb_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

void tud_network_init_cb(void)
{
    DEBUG("[tinyusb_netdev] %s\n", __func__);
    tinyusb_netdev.rx_len = 0;
    tinyusb_netdev.tx_len = 0;
}

bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
{
    DEBUG("[tinyusb_netdev] %s: %p %u\n", __func__, src, size);

    /* if the previous packet is not handled just return with false */
    if (tinyusb_netdev.rx_len) {
        return false;
    }

    assert(size <= ETHERNET_MAX_LEN);

    if (size)
    {
        tinyusb_netdev.rx_buf = src;
        tinyusb_netdev.rx_len = size;
    }

    netdev_trigger_event_isr(&tinyusb_netdev.netdev);

    return true;
}

uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg)
{
    DEBUG("[tinyusb_netdev] %s: %p %p %u\n", __func__, dst, ref, arg);

    memcpy(dst, ref, arg);
    return arg;
}
