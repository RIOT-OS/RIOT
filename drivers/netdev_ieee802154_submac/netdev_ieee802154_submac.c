/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author José I. Alamos <jose.alamos@haw-hamburg.de>
 */

#include "net/netdev/ieee802154_submac.h"
#include "event/thread.h"

static const ieee802154_submac_cb_t _cb;

static const netdev_driver_t netdev_submac_driver;

static void _ack_timeout(void *arg)
{
    (void)arg;
    netdev_ieee802154_submac_t *netdev_submac = arg;
    netdev_t *netdev = arg;

    netdev_submac->isr_flags |= NETDEV_SUBMAC_FLAGS_ACK_TIMEOUT;

    netdev->event_callback(netdev, NETDEV_EVENT_ISR);
}

static netopt_state_t _get_submac_state(ieee802154_submac_t *submac)
{
    ieee802154_submac_state_t state = ieee802154_get_state(submac);

    netopt_state_t netopt_state;
    switch (state) {
        case IEEE802154_STATE_OFF:
            netopt_state = NETOPT_STATE_SLEEP;
            break;
        case IEEE802154_STATE_IDLE:
            netopt_state = NETOPT_STATE_STANDBY;
            break;
        case IEEE802154_STATE_LISTEN:
        default:
            netopt_state = NETOPT_STATE_IDLE;
            break;
    }

    return netopt_state;
}

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    netdev_ieee802154_submac_t *netdev_submac = (netdev_ieee802154_submac_t *)netdev;
    ieee802154_submac_t *submac = &netdev_submac->submac;

    switch (opt) {
        case NETOPT_STATE:
            *((netopt_state_t*) value) = _get_submac_state(submac);
            return 0;
        case NETOPT_TX_RETRIES_NEEDED:
            if (netdev_submac->retrans < 0) {
                return -ENOTSUP;
            }
            *((uint8_t*) value) = netdev_submac->retrans;
            return 1;
        case NETOPT_IEEE802154_PHY:
            *((uint8_t*) value) = ieee802154_get_phy_mode(submac);
            return 1;
        default:
            break;
    }

    return netdev_ieee802154_get((netdev_ieee802154_t *)netdev, opt,
                                 value, max_len);
}

static int _set_submac_state(ieee802154_submac_t *submac, netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_STANDBY:
            return ieee802154_set_state(submac, IEEE802154_STATE_IDLE);
        case NETOPT_STATE_SLEEP:
            return ieee802154_set_state(submac, IEEE802154_STATE_OFF);
        case NETOPT_STATE_IDLE:
            return ieee802154_set_state(submac, IEEE802154_STATE_LISTEN);
        default:
            return -ENOTSUP;
    }
}

static int _set(netdev_t *netdev, netopt_t opt, const void *value,
                size_t value_len)
{
    netdev_ieee802154_submac_t *netdev_submac =
        (netdev_ieee802154_submac_t *)netdev;
    ieee802154_submac_t *submac = &netdev_submac->submac;

    int res;
    int16_t tx_power;

    switch (opt) {
    case NETOPT_ADDRESS:
        ieee802154_set_short_addr(submac, value);
        break;
    case NETOPT_ADDRESS_LONG:
        ieee802154_set_ext_addr(submac, value);
        break;
    case NETOPT_NID:
        ieee802154_set_panid(submac, value);
        break;
    case NETOPT_CHANNEL:
        ieee802154_set_channel_number(submac, *((uint16_t *)value));
        break;
    case NETOPT_TX_POWER:
        tx_power = *((int16_t *)value);
        res = ieee802154_set_tx_power(submac, tx_power);
        if (res >= 0) {
            netdev_submac->dev.txpower = tx_power;
        }
        return res;
    case NETOPT_STATE:
        return _set_submac_state(submac, *((netopt_state_t*) value));
    default:
        break;
    }

    return netdev_ieee802154_set((netdev_ieee802154_t *)netdev, opt,
                                 value, value_len);
}

void ieee802154_submac_ack_timer_set(ieee802154_submac_t *submac, uint16_t us)
{
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac,
                                                             netdev_ieee802154_submac_t,
                                                             submac);

    xtimer_set(&netdev_submac->ack_timer, us);
}

void ieee802154_submac_ack_timer_cancel(ieee802154_submac_t *submac)
{
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac,
                                                             netdev_ieee802154_submac_t,
                                                             submac);

    xtimer_remove(&netdev_submac->ack_timer);
}

static int _send(netdev_t *netdev, const iolist_t *pkt)
{
    netdev_ieee802154_submac_t *netdev_submac =
        (netdev_ieee802154_submac_t *)netdev;

    return ieee802154_send(&netdev_submac->submac, pkt);
}

