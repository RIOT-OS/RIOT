/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * @brief       Example application for demonstrating the RIOT's POSIX select()
 *              implementation
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <stdbool.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "od.h"
#include "net/gnrc/netif.h"
#include "thread.h"

#define SERVER_BUFFER_SIZE      (128U)
#define SERVER_PORTS            { 1350U, 4973U, 6717U, 9673U }
#define SERVER_SOCKETS_NUM      (4U)

static char server_buffer[SERVER_BUFFER_SIZE];
static char addr_str[IPV6_ADDR_MAX_STR_LEN];

static int _run_server(void *local_addr)
{
    struct sockaddr_in6 server_addr = { .sin6_family = AF_INET6,
                                        .sin6_addr = IN6ADDR_ANY_INIT };
    static const uint16_t ports[SERVER_SOCKETS_NUM] = SERVER_PORTS;
    int server_sockets[SERVER_SOCKETS_NUM] = { 0 };
    int max_fd = -1;
    int ret = 0;

    /* open SERVER_SOCKETS_NUM sockets with respective port */
    for (unsigned i = 0; i < SERVER_SOCKETS_NUM; i++) {
        server_sockets[i] = socket(AF_INET6, SOCK_DGRAM, 0);
        if (server_sockets[i] < 0) {
            puts("error initializing socket");
            ret = 1;
            goto end;
        }
        server_addr.sin6_port = htons(ports[i]);
        if (bind(server_sockets[i],
                 (struct sockaddr *)&server_addr,
                 sizeof(server_addr)) < 0) {
            puts("error binding socket");
            ret = 1;
            goto end;
        }
        printf("Started UDP server at [%s]:%u\n",
               inet_ntop(AF_INET6, local_addr, addr_str, sizeof(addr_str)),
               ports[i]);
        if (max_fd < server_sockets[i]) {
            max_fd = server_sockets[i];
        }
    }

    while (true) {
        fd_set readfds;

        /* add bound sockets to set of file descriptors to read */
        FD_ZERO(&readfds);
        for (unsigned i = 0; i < SERVER_SOCKETS_NUM; i++) {
            FD_SET(server_sockets[i], &readfds);
        }
        /* wait for bound sockets to be notified for reading*/
        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
            puts("error on select");
            continue;
        }
        for (unsigned i = 0; i < SERVER_SOCKETS_NUM; i++) {
            /* if socket is in set of file descriptors to check for reading */
            if (FD_ISSET(server_sockets[i], &readfds)) {
                int res;
                struct sockaddr_in6 src;
                socklen_t src_len = sizeof(struct sockaddr_in6);

                /* receive data from socket */
                if ((res = recvfrom(server_sockets[i], server_buffer,
                                    sizeof(server_buffer), 0,
                                    (struct sockaddr *)&src, &src_len)) < 0) {
                    puts("Error on receive");
                }
                else if (res == 0) {
                    puts("Peer did shut down");
                }
                else {
                    printf("Received data from [%s]:%u:\n",
                           inet_ntop(AF_INET6, &src.sin6_addr,
                                     addr_str, sizeof(addr_str)),
                           src.sin6_port);
                    res = ((unsigned)res < SERVER_BUFFER_SIZE) ? res : (res - 1);
                    /* terminate string */
                    server_buffer[res] = '\0';
                    printf("%s\n", server_buffer);
                }
            }
        }
    }

end:
    /* close all open sockets */
    for (unsigned i = 0; i < SERVER_SOCKETS_NUM; i++) {
        if (server_sockets[i] > 0) {
            close(server_sockets[i]);
        }
    }
    return ret;
}

int main(void)
{
    /* TODO: use TBD POSIX API to get link-local address */
    gnrc_netif_t *netif = gnrc_netif_iter(NULL);
    ipv6_addr_t addr;

    puts("RIOT select example application");

    /* get first address on the interface */
    if (gnrc_netif_ipv6_addrs_get(netif, &addr, sizeof(addr)) < 0) {
        puts("Unable to get first address of the interface");
        return 1;
    }
    return _run_server(&addr);
}
