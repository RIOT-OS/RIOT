/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Telnet server implementation
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <string.h>
#include <fcntl.h>
#include "net/sock/tcp.h"
#include "net/telnet.h"
#include "stdio_base.h"
#include "pipe.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef CONFIG_TELNET_TCP_QUEUE_SIZE
#define CONFIG_TELNET_TCP_QUEUE_SIZE (1)
#endif

static char _stdin_pipe_buf[16];
static ringbuffer_t _stdin_ringbuffer;
static pipe_t _stdin_pipe;

static sock_tcp_queue_t sock_queue;
static sock_tcp_t socks[CONFIG_TELNET_TCP_QUEUE_SIZE];
static sock_tcp_t *client;
static bool _want_disconnect;

static char telnet_stack[THREAD_STACKSIZE_DEFAULT];

#define SOCK_TCP_TIMEOUT_MS 50

enum {
    TELNET_CMD_EOF  = 236,
    TELNET_CMD_SE   = 240,
    TELNET_CMD_NOP,
    TELNET_CMD_DATA_MARK,
    TELNET_CMD_BRK,
    TELNET_CMD_IP,
    TELNET_CMD_AO,
    TELNET_CMD_AYT,
    TELNET_CMD_EC,
    TELNET_CMD_EL,
    TELNET_CMD_GA,
    TELNET_CMD_SB   = 250,
    TELNET_CMD_WILL,
    TELNET_CMD_WONT,
    TELNET_CMD_DO,
    TELNET_CMD_DONT,
    TELNET_CMD_IAC
};

enum {
    TELNET_OPT_BINARY   = 0,
    TELNET_OPT_ECHO     = 1,
    TELNET_OPT_SUP_GO_AHEAD = 3,
    TELNET_OPT_STATUS   = 5,
};

static bool connected;
static mutex_t connected_mutex = MUTEX_INIT_LOCKED;
static mutex_t sock_mutex;

__attribute__((weak))
void telnet_cb_pre_connected(sock_tcp_t *sock)
{
    (void)sock;
}

__attribute__((weak))
void telnet_cb_connected(sock_tcp_t *sock)
{
    (void)sock;
}

__attribute__((weak))
void telnet_cb_disconneced(void)
{
}

static void _acquire(void)
{
    mutex_lock(&sock_mutex);
}

static void _release(void)
{
    mutex_unlock(&sock_mutex);
}

static void _connected(void)
{
    telnet_cb_pre_connected(client);

    connected = true;
    if (!IS_USED(MODULE_STDIO_TELNET)) {
        mutex_unlock(&connected_mutex);
    }

    telnet_cb_connected(client);
}

static void _disconnect(void)
{
    if (!IS_USED(MODULE_STDIO_TELNET)) {
        mutex_trylock(&connected_mutex);
    }
    connected = false;

    DEBUG("telnet disconnect\n");
    telnet_cb_disconneced();

    _acquire();
    sock_tcp_disconnect(client);
    _release();
}

static int _write_buffer(const void* buffer, size_t len)
{
    int res = 0;
    const char *buf = buffer;
    _acquire();

    while (len) {
        /* telnet expects \r\n line endings */
        /* https://datatracker.ietf.org/doc/html/rfc5198#appendix-C */
        const char *nl = memchr(buf, '\n', len);
        if (nl) {
            const char cr = '\r';
            size_t before_nl = nl - buf;

            /* write string before \n */
            res = sock_tcp_write(client, buf, before_nl);
            if (res < 0) {
                break;
            }

            /* insert \r */
            res = sock_tcp_write(client, &cr, 1);
            if (res < 0) {
                break;
            }

            buf = nl;
            len -= before_nl;
        }

        res = sock_tcp_write(client, buf, len);
        if (res < 0) {
            break;
        }
        len -= res;
        buf += res;
    }
    _release();

    return res < 0 ? res : 0;
}

static uint8_t _will(uint8_t option)
{
    switch (option) {
    /* agree to suppress go-ahead packets */
    /* see RFC 858 */
    case TELNET_OPT_SUP_GO_AHEAD: return TELNET_CMD_DO;
    }

    return TELNET_CMD_WONT;
}

