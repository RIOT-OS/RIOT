/*
 * Copyright (C) 2013  Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file    tcp_hc.c
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 */
#ifndef TCP_HC_H_
#define TCP_HC_H_

#include "sixlowpan.h"

#include "socket_base/socket.h"

#include "tcp.h"

#ifdef TCP_HC

#define FULL_HEADER                 1
#define MOSTLY_COMPRESSED_HEADER    2
#define COMPRESSED_HEADER           3

void update_tcp_hc_context(bool incoming, socket_internal_t *current_socket, tcp_hdr_t *current_tcp_packet);
uint16_t compress_tcp_packet(socket_internal_t *current_socket, uint8_t *current_tcp_packet, ipv6_hdr_t *temp_ipv6_header, uint8_t flags, uint8_t payload_length);
socket_internal_t *decompress_tcp_packet(ipv6_hdr_t *temp_ipv6_header);
#endif
#endif /* TCP_HC_H_ */
/**
 * @}
 */
