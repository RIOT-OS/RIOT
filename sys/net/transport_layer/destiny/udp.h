/**
 * Destiny TCP header
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup destiny
 * @{
 * @file
 * @brief   UDP data structs and prototypes
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 */


#ifndef UDP_H_
#define UDP_H_

#include "ipv6.h"
#include "destiny/types.h"

#define UDP_STACK_SIZE                  KERNEL_CONF_STACKSIZE_MAIN
#define UDP_PKT_RECV_BUF_SIZE           (64)

uint16_t udp_csum(ipv6_hdr_t *ipv6_header, udp_hdr_t *udp_header);
void *udp_packet_handler(void *);

/**
 * @}
 */

#endif /* UDP_H_ */
