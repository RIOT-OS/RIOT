/*
 * SPDX-FileCopyrightText: 2015 Intel Corporation and others
 * SPDX-License-Identifier: EPL-1.0
 */

/*
 * SPDX-FileCopyrightText: 2018 Beduino Master Projekt - University of Bremen
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         lwm2m_client
 * @brief           Public API and definitions of the connection handle for
 *                  LwM2M client implementation using Wakaama
 * @{
 *
 * @file
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "net/ipv6/addr.h"
#include "net/sock/udp.h"

#include "lwm2m_client.h"
#include "lwm2m_client_config.h"

#define SCHEME_COAPS "coaps://"
#define SCHEME_COAP "coap://"

/**
 * @brief Tries to find an existing connection based on a remote UDP endpoint
 *
 * @param[in] conn_list connections list to search
 * @param[in] remote remote UDP endpoint to compare to
 * @param[in] type   type of connection to look for
 *
 * @return pointer to the connection in success
 * @return NULL otherwise
 */
lwm2m_client_connection_t *lwm2m_client_connection_find(lwm2m_client_connection_t *conn_list,
                                                        const sock_udp_ep_t *remote,
                                                        lwm2m_client_connection_type_t type);

/**
 * @brief Handles a received packet from a connection
 *
 * @param[in] conn connection from where the packet came from
 * @param[in] buffer received packet
 * @param[in] num_bytes size of the packet
 * @param[in] client_data LwM2M client data
 *
 * @return 0 on success
 * @return non-zero otherwise
 */
int lwm2m_connection_handle_packet(lwm2m_client_connection_t *conn,
                                   uint8_t *buffer, size_t num_bytes,
                                   lwm2m_client_data_t *client_data);

#ifdef __cplusplus
}
#endif

/** @} */
