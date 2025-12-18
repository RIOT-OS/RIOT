/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       LibCSP Shell
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "shell.h"
#include "shell_commands.h"
#include "xfa.h"

#include "net/l2util.h"

#include "thread.h"
#include "ztimer.h"
#include "timex.h"

#include "csp_riot.h"
#include "csp/csp_id.h"

/**
 * @brief   Priority used for CSP server
 */
#ifndef CSP_SERVER_PRIO
#define CSP_SERVER_PRIO                   (THREAD_PRIORITY_MAIN - 1)
#endif

/**
 * @brief   Stacksize used for CSP server
 */
#ifndef CSP_SERVER_STACKSIZE
#define CSP_SERVER_STACKSIZE              (THREAD_STACKSIZE_DEFAULT)
#endif

#if IS_USED(MODULE_LIBCSP_SHELL_SERVER)
static char _stack_csp_server[CSP_SERVER_STACKSIZE];
static uint8_t _server_port;
static uint32_t _timeout = CSP_MAX_TIMEOUT;
static kernel_pid_t _server_pid = KERNEL_PID_UNDEF;

void *_csp_server_thread(void *arg)
{
    (void)arg;
    /* create a connection-less socket */
    csp_socket_t sock = { .opts = CSP_SO_CONN_LESS };

    /* bind to all ports */
    csp_bind(&sock, CSP_ANY);

    /* backlog parameter is ignored */
    csp_listen(&sock, 0);

    /* wait for incoming messages */
    csp_packet_t *packet;

    while (1) {
        packet = csp_recvfrom(&sock, _timeout);
        if (!packet) {
            continue;
        }
        if (packet->id.dport == _server_port && _server_port > CSP_UPTIME) {
            printf("Packet %d bytes received on port %d: %s\n", packet->length, _server_port,
                   (char *)packet->data);
            csp_buffer_free(packet);
            continue;
        }
        /* Call the default CSP service handler, handle pings, buffer use, etc. */
        csp_service_handler(packet);
    }
    return NULL;
}

static void _start_server(const char *port_str)
{
    uint16_t port = atoi(port_str);

    /* check tha port does not belong to one of the reserved ports and that its
       in the allowed port range */
    if (port <= CSP_UPTIME || port > csp_id_get_max_port()) {
        printf("ERROR: invalid port specified, Bind port [%d,%d)\n", CSP_UPTIME + 1,
               csp_id_get_max_port());
        return;
    }
    /* check if server is already running */
    if (_server_pid == KERNEL_PID_UNDEF) {
        _server_pid = thread_create(_stack_csp_server, sizeof(_stack_csp_server),
                                    CSP_SERVER_PRIO,
                                    THREAD_CREATE_STACKTEST,
                                    _csp_server_thread, NULL,
                                    "csp_server");
        printf("started server on port %" PRIu16 "\n", port);
    }
    else {
        printf("set port to %" PRIu16 "\n", port);
    }
    _server_port = port;
}

static int _server_handler(int argc, char **argv)
{
    if (argc < 2) {
        puts("usage: csp server start <port>");
        return -1;
    }
    if (strcmp(argv[1], "start") == 0) {
        if (argc < 3) {
            puts("usage: csp server start <port>");
            return -1;
        }
        _start_server(argv[2]);
    }
    else {
        puts("usage: csp server start <port>");
        return -1;
    }
    return 0;
}
#endif

static void _get_usage(void)
{
    printf("Usage: csp get <key>\n");
    puts("      Gets a specific value\n"
         "      <key> may be one of the following\n"
         "       * \"version\" -  CSP version\n"
         "       * \"ver\" - alias for \"version\"\n"
         "       * \"dedup\" - packet de-duplication config\n"
         );
}

static int _get(char *key)
{
    if ((strcmp("version", key) == 0) || (strcmp("ver", key) == 0)) {
        uint8_t ver = libcsp_get_csp_version();
        printf("csp version: %d\n", ver);
        return 0;
    }
    if (strcmp("dedup", key) == 0) {
        uint8_t dedup = libcsp_get_dedup();
        switch (dedup) {
        case CSP_DEDUP_OFF:
            puts("csp dedup: off");
            return 0;
        case CSP_DEDUP_FWD:
            puts("csp dedup: fwd");
            return 0;
        case CSP_DEDUP_INCOMING:
            puts("csp dedup: incoming");
            return 0;
        case CSP_DEDUP_ALL:
            puts("csp dedup: all");
            return 0;
        default:
            break;
        }
    }
    _get_usage();
    return -1;
}

