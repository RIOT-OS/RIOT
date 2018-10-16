/*
 * Copyright (C) 2017 Bumsik Kim <k.bumsik@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_winc1500
 * @{
 *
 * @file
 * @brief       Netdev port for the WINC1500 driver
 *
 * @author      Bumsik Kim <k.bumsik@gmail.com>
 *
 * @}
 */

#ifdef MODULE_NETDEV_ETH

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "mutex.h"
#include "winc1500.h"
#include "winc1500_internal.h"
#include "xtimer.h"

#include "net/ethernet.h"
#include "net/netdev/eth.h"
#include "net/netstats.h"

/* From pkg/winc1500 */
#include "driver/source/m2m_hif.h"
#include "bsp/include/nm_bsp.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/** ISR for netdev */
static void winc1500_isr(void *arg);

/** netdev interface */
static void _isr(netdev_t *dev);
static int _init(netdev_t *dev);
static int _send(netdev_t *netdev, const iolist_t *iolist);
static int _recv(netdev_t *netdev, void *buf, size_t len, void *info);
static int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len);

const netdev_driver_t netdev_driver_winc1500 = {
    .init = _init,
    .isr = _isr,
    .send = _send,
    .recv = _recv,
    .get = _get,
    .set = netdev_eth_set,
};

static void winc1500_isr(void *arg)
{
    winc1500_t *dev = (winc1500_t *) arg;

    m2m_hif_isr(dev);

    /* call netdev hook */
    dev->netdev.event_callback((netdev_t*) dev, NETDEV_EVENT_ISR);
}

static void _isr(netdev_t *netdev)
{
    winc1500_t *dev = (winc1500_t *) netdev;

    _lock_bus(dev);
    /* Handling events will be done in the callback */
    m2m_wifi_handle_events(dev, NULL);
    _unlock_bus(dev);
}

static int _init(netdev_t *netdev)
{
    winc1500_t *dev = (winc1500_t *) netdev;

    DEBUG("winc1500_netdev: starting initialization...\n");
    /* Set SPI first */
    dev->rx_size = -1;
    spi_init(dev->params.spi);
    _lock_bus(dev);

    /* Initialize the WINC1500 BSP. */
    dev->pid = thread_getpid();
    _init_internal(&dev->internal);
    nm_bsp_init(dev);
    nm_bsp_register_isr(dev, winc1500_isr);

    tstrWifiInitParam wifi_param;
    memset((uint8_t *)&wifi_param, 0, sizeof(tstrWifiInitParam));

    /* Register a callback. But replaced by the custom callback.
     *  See process_event() in winc1500_callback.c */
    wifi_param.pfAppWifiCb = NULL;
    /* Setting Ethernet bypass mode. The ethernet buffer and callback will
     *  be managed by netdev driver */
    wifi_param.strEthInitParam.u8EthernetEnable = M2M_WIFI_MODE_ETHERNET;
    wifi_param.strEthInitParam.au8ethRcvBuf = NULL;
    wifi_param.strEthInitParam.u16ethRcvBufSize = 0;
    wifi_param.strEthInitParam.pfAppEthCb = NULL;

    if (M2M_SUCCESS != m2m_wifi_init(dev, &wifi_param)) {
        return WINC1500_ERR;
    }

    /* Register WINC1500's internal wifi callback instead of the driver's callback
     *  for RIOT GNRC
     */
    hif_register_cb(dev, M2M_REQ_GROUP_WIFI, _wifi_cb);

    /* Initialize winc1500 struct */
    mutex_init(&dev->mutex);
    mbox_init(&dev->event_mbox, dev->event_mbox_queue, WINC1500_EVENT_MBOX_LEN);

    m2m_wifi_get_mac_address(dev, (uint8_t *)&dev->mac_addr);
    dev->state = 0 | WINC1500_STATE_INIT;
    dev->state |= WINC1500_STATE_IDLE;

    _unlock_bus(dev);

#ifdef MODULE_NETSTATS_L2
    memset(&netdev->stats, 0, sizeof(netstats_t));
#endif
    return 0;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    winc1500_t * dev = (winc1500_t *) netdev;
    _lock_bus(dev);

    /* Copy packet */
    size_t len = 0;
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        memcpy(dev->frame_buf + len, iol->iol_base, iol->iol_len);
        len += iol->iol_len;
        /* TODO: what if the module's buffer full? */
    }
    /* Send Packet */
    m2m_wifi_send_ethernet_pkt(dev, dev->frame_buf, len);

#ifdef MODULE_NETSTATS_L2
    netdev->stats.tx_bytes += len;
#endif

    _unlock_bus(dev);
    return len;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    winc1500_t * dev = (winc1500_t *) netdev;

    (void)info;
    _lock_bus(dev);

    int size;
    uint16_t data_size;
    uint8_t rx_done;
    winc1500_event_info_t event_info;

    if (dev->rx_size == -1) {
        if (hif_receive(dev, dev->rx_addr, (uint8_t *) &event_info,
                    sizeof(event_info.recv_pkt), 0) == M2M_SUCCESS) {
            dev->rx_offset = event_info.recv_pkt.u16PktOffset;
            dev->rx_rem_size = event_info.recv_pkt.u16PktSz;
            dev->rx_size = event_info.recv_pkt.u16PktSz;
        }
    }
    size = dev->rx_size;
    if (buf != NULL) {
#ifdef MODULE_NETSTATS_L2
        netdev->stats.rx_count++;
        netdev->stats.rx_bytes += len;
#endif
        /* Then get the rest of it */
        do {
            rx_done = 1;
            if(dev->rx_rem_size > len) {
                rx_done = 0;
                data_size = len;
            }
            else {
                data_size = dev->rx_rem_size;
            }

            if(hif_receive(dev, dev->rx_addr + dev->rx_offset, buf, data_size, rx_done) == M2M_SUCCESS) {
                dev->rx_rem_size -= data_size;
                dev->rx_offset += data_size;
                buf += data_size;
            }
            else {
                break;
            }
        } while (dev->rx_rem_size > 0);
        dev->rx_size = -1;
    }
    _unlock_bus(dev);
    return size;
}

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    winc1500_t *dev = (winc1500_t *) netdev;
    int res = 0;

    switch (opt) {
        case NETOPT_ADDRESS:
            if (max_len < ETHERNET_ADDR_LEN) {
                res = -EINVAL;
            }
            else {
                winc1500_get_mac_addr(dev, (uint8_t*)value);
                res = ETHERNET_ADDR_LEN;
            }
            break;
        default:
            res = netdev_eth_get(netdev, opt, value, max_len);
            break;
    }

    return res;
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_NETDEV_ETH */
