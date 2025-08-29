/*
 * Copyright (C) 2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include <errno.h>

#include "net/gnrc/netapi/notify.h"
#include "net/ipv6/addr.h"

uint8_t gnrc_netapi_notify_copy_l2_connection_data(gnrc_netapi_notify_t *notify,
                                                   netapi_notify_l2_connection_t *data)
{
    int data_len;

    switch (notify->event) {
    case NETAPI_NOTIFY_L2_CONNECTED:
    case NETAPI_NOTIFY_L2_DISCONNECTED:
        assert(notify->_data_len == sizeof(netapi_notify_l2_connection_t));

        /* Parse event data.*/
        netapi_notify_l2_connection_t *recv_data = notify->_data;

        /* l2addr must fit in the provided buffer */
        if (data->l2addr_len < recv_data->l2addr_len) {
            data_len = -EINVAL;
            break;
        }

        memcpy(data->l2addr, recv_data->l2addr, recv_data->l2addr_len);
        data->l2addr_len = recv_data->l2addr_len;
        data->if_pid = recv_data->if_pid;

        data_len = sizeof(netapi_notify_l2_connection_t);
        break;
    default:
        data_len = -EINVAL;
        break;
    }

    /* Acknowledge the read data */
    gnrc_netapi_notify_ack(notify->ack);

    return data_len;
}

int gnrc_netapi_notify_copy_l3_address(gnrc_netapi_notify_t *notify, ipv6_addr_t *addr)
{
    int data_len;

    *addr = ipv6_addr_unspecified;

    switch (notify->event) {
    case NETAPI_NOTIFY_L3_DISCOVERED:
    case NETAPI_NOTIFY_L3_UNREACHABLE:
        assert(notify->_data_len == sizeof(ipv6_addr_t));
        memcpy(addr, notify->_data, sizeof(ipv6_addr_t));

        data_len = sizeof(ipv6_addr_t);
        break;
    default:
        data_len = -EINVAL;
        break;
    }

    /* Acknowledge the read data */
    gnrc_netapi_notify_ack(notify->ack);

    return data_len;
}

/** @} */
