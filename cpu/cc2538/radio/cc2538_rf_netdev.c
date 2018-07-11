/*
 * Copyright (C) 2016 MUTEX NZ Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file
 * @brief       Netdev adaption for the CC2538 RF driver
 *
 * @author      Aaron Sowry <aaron@mutex.nz>
 *
 * @}
 */

#include <errno.h>

#include "net/gnrc.h"
#include "net/netdev.h"

#include "cc2538_rf.h"
#include "cc2538_rf_netdev.h"
#include "cc2538_rf_internal.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#define _MAX_MHR_OVERHEAD   (25)

/* Reference pointer for the IRQ handler */
static netdev_t *_dev;

void _irq_handler(void)
{
    if (_dev->event_callback) {
        _dev->event_callback(_dev, NETDEV_EVENT_ISR);
    }
}

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    cc2538_rf_t *dev = (cc2538_rf_t *)netdev;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_AUTOACK:
            if (RFCORE->XREG_FRMCTRL0bits.AUTOACK) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_CHANNEL_PAGE:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            /* This tranceiver only supports page 0 */
            *((uint16_t *)value) = 0;
            return sizeof(uint16_t);

        case NETOPT_DEVICE_TYPE:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *) value) = NETDEV_TYPE_IEEE802154;
            return sizeof(uint16_t);

        case NETOPT_IS_CHANNEL_CLR:
            if (cc2538_channel_clear()) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_IS_WIRED:
            return -ENOTSUP;

        case NETOPT_MAX_PACKET_SIZE:
            if (max_len < sizeof(int16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)value) = CC2538_RF_MAX_DATA_LEN - _MAX_MHR_OVERHEAD;
            return sizeof(uint16_t);

        case NETOPT_PROMISCUOUSMODE:
            if (cc2538_get_monitor()) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_STATE:
            if (max_len < sizeof(netopt_state_t)) {
                return -EOVERFLOW;
            }
            *((netopt_state_t *)value) = dev->state;
            return sizeof(netopt_state_t);

        case NETOPT_TX_POWER:
            if (max_len < sizeof(int16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)value) = cc2538_get_tx_power();
            return sizeof(uint16_t);

        default:
            break;
    }

    int res;

    if (((res = netdev_ieee802154_get((netdev_ieee802154_t *)netdev, opt, value,
                                      max_len)) >= 0) || (res != -ENOTSUP)) {
        return res;
    }

    return -ENOTSUP;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *value, size_t value_len)
{
    cc2538_rf_t *dev = (cc2538_rf_t *)netdev;
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_ADDRESS:
            if (value_len > sizeof(uint16_t)) {
                res = -EOVERFLOW;
            }
            else {
                cc2538_set_addr_short(*((const uint16_t*)value));
            }
            break;

        case NETOPT_ADDRESS_LONG:
            if (value_len > sizeof(uint64_t)) {
                res = -EOVERFLOW;
            }
            else {
                cc2538_set_addr_long(*((const uint64_t*)value));
            }
            break;

        case NETOPT_AUTOACK:
            RFCORE->XREG_FRMCTRL0bits.AUTOACK = ((const bool *)value)[0];
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CHANNEL:
            if (value_len != sizeof(uint16_t)) {
                res = -EINVAL;
            }
            else {
                uint8_t chan = ((const uint8_t *)value)[0];
                if (chan < IEEE802154_CHANNEL_MIN ||
                    chan > IEEE802154_CHANNEL_MAX) {
                    res = -EINVAL;
                }
                else {
                    cc2538_set_chan(chan);
                }
            }
            break;

        case NETOPT_CHANNEL_PAGE:
            /* This tranceiver only supports page 0 */
            if (value_len != sizeof(uint16_t) ||
                *((const uint16_t *)value) != 0 ) {
                res = -EINVAL;
            }
            else {
                res = sizeof(uint16_t);
            }
            break;

        case NETOPT_IS_WIRED:
            return -ENOTSUP;

        case NETOPT_NID:
            if (value_len > sizeof(uint16_t)) {
                res = -EOVERFLOW;
            }
            else {
                cc2538_set_pan(*((const uint16_t *)value));
            }
            break;

        case NETOPT_PROMISCUOUSMODE:
            cc2538_set_monitor(((const bool *)value)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_STATE:
            if (value_len > sizeof(netopt_state_t)) {
                return -EOVERFLOW;
            }
            cc2538_set_state(dev, *((const netopt_state_t *)value));
            res = sizeof(netopt_state_t);
            break;

        case NETOPT_TX_POWER:
            if (value_len > sizeof(int16_t)) {
                return -EOVERFLOW;
            }
            cc2538_set_tx_power(*((const int16_t *)value));
            res = sizeof(uint16_t);
            break;

        default:
            break;
    }

    if (res == -ENOTSUP) {
        res = netdev_ieee802154_set((netdev_ieee802154_t *)netdev, opt,
                                    value, value_len);
    }

    return res;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    (void) netdev;

    int pkt_len = 0;

    /* Flush TX FIFO once no transmission in progress */
    RFCORE_WAIT_UNTIL(RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE == 0);
    RFCORE_SFR_RFST = ISFLUSHTX;

    /* The first byte of the TX FIFO must be the packet length,
       but we don't know what it is yet. Write a null byte to the
       start of the FIFO, so we can come back and update it later */
    rfcore_write_byte(0);

    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        pkt_len += iol->iol_len;

        if (pkt_len > CC2538_RF_MAX_DATA_LEN) {
            DEBUG("cc2538_rf: packet too large (%u > %u)\n",
                  pkt_len, CC2538_RF_MAX_DATA_LEN);
            return -EOVERFLOW;
        }

        rfcore_write_fifo(iol->iol_base, iol->iol_len);
    }

