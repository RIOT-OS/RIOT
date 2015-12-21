/*
 * Copyright (C) 2015 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file        cc2538-rf-netdev.c
 * @brief       Netdev radio driver for the CC2538
 *
 * @author      Ian Martin <ian@locicontrols.com>
 * @}
 */

#include "cc2538-rf.h"

#include "byteorder.h"
#include "thread.h"

netdev_rcv_data_cb_t netdev_rcv_data_cb = NULL;
static netdev_802154_raw_packet_cb_t raw_packet_cb = NULL;
static netdev_state_t netdev_state = NETDEV_STATE_POWER_IDLE;

/* 802.15.4 netdev driver */

static int init(netdev_t *dev)
{
    if (dev != &cc2538_netdev) {
        return -ENODEV;
    }

    cc2538_initialize();
    return 0;
}

static int add_receive_data_callback(netdev_t *dev, netdev_rcv_data_cb_t cb)
{
    if (dev != &cc2538_netdev) {
        return -ENODEV;
    }

    if (netdev_rcv_data_cb == NULL) {
        netdev_rcv_data_cb = cb;
        return 0;
    }
    else {
        return -ENOBUFS;
    }
}

static int rem_receive_data_callback(netdev_t *dev, netdev_rcv_data_cb_t cb)
{
    if (dev != &cc2538_netdev) {
        return -ENODEV;
    }

    if (netdev_rcv_data_cb == cb) {
        netdev_rcv_data_cb = NULL;
        return 0;
    }
    else {
        return -EINVAL;
    }
}

#define CASE_HELPER(name, type, expr)                                      \
    case name:                                                             \
        if (*value_len < sizeof(type))      {return -EOVERFLOW;}           \
        else if (*value_len > sizeof(type)) {*value_len = sizeof(type);}   \
        *(type *)value = (expr);                                           \
        return 0

static int get_option(netdev_t *dev, netdev_opt_t opt, void *value, size_t *value_len)
{
    if (dev != &cc2538_netdev) {
        return -ENODEV;
    }

    switch (opt) {
        CASE_HELPER(NETDEV_OPT_CHANNEL,         uint8_t,  cc2538_get_channel());
        CASE_HELPER(NETDEV_OPT_ADDRESS,         uint16_t, cc2538_get_address());
        CASE_HELPER(NETDEV_OPT_NID,             uint16_t, cc2538_get_pan());
        CASE_HELPER(NETDEV_OPT_ADDRESS_LONG,    uint64_t, cc2538_get_address_long());
        CASE_HELPER(NETDEV_OPT_TX_POWER,        int,      cc2538_get_tx_power());
        CASE_HELPER(NETDEV_OPT_MAX_PACKET_SIZE, uint8_t,  CC2538_RF_MAX_PACKET_LEN);

        default:
            return -ENOTSUP;
    }
}

static int set_option(netdev_t *dev, netdev_opt_t opt, void *value, size_t value_len)
{
    if (dev != &cc2538_netdev) {
        return -ENODEV;
    }

    switch (opt) {
        case NETDEV_OPT_CHANNEL:
            if (value_len != sizeof(uint8_t)) {
                return -EINVAL;
            }

            cc2538_set_channel(*(uint8_t *)value);
            return 0;

        case NETDEV_OPT_ADDRESS:
            if (value_len != sizeof(uint16_t)) {
                return -EINVAL;
            }

            cc2538_set_address(*(uint16_t *)value);
            return 0;

        case NETDEV_OPT_NID:
            if (value_len != sizeof(uint16_t)) {
                return -EINVAL;
            }

            cc2538_set_pan(*(uint16_t *)value);
            return 0;

        case NETDEV_OPT_ADDRESS_LONG:
            if (value_len != sizeof(uint64_t)) {
                return -EINVAL;
            }

            cc2538_set_address_long(*(uint64_t *)value);
            return 0;

        case NETDEV_OPT_TX_POWER:
            if (value_len != sizeof(int)) {
                return -EINVAL;
            }

            cc2538_set_tx_power(*(int *)value);
            return 0;

        default:
            return -ENOTSUP;
    }
}

static int get_state(netdev_t *dev, netdev_state_t *state)
{
    if (dev != &cc2538_netdev) {
        return -ENODEV;
    }

    *state = netdev_state;
    return 0;
}

static int set_state(netdev_t *dev, netdev_state_t state)
{
    if (dev != &cc2538_netdev) {
        return -ENODEV;
    }

    switch (state) {
        case NETDEV_STATE_POWER_OFF:
        case NETDEV_STATE_POWER_SLEEP:
        case NETDEV_STATE_POWER_IDLE:
            cc2538_off();
            break;

        case NETDEV_STATE_RX_MODE:
            cc2538_switch_to_rx();
            break;

        case NETDEV_STATE_PROMISCUOUS_MODE:
            cc2538_set_monitor(true);
            cc2538_switch_to_rx();
            break;

        default:
        case NETDEV_STATE_TX_BURST:
            return -ENOTSUP;
    }

    netdev_state = state;
    return 0;
}

static void event(netdev_t *dev, uint32_t event_type)
{
    (void)dev;
    (void)event_type;
}

