/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

/**
 * @defgroup    net_gcoap_forward_proxy_thread    GCoAP Forward Proxy Thread
 * @ingroup     net_gcoap
 * @brief       Forward proxy thread implementation for GCoAP
 *
 * @{
 *
 * @file
 * @brief       Definitions for the GCoAP forward proxy internal communication
 *
 * @author      Mariem Charrada <mariem.charrada@ml-pa.com>
 */

#include <stdint.h>
#include "net/coap.h"
#include "net/gcoap.h"
#include "net/sock/udp.h"
#include "ztimer.h"
#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   client ep structure
 */
typedef struct {
    coap_pkt_t pdu;                         /**< forward CoAP PDU */
    sock_udp_ep_t server_ep;                /**< forward Server endpoint */
    sock_udp_ep_t ep;                       /**< client endpoint */
    sock_udp_ep_t proxy_ep;                 /**< proxy endpoint */
    uint16_t mid;                           /**< message ID */
    uint8_t flags;                          /**< client flags */
#if IS_USED(MODULE_NANOCOAP_CACHE)
    uint8_t req_etag[COAP_ETAG_LENGTH_MAX]; /**< request ETag */
#endif
    ztimer_t empty_ack_timer;               /**< empty ACK timer */
    event_t event;                          /**< client event */
} client_ep_t;

/**
 * @brief Stack size for the forward proxy thread
 *
 */
#ifndef GCOAP_PROXY_STACK_SIZE
#define GCOAP_PROXY_STACK_SIZE (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE \
                                + sizeof(coap_pkt_t) + GCOAP_DTLS_EXTRA_STACKSIZE)
#endif

/**
 * @brief Definition of forward proxy thread msgs.
 */
enum {
    GCOAP_FORWARD_PROXY_MSG_SEND,
};

/**
 * @brief   Initialize the forward proxy thread
 */
void gcoap_forward_proxy_thread_init(void);

/**
 * @brief Forward the CoAP request to the server
 *        The client endpoint is passed as an argument
 *        and freed if the send failed.
 *
 * @param[in] cep   client endpoint
 * @return          @ref gcoap_req_send
 */
int gcoap_forward_proxy_req_send(client_ep_t *cep);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
