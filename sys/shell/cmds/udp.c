/*
 * SPDX-FileCopyrightText: 2015-17 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Command for sending and receiving of UDP data
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "fmt.h"
#include "net/sock/udp.h"
#if IS_USED(MODULE_SHELL_CMD_UDP_SERVER)
#  include "net/sock/async/event.h"
#endif
#include "net/sock/util.h"
#include "od.h"
#include "shell.h"
#include "time_units.h"
#include "ztimer.h"

#if IS_USED(SOCK_HAS_IPV6)
#  define SOCK_IP_EP_ANY  SOCK_IPV6_EP_ANY
#elif IS_USED(SOCK_HAS_IPV4)
#  define SOCK_IP_EP_ANY  SOCK_IPV4_EP_ANY
#endif

#define SERVER_PRIO             (THREAD_PRIORITY_MAIN - 1)
#define SERVER_STACKSIZE        (THREAD_STACKSIZE_DEFAULT)

#if IS_USED(MODULE_SHELL_CMD_UDP_SERVER)

static char server_stack[SERVER_STACKSIZE];

/* accessed from both main and server thread */
static event_queue_t queue;
static kernel_pid_t server_pid = KERNEL_PID_UNDEF;

static void _server_handler(sock_udp_t *sock, sock_async_flags_t type, void *arg)
{
    (void)arg;
    if (type & SOCK_ASYNC_MSG_RECV) {
        sock_udp_ep_t src;
        ssize_t res;
        char _addr_str[IPV6_ADDR_MAX_STR_LEN];
        uint16_t _port;
        void *buf;
        void *buf_ctx = NULL;

        res = sock_udp_recv_buf(sock, &buf, &buf_ctx, 0, &src);
        if (res > 0) {
            printf("Received UDP data from ");
            if (sock_udp_ep_fmt(&src, _addr_str, &_port) >= 0) {
                printf("[%s]:%u\n", _addr_str, _port);
            }
            else {
                printf("unspecified source\n");
            }

            while (res > 0) {
                od_hex_dump(buf, res, OD_WIDTH_DEFAULT);
                res = sock_udp_recv_buf(sock, &buf, &buf_ctx, 0, &src);
            }
        }
        else {
            puts("error receiving");
        }
    }
}

/* thread-local to server thread */
static bool _server_should_stop = false;

static void _server_stop_cb(event_t *event)
{
    (void)event;
    _server_should_stop = true;
}
static event_t _server_stop_event = { .handler = _server_stop_cb };

static void *_server_thread(void *arg)
{
    uint16_t port = (uint16_t)(uintptr_t)arg;

    sock_udp_ep_t local = SOCK_IP_EP_ANY;
    local.port = port;
    sock_udp_t sock;

    if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
        puts("Error creating UDP sock");
        server_pid = KERNEL_PID_UNDEF;
        return NULL;
    }

    printf("Success: started UDP server on port %" PRIu16 "\n", port);

    event_queue_claim(&queue);
    sock_udp_event_init(&sock, &queue, _server_handler, NULL);
    _server_should_stop = false;

    while (!_server_should_stop) {
        event_t *event = event_wait(&queue);
        event->handler(event);
    }

    sock_udp_close(&sock);
    server_pid = KERNEL_PID_UNDEF;

    puts("Success: stopped UDP server");
    return NULL;
}

/* thread-local to main thread */
static uint16_t server_port = 0;

static void _start_server(const char *port_str)
{
    /* check if server is already running */
    if (server_pid != KERNEL_PID_UNDEF) {
        printf("Error: server already running on port %" PRIu16 "\n", server_port);
        return;
    }
    /* parse port */
    server_port = atoi(port_str);
    if (server_port == 0) {
        printf("Error: invalid port specified\n");
        return;
    }
    /* init event queue to be used by server thread */
    event_queue_init_detached(&queue);
    /* start server */
    server_pid = thread_create(server_stack, sizeof(server_stack), SERVER_PRIO,
                                0, _server_thread, (void *)(uintptr_t)server_port, "UDP server");
    if (server_pid <= KERNEL_PID_UNDEF) {
        puts("Error: can not start server thread");
        return;
    }
}

