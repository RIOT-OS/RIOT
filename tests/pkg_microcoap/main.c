/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       CoAP test server application (using microcoap)
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>

#include "msg.h"
#include "net/netif.h"
#include "xtimer.h"

void microcoap_server_loop(void);

int main(void)
{
    puts("RIOT microcoap test application");

    puts("Waiting for address autoconfiguration...");
    xtimer_sleep(3);

    /* print network addresses */
    printf("{\"IPv6 addresses\": [\"");
    netifs_print_ipv6("\", \"");
    puts("\"]}");

    /* start coap server loop */
    microcoap_server_loop();

    /* should be never reached */
    return 0;
}
