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

#include <assert.h>
#include <stdint.h>

#include "atomic_utils.h"
#include "irq.h"
#include "net/ieee802154/radio.h"
#include "net/netdev/ieee802154_submac.h"
#include "event/thread.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static const ieee802154_submac_cb_t _cb;

static const netdev_driver_t netdev_submac_driver;

static uint32_t _isr_flags_get_clear(netdev_ieee802154_submac_t *netdev_submac, uint32_t clear)
{
    return atomic_fetch_and_u32(&netdev_submac->isr_flags, ~clear);
}

static void _isr_flags_set(netdev_ieee802154_submac_t *netdev_submac, uint32_t set)
{
    atomic_fetch_or_u32(&netdev_submac->isr_flags, set);
}

static void _ack_timeout(void *arg)
{
    netdev_ieee802154_submac_t *netdev_submac = arg;
    netdev_t *netdev = arg;

    _isr_flags_set(netdev_submac, NETDEV_SUBMAC_FLAGS_ACK_TIMEOUT);
    DEBUG("IEEE802154 submac: _ack_timeout(): post NETDEV_EVENT_ISR\n");
    netdev->event_callback(netdev, NETDEV_EVENT_ISR);
}

static netopt_state_t _get_submac_state(ieee802154_submac_t *submac)
{
    if (ieee802154_submac_state_is_idle(submac)) {
        return NETOPT_STATE_SLEEP;
    }
    else {
        return NETOPT_STATE_IDLE;
    }
}

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    netdev_ieee802154_submac_t *netdev_submac = container_of(netdev_ieee802154, netdev_ieee802154_submac_t, dev);
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
        case NETOPT_TX_POWER:
            *((int16_t *)value) = netdev_submac->dev.txpower;
            return sizeof(int16_t);
        default:
            break;
    }

    return netdev_ieee802154_get(container_of(netdev, netdev_ieee802154_t, netdev), opt,
                                 value, max_len);
}

static int _set_submac_state(ieee802154_submac_t *submac, netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_SLEEP:
            return ieee802154_set_idle(submac);
            break;
        case NETOPT_STATE_IDLE:
            return ieee802154_set_rx(submac);
            break;
        default:
            return -ENOTSUP;
    }
}

static int _set(netdev_t *netdev, netopt_t opt, const void *value,
                size_t value_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    netdev_ieee802154_submac_t *netdev_submac = container_of(netdev_ieee802154,
                                                             netdev_ieee802154_submac_t,
                                                             dev);
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
    case NETOPT_PROMISCUOUSMODE:
        return ieee802154_radio_set_frame_filter_mode(&submac->dev,
                                                      *((bool *)value) ? IEEE802154_FILTER_PROMISC
                                                                       : IEEE802154_FILTER_ACCEPT);
    default:
        break;
    }

    return netdev_ieee802154_set(container_of(netdev, netdev_ieee802154_t, netdev),
                                 opt, value, value_len);
}

void ieee802154_submac_bh_request(ieee802154_submac_t *submac)
{
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac,
                                                             netdev_ieee802154_submac_t,
                                                             submac);

    netdev_t *netdev = &netdev_submac->dev.netdev;
    _isr_flags_set(netdev_submac, NETDEV_SUBMAC_FLAGS_BH_REQUEST);
    DEBUG("IEEE802154 submac: ieee802154_submac_bh_request(): post NETDEV_EVENT_ISR\n");
    netdev->event_callback(netdev, NETDEV_EVENT_ISR);
}

void ieee802154_submac_ack_timer_set(ieee802154_submac_t *submac)
{
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac,
                                                             netdev_ieee802154_submac_t,
                                                             submac);
    ieee802154_submac_ack_timer_cancel(submac);
    DEBUG("IEEE802154 submac: Setting ACK timeout %"PRIu32" us\n", submac->ack_timeout_us);
    ztimer_set(ZTIMER_USEC, &netdev_submac->ack_timer, submac->ack_timeout_us);
}

