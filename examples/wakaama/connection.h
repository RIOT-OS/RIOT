/*******************************************************************************
*
* Copyright (c) 2015 Intel Corporation and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* and Eclipse Distribution License v1.0 which accompany this distribution.
*
* The Eclipse Public License is available at
*    http://www.eclipse.org/legal/epl-v10.html
* The Eclipse Distribution License is available at
*    http://www.eclipse.org/org/documents/edl-v10.php.
*
* Contributors:
*    Simon Bernard - initial API and implementation
*    Christian Renz - Please refer to git log
*    Christian Manal - Ported to RIOT OS
*
*******************************************************************************/

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Header for Wakaama example client DTLS/networking code.
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 */


#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <net/ipv6/hdr.h>
#include <net/udp.h>

#include "net/sock/udp.h"
#include "net/ipv6/addr.h"
#include "net/af.h"

#include "timex.h"
#include "utlist.h"
#include "xtimer.h"

#include "lwm2mconfig.h"

#ifdef MODULE_TINYDTLS
#include "tinydtls.h"
#include "dtls_debug.h"
#include "dtls.h"
#endif /* MODULE_TINYDTLS */

#include "liblwm2m.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Seconds of inactivity after which to close and re-open DTLS connections */
#define DTLS_NAT_TIMEOUT 0

typedef struct _lwm2m_connection_t {
    struct _lwm2m_connection_t *next;
    sock_udp_t sock;
    lwm2m_context_t *lwm2mH;
#ifdef MODULE_TINYDTLS
    session_t *dtls_session;
    lwm2m_object_t *security_obj;
    int security_inst_id;
    dtls_context_t *dtls_context;
    bool no_sec;
#endif /* MODULE_TINYDTLS */
    /* last time a data was sent to the server (used for NAT timeouts) */
    time_t last_send;
} lwm2m_connection_t;

/**
 * @brief  Search for an open connection by remote endpoint.
 *
 * @param[in] conn_list  The connection list to search.
 * @param[in] remote     The remote endpoint to search for.
 *
 * @return    Pointer to connection, if one was found.
 * @return    NULL, if no connection was found.
 */
lwm2m_connection_t *lwm2m_connection_find(lwm2m_connection_t *conn_list,
                                          const sock_udp_ep_t *remote);

/**
 * @brief  Create a new lwm2m connection.
 *
 * @param[in] conn_list     Connection list to add the new connection to.
 * @param[in] security_obj  The lwm2m security object to get the remote endpoint from.
 * @param[in] lwm2mH        The lwm2m context to add the connection to.
 *
 * @return    Pointer to connection on success.
 * @return    NULL on error.
 */
lwm2m_connection_t *lwm2m_connection_create(lwm2m_connection_t *conn_list,
                                            lwm2m_object_t *security_obj,
                                            int instance_id,
                                            lwm2m_context_t *lwm2mH);

/**
 * @brief  Release resources of an lwm2m connection.
 *
 * @param[in] conn_list  The connection (list) to free.
 */
void lwm2m_connection_free(lwm2m_connection_t *conn_list);

/**
 * @brief  Handles incoming lwm2m network packages.
 *
 * @param[in] connP   The connection the packet came on in.
 * @param[in] buffer  The packet buffer.
 * @param[in] length  The length of @p buffer.
 *
 * @return    0 on success
 * @return    <0 on error
 */
int lwm2m_connection_handle_packet(lwm2m_connection_t *connP, uint8_t *buffer,
                                   size_t length);

#ifdef MODULE_TINYDTLS
/**
 * @brief  Re-handshakes on open DTLS connection. Useful when your NAT
 *         timed out and your client has a new IP/PORT.
 *
 * @param[in] connP              The connection to re-handshake.
 * @param[in] send_close_notify  Set to true if DTLS should send a close
 *                               notification before re-handshaking.
 *
 * @return    0 on success
 * @return    <0 on error
 */
int lwm2m_connection_rehandshake(lwm2m_connection_t *connP, bool send_close_notify);
#endif /* MODULE_TINYDTLS */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CONNECTION_H */
