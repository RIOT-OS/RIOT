/*
 * Copyright (C) 2016 MUTEX NZ Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     cpu_cc430
 * @{
 *
 * @file
 * @brief       Netdev adaption for the CC430 RF driver
 *
 * @author      Aaron Sowry <aaron@mutex.nz>
 *
 * @}
 */


#include "cc430_rf_netdev.h"


#define ENABLE_DEBUG        (0)
#include "debug.h"


/* Reference pointer for the IRQ handler */
static netdev_t *_dev;



void _irq_handler(void)
{
    if (_dev->event_callback) {
        _dev->event_callback(_dev, NETDEV_EVENT_ISR);
    }
}

static inline int _get_iid(netdev_t *netdev, eui64_t *value, size_t max_len)
{
    cc110x_t *cc110x = &((netdev_cc110x_t *) netdev)->cc110x;
    uint8_t *eui64 = (uint8_t *) value;

    if (max_len < sizeof(eui64_t)) {
        return -EOVERFLOW;
    }

    /* make address compatible to https://tools.ietf.org/html/rfc6282#section-3.2.2*/
    memset(eui64, 0, sizeof(eui64_t));
    eui64[3] = 0xff;
    eui64[4] = 0xfe;
    eui64[7] = cc110x->radio_address;

    return sizeof(eui64_t);
}

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    cc110x_t *cc110x = &((netdev_cc110x_t *)netdev)->cc110x;

    if (cc110x == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_DEVICE_TYPE:
            *((uint16_t *) value) = NETDEV_TYPE_CC110X;
            return 2;
#ifdef MODULE_GNRC_NETIF
        case NETOPT_PROTO:
            *((gnrc_nettype_t *)value) = cc110x->proto;
            return sizeof(gnrc_nettype_t);
#endif
        case NETOPT_CHANNEL:
            *((uint16_t *)value) = (uint16_t)cc110x->radio_channel;
            return 2;
        case NETOPT_ADDRESS:
            *((uint8_t *)value) = cc110x->radio_address;
            return 1;
        case NETOPT_MAX_PACKET_SIZE:
            *((uint8_t *)value) = cc430_get_max_packet_size();
            return 1;
        case NETOPT_IPV6_IID:
            return _get_iid(netdev, value, max_len);
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            *((uint16_t *)value) = sizeof(cc110x->radio_address);
            return sizeof(uint16_t);
        default:
            break;
    }

    return -ENOTSUP;
}

static int _set(netdev_t *dev, netopt_t opt, const void *value, size_t value_len)
{
    cc110x_t *cc110x = &((netdev_cc110x_t *) dev)->cc110x;

    switch (opt) {
        case NETOPT_CHANNEL:
        {
            const uint8_t *arg = value;
            uint8_t channel = arg[value_len - 1];
            #if CC430_MIN_CHANNR
            if (channel < CC430_MIN_CHANNR) {
                return -EINVAL;
            }
            #endif /* CC430_MIN_CHANNR */
            if (channel > CC430_MIN_CHANNR) {
                return -EINVAL;
            }
            if (cc430_set_channel(cc110x, channel) == -1) {
                return -EINVAL;
            }
            return 1;
        }
        case NETOPT_ADDRESS:
            if (value_len < 1) {
                return -EINVAL;
            }
            if (!cc430_set_address(cc110x, *(const uint8_t *)value)) {
                return -EINVAL;
            }
            return 1;
#ifdef MODULE_GNRC_NETIF
        case NETOPT_PROTO:
            if (value_len != sizeof(gnrc_nettype_t)) {
                return -EINVAL;
            }
            else {
                cc110x->proto = (gnrc_nettype_t) value;
                return sizeof(gnrc_nettype_t);
            }
            break;
#endif
        default:
            return -ENOTSUP;
    }

    return 0;
}

