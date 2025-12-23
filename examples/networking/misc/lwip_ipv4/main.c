/*
 * SPDX-FileCopyrightText: 2025 Krzysztof Cabaj <kcabaj@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       lwip_ipv4 - sample application for demonstrating basic LWIP
 *              IPv4 client/server functions.
 *
 * @author      Krzysztof Cabaj <kcabaj@gmail.com>
 *
 * @}
 */
#include "stdio.h"
#include "stdlib.h"
#include "shell.h"
#include "net/sock/udp.h"
#include <arpa/inet.h>
#include "thread.h"
#include "mutex.h"

#include "ztimer.h"
#include "lwip/netif.h"
#include <arpa/inet.h>

#ifndef CONFIG_CLIENT_TIMEOUT_SEC
#define CONFIG_CLIENT_TIMEOUT_SEC 3
#endif

#ifndef CONFIG_DHCP_TIMEOUT_SEC
#define CONFIG_DHCP_TIMEOUT_SEC 10
#endif

#ifndef SERVER_PORT
#define SERVER_PORT 4444
#endif

static char server_stack[THREAD_STACKSIZE_DEFAULT];

#define SERVER_PORT 4444

static int _client_cmd(int argc, char **argv)
{
    uint32_t dest_ip;

    if (argc < 4) {
        printf("usage: %s <destination IP> <port> <text>\n", argv[0]);
        return -1;
    }

    if (inet_pton(AF_INET, argv[1], &dest_ip) != 1) {
        printf("\"%s\" - is not a valid IPv4 address!\n", argv[1]);
        return -1;
    }

    uint16_t port = atoi(argv[2]);
    size_t data_len = strlen(argv[3]);
    sock_udp_t sock;
    sock_udp_ep_t remote = { .addr = { .ipv4_u32 = dest_ip },
                             .family = AF_INET,
                             .port = port };
    int result = -1;
    char buffer[64];

    if ((result = sock_udp_create(&sock, NULL, &remote, 0)) != 0) {
        printf("sock_udp_create() failed with %d\n", result);
        return -1;
    }

    if (data_len != (size_t)sock_udp_send(&sock, argv[3], data_len, NULL)) {
        printf("sock_udp_send() failed with %d\n", (int)data_len);
        goto out;
    }

    if ((result = sock_udp_recv(&sock, buffer, sizeof(buffer) - 1,
                                CONFIG_CLIENT_TIMEOUT_SEC * US_PER_SEC, &remote)) < 0) {
        printf("sock_udp_recv() failed with %d\n", result);
        goto out;
    }

    buffer[result] = 0;
    printf("Received %d bytes - %s\n", result, buffer);

out:
    sock_udp_close(&sock);
    return result;
}

SHELL_COMMAND(client, "Send UDP datagram", _client_cmd);

void *server_thread(void *arg)
{
    (void)arg;
    sock_udp_t sock;
    sock_udp_ep_t local = { .family = AF_INET,
                            .port = SERVER_PORT };
    sock_udp_ep_t remote;
    int result;
    int error;
    char buffer[64];

    int res = sock_udp_create(&sock, &local, NULL, 0);
    if (res) {
        printf("Sock_udp_create error!\n");
        printf("Server stopped.\n");
        return NULL;
    }

    printf("Server started.\n");

    while (1) {
        if ((result = sock_udp_recv(&sock, buffer, sizeof(buffer) - 1, SOCK_NO_TIMEOUT, &remote)) < 0) {
            printf("sock_udp_recv() failed with %d\n", result);
        }
        else {
            buffer[result] = 0;
            printf("Received %d bytes - %s\n", result, buffer);

            error = sock_udp_send(&sock, buffer, result, &remote);
            if (error < 0) {
                printf("sock_udp_send() failed with %d\n", error);
            }
        }
    }

    sock_udp_close(&sock);

    return NULL;
}

#define _TEST_ADDR4_LOCAL  (0x0b64a8c0U)   /* 192.168.100.11 */
#define _TEST_ADDR4_MASK   (0x00ffffffU)   /* 255.255.255.0 */

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    sys_lock_tcpip_core();
    struct netif *iface = netif_find("ET0");

#ifndef MODULE_LWIP_DHCP_AUTO
    ip4_addr_t ip, subnet;
    ip.addr = _TEST_ADDR4_LOCAL;
    subnet.addr = _TEST_ADDR4_MASK;
    netif_set_addr(iface, &ip, &subnet, NULL);
#else
    printf("Waiting for DHCP address autoconfiguration ...\n");
    ztimer_sleep(ZTIMER_MSEC, CONFIG_DHCP_TIMEOUT_SEC * MS_PER_SEC);
#endif

    /* print network addresses */
    printf("{\"IPv4 addresses\": [\"");
    char buffer[16];
    inet_ntop(AF_INET, netif_ip_addr4(iface), buffer, 16);
    sys_unlock_tcpip_core();
    printf("%s\"]}\n", buffer);

    thread_create(server_stack, sizeof(server_stack), THREAD_PRIORITY_MAIN - 1, 0, server_thread, NULL, "server");

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
