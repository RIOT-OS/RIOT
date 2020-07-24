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
netdev_t *_netdev;
extern mutex_t stm32_eth_tx_completed;

void stm32_eth_set_mac(const char *mac);
void stm32_eth_get_mac(char *out);
int stm32_eth_init(void);
int stm32_eth_receive(void *data, size_t max_len);
int stm32_eth_send(const struct iolist *iolist);

static void _isr(netdev_t *netdev) {
    netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
}

void isr_eth(void)
{
    unsigned tmp = ETH->DMASR;

    if ((tmp & ETH_DMASR_TS)) {
        ETH->DMASR = ETH_DMASR_NIS | ETH_DMASR_TS;
        DEBUG("isr_eth: TX completed\n");
        mutex_unlock(&stm32_eth_tx_completed);
    }

    if ((tmp & ETH_DMASR_RS)) {
        ETH->DMASR = ETH_DMASR_NIS | ETH_DMASR_RS;
        DEBUG("isr_eth: RX completed\n");
        if (_netdev) {
            netdev_trigger_event_isr(_netdev);
        }
    }

    cortexm_isr_end();
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void)info;
    (void)netdev;
    return stm32_eth_receive(buf, len);
}

static int _send(netdev_t *netdev, const struct iolist *iolist)
{
    (void)netdev;
    netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
    int ret = stm32_eth_send(iolist);
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