static void _set_usage(void)
{
    printf("Usage: csp set <key> <value>\n");
    puts("      Sets a specific value\n"
         "      <key> may be one of the following\n"
         "       * \"version\" - sets CSP version\n"
         "       * \"ver\" - alias for \"version\"\n"
         "       * \"dedup\" - sets packet de-duplication config\n"
         );
}

static int _set(char *key, char *value)
{
    if ((strcmp("version", key) == 0) || (strcmp("ver", key) == 0)) {
        uint8_t ver = atoi(value);
        if (libcsp_set_csp_version(ver) != 0) {
            printf("ERROR version must be 1 or 2\n");
            return -1;
        }
        return 0;
    }
    if (strcmp("dedup", key) == 0) {
        if (strcmp("off", value) == 0) {
            libcsp_set_dedup(CSP_DEDUP_OFF);
        }
        else if (strcmp("fwd", value) == 0) {
            libcsp_set_dedup(CSP_DEDUP_FWD);
        }
        else if (strcmp("in", value) == 0) {
            libcsp_set_dedup(CSP_DEDUP_INCOMING);
        }
        else if (strcmp("all", value) == 0) {
            libcsp_set_dedup(CSP_DEDUP_ALL);
        }
        else {
            puts("ERROR valid dedup values <off,fwd,in,all>");
            return -1;
        }
        return 0;
    }
    _set_usage();
    return -1;
}

static bool _print_csp_route(void *ctx, csp_route_t *route)
{
    uint8_t count = *((uint8_t *)ctx);
    uint8_t buffer[2];
    char addr_str[sizeof("00:00")];

    byteorder_htobebufs(buffer, route->address);
    l2util_addr_to_str(buffer, sizeof(buffer), addr_str);
    printf("\t%d    %s/%d %s\r\n", count, addr_str, route->netmask, route->iface->name);
    return true;
}

static int _route_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    uint8_t count = 0;

    csp_rtable_iterate(_print_csp_route, &count);
    return 0;
}

static int _conn_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    csp_conn_print_table();
    return 0;
}

static void _if_list(void)
{
    csp_iface_t *i = csp_iflist_get();
    uint32_t tx;
    uint32_t rx;
    char tx_postfix;
    char rx_postfix;

    uint8_t buffer[2];
    char addr_str[sizeof("00:00")];

    while (i) {
        byteorder_htobebufs(buffer, i->addr);
        l2util_addr_to_str(buffer, sizeof(buffer), addr_str);
        tx = csp_bytesize(i->txbytes, &tx_postfix);
        rx = csp_bytesize(i->rxbytes, &rx_postfix);
        printf("%-10s addr: %s netmask: %" PRIu16 " mtu: %" PRIu16 "\r\n"
               "           tx: %05" PRIu32 " rx: %05" PRIu32 " txe: %05" PRIu32 " rxe: %05" PRIu32 "\r\n"
               "           drop: %05" PRIu32 " autherr: %05" PRIu32 " frame: %05" PRIu32 "\r\n"
               "           txb: %" PRIu32 " (%" PRIu32 "%c) rxb: %" PRIu32 " (%" PRIu32 "%c) \r\n\r\n",
               i->name, addr_str, i->netmask, i->mtu, i->tx, i->rx, i->tx_error, i->rx_error, i->drop,
               i->autherr, i->frame, i->txbytes, tx, tx_postfix, i->rxbytes, rx, rx_postfix);
        i = i->next;
    }
}

static int _get_csp_addr(const char *addr_str, uint16_t *addr)
{
    uint8_t addr_buf[2];
    size_t addr_len = l2util_addr_from_str(addr_str, addr_buf);

    if (addr_len != 2) {
        puts("[Error]: unable to parse address.\n"
             "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*\n"
             "(hex pairs delimited by colons)");
        return -1;
    }
    *addr = addr_buf[1] + (addr_buf[0] << 8);
    return 0;
}

