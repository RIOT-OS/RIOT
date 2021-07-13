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
 * @brief       RIOT's TCP implementation for the GNRC network stack.
 *
 * @{
 *
 * @file
 * @brief       GNRC TCP API
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef NET_GNRC_TCP_H
#define NET_GNRC_TCP_H

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
 * @brief Address information for a single TCP connection endpoint.
 * @extends sock_tcp_ep_t
 */
typedef struct {
    int family;                            /**< IP address family. */
    union {
#ifdef MODULE_GNRC_IPV6
        uint8_t ipv6[sizeof(ipv6_addr_t)]; /**< IPv6 address storage */
#endif
        uint8_t dummy;                     /**< Enable build without network module */
    } addr;                                /**< IP address storage */
    uint16_t netif;                        /**< Network interface ID */
    uint16_t port;                         /**< Port number (in host byte order) */
} gnrc_tcp_ep_t;

/**
 * @brief Initialize TCP connection endpoint.
 *
 * @param[in,out] ep          Endpoint to initialize.
 * @param[in]     family      Address family of @p addr.
 * @param[in]     addr        Address for endpoint.
 * @param[in]     addr_size   Size of @p addr.
 * @param[in]     port        Port number for endpoint.
 * @param[in]     netif       Network interface to use.
 *
 * @return   0 on success.
 * @return   -EAFNOSUPPORT if @p address_family is not supported.
 * @return   -EINVAL if @p addr_size does not match @p family.
 */
int gnrc_tcp_ep_init(gnrc_tcp_ep_t *ep, int family, const uint8_t *addr, size_t addr_size,
                     uint16_t port, uint16_t netif);

/**
 * @brief Construct TCP connection endpoint from string.
 * @note This function expects @p str in the IPv6 "URL" notation.
 *       The following strings specify a valid endpoint:
 *       - [fe80::0a00:27ff:fe9f:7a5b%5]:8080 (with Port and Interface)
 *       - [2001::0200:f8ff:fe21:67cf]:8080   (with Port)
 *       - [2001::0200:f8ff:fe21:67cf]        (addr only)
 *
 * @param[in,out] ep    Endpoint to initialize.
 * @param[in]     str   String containing IPv6-Address to parse.
 *
 * @return   0 on success.
 * @return   -EINVAL if parsing of @p str failed.
 */
int gnrc_tcp_ep_from_str(gnrc_tcp_ep_t *ep, const char *str);

/**
 * @brief Initialize TCP
 *
 * @return   PID of TCP thread on success
 * @return   -1 if TCB is already running.
 * @return   -EINVAL, if priority is greater than or equal SCHED_PRIO_LEVELS
 * @return   -EOVERFLOW, if there are too many threads running.
 */
int gnrc_tcp_init(void);

/**
 * @brief Initialize Transmission Control Block (TCB)
 * @pre @p tcb must not be NULL.
 *
 * @param[in,out] tcb   TCB that should be initialized.
 */
void gnrc_tcp_tcb_init(gnrc_tcp_tcb_t *tcb);

/**
 * @brief Initialize Transmission Control Block (TCB) queue
 * @pre @p queue must not be NULL.
 *
 * @param[in,out] queue   TCB queue to initialize.
 */
void gnrc_tcp_tcb_queue_init(gnrc_tcp_tcb_queue_t *queue);

/**
 * @brief Opens a connection.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre @p tcb must not be NULL
 * @pre @p remote must not be NULL.
 * @pre @p remote->port must not be 0.
 *
 * @note Blocks until a connection was established or an error occurred.
 *
 * @param[in,out] tcb          TCB for this connection.
 * @param[in]     remote       Remote endpoint to connect to.
 * @param[in]     local_port   If zero or PORT_UNSPEC, the connections source port
 *                             is randomly selected. If local_port is non-zero
 *                             it is used as source port.
 *
 * @return   0 on success.
 * @return   -EAFNOSUPPORT if @p remote address_family is not supported.
 * @return   -EINVAL if @p remote and @p tcb address_family do not match
 *                    or @p target_addr is invalid.
 * @return   -EISCONN if @p tcb is already connected.
 * @return   -ENOMEM if there are no receive buffer left to use for @p tcb.
 * @return   -EADDRINUSE if @p local_port is already in use.
 * @return   -ETIMEDOUT if the connection attempt timed out.
 * @return   -ECONNREFUSED if the connection attempt was reset by the peer.
 */
int gnrc_tcp_open(gnrc_tcp_tcb_t *tcb, const gnrc_tcp_ep_t *remote, uint16_t local_port);

/**
 * @brief Configures a sequence of TCBs to wait for incoming connections.
 *
 * @pre All TCBs behind @p tcbs must have been initialized via gnrc_tcp_tcb_init().
 * @pre @p queue must not be NULL.
 * @pre @p tcbs must not be NULL.
 * @pre @p tcbs_len must be greater 0.
 * @pre @p local len must be NULL.
 * @pre @p local->port must not be 0.
 *
 * @param[in,out] queue   Listening queue for incoming connections.
 * @param[in] tcbs        TCBs associated with @p queue.
 * @param[in] tcbs_len    Number of TCBs behind @p tcbs.
 * @param[in] local       Endpoint specifying address and port to listen on.
 *
 * @returns   0 on success.
 * @return   -EAFNOSUPPORT given address family in @p local is not supported.
 * @return   -EINVAL address_family in @p tcbs and @p local do not match.
 * @return   -EISCONN a TCB in @p tcbs is already connected.
 * @return   -ENOMEM all available receive buffers are in use.
 *                   Increase GNRC_TCP_RCV_BUFFERS.
 */
int gnrc_tcp_listen(gnrc_tcp_tcb_queue_t *queue, gnrc_tcp_tcb_t *tcbs, size_t tcbs_len,
                    const gnrc_tcp_ep_t *local);

/**
 * @brief Accept TCP connection from listening queue.
 *
 * @pre @p queue must not be NULL
 * @pre @p tcb must not be NULL
 *
 * @note Function blocks if user_timeout_duration_us is not zero.
 *
 * @param[in]  queue                      Listening queue to accept connection from.
 * @param[out] tcb                        Pointer to TCB associated with a established connection.
 * @param[in]  user_timeout_duration_ms   User specified timeout in milliseconds.
 *
 * @return 0 on success.
 * @return -ENOMEM if all connection in @p queue were already accepted.
 * @return -EINVAL if listen was never called on queue.
 * @return -EAGAIN if @p user_timeout_duration_ms was 0 and no connection is ready to accept.
 * @return -ETIMEDOUT if @p user_timeout_duration_ms was not 0 and no connection
 *                    could be established.
 */
int gnrc_tcp_accept(gnrc_tcp_tcb_queue_t *queue, gnrc_tcp_tcb_t **tcb,
                    uint32_t user_timeout_duration_ms);

/**
 * @brief Transmit data to connected peer.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre @p tcb must not be NULL.
 * @pre @p data must not be NULL.
 *
 * @note Blocks until up to @p len bytes were transmitted or an error occurred.
 *
 * @param[in,out] tcb                        TCB holding the connection information.
 * @param[in]     data                       Pointer to the data that should be transmitted.
 * @param[in]     len                        Number of bytes that should be transmitted.
 * @param[in]     user_timeout_duration_ms   If not zero and there was not data transmitted
 *                                           the function returns after user_timeout_duration_ms.
 *                                           If zero, no timeout will be triggered.
 *
 * @return   The number of successfully transmitted bytes.
 * @return   -ENOTCONN if connection is not established.
 * @return   -ECONNRESET if connection was reset by the peer.
 * @return   -ECONNABORTED if the connection was aborted.
 * @return   -ETIMEDOUT if @p user_timeout_duration_ms expired.
 */
ssize_t gnrc_tcp_send(gnrc_tcp_tcb_t *tcb, const void *data, const size_t len,
                      const uint32_t user_timeout_duration_ms);

/**
 * @brief Receive Data from the peer.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre @p tcb must not be NULL.
 * @pre @p data must not be NULL.
 *
 * @note Function blocks if user_timeout_duration_us is not zero.
 *
 * @param[in,out] tcb                        TCB holding the connection information.
 * @param[out]    data                       Pointer to the buffer where the received data
 *                                           should be copied into.
 * @param[in]     max_len                    Maximum amount to bytes that should be read
 *                                           into @p data.
 * @param[in]     user_timeout_duration_ms   Timeout for receive in milliseconds.
 *                                           If zero and no data is available, the function
 *                                           returns immediately. If not zero the function
 *                                           blocks until data is available or
 *                                           @p user_timeout_duration_ms milliseconds passed.
 *
 * @return   The number of bytes read into @p data.
 * @return   0, if the connection is closing and no further data can be read.
 * @return   -ENOTCONN if connection is not established.
 * @return   -EAGAIN if  user_timeout_duration_us is zero and no data is available.
 * @return   -ECONNRESET if connection was reset by the peer.
 * @return   -ECONNABORTED if the connection was aborted.
 * @return   -ETIMEDOUT if @p user_timeout_duration_ms expired.
 */
ssize_t gnrc_tcp_recv(gnrc_tcp_tcb_t *tcb, void *data, const size_t max_len,
                      const uint32_t user_timeout_duration_ms);

/**
 * @brief Close a TCP connection.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre @p tcb must not be NULL.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 */
void gnrc_tcp_close(gnrc_tcp_tcb_t *tcb);

/**
 * @brief Abort a TCP connection.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre @p tcb must not be NULL.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 */
void gnrc_tcp_abort(gnrc_tcp_tcb_t *tcb);

/**
 * @brief Close connections and stop listening on TCB queue
 *
 * @pre @p queue must not be NULL
 *
 * @note: Blocks until all currently opened connections maintained
 *        by @p queue were closed.
 *
 * @param[in,out] queue   TCB queue to stop listening
 */
void gnrc_tcp_stop_listen(gnrc_tcp_tcb_queue_t *queue);

/**
 * @brief Get the local end point of a connected TCB
 *
 * @pre tcb must not be NULL
 * @pre ep must not be NULL
 *
 * @param[in] tcb   TCB holding the connection information.
 * @param[out] ep   The local end point.
 *
 * @return  0 on success.
 * @return  -EADDRNOTAVAIL, when @p tcb in not in a connected state.
 */
int gnrc_tcp_get_local(gnrc_tcp_tcb_t *tcb, gnrc_tcp_ep_t *ep);

/**
 * @brief Get the remote end point of a connected TCB
 *
 * @pre tcb must not be NULL
 * @pre ep must not be NULL
 *
 * @param[in] tcb   TCB holding the connection information.
 * @param[out] ep   The remote end point.
 *
 * @return  0 on success.
 * @return  -ENOTCONN, when @p tcb in not in a connected state.
 */
int gnrc_tcp_get_remote(gnrc_tcp_tcb_t *tcb, gnrc_tcp_ep_t *ep);

/**
 * @brief Gets the local end point of a TCB queue
 *
 * @pre queue must not be NULL
 * @pre ep must not be NULL
 *
 * @param[in] queue   TCB queue to stop listening
 * @param[out] ep     The local end point.
 *
 * @return  0 on success.
 * @return  -EADDRNOTAVAIL, when @p queue has no local end point.
 */
int gnrc_tcp_queue_get_local(gnrc_tcp_tcb_queue_t *queue, gnrc_tcp_ep_t *ep);

/**
 * @brief Calculate and set checksum in TCP header.
 *
 * @param[in] hdr          Gnrc_pktsnip that contains TCP header.
 * @param[in] pseudo_hdr   Gnrc_pktsnip that contains network layer header.
 *
 * @return   0 on success.
 * @return   -EFAULT if @p hdr or pseudo_hdr were NULL
 * @return   -EBADMSG if @p hdr is not of type GNRC_NETTYPE_TCP
 * @return   -ENOENT if @p pseudo_hdr protocol is unsupported.
 */
int gnrc_tcp_calc_csum(const gnrc_pktsnip_t *hdr, const gnrc_pktsnip_t *pseudo_hdr);

/**
 * @brief Adds a TCP header to a given payload.
 *
 * @param[in] payload   Payload that follows the TCP header.
 * @param[in] src       Source port number.
 * @param[in] dst       Destination port number.
 *
 * @return   Not NULL on success.
 * @return   NULL if TCP header was not allocated.
 */
gnrc_pktsnip_t *gnrc_tcp_hdr_build(gnrc_pktsnip_t *payload, uint16_t src, uint16_t dst);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_TCP_H */
/** @} */