void ieee802154_submac_ack_timer_cancel(ieee802154_submac_t *submac)
{
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac,
                                                             netdev_ieee802154_submac_t,
                                                             submac);
    DEBUG("IEEE802154 submac: Removing ACK timeout\n");
    ztimer_remove(ZTIMER_USEC, &netdev_submac->ack_timer);
    /* Prevent a race condition between the RX_DONE event and the ACK timeout */
    _isr_flags_get_clear(netdev_submac, NETDEV_SUBMAC_FLAGS_ACK_TIMEOUT);
}

static int _send(netdev_t *netdev, const iolist_t *pkt)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    netdev_ieee802154_submac_t *netdev_submac = container_of(netdev_ieee802154,
                                                             netdev_ieee802154_submac_t,
                                                             dev);
    ieee802154_submac_t *submac = &netdev_submac->submac;

    int res = ieee802154_send(submac, pkt);
    if (res >= 0) {
        /* HACK: Used to mark a transmission when called
         * inside the TX Done callback */
        netdev_submac->ev = NETDEV_EVENT_TX_STARTED;
    }
    if (res == 0) {
        uint8_t len = iolist_size(pkt);
        if (len > 0) {
            len += IEEE802154_FCS_LEN;
        }
        netdev_submac->bytes_tx = len;
    }

    return res;
}

