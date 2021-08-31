/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       DHCPv6 client test application
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#include <stddef.h>

#include "net/gnrc/netif.h"
#include "net/dhcpv6/client.h"
#include "net/sock.h"
#include "xtimer.h"

static char _dhcpv6_client_stack[DHCPV6_CLIENT_STACK_SIZE];

extern int _gnrc_netif_config(int argc, char **argv);
extern int _gnrc_ipv6_nib(int argc, char **argv);

void *_dhcpv6_client_thread(void *args)
{
    event_queue_t event_queue;
    gnrc_netif_t *netif = gnrc_netif_iter(NULL);

    (void)args;
    /* initialize client event queue */
    event_queue_init(&event_queue);
    /* Configure client to use DHCPv6 IA_NA */
    netif->ipv6.aac_mode |= GNRC_NETIF_AAC_DHCP;
    /* initialize DHCPv6 client on any interface */
    dhcpv6_client_init(&event_queue, SOCK_ADDR_ANY_NETIF);
    /* configure client to request prefix delegation of /64 subnet
     * interface netif */
    dhcpv6_client_req_ia_pd(netif->pid, 64U);
    /* set client configuration mode to stateful */
    dhcpv6_client_set_conf_mode(DHCPV6_CLIENT_CONF_MODE_STATEFUL);
    /* start DHCPv6 client */
    dhcpv6_client_start();
    /* start event loop of DHCPv6 client */
    event_loop(&event_queue);   /* never returns */
    return NULL;
}

int main(void)
{
    char *pl[] = { "nib", "prefix" };

    _gnrc_netif_config(0, NULL);
    thread_create(_dhcpv6_client_stack, DHCPV6_CLIENT_STACK_SIZE,
                  DHCPV6_CLIENT_PRIORITY, THREAD_CREATE_STACKTEST,
                  _dhcpv6_client_thread, NULL, "dhcpv6-client");
    xtimer_sleep(5);
    /* global address should now be configured */
    _gnrc_netif_config(0, NULL);
    _gnrc_ipv6_nib(2, pl);
    return 0;
}
