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
 * @brief       Auto initialization for nrf24l01p network interfaces
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifdef MODULE_NRF24L01P

#include "nrf24l01p.h"
#include "nrf24l01p_params.h"
#include "gnrc_netif_nrf24l01p.h"
#include "log.h"
#include "msg.h"
#include "net/gnrc/netif/conf.h"
#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifndef NRF24L01P_EXTRA_STACKSIZE
/**
 * @brief   Additional stack size required by the driver
 *
 * With increasing of CONFIG_GNRC_NETIF_MSG_QUEUE_SIZE the required stack size
 * increases as well. A queue size of 8 messages works with default stack size,
 * so we increase the stack by `sizeof(msg_t)` for each additional element
 */
#define NRF24L01P_EXTRA_STACKSIZE       ((CONFIG_GNRC_NETIF_MSG_QUEUE_SIZE - 8) * \
                                         sizeof(msg_t))
#endif

/**
 * @brief   Calculate the stack size for the MAC layer thread(s)
 */
#define NRF24L01P_MAC_STACKSIZE         (THREAD_STACKSIZE_DEFAULT + \
                                         NRF24L01P_EXTRA_STACKSIZE + \
                                         DEBUG_EXTRA_STACKSIZE)
#ifndef NRF24L01P_MAC_PRIO
/**
 * @brief   The priority of the MAC layer thread
 */
#define NRF24L01P_MAC_PRIO              (GNRC_NETIF_PRIO)
#endif


/**
 * @brief   Statically allocate memory for device descriptors
 */
nrf24l01p_t _nrf24l01p_devs[NRF24L01P_NUM];
/**
 * @brief   Statically allocate memory for the MAC layer thread(s)
 */
static char stacks[NRF24L01P_NUM][NRF24L01P_MAC_STACKSIZE];

void auto_init_nrf24l01p(void)
{
    for (unsigned i = 0; i < NRF24L01P_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing nrf24l01p #%u\n", i);

        nrf24l01p_setup(&_nrf24l01p_devs[i], &nrf24l01p_params[i]);
        gnrc_netif_nrf24l01p_create(stacks[i], NRF24L01P_MAC_STACKSIZE,
                                    NRF24L01P_MAC_PRIO,
                                    "nrf24l01p",
                                    (netdev_t *)&_nrf24l01p_devs[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_NRF24L01P */
/** @} */
