/*
 * SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Tests the basic shell commands always provided.
 *
 * @author      Martine S. Lenders <mlenders@riot-os.org>
 *
 * @note        IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
 *              YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
 *              See https://github.com/RIOT-OS/Release-Specs/
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "fmt.h"
#include "net/coap.h"
#include "net/gnrc/netif.h"
#include "net/sock/async.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"
#include "od.h"
#include "shell.h"
#include "test_utils/expect.h"
#include "thread.h"
#include "thread_flags.h"

#define TRIGGER_PRIO        (THREAD_PRIORITY_MAIN - 2)
#define TRIGGER_STACKSIZE   (THREAD_STACKSIZE_DEFAULT)
#define NETIF_ID            5U

static char _trigger_stack[TRIGGER_STACKSIZE];
static uint8_t _payload_buf[64];
static size_t _payload_len = 0;
static sock_udp_ep_t _payload_remote;
static sock_udp_cb_t _udp_cb;
static void *_udp_cb_arg;
static sock_udp_t *_server_sock = NULL;
static bool _trigger_running;
static uint32_t _trigger_delay = 0;
static uint16_t _trigger_count = 0;
static kernel_pid_t _trigger_pid = KERNEL_PID_UNDEF;

int sock_udp_create(sock_udp_t *sock, const sock_udp_ep_t *local, const sock_udp_ep_t *remote,
                    uint16_t flags)
{
    (void)remote;
    (void)flags;
    if (local && local->port == COAP_PORT) {
        /* store server sock */
        _server_sock = sock;
    }
    return 0;
}

void sock_udp_close(sock_udp_t *sock)
{
    if (sock == _server_sock) {
        _server_sock = NULL;
    }
}

ssize_t sock_udp_recv_buf_aux(sock_udp_t *sock, void **data, void **buf_ctx, uint32_t timeout,
                              sock_udp_ep_t *remote, sock_udp_aux_rx_t *aux)
{
    (void)aux;
    (void)sock;
    (void)timeout;

    memcpy(remote, &_payload_remote, sizeof(_payload_remote));
    if (*buf_ctx == NULL) {
        ssize_t res = (ssize_t)_payload_len;
        *data = &_payload_buf[0];
        *buf_ctx = &_payload_buf[0];
        return res;
    }

    return 0;
}

ssize_t sock_udp_sendv_aux(sock_udp_t *sock, const iolist_t *snips, const sock_udp_ep_t *remote,
                           sock_udp_aux_tx_t *aux)
{
    (void)remote;
    (void)aux;

    ssize_t res = 0;

    for (const iolist_t *ptr = snips; ptr != NULL; ptr = snips->iol_next) {
        /* the test only works if there is only one snip */
        expect(ptr->iol_next == NULL);
        od_hex_dump_ext(ptr->iol_base, ptr->iol_len, OD_WIDTH_DEFAULT, res);
        res += ptr->iol_len;
    }

    if (_udp_cb) {
        _udp_cb(sock, SOCK_ASYNC_MSG_SENT, _udp_cb_arg);
    }
    return res;
}

void sock_udp_set_cb(sock_udp_t *sock, sock_udp_cb_t cb, void *cb_arg)
{
    (void)sock;
    _udp_cb = cb;
    _udp_cb_arg = cb_arg;
}

sock_async_ctx_t *sock_udp_get_async_ctx(sock_udp_t *sock)
{
    return &sock->async_ctx;
}

/* TODO: the module should have a generic `netif_numof()` dependency here */
unsigned gnrc_netif_numof(void)
{
    return 1U;
}

/* TODO: the module should have a generic `netif_iter()` dependency here */
gnrc_netif_t *gnrc_netif_iter(const gnrc_netif_t *prev)
{
    static gnrc_netif_t _netif = { .pid = NETIF_ID };

    if (prev) {
        return NULL;
    }
    else {
        return &_netif;
    }
}

