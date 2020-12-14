/*
 * Copyright (C) 2020 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @brief       Auto initialize sam0 ethernet driver
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifdef MODULE_SAM0_ETH

#include "net/gnrc/netif/ethernet.h"
#include "sam0_eth_netdev.h"

static netdev_t sam0eth;
static char stack[THREAD_STACKSIZE_DEFAULT];
static gnrc_netif_t _netif;

extern void sam0_eth_setup(netdev_t *netdev);

void auto_init_sam0_eth(void)
{
    /* setup netdev device */
    sam0_eth_setup(&sam0eth);
    /* initialize netdev <-> gnrc adapter state */
    gnrc_netif_ethernet_create(&_netif, stack, THREAD_STACKSIZE_DEFAULT,
                               GNRC_NETIF_PRIO, "sam0_eth", &sam0eth);
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_SAM0_ETH */
/** @} */
