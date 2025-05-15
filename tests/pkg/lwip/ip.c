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

#include "cli.h"
#include "kernel_defines.h"
#include "od.h"
#include "net/af.h"
#include "net/sock/async/event.h"
#include "net/sock/ip.h"
#include "shell.h"
#include "thread.h"
#include "test_utils/expect.h"
#include "utilities.h"
#include "ztimer.h"

#ifdef MODULE_SOCK_IP
static char sock_inbuf[SOCK_INBUF_SIZE];
static bool server_running;
static sock_ip_t server_sock;
static char server_stack[THREAD_STACKSIZE_DEFAULT];
static msg_t server_msg_queue[SERVER_MSG_QUEUE_SIZE];

static void _ip_recv(sock_ip_t *sock, sock_async_flags_t flags, void *arg)
{
    expect(strcmp(arg, "test") == 0);
    if (flags & SOCK_ASYNC_MSG_RECV) {
        sock_ip_ep_t src;
        int res;

        if ((res = sock_ip_recv(sock, sock_inbuf, sizeof(sock_inbuf),
                                0, &src)) < 0) {
            puts("Error on receive");
        }
        else if (res == 0) {
            puts("No data received");
        }
        else {
            char addrstr[IPV6_ADDR_MAX_STR_LEN];

            printf("Received IP data from ");
            switch (src.family) {
                case AF_INET:
#if IS_USED(MODULE_LWIP_IPV4)
                    printf("[%s]:\n",
                           ipv4_addr_to_str(addrstr,
                                            (ipv4_addr_t *)&src.addr.ipv4,
                                            sizeof(addrstr)));
                    break;
#else
                    printf("unsupported protocol IPV4\n");
                    break;
#endif
                case AF_INET6:
#if IS_USED(MODULE_LWIP_IPV6)
                    printf("[%s]:\n",
                           ipv6_addr_to_str(addrstr,
                                            (ipv6_addr_t *)&src.addr.ipv6,
                                            sizeof(addrstr)));
                    break;
#else
                    printf("unsupported protocol IPV6\n");
                    break;
#endif
                default:
                    printf("unspecified source\n");
                    break;
            }
            od_hex_dump(sock_inbuf, res, 0);
        }
    }
}

static void *_server_thread(void *args)
{
    event_queue_t queue;
    sock_ip_ep_t server_addr = SOCK_IP_EP_ANY;
    uint8_t protocol;

    msg_init_queue(server_msg_queue, SERVER_MSG_QUEUE_SIZE);
    /* parse protocol */
    protocol = atoi(args);
    if (sock_ip_create(&server_sock, &server_addr, NULL, protocol, 0) < 0) {
        return NULL;
    }
    server_running = true;
    printf("Success: started IP server on protocol %u\n", protocol);
    event_queue_init(&queue);
    sock_ip_event_init(&server_sock, &queue, _ip_recv, "test");
    event_loop(&queue);
    return NULL;
}

static int ip_send(char *addr_str, char *port_str, char *data, unsigned int num,
                   unsigned int delay)
{
    sock_ip_ep_t dst = SOCK_IP_EP_ANY;
    uint8_t protocol;
    uint8_t byte_data[SHELL_DEFAULT_BUFSIZE / 2];
    size_t data_len;

    /* parse destination address */
#if IS_USED(MODULE_LWIP_IPV6)
    if (strchr(addr_str, ':')) {
        if (ipv6_addr_from_str((ipv6_addr_t *)&dst.addr.ipv6,
                               addr_str) == NULL) {
            puts("Error: unable to parse destination address");
            return 1;
        }
        else {
            dst.family = AF_INET6;
        }
    }
#if IS_USED(MODULE_LWIP_IPV4)
    else
#endif
#endif
#if IS_USED(MODULE_LWIP_IPV4)
    if (ipv4_addr_from_str((ipv4_addr_t *)&dst.addr.ipv4,
                           addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return 1;
    }
    else {
        dst.family = AF_INET;
    }
#endif
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
            printf("Success: send %" PRIuSIZE " byte over %s to %s (next header: %u)\n",
                   data_len, (dst.family == AF_INET6) ? "IPv6" : "IPv4",
                   addr_str, protocol);
        }
        ztimer_sleep(ZTIMER_USEC, delay);
    }
    return 0;
}

static int ip_start_server(char *port_str)
{
    if (thread_create(server_stack, sizeof(server_stack), THREAD_PRIORITY_MAIN - 1,
                      0, _server_thread, port_str,
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