static void _send(const char *addr_str, const char *port_str,
                  const char *data, size_t num, unsigned int delay)
{
    uint8_t port;
    uint16_t addr;

    if (_get_csp_addr(addr_str, &addr)) {
        return;
    }
    if (addr >= csp_id_get_max_nodeid()) {
        printf("[ERROR]: invalid addr %s\n", addr_str);
        return;
    }
    /* parse port */
    port = atoi(port_str);
    if (port == 0) {
        puts("ERROR, unable to parse destination port");
        return;
    }
    csp_conn_t *conn = csp_connect(CSP_PRIO_NORM, addr, port, 0, CSP_SO_NONE);

    if (conn == NULL) {
        puts("ERROR, could not conn");
        return;
    }
    for (unsigned int i = 0; i < num; i++) {
        csp_packet_t *packet = csp_buffer_get(strlen(data) + 1 + CSP_BUFFER_PACKET_OVERHEAD);
        if (packet == NULL) {
            puts("ERROR, could not get packet");
            ztimer_sleep(ZTIMER_MSEC, delay);
            continue;
        }
        /* 3. Copy data to packet */
        memcpy(packet->data, data, strlen(data) + 1);
        packet->data[strlen(data)] = '\0';
        packet->length = strlen(data) + 1;
        csp_send(conn, packet);
        ztimer_sleep(ZTIMER_MSEC, delay);
    }
    csp_close(conn);
    return;
}

static int _send_handler(int argc, char **argv)
{
    uint32_t num = 1;
    uint32_t delay = MS_PER_SEC;

    if (argc < 4) {
        puts("usage: csp send <addr> <port> <data> [<num> [<delay in ms>]]");
        return 1;
    }
    if (argc > 4) {
        num = atoi(argv[4]);
    }
    if (argc > 5) {
        delay = atoi(argv[5]);
    }
    _send(argv[1], argv[2], argv[3], num, delay);
    return 0;
}

static int _split_prefix(char *addr_str)
{
    char *sep = addr_str;

    while (*(++sep)) {
        if (*sep == '/') {
            *sep++ = '\0';
            break;
        }
    }
    return *sep ? atoi(sep) : (int)csp_id_get_host_bits();
}

static int _addr_handler(int argc, char **argv)
{
    uint8_t netmask;
    uint16_t addr;

    if (argc < 2) {
        csp_iface_t *iface = csp_iflist_get();
        printf("ERROR, usage: csp addr 3f:ff/14 [ifname] %" PRIx16 "\n", iface->addr);
        return -1;
    }
    else {
        netmask = _split_prefix(argv[1]);
        if (netmask > csp_id_get_host_bits()) {
            printf("ERROR, netmask must be < %d\n", csp_id_get_host_bits());
            return -1;
        }
        if (_get_csp_addr(argv[1], &addr)) {
            return -1;
        }
    }
    if (argc < 3) {
        csp_iface_t *iface = csp_iflist_get();
        iface->addr = addr;
        iface->netmask = netmask;
    }
    else {
        csp_iface_t *iface = csp_iflist_get_by_name(argv[2]);
        if (!iface) {
            printf("ERROR, iface: %s not found\n", argv[2]);
            return -1;
        }
    }
    return 0;
}

static void _print_usage(void)
{
    puts("Usage: csp <addr|conn|set|get|send"
#if IS_USED(MODULE_LIBCSP_SHELL_SERVER)
         "|server"
#endif
         ">");
}

static int _csp_handler(int argc, char **argv)
{
    if (argc <= 1) {
        _if_list();
        return 0;
    }

    if (argc < 2) {
        _print_usage();
        return -1;
    }

    if (!strcmp(argv[1], "help")) {
        _print_usage();
        return 0;
    }

    if (!strcmp(argv[1], "addr")) {
        return _addr_handler(argc - 1, &argv[1]);
    }

    if (!strcmp(argv[1], "route")) {
        return _route_handler(argc - 1, &argv[1]);
    }

    if (!strcmp(argv[1], "conn")) {
        return _conn_handler(argc - 1, &argv[1]);
    }

    if (strcmp(argv[1], "get") == 0) {
        if (argc < 3) {
            _get_usage();
            return 1;
        }
        return _get(argv[2]);
    }

    if (strcmp(argv[1], "set") == 0) {
        if (argc < 4) {
            _set_usage();
            return 1;
        }
        return _set(argv[2], argv[3]);
    }

#if IS_USED(MODULE_LIBCSP_SHELL_SERVER)
    if (!strcmp(argv[1], "server")) {
        return _server_handler(argc - 1, &argv[1]);
    }
#endif

    if (!strcmp(argv[1], "send")) {
        return _send_handler(argc - 1, &argv[1]);
    }

    _print_usage();
    return -1;
}

SHELL_COMMAND(csp, "LibCSP shell commands", _csp_handler);
