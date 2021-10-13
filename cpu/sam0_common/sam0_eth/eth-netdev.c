/*
 * Copyright (C) 2020 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @{
 *
 * @file
 * @brief       Low-level Ethernet driver implementation
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */
#include <string.h>

#include "iolist.h"
#include "net/gnrc/netif/ethernet.h"
#include "net/gnrc.h"
#include "net/ethernet.h"
#include "net/netdev/eth.h"
#include "net/eui_provider.h"

#include "periph/gpio.h"

#include "sam0_eth_netdev.h"

#define ENABLE_DEBUG 0
#include "debug.h"
#include "log.h"

/* Internal helpers */
extern int sam0_eth_init(void);
extern int sam0_eth_send(const struct iolist *iolist);
extern int sam0_eth_receive_blocking(char *data, unsigned max_len);
extern void sam0_eth_set_mac(const eui48_t *mac);
extern void sam0_eth_get_mac(char *out);
extern void sam0_clear_rx_buffers(void);

/* SAM0 CPUs only have one GMAC IP, so it is safe to
statically defines one in this file */
static sam0_eth_netdev_t _sam0_eth_dev;

static int _sam0_eth_init(netdev_t *netdev)
{
    sam0_eth_init();
    eui48_t hwaddr;
    netdev_eui48_get(netdev, &hwaddr);
    sam0_eth_set_mac(&hwaddr);
    return 0;
}

static void _sam0_eth_isr(netdev_t *netdev)
{
    netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    return;
}

static int _sam0_eth_recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void)info;
    (void)netdev;
    unsigned ret = sam0_eth_receive_blocking((char *)buf, len);
    return ret;
}

static int _sam0_eth_send(netdev_t *netdev, const iolist_t *iolist)
{
    int ret;

    netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
    ret = sam0_eth_send(iolist);
    if (ret == -EOVERFLOW) {
        /* TODO: use a specific netdev callback here ? */
        return -EOVERFLOW;
    }
    netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    return ret;
}

static int _sam0_eth_get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    int res = -1;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
            sam0_eth_get_mac((char *)val);
            res = ETHERNET_ADDR_LEN;
            break;
        default:
            res = netdev_eth_get(netdev, opt, val, max_len);
            break;
    }

    return res;
}

static int _sam0_eth_set(netdev_t *netdev, netopt_t opt, const void *val, size_t max_len)
{
    int res = -1;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
            sam0_eth_set_mac((eui48_t *)val);
            res = ETHERNET_ADDR_LEN;
            break;
        default:
            res = netdev_eth_set(netdev, opt, val, max_len);
            break;
    }

    return res;
}

static const netdev_driver_t _sam0_eth_driver =
{
    .send = _sam0_eth_send,
    .recv = _sam0_eth_recv,
    .init = _sam0_eth_init,
    .isr  = _sam0_eth_isr,
    .get  = _sam0_eth_get,
    .set  = _sam0_eth_set,
};

void sam0_eth_setup(netdev_t* netdev)
{

    DEBUG_PUTS("[sam0_eth]: initializing SAM0 Ethernet MAC (GMAC) device");

    _sam0_eth_dev.netdev = netdev;
    /* set the netdev driver */
    netdev->driver = &_sam0_eth_driver;
    /* Register SAM0 Ethernet to netdev */
    netdev_register(netdev, NETDEV_SAM0_ETH, 0);
}

void isr_gmac(void)
{
    uint32_t isr;
    uint32_t rsr;

    isr = GMAC->ISR.reg;
    rsr = GMAC->RSR.reg;
    (void)isr;

    /* New frame received, signal it to netdev */
    if (rsr & GMAC_RSR_REC) {
        netdev_trigger_event_isr(_sam0_eth_dev.netdev);
    }
    /* Buffers Not Available, this can occur if there is a heavy traffic
       on the network. In this case, disable the GMAC reception, flush
       our internal buffers and re-enable the reception. This will drop
       a few packets but it allows the GMAC IP to remains functional */
    if (rsr & GMAC_RSR_BNA) {
        GMAC->NCR.reg &= ~GMAC_NCR_RXEN;
        sam0_clear_rx_buffers();
        GMAC->NCR.reg |= GMAC_NCR_RXEN;
    }
    GMAC->RSR.reg = rsr;

    cortexm_isr_end();
}