static void _isr(netdev_t *netdev)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    netdev_ieee802154_submac_t *netdev_submac = container_of(netdev_ieee802154,
                                                             netdev_ieee802154_submac_t,
                                                             dev);
    ieee802154_submac_t *submac = &netdev_submac->submac;

    uint32_t flags;
    do {
        flags = _isr_flags_get_clear(netdev_submac, NETDEV_SUBMAC_FLAGS_CRC_ERROR);
        if (flags & NETDEV_SUBMAC_FLAGS_CRC_ERROR) {
            DEBUG("IEEE802154 submac:c NETDEV_SUBMAC_FLAGS_CRC_ERROR\n");
            ieee802154_submac_crc_error_cb(submac);
            flags &= ~NETDEV_SUBMAC_FLAGS_CRC_ERROR;
            continue;
        }

        flags = _isr_flags_get_clear(netdev_submac, NETDEV_SUBMAC_FLAGS_ACK_TIMEOUT);
        if (flags & NETDEV_SUBMAC_FLAGS_ACK_TIMEOUT) {
            DEBUG("IEEE802154 submac: _isr(): NETDEV_SUBMAC_FLAGS_ACK_TIMEOUT\n");
            ieee802154_submac_ack_timeout_fired(submac);
            flags &= ~NETDEV_SUBMAC_FLAGS_ACK_TIMEOUT;
            continue;
        }

        flags = _isr_flags_get_clear(netdev_submac, NETDEV_SUBMAC_FLAGS_BH_REQUEST);
        if (flags & NETDEV_SUBMAC_FLAGS_BH_REQUEST) {
            DEBUG("IEEE802154 submac: _isr(): NETDEV_SUBMAC_FLAGS_BH_REQUEST\n");
            ieee802154_submac_bh_process(submac);
            flags &= ~NETDEV_SUBMAC_FLAGS_BH_REQUEST;
            continue;
        }

        flags = _isr_flags_get_clear(netdev_submac, NETDEV_SUBMAC_FLAGS_RX_DONE);
        if (flags & NETDEV_SUBMAC_FLAGS_RX_DONE) {
            DEBUG("IEEE802154 submac: _isr(): NETDEV_SUBMAC_FLAGS_RX_DONE\n");
            ieee802154_submac_rx_done_cb(submac);
            flags &= ~NETDEV_SUBMAC_FLAGS_RX_DONE;
            /* dispatch to netif */
        }

        flags = _isr_flags_get_clear(netdev_submac, NETDEV_SUBMAC_FLAGS_TX_DONE);
        if (flags & NETDEV_SUBMAC_FLAGS_TX_DONE) {
            DEBUG("IEEE802154 submac: _isr(): NETDEV_SUBMAC_FLAGS_TX_DONE\n");
            ieee802154_submac_tx_done_cb(submac);
            flags &= ~NETDEV_SUBMAC_FLAGS_TX_DONE;
            /* dispatch to netif */
        }

        DEBUG("IEEE802154 submac: _isr_flags_get_clear(): pending flags: %"PRIu32"\n", flags);
        assert(!flags);
        break;

    } while (1);

    if (netdev_submac->dispatch) {
        /* The SubMAC will not generate further events after calling TX Done or RX Done. */
        netdev_submac->dispatch = false;
        /* TODO: Prevent race condition when state goes to PREPARE */
        DEBUG("IEEE802154 submac: _isr(): dispatching %d\n", netdev_submac->ev);
        netdev->event_callback(netdev, netdev_submac->ev);
        /* HACK: the TX_STARTED event is used to indicate a frame was
         * sent during the event callback.
         * If no frame was sent go back to RX */
        ieee802154_set_rx(submac);
    }
    else {
        DEBUG("IEEE802154 submac: no events to dispatch\n");
    }
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    netdev_ieee802154_submac_t *netdev_submac = container_of(netdev_ieee802154,
                                                             netdev_ieee802154_submac_t,
                                                             dev);
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
#if IS_USED(MODULE_NETDEV_IEEE802154_SUBMAC_SOFT_ACK)
    const uint8_t *mhr = buf;
    if ((mhr[0] & IEEE802154_FCF_TYPE_MASK) == IEEE802154_FCF_TYPE_DATA &&
        (mhr[0] & IEEE802154_FCF_ACK_REQ)) {
        ieee802154_filter_mode_t mode;
        if (!ieee802154_radio_has_capability(&submac->dev, IEEE802154_CAP_AUTO_ACK) &&
            (ieee802154_radio_get_frame_filter_mode(&submac->dev, &mode) < 0
                || mode == IEEE802154_FILTER_ACCEPT)) {
            /* send ACK if not handled by the driver and not in promiscuous mode */
            uint8_t ack[IEEE802154_ACK_FRAME_LEN - IEEE802154_FCS_LEN]
                = { IEEE802154_FCF_TYPE_ACK, 0x00, ieee802154_get_seq(mhr) };
            iolist_t io = {
                .iol_base = ack,
                .iol_len = sizeof(ack),
                .iol_next = NULL
            };
            DEBUG("IEEE802154 submac: Sending ACK\n");
            int snd = _send(netdev, &io);
            if (snd < 0) {
                DEBUG("IEEE802154 submac: failed to send ACK (%d)\n", snd);
            }
        }
    }
#endif
    return res;
}

static void submac_tx_done(ieee802154_submac_t *submac, int status,
                           ieee802154_tx_info_t *info)
{
    (void)status;
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac,
                                                             netdev_ieee802154_submac_t,
                                                             submac);
    if (info) {
        netdev_submac->retrans = info->retrans;
    }
    assert(!netdev_submac->dispatch);
    netdev_submac->dispatch = true;
    netdev_submac->ev = NETDEV_EVENT_TX_COMPLETE;

    switch (status) {
    case TX_STATUS_MEDIUM_BUSY:
        DEBUG("IEEE802154 submac: NETDEV_EVENT_TX_MEDIUM_BUSY\n");
        netdev_submac->bytes_tx = -EBUSY;
        break;
    case TX_STATUS_NO_ACK:
        DEBUG("IEEE802154 submac: NETDEV_EVENT_TX_NOACK\n");
        netdev_submac->bytes_tx = -EHOSTUNREACH;
        break;
    default:
        DEBUG("IEEE802154 submac: TX complete status: %d\n", status);
        break;
    }
}

