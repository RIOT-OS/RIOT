/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization the nRF52840 radio in IEEE802.15.4 mode
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef MODULE_NRF802154

#include "log.h"
#include "board.h"
#include "net/gnrc/netdev.h"
#include "net/gnrc/netdev/ieee802154.h"
#include "net/gnrc.h"

#include "nrf802154.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define NRF802154_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#ifndef NRF802154_MAC_PRIO
#define NRF802154_MAC_PRIO          (GNRC_NETDEV_MAC_PRIO)
#endif

static gnrc_netdev_t gnrc_adpt;
static char stack[NRF802154_MAC_STACKSIZE];

void auto_init_nrf802154(void)
{
    LOG_DEBUG("[auto_init_netif] initializing nrf802154\n");

    int res = gnrc_netdev_ieee802154_init(&gnrc_adpt, &nrf802154_dev);
    if (res < 0) {
        LOG_ERROR("[auto_init_netif] error initializing nrf802154 radio\n");
    }
    else {
        gnrc_netdev_init(stack, NRF802154_MAC_STACKSIZE, NRF802154_MAC_PRIO,
                         "nrf802154", &gnrc_adpt);
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_NRF802154 */

/** @} */
