/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  netdev
 * @{
 *
 * @file
 * @brief       Provides helper functions to the API in @ref netdev/base.h.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include "clist.h"

#include "netdev/base.h"

size_t netdev_get_hlist_len(const netdev_hlist_t *hlist)
{
    netdev_hlist_t *ptr = (netdev_hlist_t *)hlist;
    size_t length = 0;

    if (ptr == NULL) {
        return 0;
    }

    do {
        length += ptr->header_len;
        netdev_hlist_advance(&ptr);
    } while (ptr != hlist);

    return length;
}

/**
 * @}
 */
