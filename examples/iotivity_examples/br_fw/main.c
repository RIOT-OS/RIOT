/*
 * Copyright (C) 2016 CREATE-NET
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     Examples
 * @{
 *
 * @file        main.c
 * @brief       This is a BR with multicast forwarding for OIC discovery
 *
 * @author      Mattia Antonini <mattia.antonini1@studenti.unipr.it>
 *                              <m.antonini@create-net.org>
 *
 * @}
 */

#include <stdio.h>

#include "net/af.h"
#include "shell.h"
#include "xtimer.h"
#include "msg.h"
#include "net/gnrc.h"
#include "net/gnrc/conn.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/rpl.h"
#include "net/gnrc/udp.h"
#include "net/conn/udp.h"
#include "thread.h"
#define MAIN_QUEUE_SIZE     (8)
#define OIC_FW_QUEUE_SIZE     (8)
#define MAX_PAYLOAD_SIZE (256)
#define MULTICAST_PORT (5683)

#define PRINTipaddr(addr, port)                                                  \
    printf(                                                                       \
        "[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%"    \
        "02x]:%d",                                                                 \
        (addr)[0], (addr)[1],      \
        (addr)[2], (addr)[3],      \
        (addr)[4], (addr)[5],      \
        (addr)[6], (addr)[7],      \
        (addr)[8], (addr)[9],      \
        (addr)[10], (addr)[11],    \
        (addr)[12], (addr)[13],    \
        (addr)[14], (addr)[15],    \
        port)

static uint8_t buffer2[MAX_PAYLOAD_SIZE];

static msg_t _oic_fw_msg_queue[OIC_FW_QUEUE_SIZE];
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static kernel_pid_t interface_pid;

char oic_forwarding_thread_stack[THREAD_STACKSIZE_DEFAULT];

uint8_t wk_addr[16] = { 0xff, 0x03, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0x01, 0x58 };

void handle_incoming_message(uint8_t *payload, int *size, uint8_t *addr,
                             uint8_t addr_len, uint16_t port)
{
    char *addr_str = NULL;
    uint8_t buffer[*size];

    memcpy(buffer, payload, *size);

    ipv6_addr_to_str(addr_str, (ipv6_addr_t *)addr, addr_len);
    printf("forwarder: pck from ");
    PRINTipaddr(addr, port);
    printf("\n");
    printf("forwarder: payload size %d B\n", *size);

    conn_udp_sendto(buffer, *size, addr, 16,
                    wk_addr, 16, AF_INET6,
                    port, MULTICAST_PORT);
}

void *start_oic_forwarding_loop(void *arg)
{
    (void)arg;

    msg_init_queue(_oic_fw_msg_queue, OIC_FW_QUEUE_SIZE);

    static uint8_t addr[16] = { 0 };
    uint8_t remote_addr[16] = { 0 };
    size_t remote_addr_len;
    uint16_t remote_port;

    conn_udp_t conn;

    int n = conn_udp_create(&conn, addr, sizeof(addr), AF_INET6, MULTICAST_PORT);

    if (n != 0) {
        printf("forwarder: failed to register multicast receive socket\n");
        return NULL;
    }


    ipv6_addr_t *if_addr =
        gnrc_ipv6_netif_add_addr(interface_pid, (ipv6_addr_t *)&wk_addr, 128, 0);

    if (if_addr == NULL) {
        printf("forwarder: error.. could not join multicast group\n");
        return NULL;
    }

    gnrc_ipv6_netif_addr_get(if_addr)->valid = UINT32_MAX;
    gnrc_ipv6_netif_addr_get(if_addr)->preferred = UINT32_MAX;

    while (1) {
        printf("forwarder: waiting for multicast requests...\n");
        n = conn_udp_recvfrom(&conn, (char *)buffer2, MAX_PAYLOAD_SIZE, remote_addr,
                              &remote_addr_len, &remote_port);
        if (n < 0) {
            printf("forwarder_multicast_recv: error in conn_udp_recvfrom().n=%d\n", n);
            continue;
        }

        printf("forwarder: got multicast request\n");

        handle_incoming_message(buffer2, &n, remote_addr, remote_addr_len, remote_port);
    }

    return NULL;
}


int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT border router + OIC discovery forwarder");

    kernel_pid_t interfaces[GNRC_NETIF_NUMOF];
    size_t if_num = gnrc_netif_get(interfaces);

    if (if_num == 0) {
        printf("forwarder: error.. no available network interface\n");
        return -1;
    }

    interface_pid = interfaces[0];

    thread_create(oic_forwarding_thread_stack, sizeof(oic_forwarding_thread_stack),
                  THREAD_PRIORITY_MAIN, THREAD_CREATE_STACKTEST,  start_oic_forwarding_loop,
                  NULL, "oic_fw_thread");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);



    /* should be never reached */
    return 0;
}
