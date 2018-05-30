/*
 * Copyright (C) Fundación Inria Chile 2017
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_netdev_ppp
 * @{
 *
 * @file
 * @brief       Common code for netdev ppp drivers
 *
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 *
 * @}
 */
#include <errno.h>

#include "net/netdev/ppp.h"

int netdev_ppp_get(netdev_ppp_t *dev, netopt_t opt, void *value, size_t max_len)
{
    int res = -ENOTSUP;

    (void)dev;
    (void)opt;
    (void)value;
    (void)max_len;

    return res;
}

int netdev_ppp_set(netdev_ppp_t *dev, netopt_t opt, const void *value, size_t value_len)
{
    int res = -ENOTSUP;

    (void)dev;
    (void)opt;
    (void)value;
    (void)value_len;

    return res;
}
