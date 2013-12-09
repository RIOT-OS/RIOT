/**
 * Destiny transport layer implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup destiny
 * @{
 * @file    destiny.c
 * @brief   transpor layer functions
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

#include <thread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "destiny.h"
#include "vtimer.h"

#include "socket.h"
#include "tcp.h"
#include "tcp_timer.h"
#include "udp.h"

#ifdef DESTINY_WITH_TCP
char tcp_stack_buffer[TCP_STACK_SIZE];
#endif
char udp_stack_buffer[UDP_STACK_SIZE];

int destiny_init_transport_layer(void)
{
    printf("Initializing transport layer packages. Size of socket_type: %u\n",
           sizeof(socket_internal_t));
    /* SOCKETS */
    memset(sockets, 0, MAX_SOCKETS * sizeof(socket_internal_t));

    /* UDP */
    int udp_thread_pid = thread_create(udp_stack_buffer, UDP_STACK_SIZE,
                                       PRIORITY_MAIN, CREATE_STACKTEST,
                                       udp_packet_handler, "udp_packet_handler");

    if (udp_thread_pid < 0) {
        return -1;
    }

    ipv6_register_next_header_handler(IPV6_PROTO_NUM_UDP, udp_thread_pid);

#ifdef DESTINY_WITH_TCP
    /* TCP */
    timex_t now;
    vtimer_now(&now);
    srand(now.microseconds);
#ifdef TCP_HC
    printf("TCP_HC enabled!\n");
    global_context_counter = rand();
#endif
    global_sequence_counter = rand();

    int tcp_thread_pid = thread_create(tcp_stack_buffer, TCP_STACK_SIZE,
                                       PRIORITY_MAIN, CREATE_STACKTEST,
                                       tcp_packet_handler, "tcp_packet_handler");

    if (tcp_thread_pid < 0) {
        return -1;
    }

    ipv6_register_next_header_handler(IPV6_PROTO_NUM_TCP, tcp_thread_pid);

    if (thread_create(tcp_timer_stack, TCP_TIMER_STACKSIZE, PRIORITY_MAIN + 1,
                      CREATE_STACKTEST, tcp_general_timer, "tcp_general_timer") < 0) {
        return -1;
    }
#endif

    return 0;
}
