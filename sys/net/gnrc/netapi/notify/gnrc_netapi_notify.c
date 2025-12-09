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

#include <errno.h>

#include "net/gnrc/netapi/notify.h"

int gnrc_netapi_notify_copy_event_data(gnrc_netapi_notify_t *notify, uint8_t data_len, void *data)
{
    (void) data_len;
    (void) data;

    int res;

    switch (notify->event) {
    default:
        res = -EINVAL;
        break;
    }

    /* Acknowledge the read data */
    gnrc_netapi_notify_ack(&notify->ack);

    return res;
}

/** @} */
