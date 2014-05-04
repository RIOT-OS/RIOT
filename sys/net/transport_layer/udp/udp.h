/**
 * TCP header
 *
 * Copyright (C) 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup socket 
 * @{
 * @file
 * @brief   UDP data structs and prototypes
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 */


#ifndef UDP_H_
#define UDP_H_

#include "ipv6.h"
#include "tl_socket/types.h"

#define UDP_STACK_SIZE 				KERNEL_CONF_STACKSIZE_MAIN
#define UDP_PKT_RECV_BUF_SIZE			(64)

uint16_t udp_csum(ipv6_hdr_t *ipv6_header, udp_hdr_t *udp_header);
void udp_packet_handler(void);

/**
 * @brief Assign the given address to the socket
 *
 * @param[in] s         The socket to be bound
 * @param[in] name      Assign this address to the socket
 * @param[in] namelen   Address type size
 * @param[in] pid       Receiver's thread PID
 *
 * @return On success 0, a negative value otherwise
 */
int udp_bind_socket(int s, sockaddr6_t *name, int namelen, uint8_t pid);

/**
 * @}
 */

#endif /* UDP_H_ */
