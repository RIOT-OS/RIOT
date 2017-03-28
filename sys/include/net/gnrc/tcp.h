/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_tcp TCP
 * @ingroup     net_gnrc
 * @brief       RIOT's tcp implementation for the gnrc stack
 *
 * @{
 *
 * @file
 * @brief       GNRC TCP API
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef GNRC_TCP_H
#define GNRC_TCP_H

#include <stdint.h>
#include "net/gnrc/pkt.h"
#include "net/gnrc/tcp/tcb.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize and start TCP
 *
 * @return   PID of TCP thread on success
 * @return   -1 if thread is already running.
 * @return   -EINVAL, if priority is greater than or equal SCHED_PRIO_LEVELS
 * @return   -EOVERFLOW, if there are too many threads running.
 */
int gnrc_tcp_init(void);

/**
 * @brief Initialize Transmission Control Block (tcb)
 * @pre tcb must not be NULL.
 *
 * @param[in,out] tcb          Transmission that should be initialized.
 */
void gnrc_tcp_tcb_init(gnrc_tcp_tcb_t *tcb);

 /**
  * @brief Opens a connection actively.
  *
  * @pre gnrc_tcp_tcb_init() must have been successfully called.
  * @pre tcb must not be NULL
  * @pre target_addr must not be NULL.
  * @pre target_port must not be 0.
  *
  * @note Blocks until a connection has been established or an error occured.
  *
  * @param[in,out] tcb          This connections Transmission control block.
  * @param[in] address_family   Address Family of @p target_addr.
  * @param[in] target_addr      Pointer to target address.
  * @param[in] target_port      Targets port number.
  * @param[in] local_port       If zero or GNRC_TCP_PORT_UNSPEC, the connections
  *                             source port is randomly chosen. If local_port is non-zero
  *                             the local_port is used as source port.
  *
  * @return   Zero on success.
  * @return   -EAFNOSUPPORT if @p address_family is not supported.
  * @return   -EINVAL if @p address_family is not the same the address_family use by the tcb.
  * @return   -EISCONN if transmission control block is already in use.
  * @return   -ENOMEM if the receive buffer for the tcb could not be allocated.
  *           Increase "GNRC_TCP_RCV_BUFFERS".
  * @return   -EADDRINUSE if @p local_port is already used by another connection.
  * @return   -ETIMEDOUT if the connection could not be opened.
  * @return   -ECONNREFUSED if the connection was resetted by the peer.
  */
int gnrc_tcp_open_active(gnrc_tcp_tcb_t *tcb,  const uint8_t address_family,
                         const uint8_t *target_addr, const uint16_t target_port,
                         const uint16_t local_port);

/**
 * @brief Opens a connection passively, by waiting for an incomming request.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre tcb must not be NULL.
 * @pre if local_addr is not NULL, local_addr must be assigned to a network interface.
 * @pre if local_port is not zero.
 *
 * @note Blocks until a connection has been established (incomming connection request
 *       to @p local_port) or an error occured.
 *
 * @param[in,out] tcb          This connections Transmission control block.
 * @param[in] address_family   Address Family of @p local_addr.
 *                             If local_addr == NULL, address_family is ignored.
 * @param[in] local_addr       If not NULL the connection is bound to the address @p local_addr.
 *                             If NULL a connection request to every local ip address is valid.
 * @param[in] local_port       Portnumber that should used for incomming connection requests.
 *
 * @return   Zero on success
 * @return   -EAFNOSUPPORT if local_addr != NULL and @p address_family is not supported.
 * @return   -EINVAL if @p address_family is not the same the address_family use by the tcb.
 * @return   -EISCONN if transmission control block is already in use.
 * @return   -ENOMEM if the receive buffer for the tcb could not be allocated.
 *           Increase "GNRC_TCP_RCV_BUFFERS".
 */
int gnrc_tcp_open_passive(gnrc_tcp_tcb_t *tcb,  const uint8_t address_family,
                          const uint8_t *local_addr, const uint16_t local_port);

/**
 * @brief Transmit Data to Peer.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre tcb must not be NULL.
 * @pre data must not be NULL.
 *
 * @note Blocks until up to @p len bytes were transmitted or an error occured.
 *
 * @param[in,out] tcb                    This connections Transmission control block.
 * @param[in] data                       Pointer to the data that should be transmitted.
 * @param[in] len                        Number of bytes that should be transmitted.
 * @param[in] user_timeout_duration_us   If not zero and there were not data transmitted
 *                                       successfully, the function call returns after
 *                                       user_timeout_duration_us. If zero not timeout will be
 *                                       triggered.
 *
 * @return   On success, the number of successfully transmitted bytes.
 * @return   -ENOTCONN if connection is not established.
 * @return   -ECONNRESET if connection was resetted by the peer.
 * @return   -ECONNABORTED if the connection was aborted.
 * @return   -ETIMEDOUT if @p user_timeout_duration_us expired.
 */
ssize_t gnrc_tcp_send(gnrc_tcp_tcb_t *tcb, const void *data, const size_t len,
                      const uint32_t user_timeout_duration_us);

/**
 * @brief Receive Data from the Peer.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre tcb must not be NULL.
 * @pre data must not be NULL.
 *
 * @note Function blocks if user_timeout_duration_us is not zero.
 *
 * @param[in,out] tcb                    This connections Transmission control block.
 * @param[out] data                      Pointer to the buffer where the received data
 *                                       should be copied into.
 * @param[in] max_len                    Maximum amount to bytes that should be reeived.
 *                                       Should not exceed size of @p data.
 * @param[in] user_timeout_duration_us   Timeout for receive in microseconds. If zero and no data
 *                                       is available, the function returns immediately. If not
 *                                       zero the function block until data is available or
 *                                       user_timeout_duration_us microseconds have passed.
 *
 * @return   On success, the number of bytes read into @p data.
 * @return   -ENOTCONN if connection is not established.
 * @return   -EAGAIN if  user_timeout_duration_us is zero and no data is available.
 * @return   -ECONNRESET if connection was resetted by the peer.
 * @return   -ECONNABORTED if the connection was aborted.
 * @return   -ETIMEDOUT if @p user_timeout_duration_us expired.
 */
ssize_t gnrc_tcp_recv(gnrc_tcp_tcb_t *tcb, void *data, const size_t max_len,
                      const uint32_t user_timeout_duration_us);

/**
 * @brief Close a tcp connection.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre tcb must not be NULL.
 *
 * @param[in,out] tcb   This connections Transmission control block.
 *
 * @return   Zero on success.
 */
int gnrc_tcp_close(gnrc_tcp_tcb_t *tcb);

/**
 * @brief Set checksum calculated from tcp and network-layer header in tcp-header.
 *
 * @param[in] hdr          gnrc_pktsnip that contains tcp header.
 * @param[in] pseudo_hdr   gnrc_pktsnip that contains networklayer header.
 *
 * @return   zero on succeed.
 * @return   -EFAULT if hdr or pseudo_hdr were NULL
 * @return   -EBADMSG if hdr is not of type GNRC_NETTYPE_TCP
 * @return   -ENOENT if pseudo_hdr protocol is unsupported.
 */
int gnrc_tcp_calc_csum(const gnrc_pktsnip_t *hdr, const gnrc_pktsnip_t *pseudo_hdr);

/**
 * @brief Adds a tcp header to a given payload. Be carefull, leads to huge headers.
 *        Allocates all option bytes
 *
 * @param[in] payload   payload that follows the tcp header
 * @param[in] src       Source port in host byte order
 * @param[in] dst       Destination port in host byte order
 *
 * @return   NULL, if paket buffer is full
 * @return   Not NULL on success
 */
gnrc_pktsnip_t *gnrc_tcp_hdr_build(gnrc_pktsnip_t *payload, uint16_t src, uint16_t dst);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TCP_H */
/** @} */
