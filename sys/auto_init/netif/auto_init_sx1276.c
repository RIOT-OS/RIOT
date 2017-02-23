/*
 * Copyright (C) 2017 Fundación Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for sx1276 network interfaces
 *
 * @author  José Ignacio Alamos <jose.alamos@inria.cl>
 */

#ifdef MODULE_SX1276

#include "log.h"
#include "board.h"
#include "net/gnrc/netdev2.h"
#include "net/netdev2/lorawan.h"
#include "net/gnrc.h"
#include "net/gnrc/lorawan/lorawan.h"

#include "sx1276.h"
#include "sx1276_params.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define SX1276_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#ifndef SX1276_MAC_PRIO
#define SX1276_MAC_PRIO          (GNRC_NETDEV2_MAC_PRIO)
#endif

#define SX1276_NUM (sizeof(sx1276_params) / sizeof(sx1276_params[0]))

static sx1276_t sx1276_devs[SX1276_NUM];
static char _sx1276_stacks[SX1276_NUM][SX1276_MAC_STACKSIZE];

void auto_init_sx1276(void)
{
    for (unsigned i = 0; i < SX1276_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing sx1276 #%u\n", i);

        sx1276_setup(&sx1276_devs[i], &sx1276_params[i]);

        gnrc_lorawan_init(_sx1276_stacks[i],
                          SX1276_MAC_STACKSIZE,
                          SX1276_MAC_PRIO,
                          "sx1276",
                          (netdev2_lorawan_t*) &sx1276_devs[i]);
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_SX1276 */

/** @} */
