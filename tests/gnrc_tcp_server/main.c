/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <errno.h>
#include "thread.h"
#include "net/af.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/tcp.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Number of possible parallel connections */
#ifndef CONNS
#define CONNS (1)
#endif

/* Amount of data to transmit */
#ifndef NBYTE
#define NBYTE (2048)
#endif

/* Test Pattern used by Client Application */
#ifndef TEST_PATERN_CLI
#define TEST_PATERN_CLI (0xF0)
#endif

/* Test Pattern used by Server Application */
#ifndef TEST_PATERN_SRV
#define TEST_PATERN_SRV (0xA7)
#endif

uint8_t bufs[CONNS][NBYTE];
uint8_t stacks[CONNS][THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];

/* ifconfig shell command */
extern int _netif_config(int argc, char **argv);

/* Server Thread */
void *srv_thread(void *arg);

int main(void)
{

    /* Print all configured addresses of the server */
    printf("\nStarting server: LOCAL_PORT=%d, CONNS=%d, NBYTE=%d\n\n", LOCAL_PORT, CONNS, NBYTE);
    printf("Printing Servers Network Configuration:\n");
    _netif_config(0, NULL);

    /* Start Threads to handle each connection */
    for (int i = 0; i < CONNS; i += 1) {
        thread_create((char *) stacks[i], sizeof(stacks[i]), THREAD_PRIORITY_MAIN, 0, srv_thread,
                      (void *) i, NULL);
    }
    return 0;
}

void *srv_thread(void *arg)
{
    int tid = (int) arg;
    uint32_t cycles = 0;
    uint32_t cycles_ok = 0;
    uint32_t failed_payload_verifications = 0;

    /* Transmission control block */
    gnrc_tcp_tcb_t tcb;

    /* Connection handling code */
    printf("Server running: TID=%d\n", tid);
    while (1) {
        /* Initialize tcb struct */
        gnrc_tcp_tcb_init(&tcb);

        /* Connect to Peer */
        int ret = gnrc_tcp_open_passive(&tcb, AF_INET6, NULL, LOCAL_PORT);
        switch (ret) {
            case 0:
                DEBUG("TID=%d : gnrc_tcp_open_passive() : 0 : ok\n", tid);
                break;

            case -EISCONN:
                printf("TID=%d : gnrc_tcp_open_passive() : -EISCONN\n", tid);
                return 0;

            case -EINVAL:
                printf("TID=%d : gnrc_tcp_open_passive() : -EINVAL\n", tid);
                return 0;

            case -EAFNOSUPPORT:
                printf("TID=%d : gnrc_tcp_open_passive() : -EAFNOSUPPORT\n", tid);
                return 0;

            case -ENOMEM:
                printf("TID=%d : gnrc_tcp_open_passive() : -ENOMEM\n", tid);
                return 0;

            default:
                printf("TID=%d : gnrc_tcp_open_passive() : %d\n", tid, ret);
                return 0;
        }

        /* Receive Data, stop if errors were found */
        for (size_t rcvd = 0; rcvd < sizeof(bufs[tid]) && ret >= 0; rcvd += ret) {
            ret = gnrc_tcp_recv(&tcb, (void *) (bufs[tid] + rcvd), sizeof(bufs[tid]) - rcvd,
                                GNRC_TCP_CONNECTION_TIMEOUT_DURATION);
            switch (ret) {
                case -ENOTCONN:
                    printf("TID=%d : gnrc_tcp_rcvd() : -ENOTCONN\n", tid);
                    break;

                case -EAGAIN:
                    printf("TID=%d : gnrc_tcp_rcvd() : -EAGAIN : retry after 10sec\n", tid);
                    ret = 0;
                    xtimer_sleep(10);
                    break;

                case -ECONNABORTED:
                    printf("TID=%d : gnrc_tcp_rcvd() : -ECONNABORTED\n", tid);
                    break;

                case -ECONNRESET:
                    printf("TID=%d : gnrc_tcp_rcvd() : -ECONNRESET\n", tid);
                    break;

                case -ETIMEDOUT:
                    printf("TID=%d : gnrc_tcp_rcvd() : -ETIMEDOUT\n", tid);
                    break;

                default:
                    if (ret >= 0) {
                        DEBUG("TID=%d : gnrc_tcp_rcvd() : %d Bytes read\n", tid, ret);
                    }
                    else {
                        printf("TID=%d : gnrc_tcp_rcvd() : %d\n", tid, ret);
                        return 0;
                    }
              }
        }

        /* Check received pattern */
       for (size_t i = 0; i < sizeof(bufs[tid]); ++i) {
             if (bufs[tid][i] != TEST_PATERN_CLI) {
                printf("TID=%d : Payload verfication failed\n", tid);
                failed_payload_verifications += 1;
                break;
            }
        }

        /* Fill Buffer with a test pattern */
        for (size_t i = 0; i < sizeof(bufs[tid]); ++i) {
            bufs[tid][i] = TEST_PATERN_SRV;
        }

        /* Send Data, stop if errors were found */
        for (size_t sent = 0; sent < sizeof(bufs[tid]) && ret >= 0; sent += ret) {
            ret = gnrc_tcp_send(&tcb, bufs[tid] + sent, sizeof(bufs[tid]) - sent, 0);
            switch (ret) {
                case -ENOTCONN:
                    printf("TID=%d : gnrc_tcp_send() : -ENOTCONN\n", tid);
                    break;

                case -ECONNABORTED:
                    printf("TID=%d : gnrc_tcp_send() : -ECONNABORTED\n", tid);
                    break;

                case -ETIMEDOUT:
                    printf("TID=%d : gnrc_tcp_send() : -ETIMEDOUT\n", tid);
                    break;

                case -ECONNRESET:
                    printf("TID=%d : gnrc_tcp_send() : -ECONNRESET\n", tid);
                    break;

                default:
                    if (ret >= 0) {
                        DEBUG("TID=%d : gnrc_tcp_send() : %d Bytes sent.\n", tid, ret);
                    }
                    else {
                        printf("TID=%d : gnrc_tcp_send() : %d\n", tid, ret);
                        return 0;
                    }
              }
        }

        /* Close Connection */
        gnrc_tcp_close(&tcb);

        /* Gather Data */
        cycles += 1;
        if (ret >= 0) {
            cycles_ok += 1;
        }
        printf("TID=%d : %"PRIi32" test cycles completed. %"PRIi32" ok, %"PRIi32" faulty",
               tid, cycles, cycles_ok, cycles - cycles_ok);
        printf(", %"PRIi32" failed payload verifications\n", failed_payload_verifications);
    }
    return 0;
}
