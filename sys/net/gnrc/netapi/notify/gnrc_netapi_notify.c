/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 * @ingroup     net_gnrc_netapi_notify
 * @file
 * @brief       Helper functions to extract data from netapi notify events.
 *
 * @author      Elena Frank <elena.frank@tu-dresden.de>
 */

#include <assert.h>
#include <errno.h>

#include "net/gnrc/netapi/notify.h"
#include "net/ipv6/addr.h"

int _copy_l2_connection_data(gnrc_netapi_notify_t *notify, netapi_notify_l2_connection_t *data)
{
    assert(notify->_data_len == sizeof(netapi_notify_l2_connection_t));

    /* Parse event data */
    netapi_notify_l2_connection_t *recv_data = notify->_data;

    if (recv_data->l2addr_len > GNRC_NETIF_L2ADDR_MAXLEN) {
        return -EINVAL;
    }
    memcpy(data->l2addr, recv_data->l2addr, recv_data->l2addr_len);

    data->l2addr_len = recv_data->l2addr_len;
    data->if_pid = recv_data->if_pid;

    return sizeof(netapi_notify_l2_connection_t);
}

int _copy_l3_address(gnrc_netapi_notify_t *notify, ipv6_addr_t *addr)
{
    assert(notify->_data_len == sizeof(ipv6_addr_t));

    memcpy(addr, notify->_data, sizeof(ipv6_addr_t));
    return sizeof(ipv6_addr_t);
}

int gnrc_netapi_notify_copy_event_data(gnrc_netapi_notify_t *notify, uint8_t data_len, void *data)
{
    int res;

    switch (notify->event) {
    case NETAPI_NOTIFY_L2_NEIGH_CONNECTED:
    case NETAPI_NOTIFY_L2_NEIGH_DISCONNECTED:
        if (data_len != sizeof(netapi_notify_l2_connection_t)) {
            res = -EINVAL;
            break;
        }
        res = _copy_l2_connection_data(notify, data);
        break;
    case NETAPI_NOTIFY_L3_DISCOVERED:
    case NETAPI_NOTIFY_L3_UNREACHABLE:
        if (data_len != sizeof(ipv6_addr_t)) {
            res = -EINVAL;
            break;
        }
        res = _copy_l3_address(notify, data);
        break;
    default:
        res = -EINVAL;
        break;
    }

    /* Acknowledge the read data */
    gnrc_netapi_notify_ack(&notify->ack);

    return res;
}

/** @} */
