#ifndef _DESTINY_SOCKET
#define _DESTINY_SOCKET

#include "cpu.h"

#include "destiny/socket.h"

#include "tcp.h"
#include "udp.h"

#define MAX_SOCKETS         5
// #define MAX_QUEUED_SOCKETS   2

#define INC_PACKET          0
#define OUT_PACKET          1

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

typedef struct __attribute__((packed)) {
    uint8_t             domain;
    uint8_t             type;
    uint8_t             protocol;
    tcp_cb_t            tcp_control;
    sockaddr6_t         local_address;
    sockaddr6_t         foreign_address;
} socket_t;

typedef struct {
    uint8_t             socket_id;
    kernel_pid_t        recv_pid;
    kernel_pid_t        send_pid;
    uint8_t             tcp_input_buffer_end;
    mutex_t             tcp_buffer_mutex;
    socket_t            socket_values;
    uint8_t             tcp_input_buffer[DESTINY_SOCKET_MAX_TCP_BUFFER];
} socket_internal_t;

extern socket_internal_t sockets[MAX_SOCKETS];

socket_internal_t *get_waiting_connection_socket(int socket,
        ipv6_hdr_t *ipv6_header,
        tcp_hdr_t *tcp_header);
void close_socket(socket_internal_t *current_socket);
socket_internal_t *get_socket(int s);
socket_internal_t *get_udp_socket(udp_hdr_t *udp_header);
socket_internal_t *get_tcp_socket(ipv6_hdr_t *ipv6_header,
                                  tcp_hdr_t *tcp_header);
socket_internal_t *new_tcp_queued_socket(ipv6_hdr_t *ipv6_header,
        tcp_hdr_t *tcp_header);
void print_tcp_status(int in_or_out, ipv6_hdr_t *ipv6_header,
                      tcp_hdr_t *tcp_header, socket_t *tcp_socket);
void set_tcp_cb(tcp_cb_t *tcp_control, uint32_t rcv_nxt, uint16_t rcv_wnd,
                uint32_t send_nxt, uint32_t send_una, uint16_t send_wnd);
int check_tcp_consistency(socket_t *current_tcp_socket, tcp_hdr_t *tcp_header, uint8_t tcp_payload_len);
void switch_tcp_packet_byte_order(tcp_hdr_t *current_tcp_packet);
int send_tcp(socket_internal_t *current_socket, tcp_hdr_t *current_tcp_packet,
             ipv6_hdr_t *temp_ipv6_header, uint8_t flags,
             uint8_t payload_length);
bool is_tcp_socket(int s);

#endif /* _DESTINY_SOCKET */
