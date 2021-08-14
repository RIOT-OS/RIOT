/*
 * Copyright (C) Giuseppe Tipaldi
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_auto_init_lwip_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for enc28j60 network interfaces
 *
 * @author  Giuseppe Tipaldi <ing.giuseppe.tipaldi@gmail.com>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "enc28j60.h"
#include "enc28j60_params.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_ENC28J60_NUMOF        ARRAY_SIZE(enc28j60_params)

static struct netif netif[NETIF_ENC28J60_NUMOF];
static enc28j60_t enc28j60_devs[NETIF_ENC28J60_NUMOF];

void auto_init_enc28j60(void)
{
    for (unsigned i = 0; i < NETIF_ENC28J60_NUMOF; i++) {
        enc28j60_setup(&enc28j60_devs[i], &enc28j60_params[i], i);
        if (lwip_add_ethernet(&netif[i], &enc28j60_devs[i].netdev) == NULL) {
            DEBUG("Could not add enc28j60 device\n");
            return;
        }
    }
}
/** @} */
