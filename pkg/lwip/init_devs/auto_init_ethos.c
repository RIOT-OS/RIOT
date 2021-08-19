/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief   Auto initialization for the ethernet-over-serial module
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "ethos.h"
#include "ethos_params.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_ETHOS_NUMOF        ARRAY_SIZE(ethos_params)

static struct netif netif[NETIF_ETHOS_NUMOF];
static ethos_t ethos_devs[NETIF_ETHOS_NUMOF];

static uint8_t _inbuf[NETIF_ETHOS_NUMOF][2048];

void auto_init_ethos(void)
{
    for (unsigned i = 0; i < NETIF_ETHOS_NUMOF; i++) {
        ethos_setup(&ethos_devs[i], &ethos_params[i], i, _inbuf[i], sizeof(_inbuf[i]));
        if (lwip_add_ethernet(&netif[i], &ethos_devs[i].netdev) == NULL) {
            DEBUG("Could not add ethos device\n");
            return;
        }
    }
}
/** @} */
