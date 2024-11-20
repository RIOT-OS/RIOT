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
#include "mii.h"
#include "net/gnrc/netif/ethernet.h"
#include "net/gnrc.h"
#include "net/ethernet.h"
#include "net/netdev/eth.h"
#include "net/eui_provider.h"

#include "periph/gpio.h"
#include "ztimer.h"

#include "sam0_eth_netdev.h"

#define ENABLE_DEBUG 0
#include "debug.h"
#include "log.h"

/**
 * @brief   Link auto-negotiation timeout
 */
#ifndef CONFIG_SAM0_ETH_LINK_TIMEOUT_MS
#define CONFIG_SAM0_ETH_LINK_TIMEOUT_MS         (5 * MS_PER_SEC)
#endif

/* Internal helpers */
extern int sam0_eth_init(void);
extern void sam0_eth_poweron(void);
extern void sam0_eth_poweroff(void);
extern int sam0_eth_send(const struct iolist *iolist);
extern int sam0_eth_receive_blocking(char *data, unsigned max_len);
extern bool sam0_eth_has_queued_pkt(void);
extern void sam0_eth_set_mac(const eui48_t *mac);
extern void sam0_eth_get_mac(char *out);
extern void sam0_clear_rx_buffers(void);
extern unsigned sam0_read_phy(uint8_t phy, uint8_t addr);
extern void sam0_write_phy(uint8_t phy, uint8_t addr, uint16_t data);
static void _restart_an(void *ctx);

/* SAM0 CPUs only have one GMAC IP, so it is safe to
statically defines one in this file */
static sam0_eth_netdev_t _sam0_eth_dev;

/* auto-negotiation timeout timer */
static ztimer_t _phy_tim = { .callback = _restart_an };
/* PHY interrupt status register */
static uint16_t _phy_irq;

static inline bool _get_link_status(void)
{
    return sam0_read_phy(0, MII_BMSR) & MII_BMSR_LINK;
}

static void _restart_an(void *ctx)
{
    (void)ctx;
    sam0_write_phy(0, MII_IRQ, MII_IRQ_EN_LPA_ACK);
    sam0_write_phy(0, MII_BMCR, MII_BMCR_AN_RESTART | MII_BMCR_AN_ENABLE |
                                MII_BMCR_SPEED_100 | MII_BMCR_FULL_DPLX);
}

static void _phy_isr(void *ctx)
{
    (void)ctx;

    _phy_irq = sam0_read_phy(0, MII_IRQ);

    netdev_trigger_event_isr(_sam0_eth_dev.netdev);
}

static void _handle_phy_irq(uint16_t irq)
{
    netdev_t *netdev = _sam0_eth_dev.netdev;

    if (irq & MII_IRQ_LINK_DOWN) {
        DEBUG_PUTS("[sam0_eth]: link down");

        if (IS_USED(MODULE_ZTIMER_MSEC)) {
            ztimer_remove(ZTIMER_MSEC, &_phy_tim);
        }
        /* only listen for link partner ACK events now */
        sam0_write_phy(0, MII_IRQ, MII_IRQ_EN_LPA_ACK);

        netdev->event_callback(netdev, NETDEV_EVENT_LINK_DOWN);
        return;
    }
    if (irq & MII_IRQ_LINK_UP) {
        DEBUG_PUTS("[sam0_eth]: link up");

        uint16_t adv = sam0_read_phy(0, MII_ADVERTISE);
        uint16_t lpa = sam0_read_phy(0, MII_LPA);

        if (IS_USED(MODULE_ZTIMER_MSEC)) {
            ztimer_remove(ZTIMER_MSEC, &_phy_tim);
        }

        uint32_t ncfgr = GMAC->NCFGR.reg & ~(GMAC_NCFGR_FD | GMAC_NCFGR_MTIHEN);

        if ((adv & MII_ADVERTISE_100) && (lpa & MII_LPA_100)) {
            /* 100 Mbps */
            ncfgr |= GMAC_NCFGR_SPD;
        }
        if ((adv & MII_ADVERTISE_10_F) && (lpa & MII_LPA_10_F)) {
            /* full duplex */
            ncfgr |= GMAC_NCFGR_FD;
        }

        GMAC->NCFGR.reg = ncfgr;
        netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);
        return;
    }

    if (irq & MII_IRQ_LPA_ACK) {
        DEBUG_PUTS("[sam0_eth]: link partner present");

        /* if we don't succeed, restart auto-negotiation in 5s */
        if (IS_USED(MODULE_ZTIMER_MSEC)) {
            ztimer_set(ZTIMER_MSEC, &_phy_tim, CONFIG_SAM0_ETH_LINK_TIMEOUT_MS);
        }

        /* we only care about link up / down events now */
        sam0_write_phy(0, MII_IRQ, MII_IRQ_EN_LINK_UP | MII_IRQ_EN_LINK_DOWN);
        return;
    }

    DEBUG("[sam0_eth]: unexpected PHY IRQ: %x\n", irq);
}

