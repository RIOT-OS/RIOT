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
#include "sched.h"
#include "thread.h"
#include <sys/errno.h>

uint8_t gnrc_netapi_notify_get_l2_connection_data(kernel_pid_t sender_pid,
                                                  gnrc_netapi_notify_t *notify,
                                                  uint8_t *l2addr, uint8_t l2addr_len,
                                                  kernel_pid_t *if_pid)
{
    int data_len;

    switch (notify->event) {
    case NETAPI_NOTIFY_L2_CONNECTED:
    case NETAPI_NOTIFY_L2_DISCONNECTED:
        assert(notify->_data_len == sizeof(netapi_notify_l2_connection_t));

        /* Parse event data.*/
        netapi_notify_l2_connection_t *data = notify->_data;

        /* l2addr must fit in the provided buffer */
        if (l2addr_len < data->l2addr_len) {
            data_len = -EINVAL;
            break;
        }

        memcpy(l2addr, data->l2addr, data->l2addr_len);
        *if_pid = data->if_pid;

        data_len = data->l2addr_len;
        break;
    default:
        data_len = -EINVAL;
        break;
    }

    /* Unblock the sending thread now that all data was copied over. */
    thread_flags_set(thread_get(sender_pid), NETAPI_NOTIFY_FLAG_ACK);

    return data_len;
}

int gnrc_netapi_notify_get_l3_address(kernel_pid_t sender_pid, gnrc_netapi_notify_t *notify,
                                      ipv6_addr_t *addr)
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

    /* Unblock the sending thread now that all data was copied over. */
    thread_flags_set(thread_get(sender_pid), NETAPI_NOTIFY_FLAG_ACK);

    return data_len;
}

/** @} */
