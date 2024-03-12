/*******************************************************************************
*
* Copyright (c) 2015 Intel Corporation and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* and Eclipse Distribution License v1.0 which accompany this distribution.
*
* The Eclipse Public License is available at:
*    http://www.eclipse.org/legal/epl-v10.html
* The Eclipse Distribution License is available at:
*    http://www.eclipse.org/org/documents/edl-v10.php
*
* Contributors:
*    Simon Bernard - initial API and implementation
*    Christian Renz - Please refer to git log
*    Christian Manal - Ported to RIOT OS
*
*******************************************************************************/
/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef LWM2M_CLIENT_CONNECTION_H
#define LWM2M_CLIENT_CONNECTION_H

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

#endif /* LWM2M_CLIENT_CONNECTION_H */
/** @} */