#ifdef MODULE_NETSTATS_L2
    netdev->stats.tx_bytes += pkt_len;
#endif

    /* Set first byte of TX FIFO to the packet length */
    rfcore_poke_tx_fifo(0, pkt_len + CC2538_AUTOCRC_LEN);

    RFCORE_SFR_RFST = ISTXON;

    /* Wait for transmission to complete */
    RFCORE_WAIT_UNTIL(RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE == 0);

    return pkt_len;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void) netdev;

    size_t pkt_len;

    if (buf == NULL) {
        /* GNRC wants to know how much data we've got for it */
        pkt_len = rfcore_read_byte();

        /* Make sure pkt_len is sane */
        if (pkt_len > CC2538_RF_MAX_DATA_LEN) {
            RFCORE_SFR_RFST = ISFLUSHRX;
            return -EOVERFLOW;
        }

        /* CRC check */
        if (!(rfcore_peek_rx_fifo(pkt_len) & 0x80)) {
            /* CRC failed; discard packet */
            RFCORE_SFR_RFST = ISFLUSHRX;
            return -ENODATA;
        }

        if (len > 0) {
            /* GNRC wants us to drop the packet */
            RFCORE_SFR_RFST = ISFLUSHRX;
        }

        return pkt_len - IEEE802154_FCS_LEN;
    }
    else {
        /* GNRC is expecting len bytes of data */
        pkt_len = len;
    }

#ifdef MODULE_NETSTATS_L2
    netdev->stats.rx_count++;
    netdev->stats.rx_bytes += pkt_len;
#endif
    rfcore_read_fifo(buf, pkt_len);

    if (info != NULL && RFCORE->XREG_RSSISTATbits.RSSI_VALID) {
        uint8_t corr_val;
        int8_t rssi_val;
        netdev_ieee802154_rx_info_t *radio_info = info;
        rssi_val = rfcore_read_byte() + CC2538_RSSI_OFFSET;

        RFCORE_ASSERT(rssi_val > CC2538_RF_SENSITIVITY);

        /* The number of dB above maximum sensitivity detected for the
         * received packet */
        radio_info->rssi = -CC2538_RF_SENSITIVITY + rssi_val;

        corr_val = rfcore_read_byte() & CC2538_CORR_VAL_MASK;

        if (corr_val < CC2538_CORR_VAL_MIN) {
            corr_val = CC2538_CORR_VAL_MIN;
        }
        else if (corr_val > CC2538_CORR_VAL_MAX) {
            corr_val = CC2538_CORR_VAL_MAX;
        }

        /* Interpolate the correlation value between 0 - 255
         * to provide an LQI value */
        radio_info->lqi = 255 * (corr_val - CC2538_CORR_VAL_MIN) /
                          (CC2538_CORR_VAL_MAX - CC2538_CORR_VAL_MIN);
    }

    RFCORE_SFR_RFST = ISFLUSHRX;

    return pkt_len;
}

static void _isr(netdev_t *netdev)
{
    netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
}

static int _init(netdev_t *netdev)
{
    cc2538_rf_t *dev = (cc2538_rf_t *) netdev;
    _dev = netdev;

    uint16_t pan = cc2538_get_pan();
    uint16_t chan = cc2538_get_chan();
    uint16_t addr_short = cc2538_get_addr_short();
    uint64_t addr_long = cc2538_get_addr_long();

    netdev_ieee802154_reset(&dev->netdev);

    /* Initialise netdev_ieee802154_t struct */
    netdev_ieee802154_set(&dev->netdev, NETOPT_NID,
                          &pan, sizeof(pan));
    netdev_ieee802154_set(&dev->netdev, NETOPT_CHANNEL,
                          &chan, sizeof(chan));
    netdev_ieee802154_set(&dev->netdev, NETOPT_ADDRESS,
                          &addr_short, sizeof(addr_short));
    netdev_ieee802154_set(&dev->netdev, NETOPT_ADDRESS_LONG,
                          &addr_long, sizeof(addr_long));

    cc2538_set_state(dev, NETOPT_STATE_IDLE);

#ifdef MODULE_NETSTATS_L2
    memset(&netdev->stats, 0, sizeof(netstats_t));
#endif

    return 0;
}

const netdev_driver_t cc2538_rf_driver = {
    .get  = _get,
    .set  = _set,
    .send = _send,
    .recv = _recv,
    .isr  = _isr,
    .init = _init,
};