static void submac_rx_done(ieee802154_submac_t *submac)
{
    netdev_ieee802154_submac_t *netdev_submac = container_of(submac,
                                                             netdev_ieee802154_submac_t,
                                                             submac);
    assert(!netdev_submac->dispatch);
    netdev_submac->dispatch = true;
    DEBUG("IEEE802154 submac: NETDEV_EVENT_RX_COMPLETE\n");
    netdev_submac->ev = NETDEV_EVENT_RX_COMPLETE;
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
    netdev_t *netdev = &netdev_submac->dev.netdev;

    DEBUG("IEEE802154 submac: _hal_radio_cb():\n");
    switch (status) {
    case IEEE802154_RADIO_CONFIRM_TX_DONE:
        DEBUG("IEEE802154 submac: _hal_radio_cb(): IEEE802154_RADIO_CONFIRM_TX_DONE\n");
        _isr_flags_set(netdev_submac, NETDEV_SUBMAC_FLAGS_TX_DONE);
        break;
    case IEEE802154_RADIO_INDICATION_RX_DONE:
        DEBUG("IEEE802154 submac: _hal_radio_cb(): IEEE802154_RADIO_INDICATION_RX_DONE\n");
        _isr_flags_set(netdev_submac, NETDEV_SUBMAC_FLAGS_RX_DONE);
        break;
    case IEEE802154_RADIO_INDICATION_CRC_ERROR:
        DEBUG("IEEE802154 submac: _hal_radio_cb(): IEEE802154_RADIO_INDICATION_CRC_ERROR\n");
        _isr_flags_set(netdev_submac, NETDEV_SUBMAC_FLAGS_CRC_ERROR);
        break;
    default:
        break;
    }
    DEBUG("IEEE802154 submac: _hal_radio_cb(): post NETDEV_EVENT_ISR\n");
    netdev->event_callback(netdev, NETDEV_EVENT_ISR);
}

static int _init(netdev_t *netdev)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    netdev_ieee802154_submac_t *netdev_submac = container_of(netdev_ieee802154,
                                                             netdev_ieee802154_submac_t,
                                                             dev);
    ieee802154_submac_t *submac = &netdev_submac->submac;
    netdev_ieee802154_setup(netdev_ieee802154);

    int res = ieee802154_submac_init(submac,
                                     (network_uint16_t*) netdev_ieee802154->short_addr,
                                     (eui64_t*) netdev_ieee802154->long_addr);

    if (res < 0) {
        return res;
    }

    /* This function already sets the PAN ID to the default one */
    netdev_ieee802154_reset(netdev_ieee802154);

    uint16_t chan = submac->channel_num;
    int16_t tx_power = submac->tx_pow;
    static const netopt_enable_t ack_req =
        IS_ACTIVE(CONFIG_IEEE802154_DEFAULT_ACK_REQ) ? NETOPT_ENABLE : NETOPT_DISABLE;

    /* Initialise netdev_ieee802154_t struct */
    netdev_ieee802154_set(netdev_ieee802154, NETOPT_CHANNEL,
                          &chan, sizeof(chan));
    netdev_ieee802154_set(netdev_ieee802154, NETOPT_ACK_REQ,
                          &ack_req, sizeof(ack_req));

    netdev_submac->dev.txpower = tx_power;

    /* signal link UP */
    netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static int _confirm_send(netdev_t *netdev, void *info)
{
    (void)info;
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    netdev_ieee802154_submac_t *netdev_submac = container_of(netdev_ieee802154,
                                                             netdev_ieee802154_submac_t,
                                                             dev);
    return netdev_submac->bytes_tx;
}

int netdev_ieee802154_submac_init(netdev_ieee802154_submac_t *netdev_submac)
{
    netdev_t *netdev = &netdev_submac->dev.netdev;

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
    .confirm_send = _confirm_send,
};

/** @} */
