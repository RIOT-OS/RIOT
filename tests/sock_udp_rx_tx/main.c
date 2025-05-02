/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for sending while receiving
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "time_units.h"
#include "net/sock/udp.h"
#include "shell.h"
#include "shell_commands.h"
#include "net/ipv6/addr.h"
#include "random.h"

#define MAIN_QUEUE_SIZE     (8)

#define PAYLOAD_LEN_MAX     1024
#define PORT_UDP            12345

#define FRAME_GAP_MIN_US    1000
#define FRAME_GAP_MAX_US    3000

static char sender_thread_stack[THREAD_STACKSIZE_DEFAULT];
static char receiver_thread_stack[THREAD_STACKSIZE_DEFAULT];

static unsigned _num_tx, _num_rx;
static unsigned _num_tx_byte, _num_rx_byte;

static mutex_t _send_lock = MUTEX_INIT_LOCKED;

static void *server_thread(void *ctx)
{
    int res;
    netif_t *netif = ctx;

    msg_t msg_queue[MAIN_QUEUE_SIZE];
    msg_init_queue(msg_queue, MAIN_QUEUE_SIZE);

    if (netif == NULL) {
        puts("not enough interfaces");
    }

    sock_udp_t sock;
    const sock_udp_ep_t local = {
        .family = AF_INET6,
        .netif = netif_get_id(netif),
        .port = PORT_UDP,
    };

    /* create socket for incoming UDP traffic */
    res = sock_udp_create(&sock, &local, NULL, 0);
    if (res) {
        printf("udp server: can't create socket: %d\n", res);
        return NULL;
    }

    while (1) {
        sock_udp_ep_t remote;
        static uint8_t buf[PAYLOAD_LEN_MAX];
        res = sock_udp_recv(&sock, buf, sizeof(buf), SOCK_NO_TIMEOUT, &remote);
        if (res < 0) {
            printf("rx error: %d\n", res);
        }

        /* echo back payload */
        sock_udp_send(NULL, buf, res, &remote);

        _num_rx++;
        _num_rx_byte += res;
    }

    return NULL;
}

static void *sender_thread(void *ctx)
{
    int res;
    netif_t *netif = ctx;

    msg_t msg_queue[MAIN_QUEUE_SIZE];
    msg_init_queue(msg_queue, MAIN_QUEUE_SIZE);

    if (netif == NULL) {
        puts("not enough interfaces");
    }

    sock_udp_t sock;
    const sock_udp_ep_t remote = {
        .family = AF_INET6,
        .port = PORT_UDP,
        .netif = netif_get_id(netif),
        .addr = IPV6_ADDR_ALL_NODES_LINK_LOCAL,
    };

    /* create socket for outgoing UDP traffic */
    res = sock_udp_create(&sock, NULL, &remote, 0);
    if (res) {
        printf("udp sender: can't create socket: %d\n", res);
        return NULL;
    }

    while (1) {
        static uint8_t buf[PAYLOAD_LEN_MAX];
        size_t len = random_uint32_range(sizeof(buf) / 2, sizeof(buf));
        memset(buf, 0x55, len);

        /* block here if sending is stopped */
        mutex_lock(&_send_lock);

        res = sock_udp_send(&sock, buf, len, NULL);
        if (res < 0) {
            printf("tx error: %d\n", res);
        } else {
            _num_tx++;
            _num_tx_byte += res;
        }

        mutex_unlock(&_send_lock);
#ifdef BOARD_NATIVE
        ztimer_sleep(ZTIMER_USEC, 20);
#else
        ztimer_sleep(ZTIMER_USEC,
                     10 * len + random_uint32_range(FRAME_GAP_MIN_US, FRAME_GAP_MAX_US));
#endif
    }

    return NULL;
}

static int _cmd_print_stats(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    static uint32_t last_now_ms;
    static unsigned last_num_tx, last_num_rx;
    static unsigned last_num_tx_byte, last_num_rx_byte;

    uint32_t diff_ms = ztimer_now(ZTIMER_MSEC) - last_now_ms;

    if (diff_ms == 0) {
        return -1;
    }

    printf("TX: %u (%u bytes), %lu/s (%lu bytes/s)\n",
           _num_tx, _num_tx_byte,
           MS_PER_SEC * (_num_tx - last_num_tx) / diff_ms,
           MS_PER_SEC * (_num_tx_byte - last_num_tx_byte) / diff_ms);
    printf("RX: %u (%u bytes), %lu/s (%lu bytes/s)\n",
           _num_rx, _num_rx_byte,
           MS_PER_SEC * (_num_rx - last_num_rx) / diff_ms,
           MS_PER_SEC * (_num_rx_byte - last_num_rx_byte) / diff_ms);

    last_now_ms = ztimer_now(ZTIMER_MSEC);
    last_num_tx = _num_tx;
    last_num_rx = _num_rx;
    last_num_tx_byte = _num_tx_byte;
    last_num_rx_byte = _num_rx_byte;

    return 0;
}

static int _cmd_start(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    mutex_unlock(&_send_lock);

    return 0;
}

static int _cmd_stop(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    mutex_lock(&_send_lock);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "stats", "RX / TX stats", _cmd_print_stats },
    { "start", "Start sending", _cmd_start },
    { "stop", "Stop sending", _cmd_stop },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_t msg_queue[MAIN_QUEUE_SIZE];
    msg_init_queue(msg_queue, MAIN_QUEUE_SIZE);

    netif_t *netif = netif_iter(NULL);

    thread_create(receiver_thread_stack, sizeof(receiver_thread_stack),
                  THREAD_PRIORITY_MAIN - 2, THREAD_CREATE_STACKTEST,
                  server_thread, netif, "UDP server");
    thread_create(sender_thread_stack, sizeof(sender_thread_stack),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                  sender_thread, netif_iter(netif), "UDP sender");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
