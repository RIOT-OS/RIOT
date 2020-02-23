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
 * This test application tests the emb6_conn_ip module.
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#include "common.h"
#include "emb6.h"
#include "emb6/netdev.h"
#include "uip-ds6.h"
#include "net/ipv6/addr.h"
#include "shell.h"
#include "thread.h"
#include "xtimer.h"

#define EMB6_STACKSIZE  (THREAD_STACKSIZE_DEFAULT)
#define EMB6_PRIO       (THREAD_PRIORITY_MAIN - 3)
#define EMB6_DELAY      (500)

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
    (void)args;

    emb6_process(500);  /* never stops */
    return NULL;
}

static const shell_command_t shell_commands[] = {
    { "ping6", "Send pings and receive pongs", ping_cmd },
#ifdef MODULE_EMB6_SOCK_UDP
    { "udp", "Send UDP messages and listen for messages on UDP port", udp_cmd },
#endif
    { "ifconfig", "Shows assigned IPv6 addresses", ifconfig },
    { NULL, NULL, NULL }
};

static char line_buf[SHELL_DEFAULT_BUFSIZE];

int main(void)
{
    puts("RIOT emb6 test application");

    netdev_t *netdev = NULL;
    /* Only one type of transceiver is expected here */
#ifdef MODULE_AT86RF212B
    at86rf212b_t at86rf212b_dev;
    at86rf212b_setup(&at86rf212b_dev, at86rf212b_params, 1);
    netdev = (netdev_t *)&at86rf212b_dev.base.netdev;
#elif defined MODULE_AT86RF231
    at86rf231_t at86rf231_dev;
    at86rf231_setup(&at86rf231_dev, at86rf231_params, 1);
    netdev = (netdev_t *)&at86rf231_dev.base.netdev;
#elif defined MODULE_AT86RF232
    at86rf232_t at86rf232_dev;
    at86rf232_setup(&at86rf232_dev, at86rf232_params, AT86RF232_NUM_OF);
    netdev = (netdev_t *)&at86rf232_dev.base.netdev;
#elif defined MODULE_AT86RF233
    at86rf233_t at86rf233_dev;
    at86rf233_setup(&at86rf233_dev, at86rf233_params, AT86RF233_NUM_OF);
    netdev = (netdev_t *)&at86rf233_dev.base.netdev;
#elif defined MODULE_AT86RFA1
    at86rfa1_t at86rfa1_dev;
    at86rfa1_setup(&at86rfa1_dev);
    netdev = (netdev_t *)&at86rfa1_dev.base.netdev;
#elif defined MODULE_AT86RFR2
    at86rfr2_t at86rfr2_dev;
    at86rfr2_setup(&at86rfr2_dev);
    netdev = (netdev_t *)&at86rfr2_dev.base.netdev;
#endif

    assert(netdev);
    int check = netdev->driver->init(netdev);
    if (check < 0) {
        printf("Radio driver initialization failed: %d\n", check);
        return 1;
    }
    emb6_netdev_setup(netdev);
    emb6_init(&emb6);
    thread_create(emb6_stack, sizeof(emb6_stack), EMB6_PRIO,
                  THREAD_CREATE_STACKTEST, _emb6_thread, NULL, "emb6");
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
