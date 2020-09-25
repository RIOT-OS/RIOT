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
#include <stdatomic.h>

#include "net/gnrc.h"
#include "net/netdev.h"

#include "cpu.h"
#include "cc2538_rf.h"
#include "cc2538_rf_netdev.h"
#include "cc2538_rf_internal.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/* Reference pointer for the IRQ handler */
static cc2538_rf_t *_dev;

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    cc2538_rf_t *dev = (cc2538_rf_t *)netdev;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_ADDRESS:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            else {
                cc2538_get_addr_short(value);
            }
            return sizeof(uint16_t);

        case NETOPT_ADDRESS_LONG:
            if (max_len < sizeof(uint64_t)) {
                return -EOVERFLOW;
            }
            else {
                cc2538_get_addr_long(value);
            }
            return sizeof(uint64_t);

        case NETOPT_AUTOACK:
            if (RFCORE->XREG_FRMCTRL0bits.AUTOACK) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_CHANNEL:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)value) = (uint16_t)cc2538_get_chan();
            return sizeof(uint16_t);

        case NETOPT_CHANNEL_PAGE:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            /* This transceiver only supports page 0 */
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

        case NETOPT_PRELOADING:
            if (dev->flags & CC2538_OPT_PRELOADING) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

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
                cc2538_set_addr_short(value);
                res = sizeof(uint16_t);
            }
            break;

        case NETOPT_ADDRESS_LONG:
            if (value_len > sizeof(uint64_t)) {
                res = -EOVERFLOW;
            }
            else {
                cc2538_set_addr_long(value);
                res = sizeof(uint64_t);
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
                    res = sizeof(uint16_t);
                }
            }
            break;

        case NETOPT_CHANNEL_PAGE:
            /* This transceiver only supports page 0 */
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

        case NETOPT_PRELOADING:
            dev->flags = (((const bool *)value)[0]) ? (dev->flags | CC2538_OPT_PRELOADING)
                         : (dev->flags & ~CC2538_OPT_PRELOADING);
            res = sizeof(netopt_enable_t);
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
    cc2538_rf_t *dev = (cc2538_rf_t *) netdev;

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

    /* Set first byte of TX FIFO to the packet length */
    rfcore_poke_tx_fifo(0, pkt_len + CC2538_AUTOCRC_LEN);

    if (!(dev->flags & CC2538_OPT_PRELOADING)) {
        cc2538_tx_now(dev);
    }

    return pkt_len;
}

static void _recv_complete(netdev_t *netdev)
{
    (void) netdev;
    /* flush the RX fifo*/
    RFCORE_SFR_RFST = ISFLUSHRX;
    /* go back to receiving */
    RFCORE_SFR_RFST = ISRXON;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void) netdev;

    size_t pkt_len;

    if (buf == NULL) {
        /* Check that the last byte of a new frame has been received */
        if (RFCORE->XREG_FSMSTAT1bits.FIFOP == 0) {
            DEBUG_PRINT("cc2538_rf: Frame has not finished being received\n");
            return -EAGAIN;
        }

        /* GNRC wants to know how much data we've got for it */
        pkt_len = rfcore_read_byte();

        /* Make sure pkt_len is sane */
        if (pkt_len > CC2538_RF_MAX_DATA_LEN) {
            DEBUG_PRINT("cc2538_rf: pkt_len > CC2538_RF_MAX_DATA_LEN\n");
            _recv_complete(netdev);
            return -EOVERFLOW;
        }

        /* Make sure pkt_len is not too short.
         * There are at least 2 bytes (FCS). */
        if (pkt_len < IEEE802154_FCS_LEN) {
            DEBUG_PRINT("cc2538_rf: pkt_len < IEEE802154_FCS_LEN\n");
            _recv_complete(netdev);
            return -ENODATA;
        }

        if (len > 0) {
            /* GNRC wants us to drop the packet */
            _recv_complete(netdev);
        }

        return pkt_len - IEEE802154_FCS_LEN;
    }
    else {
        /* GNRC is expecting len bytes of data */
        pkt_len = len;
    }

    rfcore_read_fifo(buf, pkt_len);

    int8_t rssi_val = rfcore_read_byte() + CC2538_RSSI_OFFSET;
    uint8_t crc_corr_val = rfcore_read_byte();

    if (info != NULL) {
        netdev_ieee802154_rx_info_t *radio_info = info;

        /* The number of dB above maximum sensitivity detected for the
         * received packet */
        radio_info->rssi = rssi_val;

        uint8_t corr_val = crc_corr_val & CC2538_CORR_VAL_MASK;

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

    /* always flush on completetion so we always know where the incoming
       bytes are located, e.g.: when probing for the crc */
    _recv_complete(netdev);

    return pkt_len;
}

