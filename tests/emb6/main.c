/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
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
 * @brief       Test for raw IPv6 connections
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This test application tests the gnrc_conn_ip module. If you select protocol 58 you can also
 * test if gnrc is able to deal with multiple subscribers to ICMPv6 (gnrc_icmpv6 and this
 * application).
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#include "common.h"
#include "emb6.h"
#include "emb6/netdev2.h"
#include "uip-ds6.h"
#include "net/ipv6/addr.h"
#include "shell.h"
#include "thread.h"
#include "xtimer.h"

#define EMB6_STACKSIZE  (THREAD_STACKSIZE_DEFAULT)
#define EMB6_PRIO       (THREAD_PRIORITY_MAIN - 3)
#define EMB6_DELAY      (500)

static at86rf2xx_t at86rf2xx;
static s_ns_t emb6 = {
    .hc = &sicslowpan_driver,
    .llsec = &nullsec_driver,
    .hmac = &nullmac_driver,
    .lmac = &sicslowmac_driver,
    .frame = &framer_802154,
    .c_configured = 1,
};
static char emb6_stack[EMB6_STACKSIZE];

static int ifconfig(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    char addrstr[IPV6_ADDR_MAX_STR_LEN];
    printf("0:  ");
    for (int i = 0; i < UIP_DS6_ADDR_NB; i++) {
        if (uip_ds6_if.addr_list[i].isused) {
            printf("inet6 %s\n",
                   ipv6_addr_to_str(addrstr,
                                    (ipv6_addr_t *)&uip_ds6_if.addr_list[i].ipaddr,
                                    sizeof(addrstr)));
            if (i != 0) {
                printf("    ");
            }
        }
    }
    puts("");
    return 0;
}

static void *_emb6_thread(void *args)
{
    emb6_process(500);  /* never stops */
    return NULL;
}

static const shell_command_t shell_commands[] = {
    { "ping6", "Send pings and receive pongs", ping_cmd },
#ifdef MODULE_CONN_UDP
    { "udp", "Send UDP messages and listen for messages on UDP port", udp_cmd },
#endif
    { "ifconfig", "Shows assigned IPv6 addresses", ifconfig },
    { NULL, NULL, NULL }
};
static char line_buf[SHELL_DEFAULT_BUFSIZE];

char conn_inbuf[CONN_INBUF_SIZE];

int main(void)
{
    netdev2_t *netdev = (netdev2_t *)&at86rf2xx;

    puts("RIOT lwip test application");

    at86rf2xx_setup(&at86rf2xx, at86rf2xx_params);
    netdev->driver->init((netdev2_t *)&at86rf2xx);
    emb6_netdev2_setup(netdev);
    emb6_init(&emb6);
    thread_create(emb6_stack, sizeof(emb6_stack), EMB6_PRIO,
                  THREAD_CREATE_STACKTEST, _emb6_thread, NULL, "emb6");
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
