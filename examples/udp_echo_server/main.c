/*
 * Copyright (C) 2016 Freie Universität Berlin
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
 * @brief       Default application that shows a lot of functionality of RIOT
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "net/af.h"
#include "net/conn/udp.h"
#include "msg.h"
#include "shell.h"
#include "shell_commands.h"
#include "thread.h"

#define ECHO_SERVER_BUFSIZE         (64)
#define ECHO_SERVER_PORT            (61616)
#define ECHO_SERVER_PRIO            (THREAD_PRIORITY_MAIN - 1)
#define ECHO_SERVER_STACK_SIZE      (THREAD_STACKSIZE_MAIN)

/* shell line buffer */
static char line_buf[SHELL_DEFAULT_BUFSIZE];
static char echo_server_buf[ECHO_SERVER_BUFSIZE];
static char echo_server_stack[ECHO_SERVER_STACK_SIZE];
static const conn_ep_udp_t server_address = { .addr = { .ipv6 = IPV6_ADDR_UNSPECIFIED },
                                              .family = AF_INET6,
                                              .netif = CONN_EP_ANY_NETIF,
                                              .port = ECHO_SERVER_PORT };

static void *echo_server(void *arg)
{
    conn_udp_t server;

    (void)arg;
    /* create connectivity end-point for server */
    if (conn_udp_create(&server, &server_address, NULL) < 0) {
        puts("Unable to start server");
        return NULL;
    }
    while (1) {
        conn_ep_udp_t client_address;
        int res;

        /* wait for packet */
        res = conn_udp_recvfrom(&server, echo_server_buf,
                                sizeof(echo_server_buf), 0, &client_address);
        /* only print out and reply to valid results*/
        if (res >= 0) {
            char addr_str[IPV6_ADDR_MAX_STR_LEN];

            echo_server_buf[res] = '\0';
            /* convert source address to string */
            switch (client_address.family) {
                case AF_INET6:
                    ipv6_addr_to_str(addr_str,
                                     &client_address.addr.ipv6,
                                     sizeof(addr_str));
                    break;
                default:
                    memcpy(addr_str, "???", sizeof("???"));
                    break;
            }
            /* output received message */
            printf("Message \"%s\" received from [%s]:%u\n",
                   echo_server_buf, addr_str, client_address.port);
            /* send reply */
            conn_udp_sendto(&server, echo_server_buf, res,
                            &client_address);
        }
    }
    return NULL;
}

int main(void)
{
    /* start server thread */
    if (thread_create(echo_server_stack, sizeof(echo_server_stack),
                      ECHO_SERVER_PRIO, THREAD_CREATE_STACKTEST, echo_server,
                      NULL, "echo_server") <= KERNEL_PID_UNDEF) {
        puts("Unable to start server thread");
        return 1;
    }

    /* initialize shell with default commands */
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
