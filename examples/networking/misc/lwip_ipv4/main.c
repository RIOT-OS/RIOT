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

#ifndef CONFIG_CLIENT_TIMEOUT_SEC
#define CONFIG_CLIENT_TIMEOUT_SEC 3
#endif

static char server_stack[THREAD_STACKSIZE_DEFAULT];
static mutex_t server_mutex = MUTEX_INIT;
static int server_running = 0;
static uint16_t server_port;

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
                            .port = server_port };
    sock_udp_ep_t remote;
    int result;
    int error;
    char buffer[64];

    int res = sock_udp_create(&sock, &local, NULL, 0);
    if (res) {
        printf("Sock_udp_create error!\n");
        printf("Server stopped.\n");

        mutex_lock(&server_mutex);
        server_running = 0;
        mutex_unlock(&server_mutex);

        return NULL;
    }

    printf("Server started.\n");

    while (1) {
        if ((result = sock_udp_recv(&sock, buffer, sizeof(buffer) - 1, SOCK_NO_TIMEOUT, &remote)) < 0) {
            printf("sock_udp_recv() failed with %d\n", result);
            printf("Server stopped.\n");

            sock_udp_close(&sock);

            mutex_lock(&server_mutex);
            server_running = 0;
            mutex_unlock(&server_mutex);

            return NULL;
        }

        buffer[result] = 0;
        printf("Received %d bytes - %s\n", result, buffer);

        error = sock_udp_send(&sock, buffer, result, &remote);
        if (error < 0) {
            printf("sock_udp_send() failed with %d\n", error);
        }
    }

    sock_udp_close(&sock);

    return NULL;
}

static int _server_cmd(int argc, char **argv)
{
    int is_running;
    if (argc < 2) {
        printf("usage: %s <port>\n", argv[0]);
        return -1;
    }

    mutex_lock(&server_mutex);
    is_running = server_running;
    mutex_unlock(&server_mutex);

    if (is_running == 1) {
        printf("Server already running!\n");
        return -1;
    }

    server_port = atoi(argv[1]);

    mutex_lock(&server_mutex);
    server_running = 1;
    mutex_unlock(&server_mutex);

    thread_create(server_stack, sizeof(server_stack), THREAD_PRIORITY_MAIN - 1, 0, server_thread, NULL, "server");

    return -1;
}

SHELL_COMMAND(server, "Starts server which receives UDP datagrams", _server_cmd);

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
