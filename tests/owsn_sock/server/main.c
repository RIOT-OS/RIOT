/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       CoAP example server application (using nanocoap)
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>

#include "openwsn.h"
#include "net/ipv6/addr.h"
#include "net/nanocoap.h"
#include "net/nanocoap_sock.h"

#include "opendefs.h"
#include "03b-IPv6/icmpv6rpl.h"
#include "cross-layers/openqueue.h"
#include "cross-layers/idmanager.h"
#include "cross-layers/packetfunctions.h"

#include "xtimer.h"

#define COAP_INBUF_SIZE (256U)

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern idmanager_vars_t idmanager_vars;
extern icmpv6rpl_vars_t icmpv6rpl_vars;

/* import "ifconfig" shell command, used for printing addresses */
extern int _gnrc_netif_config(int argc, char **argv);

static void _ifconfig(void)
{
    if (idmanager_vars.isDAGroot) {
        puts("Node is DAG root");
    }
    open_addr_t temp_my128bID;
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    memcpy(&temp_my128bID.addr_128b[0], &idmanager_vars.myPrefix.prefix, 8);
    memcpy(&temp_my128bID.addr_128b[8], &idmanager_vars.my64bID.addr_64b, 8);
    ipv6_addr_to_str(addr_str, (ipv6_addr_t *)temp_my128bID.addr_128b, sizeof(addr_str));
    printf("inet6 %s\n", addr_str);
    printf("RPL rank: %i\n", icmpv6rpl_vars.myDAGrank);
}

int main(void)
{
    puts("owsn sock server test");

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Waiting for address autoconfiguration...");
    xtimer_sleep(3);

    openwsn_bootstrap();

    /* print network addresses */
    puts("Configured network interfaces:");
    _ifconfig();

    /* initialize nanocoap server instance */
    uint8_t buf[COAP_INBUF_SIZE];
    sock_udp_ep_t local = { .port=COAP_PORT, .family=AF_INET6 };
    nanocoap_server(&local, buf, sizeof(buf));

    /* should be never reached */
    return 0;
}