static void _isr(netdev_t *netdev)
{
    netdev_ieee802154_submac_t *netdev_submac =
        (netdev_ieee802154_submac_t *)netdev;
    ieee802154_submac_t *submac = &netdev_submac->submac;

    do {
        irq_disable();
        int flags = netdev_submac->isr_flags;
        netdev_submac->isr_flags = 0;
        irq_enable();

        if (flags & NETDEV_SUBMAC_FLAGS_ACK_TIMEOUT) {
            ieee802154_submac_ack_timeout_fired(&netdev_submac->submac);
        }

        if (flags & NETDEV_SUBMAC_FLAGS_TX_DONE) {
            ieee802154_submac_tx_done_cb(&netdev_submac->submac);
        }

        if (flags & NETDEV_SUBMAC_FLAGS_RX_DONE) {
            ieee802154_submac_rx_done_cb(submac);
        }

        if (flags & NETDEV_SUBMAC_FLAGS_CRC_ERROR) {
            ieee802154_submac_crc_error_cb(submac);
        }
    } while (netdev_submac->isr_flags != 0);
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    netdev_ieee802154_submac_t *netdev_submac =
        (netdev_ieee802154_submac_t *)netdev;
    ieee802154_submac_t *submac = &netdev_submac->submac;
    ieee802154_rx_info_t rx_info;

    if (buf == NULL && len == 0) {
        return ieee802154_get_frame_length(submac);
    }

    int res = ieee802154_read_frame(submac, buf, len, &rx_info);

    if (info) {
        netdev_ieee802154_rx_info_t *netdev_rx_info = info;

        /* The Radio HAL uses the IEEE 802.15.4 definition for RSSI.
         * Netdev uses dBm. Therefore we need a translation here */
        netdev_rx_info->rssi = ieee802154_rssi_to_dbm(rx_info.rssi);

        netdev_rx_info->lqi = rx_info.lqi;
    }

    return res;
}

static void submac_tx_done(ieee802154_submac_t *submac, int status,
                           ieee802154_tx_info_t *info)
{
    (void)status;
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac,
                                                             netdev_ieee802154_submac_t,
                                                             submac);
    netdev_t *netdev = (netdev_t *)netdev_submac;

    if (info) {
        netdev_submac->retrans = info->retrans;
    }

    switch (status) {
    case TX_STATUS_SUCCESS:
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
        break;
    case TX_STATUS_FRAME_PENDING:
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE_DATA_PENDING);
        break;
    case TX_STATUS_MEDIUM_BUSY:
        netdev->event_callback(netdev, NETDEV_EVENT_TX_MEDIUM_BUSY);
        break;
    case TX_STATUS_NO_ACK:
        netdev->event_callback(netdev, NETDEV_EVENT_TX_NOACK);
        break;
    default:
        break;
    }
}

static void submac_rx_done(ieee802154_submac_t *submac)
{
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac,
                                                             netdev_ieee802154_submac_t,
                                                             submac);
    netdev_t *netdev = (netdev_t *)netdev_submac;

    netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
}

static const ieee802154_submac_cb_t _cb = {
    .rx_done = submac_rx_done,
    .tx_done = submac_tx_done,
};

/* Event Notification callback */
static void _hal_radio_cb(ieee802154_dev_t *dev, ieee802154_trx_ev_t status)
{
    ieee802154_submac_t *submac = container_of(dev, ieee802154_submac_t, dev);
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac,
                                                             netdev_ieee802154_submac_t,
                                                             submac);
    netdev_t *netdev = (netdev_t *)netdev_submac;

    switch (status) {
    case IEEE802154_RADIO_CONFIRM_TX_DONE:
        netdev_submac->isr_flags |= NETDEV_SUBMAC_FLAGS_TX_DONE;
        break;
    case IEEE802154_RADIO_INDICATION_RX_DONE:
        netdev_submac->isr_flags |= NETDEV_SUBMAC_FLAGS_RX_DONE;
        break;
    case IEEE802154_RADIO_INDICATION_CRC_ERROR:
        netdev_submac->isr_flags |= NETDEV_SUBMAC_FLAGS_CRC_ERROR;
        break;
    default:
        break;
    }
    netdev->event_callback(netdev, NETDEV_EVENT_ISR);
}

static int _init(netdev_t *netdev)
{
    netdev_ieee802154_submac_t *netdev_submac =
        (netdev_ieee802154_submac_t *)netdev;
    ieee802154_submac_t *submac = &netdev_submac->submac;
    netdev_ieee802154_t *netdev_ieee802154 = (netdev_ieee802154_t *)netdev;
    netdev_ieee802154_setup(netdev_ieee802154);

    ieee802154_submac_init(submac, (network_uint16_t*) netdev_ieee802154->short_addr, (eui64_t*) netdev_ieee802154->long_addr);

    /* This function already sets the PAN ID to the default one */
    netdev_ieee802154_reset(netdev_ieee802154);

    uint16_t chan = CONFIG_IEEE802154_DEFAULT_CHANNEL;
    int16_t tx_power = CONFIG_IEEE802154_DEFAULT_TXPOWER;
    netopt_enable_t enable = NETOPT_ENABLE;

    /* Initialise netdev_ieee802154_t struct */
    netdev_ieee802154_set(netdev_ieee802154, NETOPT_CHANNEL,
                          &chan, sizeof(chan));
    netdev_ieee802154_set(netdev_ieee802154, NETOPT_ACK_REQ,
                          &enable, sizeof(enable));

    netdev_submac->dev.txpower = tx_power;
    return 0;
}

int netdev_ieee802154_submac_init(netdev_ieee802154_submac_t *netdev_submac)
{
    netdev_t *netdev = (netdev_t *)netdev_submac;

    netdev->driver = &netdev_submac_driver;
    ieee802154_submac_t *submac = &netdev_submac->submac;

    submac->cb = &_cb;

    /* Set the Event Notification */
    submac->dev.cb = _hal_radio_cb;

    netdev_submac->ack_timer.callback = _ack_timeout;
    netdev_submac->ack_timer.arg = netdev_submac;

    return 0;
}

static const netdev_driver_t netdev_submac_driver = {
    .get = _get,
    .set = _set,
    .send = _send,
    .recv = _recv,
    .isr = _isr,
    .init = _init,
};

/** @} */
