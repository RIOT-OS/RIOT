/*
 * Copyright (C) 2016 TriaGnoSys GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_stm32_common
 * @{
 *
 * @file
 * @brief       Netdev wrapper for stm32 ethernet
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 *
 * @}
 */

#include "periph_conf.h"
#include "mutex.h"
#include "net/netdev/eth.h"
#include "net/ethernet.h"
#include "iolist.h"
#define ENABLE_DEBUG (0)
#include "debug.h"

#include <string.h>
static mutex_t _tx = MUTEX_INIT;
static mutex_t _rx = MUTEX_INIT;
netdev_t *_netdev;

void stm32_eth_set_mac(const char *mac);
void stm32_eth_get_mac(char *out);
int stm32_eth_init(void);
int stm32_eth_receive_blocking(char *data, unsigned max_len);
int stm32_eth_send(const struct iolist *iolist);
int stm32_eth_get_rx_status_owned(void);

static void _isr(netdev_t *netdev) {
    if(stm32_eth_get_rx_status_owned()) {
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
}

void isr_eth(void)
{
    volatile unsigned tmp = ETH->DMASR;

    if ((tmp & ETH_DMASR_TS)) {
        ETH->DMASR = ETH_DMASR_TS | ETH_DMASR_NIS;
        mutex_unlock(&_tx);
    }

    if ((tmp & ETH_DMASR_RS)) {
        ETH->DMASR = ETH_DMASR_RS | ETH_DMASR_NIS;
        mutex_unlock(&_rx);
        if (_netdev) {
            netdev_trigger_event_isr(_netdev);
        }
    }

    /* printf("r:%x\n\n", tmp); */

    cortexm_isr_end();
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void)info;
    (void)netdev;
    if(!stm32_eth_get_rx_status_owned()){
                mutex_lock(&_rx);
    }
    int ret = stm32_eth_receive_blocking((char *)buf, len);
    DEBUG("stm32_eth_netdev: _recev: %d\n", ret);

    return ret;
}

static int _send(netdev_t *netdev, const struct iolist *iolist)
{
    (void)netdev;
    int ret = 0;
    if(stm32_eth_get_rx_status_owned()) {
        mutex_lock(&_tx);
    }
    netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
    ret = stm32_eth_send(iolist);
    DEBUG("stm32_eth_netdev: _send: %d %d\n", ret, iolist_size(iolist));
    if (ret < 0)
    {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_MEDIUM_BUSY);
        return ret;
    }
    netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);

    return ret;
}

static int _set(netdev_t *dev, netopt_t opt, const void *value, size_t max_len)
{
    int res = -1;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
            stm32_eth_set_mac((char *)value);
            res = ETHERNET_ADDR_LEN;
            break;
        default:
            res = netdev_eth_set(dev, opt, value, max_len);
            break;
    }

    return res;
}

static int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    int res = -1;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
            stm32_eth_get_mac((char *)value);
            res = ETHERNET_ADDR_LEN;
            break;
        default:
            res = netdev_eth_get(dev, opt, value, max_len);
            break;
    }

    return res;
}

static int _init(netdev_t *netdev)
{
    (void)netdev;
    return stm32_eth_init();
}

static const netdev_driver_t netdev_driver_stm32f4eth = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

void stm32_eth_netdev_setup(netdev_t *netdev)
{
    _netdev = netdev;
    netdev->driver = &netdev_driver_stm32f4eth;
}
