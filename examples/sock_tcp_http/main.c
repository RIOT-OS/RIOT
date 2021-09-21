/*
 * Copyright (C) 2021 Hugues Larrive
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
 * @brief       A simple web sensor example
 *
 * @author      Hugues Larrive <hlarrive@pm.me>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"

#include "net/sock/tcp.h"
#include "index.html.h"
#include "thread.h"

#include "dht.h"
#include "dht_params.h"
#include "periph/cpuid.h"

#define SOCK_QUEUE_LEN  (1U)
#define GET_BUF_LEN     (10U)

static dht_t dev;
static int cpuid;

sock_tcp_t sock_queue[SOCK_QUEUE_LEN];
static sock_tcp_t *sock;

static char webui_stack[THREAD_STACKSIZE_DEFAULT];

static const char html_header[] =
    "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n";
static const char plain_header[] =
    "HTTP/1.0 200 OK\r\nContent-type: text/plain\r\n\r\n";

static char get[GET_BUF_LEN + 1];

static void _http_srv(char *get)
{
    char json[43];
    int16_t temp, rh;

    if (strstr(get, "GET /?json")) {
        dht_read(&dev, &temp, &rh);
        sprintf(json,
            "{\"cpuid\":\"%x\",\"temp\":%d,\"rh\":%d}", cpuid, temp, rh);

        sock_tcp_write(sock, plain_header, strlen(plain_header));
        sock_tcp_write(sock, json, strlen(json));
    }
    else if(strstr(get, "GET /")) {
        sock_tcp_write(sock, html_header, strlen(html_header));
        sock_tcp_write(sock, index_html, strlen(index_html));
    }
}

void *webui_thread(void *arg)
{
    (void) arg;

    char buf[sizeof(get)] = {0}, buf_end[5] = {0};

    sock_tcp_ep_t local = SOCK_IPV4_EP_ANY;
    sock_tcp_queue_t queue;
    local.port = 80;

    sock_tcp_listen(&queue, &local, sock_queue, SOCK_QUEUE_LEN, 0);

    while (1) {
        if (sock_tcp_accept(&queue, &sock, SOCK_NO_TIMEOUT) < 0) {
            puts("Error accepting new sock");
        }
        else {
            int read_res = 0;
            puts("Reading data");
            while (read_res >= 0) {
                read_res = sock_tcp_read(sock, &buf, sizeof(buf) - 1,
                                         SOCK_NO_TIMEOUT);
                if (read_res < 0) {
                    break;
                }
                else {
                    buf[read_res] = '\0';
                    if (strstr(buf, "GET /")) strncpy(get, buf, 10);
                    if (read_res < 4) {
                        for (int i = 3; i >= 0; --i) {
                            buf_end[i] = 3 - i < read_res
                                ? buf[read_res - 4 + i]
                                : buf[sizeof(buf) - 5 + i + read_res];
                        }
                    }
                    if (strstr(buf, "\r\n\r\n")
                        || strstr(buf_end, "\r\n\r\n")) {
                        _http_srv(get);
                        break;
                    }
                }
            }
            sock_tcp_disconnect(sock);
            puts("Disconnected");
        }
    }
    sock_tcp_stop_listen(&queue);

    return NULL;
}

static void webui_init(void)
{
    (void) thread_create(
            webui_stack, sizeof(webui_stack),
            THREAD_PRIORITY_MAIN - 1,
            THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
            webui_thread, NULL, "webui");
}    

static const shell_command_t shell_commands[] = {
    { NULL, NULL, NULL }
};

int main(void)
{
    cpuid_get(&cpuid);

    dht_init(&dev, &dht_params[0]);

    webui_init();
    
    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
