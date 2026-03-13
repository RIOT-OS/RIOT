/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_libcsp
 * @{
 *
 * @file
 * @brief       LibCSP RIOT configurations
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef CSP_AUTOCONFIG_H
#define CSP_AUTOCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Enable CSP debugging option
 */
#define CSP_HAVE_STDIO      1
/**
 * @brief   Length of incoming queue for router task. (as exponent of 2^n).
 *
 *          As the queue size ALWAYS needs to be power of two, this option
 *          represents the exponent of 2^n, which will be used as the size of
 *          the queue.
 */
#ifndef CONFIG_CSP_QFIFO_LEN
#define CONFIG_CSP_QFIFO_LEN            3
#endif
/**
 * @brief   Length of incoming queue for router task.
 */
#define CSP_QFIFO_LEN                   (1 << CONFIG_CSP_QFIFO_LEN)
/**
 * @brief   Max amount of ports to bind to
 */
#ifndef CSP_PORT_MAX_BIND
#define CSP_PORT_MAX_BIND               15
#endif
/**
 * @brief   Number of packets in connection queue (as exponent of 2^n).
 *
 *          As the queue size ALWAYS needs to be power of two, this option
 *          represents the exponent of 2^n, which will be used as the size of
 *          the queue.
 */
#ifndef CONFIG_CSP_CONN_RXQUEUE_LEN
#define CONFIG_CSP_CONN_RXQUEUE_LEN     3
#endif
/**
 * @brief   Number of packets in connection queue
 */
#define CSP_CONN_RXQUEUE_LEN            (1 << CONFIG_CSP_CONN_RXQUEUE_LEN)
/**
 * @brief   Maximum number of new connections on socket queue
 */
#ifndef CSP_CONN_MAX
#define CSP_CONN_MAX                    4
#endif
/**
 * @brief   Data bytes in each packet buffer
 */
#ifndef CSP_BUFFER_SIZE
#define CSP_BUFFER_SIZE                 127
#endif
/**
 * @brief   Number of total packet buffers (as exponent of 2^n).
 *
 *          As the queue size ALWAYS needs to be power of two, this option
 *          represents the exponent of 2^n, which will be used as the size of
 *          the queue.
 */
#ifndef CONFIG_CSP_BUFFER_COUNT
#define CONFIG_CSP_BUFFER_COUNT         3
#endif
/**
 * @brief   Number of total packet buffers
 */
#define CSP_BUFFER_COUNT                (1 << CONFIG_CSP_BUFFER_COUNT)
/**
 * @brief Number of elements in routing table
 */
#ifndef CSP_RTABLE_SIZE
#define CSP_RTABLE_SIZE                 5
#endif
/**
 * @brief   Hash-based message authentication code
 */
#ifndef  CSP_USE_HMAC
#define  CSP_USE_HMAC                   0
#endif
/**
 * @brief   Reliable Datagram Protocol
 */
#ifndef  CSP_USE_RDP
#define  CSP_USE_RDP                    0
#endif
/**
 * @brief   Max window size for RDP
 */
#ifndef  CSP_RDP_MAX_WINDOW
#define  CSP_RDP_MAX_WINDOW             5
#endif
#ifdef __cplusplus
}
#endif

#endif /* CSP_AUTOCONFIG_H */
