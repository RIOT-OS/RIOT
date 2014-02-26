#ifndef SOCKET_INTERNAL_H

#define SOCKET_INTERNAL_H

#include "cpu.h"
#include "socket_internal.h"
#include "tl_socket/socket.h"
#include "tl_socket/types.h"

#define MAX_SOCKETS			5
// #define MAX_QUEUED_SOCKETS	2

#define INC_PACKET			0
#define OUT_PACKET			1

typedef struct __attribute__((packed)) {
    uint16_t 		context_id;
    uint32_t 		seq_rcv; // Last received packet values
    uint32_t 		ack_rcv;
    uint16_t		wnd_rcv;
    uint32_t		seq_snd; // Last sent packet values
    uint32_t		ack_snd;
    uint16_t		wnd_snd;
    uint8_t			hc_type;
} tcp_hc_context_t;

typedef struct __attribute__((packed)) {
    uint32_t			send_una;
    uint32_t			send_nxt;
    uint16_t			send_wnd;
    uint32_t			send_iss;

    uint32_t			rcv_nxt;
    uint16_t			rcv_wnd;
    uint32_t			rcv_irs;

    timex_t				last_packet_time;
    uint8_t				no_of_retries;
    uint16_t			mss;

    uint8_t 			state;

    double				srtt;
    double				rttvar;
    double				rto;

#ifdef TCP_HC
    tcp_hc_context_t	tcp_context;
#endif
} tcp_cb_t;

typedef struct __attribute__((packed)) {
    uint8_t				domain;
    uint8_t				type;
    uint8_t				protocol;
    tcp_cb_t			tcp_control;
    sockaddr6_t			local_address;
    sockaddr6_t			foreign_address;
} socket_t;

typedef struct __attribute__((packed)) {
    uint8_t				socket_id;
    uint8_t				recv_pid;
    uint8_t				send_pid;
    uint8_t				tcp_input_buffer_end;
    mutex_t				tcp_buffer_mutex;
    socket_t			socket_values;
    uint8_t				tcp_input_buffer[TL_SOCKET_SOCKET_MAX_TCP_BUFFER];
} socket_internal_t;

void close_socket(socket_internal_t *current_socket);

socket_internal_t *get_socket(int s);

bool socket_is_udp(int s);
bool socket_is_tcp(int s);
int socket_exist(int socket);
uint16_t socket_get_free_source_port(uint8_t protocol);
void tl_socket_print_sockets(void);
#endif /* SOCKET_INTERNAL_H */