void cc2538_irq_handler(void)
{
    /* read an clear the RF interrupt flags*/
    uint8_t irq_mask_0 = (uint8_t) RFCORE_SFR_RFIRQF0;
    uint8_t irq_mask_1 = (uint8_t) RFCORE_SFR_RFIRQF1;
    RFCORE_SFR_RFIRQF0 = 0;
    RFCORE_SFR_RFIRQF1 = 0;

    DEBUG("[cc2538_rf_isr] RFIRQF0 %02x\n", irq_mask_0 );
    DEBUG("[cc2538_rf_isr] RFIRQF1 %02x\n", irq_mask_1 );

    uint16_t new_flags = 0x0000;
    uint16_t current_flags = atomic_load(&_dev->isr_flags);

    /* SFD ISR is triggered when an SFD has been received or transmitted */
    if (irq_mask_0 & SFD) {
        if (RFCORE->XREG_FSMSTAT1bits.RX_ACTIVE) {
            new_flags |= CC2538_RF_SFD_RX;
        }
    }

    if (irq_mask_0 & (RXPKTDONE | FIFOP)) {
        /* Disable future RX while the frame has not been processed */
        RFCORE_XREG_RXMASKCLR = 0xFF;
        new_flags |= CC2538_RF_RXPKTDONE;
    }

    if (irq_mask_1 & TXDONE) {
        new_flags |= CC2538_RF_TXDONE;
    }

    /* is ISR are serviced to slow then an event might be missed */
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if (new_flags & current_flags) {
            printf("[cc2538_rf]: ERROR lost IRQ events isr_flags: %02x\n",
                  new_flags & current_flags);
        }
    }

    atomic_store(&_dev->isr_flags, new_flags | current_flags);

    netdev_trigger_event_isr((netdev_t *) &_dev->netdev);
}


void cc2538_tx_now(cc2538_rf_t *dev)
{
    netdev_t *netdev = (netdev_t *)dev;
    (void) netdev;
    RFCORE_SFR_RFST = ISTXON;

    /* Because the ISR are offloaded this seems to yield a more accurate
       value for when the TX SFD is sent out than relying on the SFD ISR
     */
    while (RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE) {
        if (RFCORE->XREG_FSMSTAT1bits.SFD) {
            DEBUG_PUTS("[cc2538_rf] EVT - TX_STARTED");
            netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
            break;
        } else {
            thread_yield();
        }
    }

    /* Wait for transmission to complete */
    RFCORE_WAIT_UNTIL(RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE == 0);

    dev->state = NETOPT_STATE_IDLE;
}

static void _isr(netdev_t *netdev)
{
    unsigned state = irq_disable();
    uint16_t flags = atomic_load(&_dev->isr_flags);
    atomic_store(&_dev->isr_flags, 0x0000);
    irq_restore(state);

    if (flags & CC2538_RF_TXDONE) {
        DEBUG_PUTS("[cc2538_rf] EVT - TX_COMPLETE");
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
    if (flags & CC2538_RF_SFD_RX) {
        DEBUG_PUTS("[cc2538_rf] EVT - RX_STARTED");
        netdev->event_callback(netdev, NETDEV_EVENT_RX_STARTED);
    }
    if (flags & CC2538_RF_RXPKTDONE) {
        /* CRC check */
        uint8_t pkt_len = rfcore_peek_rx_fifo(0);
        if (rfcore_peek_rx_fifo(pkt_len) & CC2538_CRC_BIT_MASK) {
            DEBUG_PUTS("[cc2538_rf] EVT - RX_COMPLETE");
            netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
        }
        else {
            /* CRC failed, discard packet */
            DEBUG_PUTS("[cc2538_rf] EVT - CRC_ERROR\n");
            netdev->event_callback(netdev, NETDEV_EVENT_CRC_ERROR);
            _recv_complete(netdev);
        }
    }
}

static int _init(netdev_t *netdev)
{
    cc2538_rf_t *dev = (cc2538_rf_t *) netdev;
    _dev = dev;

    uint16_t chan = cc2538_get_chan();

    netdev_ieee802154_reset(&dev->netdev);

    /* Initialise netdev_ieee802154_t struct */
    netdev_ieee802154_set(&dev->netdev, NETOPT_CHANNEL,
                          &chan, sizeof(chan));

    cc2538_set_state(dev, NETOPT_STATE_IDLE);

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