static void *_trigger_thread(void *arg)
{
    (void)arg;
    while (true) {
        thread_flags_wait_any(0x1000);
        _trigger_running = true;
        ztimer_sleep(ZTIMER_USEC, 500000);
        for (unsigned c = 0; c < _trigger_count; c++) {
            _udp_cb(_server_sock, SOCK_ASYNC_MSG_RECV, _udp_cb_arg);
            if (c < (_trigger_count - 1U)) {
                ztimer_sleep(ZTIMER_USEC, _trigger_delay);
            }
        }
        _trigger_running = false;
    }


    return NULL;
}

static int _preload_payload(int argc, char **argv)
{
    ssize_t data_len;

    if (argc < 2) {
        printf("usage: %s <data hex>\n", argv[0]);
        return 1;
    }

    if ((_payload_len + (strlen(argv[1]) / 2)) > sizeof(_payload_buf)) {
        printf("error: \"%s\" does not fit into remaining buffer " "(buffer size: %lu).\n", argv[1],
               (unsigned long)sizeof(_payload_buf));
        return 1;
    }

    if ((data_len = scn_buf_hex(&_payload_buf[_payload_len], sizeof(_payload_buf) - _payload_len,
                                argv[1], strlen(argv[1]))) < 0) {
        printf("error: Can not parse \"%s\" as hex string.\n", argv[1]);
        return 1;
    }
    _payload_len += data_len;

    od_hex_dump(_payload_buf, _payload_len, OD_WIDTH_DEFAULT);

    return 0;
}

static int _set_payload_remote(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [addr]:port\n", argv[0]);
        return 1;
    }

    if (sock_udp_str2ep(&_payload_remote, argv[1]) != 0) {
        puts("error: unable to parse address-port-pair");
        return 1;
    }
    _payload_remote.netif = NETIF_ID;

    static char addr_str[IPV6_ADDR_MAX_STR_LEN];
    uint16_t port;
    if (sock_udp_ep_fmt(&_payload_remote, addr_str, &port) > 0) {
        printf("success: set payload remote to [%s]:%u\n", addr_str, port);
    }
    else {
        puts("error: can not format response remote");
        return 1;
    }
    return 0;
}

static int _trigger_receive(int argc, char **argv)
{
    if (_trigger_running) {
        puts("error: there is already a trigger sequence running");
        return 1;
    }

    if (argc == 1) {
        _trigger_count = 1;
    }
    else {
        _trigger_count = atoi(argv[1]);
        if (_trigger_count == 0) {
            printf("usage: %s [count [delay_us]]\n", argv[0]);
            return 1;
        }
    }
    if (argc < 3) {
        _trigger_delay = 1000000;
    }
    else {
        _trigger_delay = atoi(argv[2]);
        if (_trigger_delay == 0) {
            printf("usage: %s [count [delay_us]]\n", argv[0]);
            return 1;
        }
    }

    if (_server_sock && _udp_cb) {
        if (_trigger_pid == KERNEL_PID_UNDEF) {
            _trigger_pid = thread_create(_trigger_stack, sizeof(_trigger_stack),
                                         TRIGGER_PRIO, 0, _trigger_thread, NULL,
                                         "receive trigger");
            if (_trigger_pid <= 0) {
                puts("error: unable to start trigger thread.");
                return 1;
            }
        }
        thread_t *trigger_thread = thread_get(_trigger_pid);
        thread_flags_set(trigger_thread, 0x1000);
        return 0;
    }
    puts("error: sock not created or UDP callback not set");
    return 1;
}

static int _reset_payload(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    _payload_len = 0;
    return 0;
}

SHELL_COMMAND(preload_payload, "Preload received UDP payload", _preload_payload);
SHELL_COMMAND(reset_payload, "Reset UDP payload", _reset_payload);
SHELL_COMMAND(payload_remote, "Prepare remote of received UDP payload", _set_payload_remote);
SHELL_COMMAND(trigger_receive, "Trigger reception of UDP payload", _trigger_receive);

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    puts("Starting riotctrl_shell.udp test");

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
