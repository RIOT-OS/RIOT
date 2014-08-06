/**
 * Destiny transport layer implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup destiny
 * @{
 * @file    destiny.c
 * @brief   transpor layer functions
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "thread.h"

#include "destiny.h"
#include "vtimer.h"

#include "socket.h"
#include "tcp.h"
#include "tcp_timer.h"
#include "udp.h"

char tcp_stack_buffer[TCP_STACK_SIZE];
char udp_stack_buffer[UDP_STACK_SIZE];

char tcp_timer_stack[TCP_TIMER_STACKSIZE];

int destiny_init_transport_layer(void)
{
    printf("Initializing transport layer packages. Size of socket_type: %u\n",
           (unsigned int) sizeof(socket_internal_t));
    /* SOCKETS */
    memset(sockets, 0, MAX_SOCKETS * sizeof(socket_internal_t));

    /* UDP */
    kernel_pid_t udp_thread_pid = thread_create(udp_stack_buffer, UDP_STACK_SIZE,
                                       PRIORITY_MAIN, CREATE_STACKTEST,
                                       udp_packet_handler, NULL, "udp_packet_handler");

    if (udp_thread_pid == KERNEL_PID_UNDEF) {
        return -1;
    }

    ipv6_register_next_header_handler(IPV6_PROTO_NUM_UDP, udp_thread_pid);

    /* TCP */
    timex_t now;
    vtimer_now(&now);
    srand(timex_uint64(now));
#ifdef TCP_HC
    printf("TCP_HC enabled!\n");
    global_context_counter = rand();
#endif
    global_sequence_counter = rand();

    kernel_pid_t tcp_thread_pid = thread_create(tcp_stack_buffer, TCP_STACK_SIZE,
                                       PRIORITY_MAIN, CREATE_STACKTEST,
                                       tcp_packet_handler, NULL, "tcp_packet_handler");

    if (tcp_thread_pid == KERNEL_PID_UNDEF) {
        return -1;
    }

    ipv6_register_next_header_handler(IPV6_PROTO_NUM_TCP, tcp_thread_pid);

    if (thread_create(tcp_timer_stack, TCP_TIMER_STACKSIZE, PRIORITY_MAIN + 1,
                      CREATE_STACKTEST, tcp_general_timer, NULL, "tcp_general_timer") < 0) {
        return -1;
    }

    return 0;
}
