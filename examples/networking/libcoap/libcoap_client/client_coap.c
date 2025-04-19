/*
 * client_coap.c -- RIOT client example
 *
 * Copyright (C) 2023-2025 Jon Shallow <supjps-libcoap@jpshallow.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

#include <thread.h>
#include <debug.h>
#include <coap3/coap.h>
#include "client_coap.h"
#include <stdio.h>
#include "macros/utils.h"

#ifdef CONFIG_LIBCOAP_CLIENT_URI
#define COAP_CLIENT_URI CONFIG_LIBCOAP_CLIENT_URI
#else /* ! CONFIG_LIBCOAP_CLIENT_URI */
#define COAP_CLIENT_URI "coap://[fe80::405:5aff:fe15:9b7f]/.well-known/core"
#endif /* ! CONFIG_LIBCOAP_CLIENT_URI */

#ifdef CONFIG_LIBCOAP_USE_PSK
#define COAP_USE_PSK CONFIG_LIBCOAP_USE_PSK
#else /* ! CONFIG_LIBCOAP_USE_PSK */
#define COAP_USE_PSK NULL
#endif /* ! CONFIG_LIBCOAP_USE_PSK */

#ifdef CONFIG_LIBCOAP_USE_PSK_ID
#define COAP_USE_PSK_ID CONFIG_LIBCOAP_USE_PSK_ID
#else /* ! CONFIG_LIBCOAP_USE_PSK_ID */
#define COAP_USE_PSK_ID NULL
#endif /* ! CONFIG_LIBCOAP_USE_PSK_ID */

static coap_context_t *main_coap_context = NULL;
static coap_optlist_t *optlist = NULL;

static int quit = 0;
static int is_mcast = 0;

#define DEFAULT_WAIT_TIME 15

unsigned int wait_seconds = DEFAULT_WAIT_TIME; /* default timeout in seconds */

static coap_response_t
message_handler(coap_session_t *session,
                const coap_pdu_t *sent,
                const coap_pdu_t *received,
                const coap_mid_t id)
{
    const uint8_t *data;
    size_t len;
    size_t offset;
    size_t total;

    (void)session;
    (void)sent;
    (void)id;
    if (coap_get_data_large(received, &len, &data, &offset, &total)) {
        printf("%*.*s", (int)len, (int)len, (const char *)data);
        if (len + offset == total) {
            printf("\n");
            quit = 1;
        }
    }
    return COAP_RESPONSE_OK;
}

static void
nack_handler(coap_session_t *session COAP_UNUSED,
             const coap_pdu_t *sent COAP_UNUSED,
             const coap_nack_reason_t reason,
             const coap_mid_t id COAP_UNUSED)
{

    switch (reason) {
    case COAP_NACK_TOO_MANY_RETRIES:
    case COAP_NACK_NOT_DELIVERABLE:
    case COAP_NACK_RST:
    case COAP_NACK_TLS_FAILED:
    case COAP_NACK_TLS_LAYER_FAILED:
    case COAP_NACK_WS_LAYER_FAILED:
    case COAP_NACK_WS_FAILED:
        coap_log_err("cannot send CoAP pdu\n");
        quit = 1;
        break;
    case COAP_NACK_ICMP_ISSUE:
    case COAP_NACK_BAD_RESPONSE:
    default:
        break;
    }
    return;
}

static int
resolve_address(const char *host, const char *service, coap_address_t *dst,
                int scheme_hint_bits)
{
    uint16_t port = service ? atoi(service) : 0;
    int ret = 0;
    coap_str_const_t str_host;
    coap_addr_info_t *addr_info;

    str_host.s = (const uint8_t *)host;
    str_host.length = strlen(host);
    addr_info = coap_resolve_address_info(&str_host, port, port,  port, port,
                                          AF_UNSPEC, scheme_hint_bits,
                                          COAP_RESOLVE_TYPE_REMOTE);
    if (addr_info) {
        ret = 1;
        *dst = addr_info->addr;
        is_mcast = coap_is_mcast(dst);
    }

    coap_free_address_info(addr_info);
    return ret;
}

