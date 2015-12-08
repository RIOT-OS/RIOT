/*
 * Copyright (C) 2015 Matt Poppe <matt@poppe.me>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */
/**
 * @ingroup auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for ksz8851snl ethernet devices
 *
 * @author  Matt Poppe <matt@poppe.me>
 */
#ifdef MODULE_KSZ8851SNL
#define ENABLE_DEBUG (0)
#include "debug.h"
#include "ksz8851snl.h"
#include "net/gnrc/gnrc_netdev2_eth.h"
static ksz8851snl_t ksz8851snl;
/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define MAC_STACKSIZE           (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#define MAC_PRIO                (THREAD_PRIORITY_MAIN - 4)
/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev2_eth_stack[MAC_STACKSIZE + DEBUG_EXTRA_STACKSIZE];
static gnrc_netdev2_t _gnrc_ksz8851snl;
void auto_init_ksz8851snl(void)
{
	DEBUG("auto_init_ksz8851snl(): initializing device...\n");
	/* setup netdev2 device */
	ksz8851snl_setup(&ksz8851snl, KSZ8851SNL_SPI, KSZ8851SNL_CS, KSZ8851SNL_INT,
			 KSZ8851SNL_RST);
	/* initialize netdev2<->gnrc adapter state */
	gnrc_netdev2_eth_init(&_gnrc_ksz8851snl, (netdev2_t*)&ksz8851snl);
	/* start gnrc netdev2 thread */
	gnrc_netdev2_init(_netdev2_eth_stack, MAC_STACKSIZE,
			  MAC_PRIO, "gnrc_ksz8851snl", &_gnrc_ksz8851snl);
}
#else
typedef int dont_be_pedantic;
#endif /*  MODULE_KSZ8851SNL */
/** @} */