static int _send(netdev_t *dev, const iolist_t *iolist)
{
    netdev_cc110x_t *netdev_cc110x = (netdev_cc110x_t *)dev;
    cc110x_pkt_t *cc110x_pkt = iolist->iol_base;
    uint8_t state_was_RX = 0;

    uint16_t size;

    switch (netdev_cc110x->cc110x.radio_state) {
        case RADIO_RX_BUSY:
        case RADIO_TX_BUSY:
            return -EAGAIN;
    }
    netdev_cc110x->cc110x.radio_state = RADIO_TX_BUSY;
    /*
     * Number of bytes to send is:
     * length of phy payload (packet->length)
     * + size of length field (1 byte)
     */
    size = cc110x_pkt->length + 1;

    if (size > cc430_get_max_packet_size()) {
        return -ENOSPC;
    }

    /* set source address */
    cc110x_pkt->phy_src = netdev_cc110x->cc110x.radio_address;

    /* Put CC110x in IDLE mode to flush the FIFO */
    cc430_radio_strobe(RF_SIDLE);

    cc430_radio_delay_rf(RX_TO_IDLE_TIME);
    /* Flush TX FIFO to be sure it is empty */
    cc430_radio_strobe(RF_SFTX);
    /* Transmit the packet*/
    cc430_radio_transmit(cc110x_pkt, size);

    netdev_cc110x->cc110x.radio_state = RADIO_IDLE;

    return (int)size;
}

static int _recv(netdev_t *dev, void *buf, size_t len, void *info)
{

    cc110x_t *cc110x = &((netdev_cc110x_t *) dev)->cc110x;

    //(void) dev;
    (void) info;
    size_t pkt_len;
    uint8_t bytesRead;
    uint8_t bytesInRxFIFO;

    if (buf == NULL) {
        /* GNRC wants to know how much data we've got for it */
        cc430_radio_read_burst_reg(RF_RXFIFORD, &pkt_len, 1);

        /* Make sure pkt_len is sane */
        if (pkt_len > cc430_get_max_packet_size()) {
            cc430_radio_strobe(RF_SFRX);
            return -EOVERFLOW;
        }

        if (len > 0) {
            /* GNRC wants us to drop the packet */
            cc430_radio_strobe(RF_SFRX);
        }

        return pkt_len;
    }
    else {
        /* GNRC is expecting len bytes of data */
        pkt_len = len;
    }

    ((uint8_t *)buf)[0] = pkt_len;
    cc430_radio_read_burst_reg(RF_RXFIFORD, &(((uint8_t *)buf)[1]), pkt_len);

    /* Read the Received Signal Strength Indication (RSSI) */
    cc110x->pkt_buf.rssi = cc430_radio_read_single_reg(RSSI);

    /* Read the Link Quality Indication (LQI) */
    cc110x->pkt_buf.lqi = cc430_radio_read_single_reg(LQI);

    if (info != NULL) {
        netdev_cc110x_rx_info_t *cc110x_info = info;

        if (cc110x->pkt_buf.rssi >= 128) {
            cc110x_info->rssi = ((int16_t)cc110x->pkt_buf.rssi - 256) / 2 - CC110X_RSSI_OFFSET;
        }
        else {
            cc110x_info->rssi = (int16_t)cc110x->pkt_buf.rssi / 2 - CC110X_RSSI_OFFSET;
        }

        cc110x_info->lqi = cc110x->pkt_buf.lqi;
    }

    cc430_radio_strobe(RF_SFRX);

    return pkt_len;
}

static void _isr(netdev_t *dev)
{
    dev->event_callback(dev, NETDEV_EVENT_RX_COMPLETE);
}

static int _init(netdev_t *dev)
{
    cc110x_t *cc110x = &((netdev_cc110x_t *) dev)->cc110x;

    _dev = dev;

    /* Get the device address*/
    cc110x->radio_address = cc430_radio_read_single_reg(ADDR);

    /*Get the device channel*/
    cc110x->radio_channel = cc430_radio_read_single_reg(CHANNR);

    cc430_radio_receive_on();
    ((netdev_cc110x_t *)dev)->cc110x.radio_state = RADIO_RX;

    return 0;
}

const netdev_driver_t netdev_cc110x_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .get = _get,
    .set = _set,
    .isr = _isr
};
