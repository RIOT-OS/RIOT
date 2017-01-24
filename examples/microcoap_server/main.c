/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       CoAP example server application (using microcoap)
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>
#include "msg.h"
#include "xtimer.h"

void microcoap_server_loop(void);

/* import "ifconfig" shell command, used for printing addresses */
extern int _netif_config(int argc, char **argv);

int main(void)
{
    puts("RIOT microcoap example application");

    puts("Waiting for address autoconfiguration...");
    xtimer_sleep(3);

    /* print network addresses */
    puts("Configured network interfaces:");
    _netif_config(0, NULL);

    /* start coap server loop */
    microcoap_server_loop();

    /* should be never reached */
    return 0;
}
