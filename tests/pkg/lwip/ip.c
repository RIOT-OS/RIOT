/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @}
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "common.h"
#include "od.h"
#include "net/af.h"
#include "net/sock/ip.h"
#include "net/ipv6.h"
#include "shell.h"
#include "thread.h"
#include "xtimer.h"

#ifdef MODULE_SOCK_IP
static char sock_inbuf[SOCK_INBUF_SIZE];
static bool server_running;
static sock_ip_t server_sock;
static char server_stack[THREAD_STACKSIZE_DEFAULT];
static msg_t server_msg_queue[SERVER_MSG_QUEUE_SIZE];

static void *_server_thread(void *args)
{
    sock_ip_ep_t server_addr = SOCK_IPV6_EP_ANY;
    uint8_t protocol;

    msg_init_queue(server_msg_queue, SERVER_MSG_QUEUE_SIZE);
    /* parse protocol */
    protocol = atoi(args);
    if (sock_ip_create(&server_sock, &server_addr, NULL, protocol, 0) < 0) {
        return NULL;
    }
    server_running = true;
    printf("Success: started IP server on protocol %u\n", protocol);
    while (1) {
        int res;
        sock_ip_ep_t src;

        if ((res = sock_ip_recv(&server_sock, sock_inbuf, sizeof(sock_inbuf),
                                SOCK_NO_TIMEOUT, &src)) < 0) {
            puts("Error on receive");
        }
        else if (res == 0) {
            puts("No data received");
        }
        else {
            char addrstr[IPV6_ADDR_MAX_STR_LEN];

            printf("Received IP data from [%s]:\n",
                   ipv6_addr_to_str(addrstr, (ipv6_addr_t *)&src.addr.ipv6,
                                    sizeof(addrstr)));
            od_hex_dump(sock_inbuf, res, 0);
        }
    }
    return NULL;
}

static int ip_send(char *addr_str, char *port_str, char *data, unsigned int num,
                   unsigned int delay)
{
    sock_ip_ep_t dst = SOCK_IPV6_EP_ANY;
    uint8_t protocol;
    uint8_t byte_data[SHELL_DEFAULT_BUFSIZE / 2];
    size_t data_len;

    /* parse destination address */
    if (ipv6_addr_from_str((ipv6_addr_t *)&dst.addr.ipv6, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return 1;
    }
    /* parse protocol */
    protocol = atoi(port_str);
    data_len = hex2ints(byte_data, data);
    for (unsigned int i = 0; i < num; i++) {
        sock_ip_t *sock = NULL;

        if (server_running) {
            sock = &server_sock;
        }
        if (sock_ip_send(sock, byte_data, data_len, protocol, &dst) < 0) {
            puts("could not send");
        }
        else {
            printf("Success: send %u byte over IPv6 to %s (next header: %u)\n",
                   (unsigned)data_len, addr_str, protocol);
        }
        xtimer_usleep(delay);
    }
    return 0;
}

static int ip_start_server(char *port_str)
{
    if (thread_create(server_stack, sizeof(server_stack), THREAD_PRIORITY_MAIN - 1,
                      THREAD_CREATE_STACKTEST, _server_thread, port_str,
                      "IP server") <= KERNEL_PID_UNDEF) {
        return 1;
    }
    return 0;
}

int ip_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [send|server]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "send") == 0) {
        uint32_t num = 1;
        uint32_t delay = 1000000;
        if (argc < 5) {
            printf("usage: %s send <addr> <protocol> <hex data> [<num> [<delay in us>]]\n",
                   argv[0]);
            return 1;
        }
        if (argc > 5) {
            num = atoi(argv[5]);
        }
        if (argc > 6) {
            delay = atoi(argv[6]);
        }
        return ip_send(argv[2], argv[3], argv[4], num, delay);
    }
    else if (strcmp(argv[1], "server") == 0) {
        if (argc < 3) {
            printf("usage: %s server [start|stop]\n", argv[0]);
            return 1;
        }
        if (strcmp(argv[2], "start") == 0) {
            if (argc < 4) {
                printf("usage %s server start <protocol>\n", argv[0]);
                return 1;
            }
            return ip_start_server(argv[3]);
        }
        else {
            puts("error: invalid command");
            return 1;
        }
    }
    else {
        puts("error: invalid command");
        return 1;
    }
}
#else
typedef int dont_be_pedantic;
#endif

/** @} */