static void _process_cmd(uint8_t cmd, uint8_t option)
{
    DEBUG("cmd: %u, option: %u\n", cmd, option);
    switch (cmd) {
    case TELNET_CMD_WILL:
        {
        uint8_t reply[] = {TELNET_CMD_IAC, _will(option), option};
        _write_buffer(reply, sizeof(reply));
        break;
        }
    }
}

static void _send_opts(void)
{
    if (IS_USED(MODULE_STDIO_TELNET)) {
        /* RIOT will echo stdio, disable local echo */
        const uint8_t opt_echo[] = {
            TELNET_CMD_IAC, TELNET_CMD_WILL, TELNET_OPT_ECHO
        };
        _write_buffer(opt_echo, sizeof(opt_echo));
    }
}

static void *telnet_thread(void *arg)
{
    (void)arg;

    static uint8_t rx_buf[64];

    while (1) {
        ssize_t res = sock_tcp_accept(&sock_queue, &client, SOCK_NO_TIMEOUT);
        if (res < 0) {
            DEBUG("accept error: %s\n", strerror(-res));
            continue;
        }

        DEBUG("connected\n");
        _connected();
        _send_opts();

        bool is_cmd = false;
        uint8_t is_option = 0;
        while (1) {
            _acquire();
            res = sock_tcp_read(client, rx_buf, sizeof(rx_buf), SOCK_TCP_TIMEOUT_MS);
            _release();

            if (_want_disconnect) {
                _want_disconnect = false;
                break;
            }

            if (res == -ETIMEDOUT) {
                continue;
            }

            if (res <= 0) {
                break;
            }

            for (int i = 0; i < res; ++i) {
                uint8_t c = rx_buf[i];

                if (is_cmd) {
                    is_cmd = false;
                    switch (c) {
                    case TELNET_CMD_IAC:
                        goto write;
                    case TELNET_CMD_EOF:
                        goto disco;
                    default:
                        is_option = c;
                    }
                    continue;
                }

                if (is_option) {
                    _process_cmd(is_option, c);
                    is_option = 0;
                    continue;
                }

                if (c == TELNET_CMD_IAC) {
                    is_cmd = true;
                    continue;
                }

                if (c == 0) {
                    continue;
                }
write:
                if (IS_USED(MODULE_STDIO_TELNET)) {
                    isrpipe_write_one(&stdin_isrpipe, c);
                }
                else {
                    pipe_write(&_stdin_pipe, &c, 1);
                }
            }
        }
disco:
        _disconnect();

        if (res < 0) {
            DEBUG("telnet: read: %s\n", strerror(res));
        }
    }

    return NULL;
}

int telnet_server_write(const void* buffer, size_t len)
{
    if (connected) {
        int res = _write_buffer(buffer, len);
        return res ? res : (int)len;
    }
    return -ENOTCONN;
}

#ifndef MODULE_STDIO_TELNET
int telnet_server_read(void* buffer, size_t count)
{
    /* block until a connection is established */
    mutex_lock(&connected_mutex);
    int res = pipe_read(&_stdin_pipe, buffer, count);
    if (connected) {
        mutex_unlock(&connected_mutex);
    }
    return res;
}
#endif

void telnet_server_disconnect(void)
{
    if (connected) {
        _want_disconnect = true;
    }
}

int telnet_server_start(void)
{
    sock_tcp_ep_t ep = SOCK_IPV6_EP_ANY;
    ep.port = CONFIG_TELNET_PORT;

    int res = sock_tcp_listen(&sock_queue, &ep, socks, ARRAY_SIZE(socks), 0);
    if (res) {
        return res;
    }

    if (!IS_USED(MODULE_STDIO_TELNET)) {
        /* init RX ringbuffer */
        ringbuffer_init(&_stdin_ringbuffer, _stdin_pipe_buf, sizeof(_stdin_pipe_buf));
        pipe_init(&_stdin_pipe, &_stdin_ringbuffer, NULL);
    }

    /* initiate telnet server */
    thread_create(telnet_stack, sizeof(telnet_stack),
                  THREAD_PRIORITY_MAIN - 1, 0,
                  telnet_thread, NULL, "telnet");

    return 0;
}
