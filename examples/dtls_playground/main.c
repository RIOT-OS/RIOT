/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       DTLS playground
 *
 * @author      Jonatan Zint <j.zint@fu-berlin.de>
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 *
 * @}
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include "kernel.h"
#include "thread.h"
#include "posix_io.h"
#include "shell.h"
#include "board_uart0.h"
#include "net_if.h"
#include "sixlowpan.h"
#include "inet_pton.h"
#include "dtls.h"
#include "utils.h"

#define IF_ID   (0)
#define SERVER_PORT 8443
#define BUFFER_SIZE     (128)

char server_stack_buffer[KERNEL_CONF_STACKSIZE_MAIN];


int listen_callback(dtls_connection_t *conn, uint8_t *data, size_t size)
{
    (void) conn;
    (void) data;

    printf("GOT DATA\n");
    for (size_t i=0; i < size; ++i)
        printf("\t%d: %d (%X)\n", i, data[i], data[i]);

    return 0;
}


void init_dtls_listen(void)
{
    dtls_listen(SERVER_PORT, &listen_callback);
}


void shell_cmd_init(int argc, char **argv)
{
    int server_thread_pid;
    ipv6_addr_t ipaddr;
    net_if_addr_t *addr_ptr = NULL;
    char addr_str[IPV6_MAX_ADDR_STR_LEN];

    (void) argc;
    (void) argv;

    /* configure link-local address */
    ipv6_addr_set_link_local_prefix(&ipaddr);

    if (!ipv6_addr_set_by_eui64(&ipaddr, IF_ID, &ipaddr)) {
        printf("Can not set link-local by EUI-64 on interface %d\n", IF_ID);
        return;
    }

    if (!ipv6_net_if_add_addr(IF_ID, &ipaddr, NDP_ADDR_STATE_PREFERRED,
                NDP_OPT_PI_VLIFETIME_INFINITE,
                NDP_OPT_PI_PLIFETIME_INFINITE, 0)) {
        printf("Can not add link-local address to interface %d\n", IF_ID);
        return;
    }

    /* Print configured IPv6-Addresses */
    printf("Configured IPv6 addresses:\n");
    while (net_if_iter_addresses(0, &addr_ptr)) {
        if (inet_ntop(AF_INET6, addr_ptr->addr_data, addr_str,
                      IPV6_MAX_ADDR_STR_LEN)) {
            printf("\t%s / %d\n", addr_str, addr_ptr->addr_len);
        }
    }

    /* Start DTLS Server */
    server_thread_pid = thread_create(server_stack_buffer,
            KERNEL_CONF_STACKSIZE_MAIN, PRIORITY_MAIN, CREATE_STACKTEST,
            init_dtls_listen, "init_dtls_listen");
    printf("\nDTLS SERVER ON PORT %d (THREAD PID: %d)\n\n", SERVER_PORT,
            server_thread_pid);
}


void shell_cmd_send(int argc, char **argv)
{
    dtls_connection_t conn = DTLS_CONNECTION_INIT;
    uint8_t buffer[BUFFER_SIZE];
    int size;


    if (argc < 2) {
        printf("Usage: send <ipv6address> <data>\n");
        return;
    }

    dtls_connect(&conn, argv[1], SERVER_PORT);
    size = hex2bin(buffer, argv[2]);
    if (dtls_record_stream_send(&conn, TLS_CONTENT_TYPE_APPLICATION_DATA,
              buffer, size) < 0)
        printf("Error sending packet!\n");

    dtls_close(&conn);
}


const shell_command_t shell_commands[] = {
    {"init", "configure link-local addresses", shell_cmd_init},
    {"send", "record layer", shell_cmd_send},
    {NULL, NULL, NULL}
};


int main(void)
{
    shell_t shell;

    /* start shell */
    posix_open(uart0_handler_pid, 0);
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);
    shell_run(&shell);

    return 0;
}
