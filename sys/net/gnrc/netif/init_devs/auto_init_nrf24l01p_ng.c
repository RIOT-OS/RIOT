 /*
 * Copyright (C) 2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for NRF24L01+ (NG) network interfaces
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifdef MODULE_NRF24L01P_NG

#include "nrf24l01p_ng.h"
#include "nrf24l01p_ng_params.h"
#include "gnrc_netif_nrf24l01p_ng.h"
#include "log.h"
#include "msg.h"
#include "net/gnrc/netif/conf.h"
#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifndef NRF24L01P_NG_EXTRA_STACKSIZE
/**
 * @brief   Additional stack size required by the driver
 *
 * With increasing of CONFIG_GNRC_NETIF_MSG_QUEUE_SIZE the required stack size
 * increases as well. A queue size of 8 messages works with default stack size,
 * so we increase the stack by `sizeof(msg_t)` for each additional element
 */
#define NRF24L01P_NG_EXTRA_STACKSIZE  ((GNRC_NETIF_MSG_QUEUE_SIZE - 8) \
                                      * sizeof(msg_t))
#endif

/**
 * @brief   Calculate the stack size for the MAC layer thread(s)
 */
#define NRF24L01P_NG_MAC_STACKSIZE          (THREAD_STACKSIZE_DEFAULT + \
                                            NRF24L01P_NG_EXTRA_STACKSIZE + \
                                            DEBUG_EXTRA_STACKSIZE)
#ifndef NRF24L01P_NG_MAC_PRIO
/**
 * @brief   The priority of the MAC layer thread
 */
#define NRF24L01P_NG_MAC_PRIO               (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Statically allocate memory for device descriptors
 */
nrf24l01p_ng_t _nrf24l01p_ng_devs[NRF24L01P_NG_NUM];

/**
 * @brief   Statically allocate memory for interfaces
 */
static gnrc_netif_t _netif[NRF24L01P_NG_NUM];

/**
 * @brief   Statically allocate memory for the MAC layer thread(s)
 */
static char stacks[NRF24L01P_NG_NUM][NRF24L01P_NG_MAC_STACKSIZE];

void auto_init_nrf24l01p_ng(void)
{
    for (unsigned i = 0; i < NRF24L01P_NG_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing nrf24l01p #%u\n", i);

        nrf24l01p_ng_setup(&_nrf24l01p_ng_devs[i], &nrf24l01p_ng_params[i], i);
        gnrc_netif_nrf24l01p_ng_create(&_netif[i], stacks[i],
                                       NRF24L01P_NG_MAC_STACKSIZE,
                                       NRF24L01P_NG_MAC_PRIO,
                                       "nrf24l01p_ng",
                                       (netdev_t *)&_nrf24l01p_ng_devs[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_NRF24L01P_NG */
/** @} */
