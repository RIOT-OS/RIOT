/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netif2
 * @{
 *
 * @file
 * @brief   CC110x adaption for @ref net_gnrc_netif2
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef GNRC_NETIF2_CC110X_H
#define GNRC_NETIF2_CC110X_H

#include "net/gnrc/netif2.h"

#ifdef __cplusplus
extern "C" {
#endif

gnrc_netif2_t *gnrc_netif2_cc110x_create(char *stack, int stacksize, char priority,
                                         char *name, netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NETIF2_CC110X_H */
/** @} */
