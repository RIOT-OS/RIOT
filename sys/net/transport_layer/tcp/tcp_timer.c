/**
 * TCP timer implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup transport_layer
 * @{
 * @file    tcp_timer.c
 * @brief   TCP timer
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sixlowpan.h"
#include "thread.h"
#include "vtimer.h"

#include "msg_help.h"
#include "socket.h"

#include "tcp.h"
#include "tcp_timer.h"

void handle_synchro_timeout(socket_internal_t *current_socket)
{
    msg_t send;

    if (thread_getstatus(current_socket->recv_pid) == STATUS_RECEIVE_BLOCKED) {
        timex_t now;
        vtimer_now(&now);

        if ((current_socket->socket_values.tcp_control.no_of_retries == 0) &&
            (timex_uint64(timex_sub(now, current_socket->socket_values.tcp_control.last_packet_time)) > TCP_SYN_INITIAL_TIMEOUT)) {
            current_socket->socket_values.tcp_control.no_of_retries++;
            socket_base_net_msg_send(&send, current_socket->recv_pid, 0, TCP_RETRY);
        }
        else if ((current_socket->socket_values.tcp_control.no_of_retries > 0) &&
                 (timex_uint64(timex_sub(now, current_socket->socket_values.tcp_control.last_packet_time)) >
                  (current_socket->socket_values.tcp_control.no_of_retries *
                   TCP_SYN_TIMEOUT + TCP_SYN_INITIAL_TIMEOUT))) {
            current_socket->socket_values.tcp_control.no_of_retries++;

            if (current_socket->socket_values.tcp_control.no_of_retries >
                TCP_MAX_SYN_RETRIES) {
                socket_base_net_msg_send(&send, current_socket->recv_pid, 0, TCP_TIMEOUT);
            }
            else {
                socket_base_net_msg_send(&send, current_socket->recv_pid, 0, TCP_RETRY);
            }
        }
    }
}

void handle_established(socket_internal_t *current_socket)
{
    msg_t send;
    double current_timeout = current_socket->socket_values.tcp_control.rto;

    if (current_timeout < SECOND) {
        current_timeout = SECOND;
    }


    if ((current_socket->socket_values.tcp_control.send_nxt >
         current_socket->socket_values.tcp_control.send_una) &&
        (thread_getstatus(current_socket->send_pid) == STATUS_RECEIVE_BLOCKED)) {
        for (uint8_t i = 0; i < current_socket->socket_values.tcp_control.no_of_retries;
             i++) {
            current_timeout *= 2;
        }

        timex_t now;
        vtimer_now(&now);

        if (current_timeout > TCP_ACK_MAX_TIMEOUT) {
            socket_base_net_msg_send(&send, current_socket->send_pid, 0, TCP_TIMEOUT);
        }
        else if (timex_uint64(timex_sub(now, current_socket->socket_values.tcp_control.last_packet_time)) >
                 current_timeout) {
            current_socket->socket_values.tcp_control.no_of_retries++;
            socket_base_net_msg_send(&send, current_socket->send_pid, 0, TCP_RETRY);
        }
    }
}

void check_sockets(void)
{
    uint8_t i = 1;

    while (i < MAX_SOCKETS + 1) {
        socket_internal_t *current_socket = socket_base_get_socket(i);

        if (tcp_socket_compliancy(i)) {
            switch (current_socket->socket_values.tcp_control.state) {
                case TCP_ESTABLISHED: {
                    handle_established(current_socket);
                    break;
                }

                case TCP_SYN_SENT: {
                    handle_synchro_timeout(current_socket);
                    break;
                }

                case TCP_SYN_RCVD: {
                    handle_synchro_timeout(current_socket);
                    break;
                }

                default: {
                    break;
                }
            }
        }

        i++;
    }
}

void inc_global_variables(void)
{
    mutex_lock(&global_sequence_counter_mutex);
    global_sequence_counter += rand();
    mutex_unlock(&global_sequence_counter_mutex);
#ifdef TCP_HC
    mutex_lock(&global_context_counter_mutex);
    global_context_counter += rand();
    mutex_unlock(&global_context_counter_mutex);
#endif
}

void *tcp_general_timer(void *arg)
{
    (void) arg;

    vtimer_t tcp_vtimer;
    timex_t interval = timex_set(0, TCP_TIMER_RESOLUTION);

    while (1) {
        inc_global_variables();
        check_sockets();

        vtimer_set_wakeup(&tcp_vtimer, interval, sched_active_pid);
        thread_sleep();

        vtimer_remove(&tcp_vtimer);
    }
}
