/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_lpc1768_eth
 *
 * @{
 * @file
 * @brief       netdev driver for the LPC1768 EMAC peripheral
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "config.h"
#include "kernel_defines.h"
#include "net/ethernet.h"
#include "net/eui_provider.h"
#include "net/eui48.h"
#include "net/netdev.h"
#include "net/netdev/eth.h"
#include "net/netopt.h"
#include "periph/eth.h"
#include "ztimer/periodic.h"

#include "lpc1768_eth_netdev.h"

#define ENABLE_DEBUG 0
#define ENABLE_TRACE 0
#include "debug.h"

#if ENABLE_TRACE
#  define TRACE(...) DEBUG(__VA_ARGS__)
#  define TRACE_PUTS(...) DEBUG_PUTS(__VA_ARGS__)
#else
#  define TRACE(...)
#  define TRACE_PUTS(...)
#endif

/**
 * @brief   Singleton netdev instance for the LPC1768 EMAC peripheral.
 *
 * This is used by the low-level ethernet driver to deliver ISR events.
 */
netdev_t *lpc1768_eth_netdev;

static ztimer_periodic_t _link_timer;
static bool _link_state;
static volatile bool _link_check;

static bool _link_timer_cb(void *arg)
{
    netdev_t *netdev = arg;

    /* link check happens in event loop to not block the ISR */
    _link_check = true;
    netdev_trigger_event_isr(netdev);

    return ZTIMER_PERIODIC_KEEP_GOING;
}

static int _init(netdev_t *netdev)
{
    eui48_t hwaddr;

    DEBUG_PUTS("[eth-netdev] _init: netdev init");

    netdev_eui48_get(netdev, &hwaddr);

    int res = lpc1768_eth_init(hwaddr.uint8);

    if (res < 0) {
        DEBUG("[eth-netdev] _init: init failed (%d)\n", res);
        return res;
    }

    if (IS_USED(MODULE_LPC1768_ETH_LINK_UP)) {
        if (IS_USED(MODULE_LPC1768_ETH_AUTO)) {
            /* start auto-negotiation of the link speed */
            lpc1768_eth_start_auto_negotiation();
        }

        /* periodically wake the event loop to poll the link state */
        ztimer_periodic_init(ZTIMER_MSEC, &_link_timer, _link_timer_cb, netdev,
                             CONFIG_LPC1768_ETH_LINK_POLL_MS);
        ztimer_periodic_start(&_link_timer);
    }
    else {
        /* assume link is up */
        netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);
    }

    return 0;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    (void)netdev;

    TRACE("[eth-netdev] _send: sending packet, length=%" PRIuSIZE " bytes\n", iolist_size(iolist));

    netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);

    int res = lpc1768_eth_send(iolist);

    if (res < 0) {
        return res;
    }

    return 0;
}

static int _confirm_send(netdev_t *netdev, void *info)
{
    (void)netdev;
    (void)info;

    return lpc1768_eth_tx_status();
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void)netdev;
    (void)info;

    TRACE("[eth-netdev] _recv: receiving frame, length=%" PRIuSIZE " bytes\n", len);

    int res = lpc1768_eth_recv(buf, len);

    /* signal event loop that there is more to consume, in case an interrupt
     * was missed */
    if (lpc1768_eth_rx_pending()) {
        netdev_trigger_event_isr(netdev);
    }

    return res;
}

static void _isr(netdev_t *netdev)
{
    /* poll the link state in thread context when the timer requested it */
    if (IS_USED(MODULE_LPC1768_ETH_LINK_UP) && _link_check) {
        _link_check = false;

        bool up = lpc1768_eth_link_up();

        if (up != _link_state) {
            _link_state = up;

            if (up) {
                DEBUG_PUTS("[eth-netdev] _isr: link up");
                if (IS_USED(MODULE_LPC1768_ETH_AUTO)) {
                    /* complete auto-negotiation of the link */
                    lpc1768_eth_complete_auto_negotiation();
                }
                netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);
            }
            else {
                DEBUG_PUTS("[eth-netdev] _isr: link down");
                netdev->event_callback(netdev, NETDEV_EVENT_LINK_DOWN);
            }
        }
    }

    /* receive if necessary */
    if (lpc1768_eth_rx_pending()) {
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    switch (opt) {
    case NETOPT_ADDRESS:
        assert(max_len >= ETHERNET_ADDR_LEN);
        lpc1768_eth_get_mac((uint8_t *)val);
        return ETHERNET_ADDR_LEN;
    case NETOPT_LINK:
        assert(max_len == sizeof(netopt_enable_t));
        *(netopt_enable_t *)val = lpc1768_eth_link_up() ? NETOPT_ENABLE : NETOPT_DISABLE;
        return sizeof(netopt_enable_t);
    case NETOPT_PROMISCUOUSMODE:
        assert(max_len == sizeof(netopt_enable_t));
        *(netopt_enable_t *)val = lpc1768_eth_get_promiscuous() ? NETOPT_ENABLE : NETOPT_DISABLE;
        return sizeof(netopt_enable_t);
    default:
        return netdev_eth_get(netdev, opt, val, max_len);
    }
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    switch (opt) {
    case NETOPT_ADDRESS:
        assert(len >= ETHERNET_ADDR_LEN);
        lpc1768_eth_set_mac((const uint8_t *)val);
        return ETHERNET_ADDR_LEN;
    case NETOPT_PROMISCUOUSMODE:
        assert(len == sizeof(netopt_enable_t));
        lpc1768_eth_set_promiscuous(*(const netopt_enable_t *)val == NETOPT_ENABLE);
        return sizeof(netopt_enable_t);
    default:
        return netdev_eth_set(netdev, opt, val, len);
    }
}

const netdev_driver_t lpc1768_eth_driver = {
    .send = _send,
    .confirm_send = _confirm_send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

void lpc1768_eth_netdev_setup(netdev_t *netdev)
{
    DEBUG_PUTS("[eth-netdev] lpc1768_eth_netdev_setup: registering netdev");

    /* keep local netdev copy to invoke netdev ISR from ethernet ISR */
    lpc1768_eth_netdev = netdev;

    /* initialize netdev fields */
    netdev->event_callback = NULL;
    netdev->context = NULL;
    netdev->driver = &lpc1768_eth_driver;

    netdev_register(netdev, NETDEV_LPC1768_ETH, 0);
}
