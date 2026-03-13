/*
 * libcoap-test.c -- RIOT example
 *
 * Copyright (C) 2023-2025 Jon Shallow <supjps-libcoap@jpshallow.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

#include "coap_config.h"
#include <coap3/coap.h>
#include "libcoap-test.h"
#include <stdio.h>
#include "macros/utils.h"

#ifdef CONFIG_LIBCOAP_USE_PSK
#define COAP_USE_PSK CONFIG_LIBCOAP_USE_PSK
#else /* CONFIG_LIBCOAP_USE_PSK */
#define COAP_USE_PSK NULL
#endif /* CONFIG_LIBCOAP_USE_PSK */

#ifdef CONFIG_LIBCOAP_USE_PSK_ID
#define COAP_USE_PSK_ID CONFIG_LIBCOAP_USE_PSK_ID
#else /* CONFIG_LIBCOAP_USE_PSK_ID */
#define COAP_USE_PSK_ID NULL
#endif /* CONFIG_LIBCOAP_USE_PSK_ID */

static int quit;

#define INDEX "This is a DTLS loopback test client/server made with libcoap\n"

static void
hnd_get_index(coap_resource_t *resource, coap_session_t  *session,
             const coap_pdu_t *request, const coap_string_t *query,
             coap_pdu_t *response)
{
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
    coap_add_data_large_response(resource, session, request, response,
                                 query, COAP_MEDIATYPE_TEXT_PLAIN,
                                 0x7fff, 0, strlen(INDEX),
                                 (const uint8_t *)INDEX, NULL, NULL);
}

static void
init_coap_resources(coap_context_t *ctx)
{
    coap_resource_t *r;

    r = coap_resource_init(NULL, 0);
    if (r == NULL) {
        goto error;
    }
    coap_register_handler(r, COAP_REQUEST_GET, hnd_get_index);

    coap_add_resource(ctx, r);

    return;
error:
    coap_log_crit("cannot create resource\n");
}

static int
init_coap_endpoints(coap_context_t *ctx, const char *use_psk)
{
    char addr_str[INET6_ADDRSTRLEN + 8];
    int scheme_hint_bits = 1 << COAP_URI_SCHEME_COAP;
    coap_addr_info_t *info = NULL;
    coap_addr_info_t *info_list = NULL;
    coap_str_const_t local;
    int have_ep = 0;

    if (use_psk && coap_dtls_is_supported()) {
        coap_dtls_spsk_t setup_data;

        /* Need PSK set up before setting up endpoints */
        memset(&setup_data, 0, sizeof(setup_data));
        setup_data.version = COAP_DTLS_SPSK_SETUP_VERSION;
        setup_data.psk_info.key.s = (const uint8_t *)use_psk;
        setup_data.psk_info.key.length = strlen(use_psk);
        coap_context_set_psk2(ctx, &setup_data);
        scheme_hint_bits |= 1 << COAP_URI_SCHEME_COAPS;
    }

    local.s = (uint8_t *)"::1";
    local.length = strlen(addr_str);
    info_list = coap_resolve_address_info(&local, COAP_DEFAULT_PORT,
                                          COAPS_DEFAULT_PORT,
                                          0, 0,
                                          0,
                                          scheme_hint_bits,
                                          COAP_RESOLVE_TYPE_REMOTE);
    for (info = info_list; info != NULL; info = info->next) {
        coap_endpoint_t *ep;

        ep = coap_new_endpoint(ctx, &info->addr, info->proto);
        if (!ep) {
            coap_log_warn("cannot create endpoint for proto %u\n",
                          info->proto);
        }
        else {
            have_ep = 1;
        }
    }
    coap_free_address_info(info_list);
    if (!have_ep) {
        return 0;
    }

    return 1;
}

static int
coap_server_init(coap_context_t *ctx)
{
    if (!init_coap_endpoints(ctx, COAP_USE_PSK)) {
        return 0;
    }

    init_coap_resources(ctx);
    return 1;
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
    }

    coap_free_address_info(addr_info);
    return ret;
}

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

static int
coap_client_init(coap_context_t *ctx)
{
    coap_session_t *session = NULL;
    coap_pdu_t *pdu;
    coap_address_t dst;
    coap_mid_t mid;
    int len;
    coap_uri_t uri;
    char portbuf[8];
    coap_optlist_t *optlist = NULL;

#define BUFSIZE 100
    unsigned char buf[BUFSIZE];
    int res;
    const char *coap_uri = "coaps://[::1]";

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

    if (uri.scheme == COAP_URI_SCHEME_COAP) {
        session = coap_new_client_session(ctx, NULL, &dst,
                                          COAP_PROTO_UDP);
    }
    else if (uri.scheme == COAP_URI_SCHEME_COAP_TCP) {
        goto fail;
    }
    else if (uri.scheme == COAP_URI_SCHEME_COAPS_TCP) {
        goto fail;
    }
    else if (uri.scheme == COAP_URI_SCHEME_COAPS) {
#if defined (COAP_USE_PSK) && defined(COAP_USE_PSK_ID)
        static coap_dtls_cpsk_t dtls_psk;
        static char client_sni[256];

        memset(client_sni, 0, sizeof(client_sni));
        memset(&dtls_psk, 0, sizeof(dtls_psk));
        dtls_psk.version = COAP_DTLS_CPSK_SETUP_VERSION;
        if (uri.host.length) {
            memcpy(client_sni, uri.host.s,
                   MIN(uri.host.length, sizeof(client_sni) - 1));
        }
        else {
            memcpy(client_sni, "localhost", 9);
        }
        dtls_psk.client_sni = client_sni;
        dtls_psk.psk_info.identity.s = (const uint8_t *)COAP_USE_PSK_ID;
        dtls_psk.psk_info.identity.length = strlen(COAP_USE_PSK_ID);
        dtls_psk.psk_info.key.s = (const uint8_t *)COAP_USE_PSK;
        dtls_psk.psk_info.key.length = strlen(COAP_USE_PSK);

        session = coap_new_client_session_psk2(ctx, NULL, &dst,
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

    coap_register_response_handler(ctx, message_handler);

    /* construct CoAP message */
    pdu = coap_pdu_init(COAP_MESSAGE_CON,
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

    /* and send the PDU */
    mid = coap_send(session, pdu);
    if (mid == COAP_INVALID_MID) {
        coap_log_warn("Failed to send PDU\n");
        goto fail;
    }
    coap_delete_optlist(optlist);
    return 1;

fail:
    coap_delete_optlist(optlist);
    return 0;
}

void
libcoap_test_run(void)
{
    coap_context_t *coap_context;

    /* Initialize libcoap library */
    coap_startup();

    coap_set_log_level(COAP_MAX_LOGGING_LEVEL);

    coap_context = coap_new_context(NULL);
    if (!coap_context) {
        goto fail;
    }
    coap_context_set_block_mode(coap_context, COAP_BLOCK_USE_LIBCOAP);

    /* Set up server logic */
    coap_server_init(coap_context);
    coap_log_info("libcoap test server ready\n");

    /* Set up and initiate client logic */
    coap_client_init(coap_context);
    coap_log_info("libcoap test client started\n");

    /* Keep on processing until response is back in ... */
    while (quit == 0) {
        coap_io_process(coap_context, 1000);
    }
fail:
    /* Clean up library usage so client can be run again */
    coap_free_context(coap_context);

    coap_cleanup();
    coap_log_info("libcoap test stopped\n");
    return;
}