static inline void _setup_phy_irq(gpio_cb_t cb, void *arg)
{
    gpio_init_int(sam_gmac_config[0].int_pin, GPIO_IN, GPIO_FALLING, cb, arg);
}

static int _sam0_eth_init(netdev_t *netdev)
{
    sam0_eth_init();
    eui48_t hwaddr;
    netdev_eui48_get(netdev, &hwaddr);
    sam0_eth_set_mac(&hwaddr);

    /* wait for PHY to be ready */
    while (MII_BMCR_RESET & sam0_read_phy(0, MII_BMCR)) {}

    _setup_phy_irq(_phy_isr, NULL);
    _restart_an(NULL);

    return 0;
}

static void _sam0_eth_isr(netdev_t *netdev)
{
    if (_phy_irq) {
        uint16_t tmp = _phy_irq;
        _phy_irq = 0;

        _handle_phy_irq(tmp);
        return;
    }

    netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    return;
}

static int _sam0_eth_recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void)info;
    (void)netdev;
    unsigned ret = sam0_eth_receive_blocking((char *)buf, len);

    /* frame received, check if another frame is queued */
    if (buf && sam0_eth_has_queued_pkt()) {
        netdev_trigger_event_isr(netdev);
    }

    return ret;
}

static int _sam0_eth_send(netdev_t *netdev, const iolist_t *iolist)
{
    netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
    return sam0_eth_send(iolist);
}

static int _sam0_eth_confirm_send(netdev_t *netdev, void *info)
{
    (void)netdev;
    (void)info;

    uint32_t tsr = GMAC->TSR.reg;
    GMAC->TSR.reg = tsr; /* clear flags */

    /* transmit is active */
    if (tsr & GMAC_TSR_TXGO) {
        return -EAGAIN;
    }

    /* Retry Limit Exceeded, Collision Occurred */
    if (tsr & (GMAC_TSR_RLE | GMAC_TSR_COL)) {
        return -EBUSY;
    }

    /* Transmit Frame Corruption */
    if (tsr & GMAC_TSR_TFC) {
        return -EIO;
    }

    extern unsigned _sam0_eth_get_last_len(void);
    return _sam0_eth_get_last_len();
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
        case NETOPT_LINK:
            assert(max_len == sizeof(netopt_enable_t));
            *(netopt_enable_t *)val = _get_link_status();
            res = sizeof(netopt_enable_t);
            break;
        default:
            res = netdev_eth_get(netdev, opt, val, max_len);
            break;
    }

    return res;
}

static int _set_state(netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_SLEEP:
            if (IS_USED(MODULE_ZTIMER_MSEC)) {
                ztimer_remove(ZTIMER_MSEC, &_phy_tim);
            }
            sam0_eth_poweroff();
            _sam0_eth_dev.netdev->event_callback(_sam0_eth_dev.netdev,
                                                 NETDEV_EVENT_LINK_DOWN);
            break;
        case NETOPT_STATE_IDLE:
            sam0_eth_poweron();
            _restart_an(NULL);
            break;
        default:
            return -ENOTSUP;
    }

    return sizeof(netopt_state_t);
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
        case NETOPT_STATE:
            assert(max_len <= sizeof(netopt_state_t));
            return _set_state(*((const netopt_state_t *)val));
        default:
            res = netdev_eth_set(netdev, opt, val, max_len);
            break;
    }

    return res;
}

static const netdev_driver_t _sam0_eth_driver =
{
    .send = _sam0_eth_send,
    .confirm_send = _sam0_eth_confirm_send,
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
    netdev_t* netdev = _sam0_eth_dev.netdev;

    isr = GMAC->ISR.reg;
    rsr = GMAC->RSR.reg;

    /* TX done, signal it to netdev */
    if (isr & GMAC_ISR_TCOMP) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }

    /* New frame received, signal it to netdev */
    if (rsr & GMAC_RSR_REC) {
        netdev_trigger_event_isr(netdev);
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
