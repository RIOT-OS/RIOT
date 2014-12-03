/**
 * UDP header
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup transport_layer
 * @{
 * @file
 * @brief   UDP data structs and prototypes
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 */


#ifndef UDP_H_
#define UDP_H_

#include "ipv6.h"
#include "socket_base/types.h"

#ifdef __cplusplus
extern "C" {
#endif
#define UDP_STACK_SIZE (KERNEL_CONF_STACKSIZE_DEFAULT-512)
#define UDP_PKT_RECV_BUF_SIZE           (64)

int udp_bind_socket(int s, sockaddr6_t *name, int namelen, uint8_t pid);
int32_t udp_recvfrom(int s, void *buf, uint32_t len, int flags, sockaddr6_t *from, uint32_t *fromlen);
int32_t udp_sendto(int s, const void *buf, uint32_t len, int flags, sockaddr6_t *to, uint32_t tolen);
bool udp_socket_compliancy(int s);
int32_t udp_recvfrom(int s, void *buf, uint32_t len, int flags, sockaddr6_t *from, uint32_t *fromlen);
int32_t udp_sendto(int s, const void *buf, uint32_t len, int flags, sockaddr6_t *to, socklen_t tolen);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* UDP_H_ */
