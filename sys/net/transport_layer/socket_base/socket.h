/*
 * Copyright (C) 2014  Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _SOCKET_BASE_SOCKET
#define _SOCKET_BASE_SOCKET

#include "cpu.h"

#include "socket_base/socket.h"

#ifdef MODULE_UDP
#include "udp.h"
#endif

#ifdef MODULE_TCP
#include "tcp.h"
#endif

#define MAX_SOCKETS         5
// #define MAX_QUEUED_SOCKETS   2

#define INC_PACKET          0
#define OUT_PACKET          1

#ifdef MODULE_TCP
typedef struct __attribute__((packed)) {
    uint16_t        context_id;
    uint32_t        seq_rcv; // Last received packet values
    uint32_t        ack_rcv;
    uint16_t        wnd_rcv;
    uint32_t        seq_snd; // Last sent packet values
    uint32_t        ack_snd;
    uint16_t        wnd_snd;
    uint8_t         hc_type;
} tcp_hc_context_t;

typedef struct __attribute__((packed)) {
    uint32_t            send_una;
    uint32_t            send_nxt;
    uint16_t            send_wnd;
    uint32_t            send_iss;

    uint32_t            rcv_nxt;
    uint16_t            rcv_wnd;
    uint32_t            rcv_irs;

    timex_t             last_packet_time;
    uint8_t             no_of_retries;
    uint16_t            mss;

    uint8_t             state;

    double              srtt;
    double              rttvar;
    double              rto;

#ifdef TCP_HC
    tcp_hc_context_t    tcp_context;
#endif
} tcp_cb_t;
#endif

typedef struct {
    uint8_t             domain;
    uint8_t             type;
    uint8_t             protocol;
#ifdef MODULE_TCP
    tcp_cb_t            tcp_control;
#endif
    sockaddr6_t         local_address;
    sockaddr6_t         foreign_address;
} socket_t;

typedef struct {
    uint8_t             socket_id;
    uint8_t             recv_pid;
    uint8_t             send_pid;
    socket_t            socket_values;
#ifdef MODULE_TCP
    uint8_t             tcp_input_buffer_end;
    mutex_t             tcp_buffer_mutex;
    uint8_t             tcp_input_buffer[TRANSPORT_LAYER_SOCKET_MAX_TCP_BUFFER];
#endif
} socket_internal_t;

extern socket_internal_t socket_base_sockets[MAX_SOCKETS];

#ifdef __cplusplus
extern "C" {
#endif

socket_internal_t *socket_base_get_socket(int s);
uint16_t socket_base_get_free_source_port(uint8_t protocol);
int socket_base_exists_socket(int socket);
int socket_base_socket(int domain, int type, int protocol);
void socket_base_print_sockets(void);

#ifdef __cplusplus
}
#endif

#endif /* _SOCKET_BASE_SOCKET */