static netdev_802154_tx_status_t load_tx(
    netdev_t *dev,
    netdev_802154_pkt_kind_t kind,
    netdev_802154_node_addr_t *dest,
    int use_long_addr,
    int wants_ack,
    netdev_hlist_t *upper_layer_hdrs,
    void *buf,
    unsigned int len)
{
    mac_header_t hdr;
    uint_fast8_t hdr_size;
    static uint_fast8_t sequence_nr = 0;

    if (dev != &cc2538_netdev) {
        return NETDEV_802154_TX_STATUS_NO_DEV;
    }

    /* FCS : frame version 0, we don't manage security,
       nor batches of packets */
    switch (kind) {
        case PACKET_KIND_BEACON:
        case PACKET_KIND_DATA:
        case PACKET_KIND_ACK:
            hdr.type = kind;
            break;

        default:
            return NETDEV_802154_TX_STATUS_INVALID_PARAM;
    }

    if (wants_ack) {
        hdr.type |= WANTS_ACK;
    }

    if (use_long_addr) {
        hdr.addr_type = ADDR_TYPE_LONG;
        hdr.long_addrs.dest_addr = HTONLL(dest->long_addr);
        hdr.long_addrs.src_addr  = HTONLL(cc2538_get_address_long());
        hdr_size = 3 + sizeof(long_addrs_t);
    }
    else {
        hdr.addr_type = ADDR_TYPE_SHORT;
        uint_fast16_t src_pan = cc2538_get_pan();

        if (dest->pan.id == src_pan) {
            hdr.type |= INTRA_PAN_COMPRESSION;
            hdr.short_addrs_compr.dest_pan  = HTONS(dest->pan.id);
            hdr.short_addrs_compr.dest_addr = HTONS(dest->pan.addr);
            hdr.short_addrs_compr.src_addr  = HTONS(cc2538_get_address());
            hdr_size = 3 + sizeof(short_addrs_compr_t);
        }
        else {
            hdr.short_addrs.dest_pan  = HTONS(dest->pan.id);
            hdr.short_addrs.dest_addr = HTONS(dest->pan.addr);
            hdr.short_addrs.src_pan   = HTONS(src_pan);
            hdr.short_addrs.src_addr  = HTONS(cc2538_get_address());
            hdr_size = 3 + sizeof(short_addrs_t);
        }
    }

    /* total frame size */
    if (CC2538_PACKET_LENGTH_SIZE + hdr_size + len > CC2538_RF_FIFO_SIZE) {
        return NETDEV_802154_TX_STATUS_PACKET_TOO_LONG;
    }

    if (RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE) {
        return NETDEV_802154_TX_STATUS_MEDIUM_BUSY;
    }

    /* sequence number */
    hdr.sequence_nr = sequence_nr++;

    RFCORE_ASSERT(cc2538_is_on());

    rfcore_strobe(ISFLUSHTX);

    /* Send the phy length byte first */
    rfcore_write_byte(hdr_size + len + CC2538_AUTOCRC_LEN);
    rfcore_write_fifo(&hdr, hdr_size);
    rfcore_write_fifo(buf, len);
    rfcore_tx_frame_hook(buf, len);

    /* TODO: Wait for ACK if wants_ack? */

    return NETDEV_802154_TX_STATUS_OK;
}

static netdev_802154_tx_status_t transmit(netdev_t *dev)
{
    bool was_off = false;

    if (dev != &cc2538_netdev) {
        return NETDEV_802154_TX_STATUS_NO_DEV;
    }

    if (!cc2538_is_on()) {
        was_off = true;
        cc2538_on();
        RFCORE_WAIT_UNTIL(RFCORE->XREG_FSMSTAT0bits.FSM_FFCTRL_STATE > FSM_STATE_RX_CALIBRATION);
    }

    if (!cc2538_channel_clear()) {
        return NETDEV_802154_TX_STATUS_COLLISION;
    }

    if (RFCORE->XREG_FSMSTAT1bits.SFD) {
        return NETDEV_802154_TX_STATUS_MEDIUM_BUSY;
    }

    /* Start the transmission */
    rfcore_strobe(ISTXON);

    if (was_off) {
        /* Wait for transmission to start */
        RFCORE_WAIT_UNTIL(RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE);

        cc2538_off();
    }

    return NETDEV_802154_TX_STATUS_OK;
}

static int add_receive_raw_callback(netdev_t *dev, netdev_802154_raw_packet_cb_t recv_func)
{
    if (dev != &cc2538_netdev) {
        return -ENODEV;
    }

    if (raw_packet_cb == NULL) {
        raw_packet_cb = recv_func;
        return 0;
    }
    else {
        return -ENOBUFS;
    }
}

static int rem_receive_raw_callback(netdev_t *dev, netdev_802154_raw_packet_cb_t recv_func)
{
    if (dev != &cc2538_netdev) {
        return -ENODEV;
    }

    if (raw_packet_cb == recv_func) {
        raw_packet_cb = NULL;
        return 0;
    }
    else {
        return -EINVAL;
    }
}

static int channel_is_clear(netdev_t *dev)
{
    if (dev != &cc2538_netdev) {
        return -ENODEV;
    }

    return cc2538_channel_clear() ? 1 : 0;
}

const netdev_802154_driver_t cc2538_driver = {
    .init                      = init,
    .send_data                 = netdev_802154_send_data,
    .add_receive_data_callback = add_receive_data_callback,
    .rem_receive_data_callback = rem_receive_data_callback,
    .get_option                = get_option,
    .set_option                = set_option,
    .get_state                 = get_state,
    .set_state                 = set_state,
    .event                     = event,
    .load_tx                   = load_tx,
    .transmit                  = transmit,
    .send                      = netdev_802154_send,
    .add_receive_raw_callback  = add_receive_raw_callback,
    .rem_receive_raw_callback  = rem_receive_raw_callback,
    .channel_is_clear          = channel_is_clear,
};

const netdev_t cc2538_netdev = { NETDEV_TYPE_802154, (netdev_driver_t *) &cc2538_driver, NULL };
