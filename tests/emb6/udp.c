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
 * @brief       Demonstrating the sending and receiving of UDP data over POSIX sockets.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "common.h"
#include "od.h"
#include "net/af.h"
#include "net/sock/udp.h"
#include "net/ipv6.h"
#include "thread.h"
#include "xtimer.h"

#ifdef MODULE_EMB6_SOCK_UDP
static char sock_inbuf[SOCK_INBUF_SIZE];
static bool server_running;
static sock_udp_t server_sock;
static char server_stack[THREAD_STACKSIZE_DEFAULT];

static void *_server_thread(void *args)
{
    int res;

    /* parse port */
    uint16_t port = atoi(args);
    const sock_udp_ep_t server_addr = { .family = AF_INET6,
                                        .port = port };
    if ((res = sock_udp_create(&server_sock, &server_addr, NULL, 0)) < 0) {
        printf("Unable to open UDP server on port %" PRIu16 " (error code %d)\n",
               port, -res);
        return NULL;
    }
    server_running = true;
    printf("Success: started UDP server on port %" PRIu16 "\n", port);
    while (1) {
        sock_udp_ep_t client_addr;

        if ((res = sock_udp_recv(&server_sock, sock_inbuf, sizeof(sock_inbuf),
                                 SOCK_NO_TIMEOUT, &client_addr)) < 0) {
            puts("Error on receive");
        }
        else if (res == 0) {
            puts("No data received");
        }
        else {
            char addrstr[IPV6_ADDR_MAX_STR_LEN];
            printf("Received from [%s]:%" PRIu16 ":\n",
                   ipv6_addr_to_str(addrstr, (ipv6_addr_t *)&client_addr.addr,
                                    sizeof(addrstr)), client_addr.port);
            od_hex_dump(sock_inbuf, res, 0);
        }
    }
    return NULL;
}

static int udp_send(char *addr_str, char *port_str, char *data, unsigned int num,
                    unsigned int delay)
{
    uint8_t byte_data[strlen(data) / 2];
    sock_udp_ep_t dst = { .family = AF_INET6 };
    size_t data_len;

    /* parse destination address */
    if (ipv6_addr_from_str((ipv6_addr_t *)&dst.addr, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return 1;
    }
    /* parse port */
    dst.port = atoi(port_str);
    data_len = hex2ints(byte_data, data);
    for (unsigned int i = 0; i < num; i++) {
        if (sock_udp_send(NULL, byte_data, data_len, &dst) < 0) {
            puts("could not send");
        }
        else {
            printf("Success: send %u byte to [%s]:%" PRIu16 ")\n",
                   (unsigned)data_len, addr_str, dst.port);
        }
        xtimer_usleep(delay);
    }
    return 0;
}

static int udp_start_server(char *port_str)
{
    if (thread_create(server_stack, sizeof(server_stack), THREAD_PRIORITY_MAIN - 1,
                      THREAD_CREATE_STACKTEST, _server_thread, port_str,
                      "UDP server") <= KERNEL_PID_UNDEF) {
        return 1;
    }
    return 0;
}

int udp_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [send|server]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "send") == 0) {
        uint32_t num = 1;
        uint32_t delay = 1000000;
        if (argc < 5) {
            printf("usage: %s send <addr> <port> <hex data> [<num> [<delay in us>]]\n",
                   argv[0]);
            return 1;
        }
        if (argc > 5) {
            num = atoi(argv[5]);
        }
        if (argc > 6) {
            delay = atoi(argv[6]);
        }
        return udp_send(argv[2], argv[3], argv[4], num, delay);
    }
    else if (strcmp(argv[1], "server") == 0) {
        if (argc < 3) {
            printf("usage: %s server [start|stop]\n", argv[0]);
            return 1;
        }
        if (strcmp(argv[2], "start") == 0) {
            if (argc < 4) {
                printf("usage %s server start <port>\n", argv[0]);
                return 1;
            }
            return udp_start_server(argv[3]);
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
