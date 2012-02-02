/*
 * tcp_hc.h
 *
 *  Created on: 01.02.2012
 *      Author: Oliver
 */

#ifndef TCP_HC_H_
#define TCP_HC_H_

//#define TCP_HC

#include "tcp.h"
#include "sys/net/sixlowpan/sixlowip.h"
#include "socket.h"

#ifdef TCP_HC
void update_tcp_hc_context(bool incoming, tcp_hc_context_t *current_context, tcp_hdr_t *current_tcp_packet);
uint16_t compress_tcp_packet(socket_t *current_tcp_socket, uint8_t *current_tcp_packet, ipv6_hdr_t *temp_ipv6_header, uint8_t flags, uint8_t payload_length);
#endif
#endif /* TCP_HC_H_ */
