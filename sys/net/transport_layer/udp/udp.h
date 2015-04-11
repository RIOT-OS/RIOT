/*
 * Copyright (C) 2013  INRIA
 * Copyright (C) 2015  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 /**
 * @ingroup transport_layer
 * @{
 *
 * @file
 * @brief   UDP data structs and prototypes
 *
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#ifndef UDP_H_
#define UDP_H_

#include "ipv6.h"
#include "socket_base/types.h"

#ifdef __cplusplus
extern "C" {
#endif
#define UDP_STACK_SIZE (KERNEL_CONF_STACKSIZE_DEFAULT-512)
/**
 * @brief   Stack size used for the UDP thread
 */

/**
 * @brief   Size of the UDP receive buffer
 */
#define UDP_PKT_RECV_BUF_SIZE           (64)

/**
 * @brief   Bind a UDP socket to a given port
 *
 * @note    The name parameter needs to be generalized
 *
 * @param[in] s             socket identifier
 * @param[in] name          address struct defining the port
 * @param[in] namelen       the length of the address struct
 * @param[in] pid           PID of the UDP thread
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int udp_bind_socket(int s, sockaddr6_t *name, int namelen, uint8_t pid);

/**
 * @brief   Receive data via UDP from a given socket
 *
 * @param[in]  s            socket identifier
 * @param[out] buf          buffer to write the received data into
 * @param[in]  len          the maximum number of bytes that fit into the buffer
 * @param[in]  flags        UDP socket flags
 * @param[in]  from         the UDP packets source address
 * @param[in]  fromlen      length of the source address struct
 *
 * @return                  the number of bytes that were received
 */
int32_t udp_recvfrom(int s, void *buf, uint32_t len, int flags,
                     sockaddr6_t *from, uint32_t *fromlen);

/**
 * @brief   Send data via UDP to a given address
 *
 * @param[in] s             socket identifier
 * @param[in] buf           buffer that contains the data to be send
 * @param[in] len           number of bytes to send
 * @param[in] flags         UDP socket flags
 * @param[in] to            the destination address
 * @param[in] tolen         size of the destination address struct
 *
 * @return                  the number of bytes that were sent
 */
int32_t udp_sendto(int s, const void *buf, uint32_t len, int flags,
                   sockaddr6_t *to, socklen_t tolen);

/**
 * @brief   Check if the given socket is a valid UDP socket
 *
 * @note    This functions should be generalized to work also for non IPv6 configurations
 *
 * @param[in] s             socket identifier
 *
 * @return                  true if socket is valid
 * @return                  false if socket is not valid
 */
bool udp_socket_compliancy(int s);

#ifdef __cplusplus
}
#endif

#endif /* UDP_H_ */
/** @} */
