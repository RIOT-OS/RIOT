/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for at86rf215 network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_AT86RF215

#ifndef AT86RF215_DISABLE_SUBGHZ
#define USE_DUALBAND (GNRC_NETIF_NUMOF >= 2)
#endif

#include "log.h"
#include "board.h"
#include "net/gnrc/netif/ieee802154.h"
#ifdef MODULE_GNRC_LWMAC
#include "net/gnrc/lwmac/lwmac.h"
#endif
#ifdef MODULE_GNRC_GOMACH
#include "net/gnrc/gomach/gomach.h"
#endif
#include "net/gnrc.h"

#include "at86rf215.h"
#include "at86rf215_params.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define AT86RF215_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#ifndef AT86RF215_MAC_PRIO
#define AT86RF215_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif
#ifndef AT86RF215_MAC_PRIO_SUBGHZ
#define AT86RF215_MAC_PRIO_SUBGHZ   (AT86RF215_MAC_PRIO)
#endif

#if USE_DUALBAND
#define AT86RF215_NUM (2 * ARRAY_SIZE(at86rf215_params))
#else
#define AT86RF215_NUM ARRAY_SIZE(at86rf215_params)
#endif

static at86rf215_t at86rf215_devs[AT86RF215_NUM];
static char _at86rf215_stacks[AT86RF215_NUM][AT86RF215_MAC_STACKSIZE];

static void _setup_netif(void* netdev, void* stack, int prio) {
#if defined(MODULE_GNRC_GOMACH)
        gnrc_netif_gomach_create(stack,
                                 AT86RF215_MAC_STACKSIZE,
                                 prio, "at86rf215-gomach",
                                 netdev);
#elif defined(MODULE_GNRC_LWMAC)
        gnrc_netif_lwmac_create(stack,
                                 AT86RF215_MAC_STACKSIZE,
                                 prio, "at86rf215-lwmac",
                                 netdev);
#else
        gnrc_netif_ieee802154_create(stack,
                                 AT86RF215_MAC_STACKSIZE,
                                 prio, "at86rf215",
                                 netdev);
#endif
}


void auto_init_at86rf215(void)
{
#if USE_DUALBAND
    for (unsigned i = 0; i < AT86RF215_NUM; i+=2) {

        at86rf215_setup(&at86rf215_devs[i], &at86rf215_devs[i+1], &at86rf215_params[i/2]);

        /* setup sub-GHz interface */
        _setup_netif(&at86rf215_devs[i], &_at86rf215_stacks[i], AT86RF215_MAC_PRIO_SUBGHZ);

        /* setup 2.4-GHz interface */
        _setup_netif(&at86rf215_devs[i+1], &_at86rf215_stacks[i+1], AT86RF215_MAC_PRIO);
    }
#else
    for (unsigned i = 0; i < AT86RF215_NUM; i++) {
        /* AT86RF215M only has sub-GHz interface */
#ifdef AT86RF215_DISABLE_SUBGHZ
        at86rf215_setup(NULL, &at86rf215_devs[i], &at86rf215_params[i]);
#else
        at86rf215_setup(&at86rf215_devs[i], NULL, &at86rf215_params[i]);
#endif
        /* setup sub-GHz interface */
        _setup_netif(&at86rf215_devs[i], &_at86rf215_stacks[i], AT86RF215_MAC_PRIO);
    }
#endif
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_AT86RF215 */

/** @} */
