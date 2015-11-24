/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_net_tcp TCP
 * @ingroup     net
 * @breif       RIOT's implementation of the TCP protocol
 *
 * @{
 *
 * @file
 * @brief       TCP interface definition
 *
 * @author      Simon Brummer <simon.brummer@haw-hamburg.de>
 * @}
 */

#ifndef GNRC_TCP_H_
#define GNRC_TCP_H_

#include "net/gnrc/netapi.h"
#include "net/gnrc/nettype.h"

#include "net/gnrc/tcp/hdr.h"
#include "net/gnrc/tcp/tcb.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Specifies if active or passive open is performed
 */
#define AI_PASSIVE (1<<0)


/**
 * @brief Initialize and start TCP
 *
 * @param[in] priority      priority to use for the TCP thread
 *
 * @return                  PID of TCP thread on success
 * @return                  -1 if thread is already running.
 * @return                  -EINVAL, if priority is greater than or equal SCHED_PRIO_LEVELS
 * @return                  -EOVERFLOW, if there are too many threads running.
 */
int gnrc_tcp_init(void);

/**
 * @brief Set checksum calculated from tcp and network-layer header in tcp-header.
 *
 * @param[in] hdr          ng_pktsnip that contains tcp header.
 * @param[in] pseudo_hdr   ng_pktsnip that contains networklayer header.
 *
 * @return                 zero on succeed.
 * @return                 -EFAULT if hdr or pseudo_hdr were NULL
 * @return                 -EBADMSG if hdr is not of type GNRC_NETTYPE_TCP
 * @return                 -ENOENT if pseudo_hdr protocol is unsupported.
 */
int gnrc_tcp_calc_csum(const gnrc_pktsnip_t *hdr, const gnrc_pktsnip_t *pseudo_hdr);

/**
 * TODO: Header Stuff
 */
gnrc_pktsnip_t *gnrc_tcp_hdr_build(gnrc_pktsnip_t *payload, uint8_t *src, size_t src_len,
                                   uint8_t *dst, size_t dst_len);


/**
 * @brief Initialize transmission control block. Must be called before tcb usage.
 *
 * @param[out] tcb   transmision control block to be initialized.
 *
 * @return           zero on success.
 */
int8_t gnrc_tcp_tcb_init(gnrc_tcp_tcb_t *tcb);


/**
 * @brief TODO
 */
int8_t gnrc_tcp_tcb_destroy(gnrc_tcp_tcb_t *tcb);


/**
 * @brief Opens a connection. Block until a connection is established, closed or an critical error occured.
 *
 * @param[in/out] tcb        transmission control block to handle this connection
 * @param[in]     local_port      local port number to use. Necessary for passiv Open
 * @param[in/out] peer_addr       peer address to connect to
 * @param[in]     peer_addr_len   peer address size
 * @param[in]     peer_port       peer port to connection. Necessary for active Open
 * @param[in]     options         configuration flags for this connection, prefix AI is used for them.
 *
 * @return         zero on success
 * @return         negative value on error
 * @return         -EFAULT if addr was NULL or addr_len was zero
 * @return         -EALREADY if connection is already in use
 * @return         -EDESTADDRREQ if destination or ipv6 address is unspecified
 * @return         -EINVAL if peer_addr is NULL or peer_addr_len is zero
 * @return         -ETIME if case of user timeout. Only on active Open.
 */

int8_t gnrc_tcp_open(gnrc_tcp_tcb_t *tcb, uint16_t local_port, uint8_t *peer_addr, size_t peer_addr_len, uint16_t peer_port, uint8_t options);

/**
 * @brief TODO
 */
ssize_t gnrc_tcp_send(gnrc_tcp_tcb_t *tcb, uint8_t *buf, size_t byte_count, bool push, bool urgent);

/**
 * @brief Received data from a TCP connection
 *
 * @param[in/out] tcb       Connection to read on
 * @param[in]     buf       Buffer the read Bytes are copied into.
 * @param[in]     nByte     Maximum Number of Bytes to read from this connection.
 *
 * @return        On success, a non negative Integer indicating the number bytes read.
 * @return        On error, negative value.
 * @return        -ETIME in case of user timeout.
 */
ssize_t gnrc_tcp_recv(gnrc_tcp_tcb_t *tcb, void* buf, size_t nByte);

/**
 * @brief TODO
 */
int8_t gnrc_tcp_close(gnrc_tcp_tcb_t *tcb);

#ifdef __cpluscplus
}
#endif

#endif /* GNRC_TCP_H_ */
/** @} */
