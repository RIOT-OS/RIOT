/**
 * @file main.c
 * @brief Main application for TLS over TCP example using RIOT OS
 * @author Bilal-Alali
 * @date 2025-05-21 12:37:35
 */

#include <stdio.h>
#include <string.h>
#include "shell.h"
#include "msg.h"
#include "net/sock/tcp.h"
#include "net/gnrc.h"
#include "net/ipv6/addr.h"
#include "net/sock/tls.h"
#include <wolfssl/ssl.h>
#include "ztimer.h"
#include "ca_cert.h"
#include "cert_data.h"

#define SERVER_PORT         12345
#define BUFFER_SIZE         1024
#define MAIN_QUEUE_SIZE     8
#define TLS_IO_TIMEOUT      30 //(in sec)
#define RESPONSE_MSG_SERVER        "Hello from TLS server!"
#define RESPONSE_MSG_LEN    (sizeof(RESPONSE_MSG_SERVER) - 1)
#define READ_CHUNK_SIZE     256
#define MAX_MSG_SIZE        1024

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static int tls_server(int argc, char **argv) {
    (void)argc;
    (void)argv;

    sock_tls_tcp_queue_t tls_queue;
    sock_tcp_ep_t local = {
        .family = AF_INET6,
        .port = SERVER_PORT,
        .netif = SOCK_ADDR_ANY_NETIF
    };
    sock_tcp_t queue_array[1];

    if (wolfSSL_Init() != WOLFSSL_SUCCESS) {
        printf("Failed to initialize wolfSSL\n");
        return 1;
    }

    printf("Server: Starting TLS server on port %d...\n", SERVER_PORT);
    int ret = sock_tls_tcp_listen(&tls_queue, &local, queue_array, 1, 0,
                                 wolfTLSv1_2_server_method(),
                                 riot_cert_pem, riot_cert_pem_len,
                                 riot_key_pem, riot_key_pem_len);
    if (ret < 0) {
        printf("Server: Failed to start TLS server: %d\n", ret);
        wolfSSL_Cleanup();
        return 1;
    }

    while (1) {
        sock_tls_tcp_t *client_sock = NULL;
        ret = sock_tls_tcp_accept(&tls_queue, &client_sock, TLS_IO_TIMEOUT * 1000000);
        if (ret < 0) {
            printf("Server: Accept failed: %d\n", ret);
            ztimer_sleep(ZTIMER_MSEC, 1000);
            continue;
        }

        char buffer[READ_CHUNK_SIZE];
        size_t total_received = 0;

        while (total_received < MAX_MSG_SIZE) {
            ssize_t received = sock_tls_tcp_read(client_sock, buffer, sizeof(buffer));
            if (received <= 0) break;

            total_received += received;
            printf("Server: Received %d bytes: %.*s\n",
                   (int)received, (int)received, buffer);

            ssize_t sent = sock_tls_tcp_write(client_sock, RESPONSE_MSG_SERVER, RESPONSE_MSG_LEN);
            if (sent < 0) break;
            printf("Server: Sent response successfully\n");
        }

        sock_tls_tcp_disconnect(client_sock);
        free(client_sock);
        printf("Server: Connection closed\n");
    }
    return 0;
}

static int tls_client(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <IPv6 address>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    sock_tls_tcp_t tls_sock;
    sock_tcp_ep_t remote = {
        .family = AF_INET6,
        .port = SERVER_PORT,
        .netif = SOCK_ADDR_ANY_NETIF
    };

    if (wolfSSL_Init() != WOLFSSL_SUCCESS) {
        printf("Failed to initialize wolfSSL\n");
        return 1;
    }

    if (ipv6_addr_from_str((ipv6_addr_t *)&remote.addr.ipv6, server_ip) == NULL) {
        puts("Client: Error parsing IPv6 address");
        wolfSSL_Cleanup();
        return 1;
    }

    int ret = sock_tls_tcp_create(&tls_sock, wolfTLSv1_2_client_method(), ca_cert_pem, ca_cert_pem_len);
    if (ret < 0) {
        printf("Client: Failed to create TLS socket: %d\n", ret);
        wolfSSL_Cleanup();
        return 1;
    }

    printf("Client: Connecting to server at %s...\n", server_ip);
    ret = sock_tls_tcp_connect(&tls_sock, &remote, 0, 0);
    if (ret < 0) {
        printf("Client: Connection failed: %d\n", ret);
        wolfSSL_Cleanup();
        return 1;
    }

    printf("Client: Connected successfully\n");
    printf("Client: Using cipher: %s\n", wolfSSL_get_cipher(tls_sock.ssl));

    const char *message = "Hello from TLS client!";
    size_t message_len = strlen(message);
    ssize_t sent = sock_tls_tcp_write(&tls_sock, message, message_len);
    if (sent < 0) {
        printf("Client: Failed to send message: %d\n", (int)sent);
        sock_tls_tcp_disconnect(&tls_sock);
        wolfSSL_Cleanup();
        return 1;
    }

    char buffer[BUFFER_SIZE];
    ret = sock_tls_tcp_read(&tls_sock, buffer, sizeof(buffer));
    if (ret > 0) {
        printf("Client: Received %d bytes: %.*s\n", (int)ret, (int)ret, buffer);
    } else {
        printf("Client: Failed to receive response: %d\n", ret);
    }

    sock_tls_tcp_disconnect(&tls_sock);
    wolfSSL_Cleanup();
    printf("Client: Connection closed\n");
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "tls_server", "Start TLS server", tls_server },
    { "tls_client", "Start TLS client", tls_client },
    { NULL, NULL, NULL }
};

int main(void) {
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT TLS over TCP example application");

    if (wolfSSL_Init() != WOLFSSL_SUCCESS) {
        puts("Failed to initialize wolfSSL");
        return 1;
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    wolfSSL_Cleanup();
    return 0;
}
