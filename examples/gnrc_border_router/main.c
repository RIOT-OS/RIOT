/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating the RIOT network stack
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>

#include "event.h"
#include "net/dhcpv6/client.h"
#include "net/ipv6/addr.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/nib/ft.h"
#include "shell.h"
#include "msg.h"

#define MAIN_QUEUE_SIZE     (8)
static char _dhcpv6_client_stack[DHCPV6_CLIENT_STACK_SIZE];
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

void *_dhcpv6_client_thread(void *args)
{
    ipv6_addr_t addr = {
            .u8 = { 0xfe, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }
        };

    event_queue_t event_queue;
    gnrc_netif_t *border_interface = NULL;
    gnrc_netif_t *wpan_interface = NULL;
    gnrc_netif_t *netif = NULL;

    (void)args;
    /* iterate over all interfaces */
    while ((netif = gnrc_netif_iter(netif))) {
        /* check if interface is wired */
        int is_wired = gnrc_netapi_get(netif->pid, NETOPT_IS_WIRED, 0, NULL, 0);

        /* use wired interface as "border" interface (facing to an upstream
         * router) */
        if ((border_interface == NULL) && (is_wired == 1)) {
            border_interface = netif;
        }
        /* use wireless interface as WPAN interface (facing to the WPAN) */
        else if ((wpan_interface == NULL) && (is_wired != 1)) {
            wpan_interface = netif;
        }
        /* we found two interfaces => stop searching */
        if ((border_interface != NULL) && (wpan_interface != NULL)) {
            break;
        }
    }
    /* the border router application assumes both interfaces have been found */
    assert((border_interface != NULL) && (wpan_interface != NULL));
    /* set default route to host machine (as set-up in setup_network.sh) */
    gnrc_ipv6_nib_ft_add(NULL, 0, &addr, border_interface->pid, 0);
    /* set additional link-local address to allow for easy static route
     * configuration */
    addr.u8[15] = 2;
    gnrc_netif_ipv6_addr_add(border_interface, &addr, 64, 0);
    /* initialize client event queue */
    event_queue_init(&event_queue);
    /* initialize DHCPv6 client on border interface */
    dhcpv6_client_init(&event_queue, border_interface->pid);
    /* configure client to request prefix delegation for WPAN interface */
    dhcpv6_client_req_ia_pd(wpan_interface->pid, 64U);
    /* start DHCPv6 client */
    dhcpv6_client_start();
    /* start event loop of DHCPv6 client */
    event_loop(&event_queue);   /* never returns */
    return NULL;
}

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT border router example application");

    /* start DHCPv6 client thread to request prefix for WPAN */
    thread_create(_dhcpv6_client_stack, DHCPV6_CLIENT_STACK_SIZE,
                  DHCPV6_CLIENT_PRIORITY,
                  THREAD_CREATE_STACKTEST,
                  _dhcpv6_client_thread, NULL, "dhcpv6-client");
    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
