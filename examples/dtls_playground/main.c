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
#include "posix_io.h"
#include "shell.h"
#include "board_uart0.h"
#include "net_if.h"
#include "sixlowpan.h"
#include "sys/socket.h"
#include "inet_pton.h"
#include "../../sys/net/transport_layer/dtls/dtls.h"
#include "../../sys/net/transport_layer/dtls/record.h"

#define IF_ID   (0)

size_t hexstrlen(char* str)
{
    size_t size = 1;
    do {
        if (*str == ' ')
            size++;
    } while (*(str++));

    return size;
}


int hex2bin(uint8_t* buffer, char* str)
{
    int size = hexstrlen(str);
    for (int i=0; i < size; ++i)
        buffer[i] = strtol(str, &str, 16);
    return size;
}


void shell_cmd_init(int argc, char **argv)
{
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
            printf("\t%s/%d\n", addr_str, addr_ptr->addr_len);
        }
    }
}


void shell_cmd_send(int argc, char **argv)
{
    ipv6_addr_t dest;
    dtls_connection_t conn = DTLS_CONNECTION_INIT;
    uint8_t data[32] = {0};
    int len, size;


    if (argc < 2) {
        printf("Usage: send <ipv6address> <data>\n");
        return;
    }
    
    if (!inet_pton(AF_INET6, argv[1], &dest)) {
        printf("ERROR: %s is not a valid IPv6 address\n", argv[1]);
        return;
    }
    
    /* neighbour discovery protocol buggy in RIOT, so we need to add
       neighbours manually. */
    if (!ndp_neighbor_cache_search(&dest)) {
        ndp_neighbor_cache_add(IF_ID, &dest, &(dest.uint16[7]), 2, 0,
                               NDP_NCE_STATUS_REACHABLE,
                               NDP_NCE_TYPE_TENTATIVE,
                               0xffff);
    }

    conn.socket = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == conn.socket) {
        printf("Error Creating Socket!");
        return;
    }

    memset(&conn.socket_addr, 0, sizeof(conn.socket_addr));
    conn.socket_addr.sin6_family = AF_INET;
    memcpy(&conn.socket_addr.sin6_addr, &dest, 16);
    conn.socket_addr.sin6_port = HTONS(8443);


    size = hex2bin(data, argv[2]);
    len = dtls_record_stream_send(&conn, TLS_CONTENT_TYPE_APPLICATION_DATA,
              data, size);
    if (len < 0) {
        printf("Error sending packet!\n");
    }

    close(conn.socket);
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