static void _stop_server(void)
{
    /* check if server is running at all */
    if (server_pid == KERNEL_PID_UNDEF) {
        printf("Error: server was not running\n");
        return;
    }
    /* stop server */
    event_post(&queue, &_server_stop_event);
}

#else

static void _start_server(const char *port_str)
{
    (void)port_str;
    /* not reachable since all calls are module-guarded */
    assert(0);
}

static void _stop_server(void)
{
    /* not reachable since all calls are module-guarded */
    assert(0);
}

#endif /* MODULE_SHELL_CMD_UDP_SERVER*/

static void _send(const char *addr_str, const char *_data, size_t num,
                  unsigned int delay, bool data_is_hex)
{
    int res = -1;

    /* parse destination address */
    sock_udp_ep_t dst;
    if (sock_udp_name2ep(&dst, addr_str) < 0) {
        puts("Error: unable to parse destination address");
        return;
    }
    if (dst.port == 0) {
        puts("Error: no port or illegal port value provided");
        return;
    }

    const void *data = _data;
    size_t data_len = strlen(data);
    uint8_t data_bytes[SHELL_DEFAULT_BUFSIZE / 2];
    if (data_is_hex) {
        ssize_t ret = scn_buf_hex(data_bytes, sizeof(data_bytes), data, data_len);
        if (ret < 0) {
            puts("Error: unable to parse hexadecimal data");
            return;
        }
        data = data_bytes;
        data_len = ret;
    }

    sock_udp_t sock;
    if ((res = sock_udp_create(&sock, NULL, &dst, 0)) < 0) {
        puts("Error creating socket");
        return;
    }

    while (num--) {
        if ((res = sock_udp_send(&sock, data, data_len, NULL)) < 0) {
            puts("Error sending data");
            sock_udp_close(&sock);
            return;
        }
        printf("Success: sent %u byte over UDP to %s\n", res, addr_str);
        if (num) {
#if IS_USED(MODULE_ZTIMER_USEC)
            ztimer_sleep(ZTIMER_USEC, delay);
#elif IS_USED(MODULE_ZTIMER_MSEC)
            ztimer_sleep(ZTIMER_MSEC, (delay + US_PER_MS - 1) / US_PER_MS);
#endif
        }
    }

    sock_udp_close(&sock);
}

static int _udp_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [send|server]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "send") == 0) {
        uint32_t num = 1;
        uint32_t delay = 1 * US_PER_SEC;
        if (argc < 3) {
            printf("usage: %s send [-b] <addr>:<port> "
                   "<data> [<num> [<delay in us>]]\n", argv[0]);
            return 1;
        }
        bool binary = strcmp(argv[2], "-b") == 0;
        if (binary) {
            /* shift arguments by 1 to account for encountered -b option */
            argv++;
            argc--;
        }
        if (argc > 4) {
            if (!IS_USED(MODULE_ZTIMER)) {
                puts("Error: retransmission requires ztimer. "
                     "Add ztimer_msec or ztimer_usec to USEMODULE.");
                return 1;
            }
            num = atoi(argv[4]);
            if (argc > 5) {
                delay = atoi(argv[5]);
            }
        }
        _send(argv[2], argv[3], num, delay, binary);
    }
    else if (strcmp(argv[1], "server") == 0) {
        if (!IS_USED(MODULE_SHELL_CMD_UDP_SERVER)) {
            puts("Error: server command not supported in this build. "
                 "Add shell_cmd_udp_server to USEMODULE.");
            return 1;
        }
        if (argc < 3) {
            printf("usage: %s server [start|stop]\n", argv[0]);
            return 1;
        }
        if (strcmp(argv[2], "start") == 0) {
            if (argc < 4) {
                printf("usage %s server start <port>\n", argv[0]);
                return 1;
            }
            _start_server(argv[3]);
        }
        else if (strcmp(argv[2], "stop") == 0) {
            _stop_server();
        }
        else {
            printf("error: invalid command\n");
        }
    }
    else {
        printf("error: invalid command\n");
    }
    return 0;
}

SHELL_COMMAND(udp, "send data over UDP and listen on UDP ports", _udp_cmd);
