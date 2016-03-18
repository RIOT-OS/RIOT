/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <string.h>

#include "net/netdev2/ieee802154.h"
#include "pcap_dev.h"

static int _get_max_packet_size(netdev2_t *netdev, netopt_t opt, void *value,
                                size_t max_len);

#ifdef MODULE_NETDEV2_IEEE802154
static const pcap_dev_opt_fn_t _getters[NETOPT_NUMOF] = {
    /* casting needed due to different netdev2 pointer types */
    [NETOPT_CHANNEL]         = (pcap_dev_opt_fn_t)netdev2_ieee802154_get,
    [NETOPT_ADDRESS]         = (pcap_dev_opt_fn_t)netdev2_ieee802154_get,
    [NETOPT_ADDRESS_LONG]    = (pcap_dev_opt_fn_t)netdev2_ieee802154_get,
    [NETOPT_ADDR_LEN]        = (pcap_dev_opt_fn_t)netdev2_ieee802154_get,
    [NETOPT_SRC_LEN]         = (pcap_dev_opt_fn_t)netdev2_ieee802154_get,
    [NETOPT_NID]             = (pcap_dev_opt_fn_t)netdev2_ieee802154_get,
    [NETOPT_IPV6_IID]        = (pcap_dev_opt_fn_t)netdev2_ieee802154_get,
    [NETOPT_MAX_PACKET_SIZE] = _get_max_packet_size,
    [NETOPT_PROTO]           = (pcap_dev_opt_fn_t)netdev2_ieee802154_get,
    [NETOPT_DEVICE_TYPE]     = (pcap_dev_opt_fn_t)netdev2_ieee802154_get,
};

static const pcap_dev_opt_fn_t _setters[NETOPT_NUMOF] = {
    [NETOPT_CHANNEL]         = (pcap_dev_opt_fn_t)netdev2_ieee802154_set,
    [NETOPT_ADDRESS]         = (pcap_dev_opt_fn_t)netdev2_ieee802154_set,
    [NETOPT_ADDRESS_LONG]    = (pcap_dev_opt_fn_t)netdev2_ieee802154_set,
    [NETOPT_ADDR_LEN]        = (pcap_dev_opt_fn_t)netdev2_ieee802154_set,
    [NETOPT_SRC_LEN]         = (pcap_dev_opt_fn_t)netdev2_ieee802154_set,
    [NETOPT_NID]             = (pcap_dev_opt_fn_t)netdev2_ieee802154_set,
    [NETOPT_PROTO]           = (pcap_dev_opt_fn_t)netdev2_ieee802154_set,
};

void pcap_dev_setup_ieee802154(pcap_dev_t *dev, const pcap_dev_sd_t *sd,
                               void *sd_state, uint32_t drain_delay,
                               uint8_t *drain_buf, uint16_t drain_buf_size)
{
    static const uint16_t chan = PCAP_DEV_DEFAULT_CHANNEL;
    static const uint16_t nid = PCAP_DEV_DEFAULT_NID;
    static const uint16_t addr_short = PCAP_DEV_DEFAULT_IEEE802154_ADDR_SHORT;
    static const uint8_t addr_long[] = PCAP_DEV_DEFAULT_IEEE802154_ADDR_LONG;

    pcap_dev_setup(dev, sd, sd_state, drain_delay, drain_buf, drain_buf_size,
                   PCAP_DEV_DEFAULT_IEEE802154_NET_MODE, _getters, _setters);
    netdev2_ieee802154_set((netdev2_ieee802154_t *)dev, NETOPT_CHANNEL,
                           (void *)&chan, sizeof(chan));
    netdev2_ieee802154_set((netdev2_ieee802154_t *)dev, NETOPT_NID,
                           (void *)&nid, sizeof(nid));
    netdev2_ieee802154_set((netdev2_ieee802154_t *)dev, NETOPT_ADDRESS,
                           (void *)&addr_short, sizeof(addr_short));
    netdev2_ieee802154_set((netdev2_ieee802154_t *)dev, NETOPT_ADDRESS_LONG,
                           (void *)&addr_long, sizeof(addr_long));
}

static int _get_max_packet_size(netdev2_t *netdev, netopt_t opt, void *value,
                                size_t max_len)
{
    netdev2_ieee802154_t *dev = (netdev2_ieee802154_t *)netdev;
    /* max. IEEE 802.15.4 frame size - 2 FCF - 2 Dst. PAN - 8 Dst. Addr. - 2 Src. Addr. */
    uint16_t max_packet_size = 115;

    (void)opt;
    if (max_len != sizeof(uint16_t)) {
        return -EOVERFLOW;
    }

    if (((dev->flags & IEEE802154_FCF_SRC_ADDR_MASK) == IEEE802154_FCF_SRC_ADDR_LONG)) {
        /* subtract 6 byte for 8 byte Src. Addr. */
        max_packet_size -= 6;
    }

    if (!(dev->flags & IEEE802154_FCF_PAN_COMP)) {
        /* subtract 2 byte for not compressed Src. PAN */
        max_packet_size -= 2;
    }
    memcpy(value, &max_packet_size, sizeof(uint16_t));

    return sizeof(uint16_t);
}

#else
typedef int dont_be_pedantic;
#endif

/** @} */
