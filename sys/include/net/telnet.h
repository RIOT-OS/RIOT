/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_telnet basic Telnet server implementation
 * @ingroup     net_ipv6
 * @brief       Telnet server functions
 * @{
 *
 * @file
 * @brief   minimal Telnet server ([RFC 854](https://tools.ietf.org/html/rfc854)) implementation
 * @note    This implementation only supports a single client and no options.
 *
 * @warning Telnet is entirely unencrypted! Do not use it on public networks.
 *          This is intended to aid debugging on networks that are isolated from the Internet.
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef NET_TELNET_H
#define NET_TELNET_H

#include "net/sock/tcp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The port for the Telnet server to listen on
 */
#ifndef CONFIG_TELNET_PORT
#define CONFIG_TELNET_PORT (23)
#endif

/**
 * @brief   Start the Telnet server thread
 *
 * @return  0 on success, error otherwise
 */
int telnet_server_start(void);

/**
 * @brief   Write data to the telnet client
 *
 * @param[in] buffer    The buffer to send to the client
 * @param[in] len       The length of the buffer
 *
 * @return  0 on success, error otherwise
 */
int telnet_server_write(const void* buffer, size_t len);

/**
 * @brief   Read data from the telnet client, will block until data is available.
 *
 * @param[out] buffer   The buffer to write data from the client
 * @param[in]  count    Number of bytes to read
 *
 * @return  number of bytes read, error otherwise
 */
int telnet_server_read(void* buffer, size_t count);

/**
 * @brief   Callback function that gets called when a telnet client connects
 *          but before stdio is redirected.
 *
 * @param[in] sock  Socket of the client that just connected
 *                  only use with @ref sock_tcp_get_local
 */
void telnet_cb_pre_connected(sock_tcp_t *sock);

/**
 * @brief   Callback function that gets called when a telnet client connects
 *          after stdio is redirected.
 *
 * @param[in] sock  Socket of the client that just connected
 *                  only use with @ref sock_tcp_get_local
 */
void telnet_cb_connected(sock_tcp_t *sock);

/**
 * @brief   Callback function that gets called after a telnet client disconnected.
 */
void telnet_cb_disconneced(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_TELNET_H */
/** @} */