void
client_coap_init(int argc, char **argv)
{
    coap_session_t *session = NULL;
    coap_pdu_t *pdu;
    coap_address_t dst;
    coap_mid_t mid;
    int len;
    coap_uri_t uri;
    char portbuf[8];
    unsigned int wait_ms = 0;
    int result = -1;
#define BUFSIZE 100
    unsigned char buf[BUFSIZE];
    int res;
    const char *coap_uri = COAP_CLIENT_URI;

    if (argc > 1) {
        coap_uri = argv[1];
    }

    /* Initialize libcoap library */
    coap_startup();

    coap_set_log_level(COAP_MAX_LOGGING_LEVEL);

    /* Parse the URI */
    len = coap_split_uri((const unsigned char *)coap_uri, strlen(coap_uri), &uri);
    if (len != 0) {
        coap_log_warn("Failed to parse uri %s\n", coap_uri);
        goto fail;
    }

    snprintf(portbuf, sizeof(portbuf), "%d", uri.port);
    snprintf((char *)buf, sizeof(buf), "%*.*s", (int)uri.host.length,
             (int)uri.host.length, (const char *)uri.host.s);
    /* resolve destination address where packet should be sent */
    len = resolve_address((const char *)buf, portbuf, &dst, 1 << uri.scheme);
    if (len <= 0) {
        coap_log_warn("Failed to resolve address %*.*s\n", (int)uri.host.length,
                      (int)uri.host.length, (const char *)uri.host.s);
        goto fail;
    }

    main_coap_context = coap_new_context(NULL);
    if (!main_coap_context) {
        coap_log_warn("Failed to initialize context\n");
        goto fail;
    }

    coap_context_set_block_mode(main_coap_context, COAP_BLOCK_USE_LIBCOAP);

    if (uri.scheme == COAP_URI_SCHEME_COAP) {
        session = coap_new_client_session(main_coap_context, NULL, &dst,
                                          COAP_PROTO_UDP);
    }
    else if (uri.scheme == COAP_URI_SCHEME_COAP_TCP) {
        session = coap_new_client_session(main_coap_context, NULL, &dst,
                                          COAP_PROTO_TCP);
#if defined (COAP_USE_PSK) && defined(COAP_USE_PSK_ID)
    }
    else {
        static coap_dtls_cpsk_t dtls_psk;
        static char client_sni[256];

        memset(&dtls_psk, 0, sizeof(dtls_psk));
        dtls_psk.version = COAP_DTLS_CPSK_SETUP_VERSION;
        snprintf(client_sni, sizeof(client_sni), "%*.*s", (int)uri.host.length, (int)uri.host.length, uri.host.s);
        dtls_psk.client_sni = client_sni;
        dtls_psk.psk_info.identity.s = (const uint8_t *)COAP_USE_PSK_ID;
        dtls_psk.psk_info.identity.length = strlen(COAP_USE_PSK_ID);
        dtls_psk.psk_info.key.s = (const uint8_t *)COAP_USE_PSK;
        dtls_psk.psk_info.key.length = strlen(COAP_USE_PSK);

        session = coap_new_client_session_psk2(main_coap_context, NULL, &dst,
                                               COAP_PROTO_DTLS, &dtls_psk);
#else /* ! COAP_USE_PSK && ! COAP_USE_PSK_ID */
        coap_log_err("CONFIG_LIBCOAP_USE_PSK and CONFIG_LIBCOAP_USE_PSK_ID not defined\n");
        goto fail;
#endif /* ! COAP_USE_PSK && ! COAP_USE_PSK_ID */
    }

    if (!session) {
        coap_log_warn("Failed to create session\n");
        goto fail;
    }

    coap_register_response_handler(main_coap_context, message_handler);
    coap_register_nack_handler(main_coap_context, nack_handler);

    /* construct CoAP message */
    pdu = coap_pdu_init(is_mcast ? COAP_MESSAGE_NON : COAP_MESSAGE_CON,
                        COAP_REQUEST_CODE_GET,
                        coap_new_message_id(session),
                        coap_session_max_pdu_size(session));
    if (!pdu) {
        coap_log_warn("Failed to create PDU\n");
        goto fail;
    }

    res = coap_uri_into_optlist(&uri, &dst, &optlist, 1);
    if (res != 1) {
        coap_log_warn("Failed to create options\n");
        goto fail;
    }

    /* Add option list (which will be sorted) to the PDU */
    if (optlist) {
        res = coap_add_optlist_pdu(pdu, &optlist);
        if (res != 1) {
            coap_log_warn("Failed to add options to PDU\n");
            goto fail;
        }
    }
    if (is_mcast) {
        /* Allow for other servers to respond within DEFAULT_LEISURE RFC7252 8.2 */
        wait_seconds = coap_session_get_default_leisure(session).integer_part + 1;
    }
    wait_ms = wait_seconds * 1000;

    /* and send the PDU */
    mid = coap_send(session, pdu);
    if (mid == COAP_INVALID_MID) {
        coap_log_warn("Failed to send PDU\n");
        goto fail;
    }
    while (!quit || is_mcast) {
        result = coap_io_process(main_coap_context, 1000);
        if (result >= 0) {
            if (wait_ms > 0) {
                if ((unsigned)result >= wait_ms) {
                    coap_log_info("timeout\n");
                    break;
                } else {
                    wait_ms -= result;
                }
            }
        }
    }
fail:
    /* Clean up library usage so client can be run again */
    quit = 0;
    is_mcast = 0;
    wait_seconds = DEFAULT_WAIT_TIME; /* default timeout in seconds */
    coap_delete_optlist(optlist);
    optlist = NULL;
    coap_session_release(session);
    session = NULL;
    coap_free_context(main_coap_context);
    main_coap_context = NULL;
    coap_cleanup();
}
