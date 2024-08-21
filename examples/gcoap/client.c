/*
 * Copyright (c) 2015-2017 Ken Bannister. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       gcoap CLI support
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "net/gcoap.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"
#include "od.h"
#include "uri_parser.h"

#include "gcoap_example.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if IS_USED(MODULE_GCOAP_DTLS)
#include "net/dsm.h"
#endif

#ifndef CONFIG_URI_MAX
#define CONFIG_URI_MAX      128
#endif

static sock_udp_ep_t _proxy_remote;
static char _proxy_uri[CONFIG_URI_MAX];

/* Retain request URI to re-request if response includes block. User must not
 * start a new request (with a new path) until any blockwise transfer
 * completes or times out. */
static char _last_req_uri[CONFIG_URI_MAX];

/* Last remote endpoint where an Observe request has been sent to */
static sock_udp_ep_t obs_remote;

/* the token used for observing a remote resource */
static uint8_t obs_req_token[GCOAP_TOKENLEN_MAX];

/* actual length of above token */
static size_t obs_req_tkl = 0;

uint16_t req_count = 0;

static gcoap_socket_type_t _get_tl(const char *uri);
static ssize_t _send(uint8_t *buf, size_t len, const sock_udp_ep_t *remote,
                     void *ctx, gcoap_socket_type_t tl);

/*
 * Response callback.
 */
static void _resp_handler(const gcoap_request_memo_t *memo, coap_pkt_t* pdu,
                          const sock_udp_ep_t *remote)
{
    (void)remote;       /* not interested in the source currently */

    if (memo->state == GCOAP_MEMO_TIMEOUT) {
        printf("gcoap: timeout for msg ID %02u\n", coap_get_id(pdu));
        return;
    }
    else if (memo->state == GCOAP_MEMO_RESP_TRUNC) {
        /* The right thing to do here would be to look into whether at least
         * the options are complete, then to mentally trim the payload to the
         * next block boundary and pretend it was sent as a Block2 of that
         * size. */
        printf("gcoap: warning, incomplete response; continuing with the truncated payload\n");
    }
    else if (memo->state != GCOAP_MEMO_RESP) {
        printf("gcoap: error in response\n");
        return;
    }

    coap_block1_t block;
    if (coap_get_block2(pdu, &block) && block.blknum == 0) {
        puts("--- blockwise start ---");
    }

    char *class_str = (coap_get_code_class(pdu) == COAP_CLASS_SUCCESS)
                            ? "Success" : "Error";
    printf("gcoap: response %s, code %1u.%02u", class_str,
                                                coap_get_code_class(pdu),
                                                coap_get_code_detail(pdu));
    if (pdu->payload_len) {
        unsigned content_type = coap_get_content_type(pdu);
        if (content_type == COAP_FORMAT_TEXT
                || content_type == COAP_FORMAT_LINK
                || coap_get_code_class(pdu) == COAP_CLASS_CLIENT_FAILURE
                || coap_get_code_class(pdu) == COAP_CLASS_SERVER_FAILURE) {
            /* Expecting diagnostic payload in failure cases */
            printf(", %u bytes\n%.*s\n", pdu->payload_len, pdu->payload_len,
                                                          (char *)pdu->payload);
        }
        else {
            printf(", %u bytes\n", pdu->payload_len);
            od_hex_dump(pdu->payload, pdu->payload_len, OD_WIDTH_DEFAULT);
        }
    }
    else {
        printf(", empty payload\n");
    }

    /* ask for next block if present */
    if (coap_get_block2(pdu, &block)) {
        if (block.more) {
            unsigned msg_type = coap_get_type(pdu);
            if (block.blknum == 0 && !strlen(_last_req_uri)) {
                puts("Path too long; can't complete blockwise");
                return;
            }
            uri_parser_result_t urip;
            uri_parser_process(&urip, _last_req_uri, strlen(_last_req_uri));
            if (*_proxy_uri) {
                gcoap_req_init(pdu, (uint8_t *)pdu->hdr, CONFIG_GCOAP_PDU_BUF_SIZE,
                               COAP_METHOD_GET, NULL);
            }
            else {
                gcoap_req_init(pdu, (uint8_t *)pdu->hdr, CONFIG_GCOAP_PDU_BUF_SIZE,
                               COAP_METHOD_GET, urip.path);
            }

            if (msg_type == COAP_TYPE_ACK) {
                coap_hdr_set_type(pdu->hdr, COAP_TYPE_CON);
            }
            block.blknum++;
            coap_opt_add_block2_control(pdu, &block);

            if (*_proxy_uri) {
                coap_opt_add_proxy_uri(pdu, urip.scheme);
            }

            int len = coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
            gcoap_socket_type_t tl = _get_tl(*_proxy_uri ? _proxy_uri : _last_req_uri);
            _send((uint8_t *)pdu->hdr, len, remote, memo->context, tl);
        }
        else {
            puts("--- blockwise complete ---");
        }
    }
}

static gcoap_socket_type_t _get_tl(const char *uri)
{
    if (!strncmp(uri, "coaps", 5)) {
        return GCOAP_SOCKET_TYPE_DTLS;
    }
    else if (!strncmp(uri, "coap", 4)) {
        return GCOAP_SOCKET_TYPE_UDP;
    }
    return GCOAP_SOCKET_TYPE_UNDEF;
}

static ssize_t _send(uint8_t *buf, size_t len, const sock_udp_ep_t *remote,
                     void *ctx, gcoap_socket_type_t tl)
{
    ssize_t bytes_sent = gcoap_req_send(buf, len, remote, NULL, _resp_handler, ctx, tl);
    if (bytes_sent > 0) {
        req_count++;
    }
    return bytes_sent;
}

static int _print_usage(char **argv)
{
    printf("usage: %s <get [-o|-d]|post|put> [-c] <URI> [data]\n", argv[0]);
    printf("       %s ping <scheme>://<host>[:port]\n", argv[0]);
    printf("       %s info\n", argv[0]);
    printf("       %s proxy set <scheme>://<host>[:port]\n", argv[0]);
    printf("       %s proxy unset\n", argv[0]);
    printf("Options\n");
    printf("    -c  Send confirmably (defaults to non-confirmable)\n");
    printf("    -o  include Observe registration option\n");
    printf("    -d  include Observe deregistration option\n");
    return 1;
}

static int _uristr2remote(const char *uri, sock_udp_ep_t *remote, const char **path,
                          char *buf, size_t buf_len)
{
    if (strlen(uri) >= buf_len) {
        DEBUG_PUTS("URI too long");
        return 1;
    }
    uri_parser_result_t urip;
    if (uri_parser_process(&urip, uri, strlen(uri))) {
        DEBUG("'%s' is not a valid URI\n", uri);
        return 1;
    }
    memcpy(buf, urip.host, urip.host_len);
    buf[urip.host_len] = '\0';
    if (urip.port_str_len) {
        strcat(buf, ":");
        strncat(buf, urip.port_str, urip.port_str_len);
        buf[urip.host_len + 1 + urip.port_str_len] = '\0';
    }
    if (sock_udp_name2ep(remote, buf) != 0) {
        DEBUG("Could not resolve address '%s'\n", buf);
        return -1;
    }
    if (remote->port == 0) {
        remote->port = !strncmp("coaps", urip.scheme, 5) ? CONFIG_GCOAPS_PORT : CONFIG_GCOAP_PORT;
    }
    if (path) {
        *path = urip.path;
    }
    strcpy(buf, uri);
    return 0;
}

int gcoap_cli_cmd(int argc, char **argv)
{
    /* Ordered like the RFC method code numbers, but off by 1. GET is code 0. */
    char *method_codes[] = {"ping", "get", "post", "put"};
    uint8_t buf[CONFIG_GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;
    size_t len;
    unsigned observe = false;
    uint32_t obs_value = COAP_OBS_REGISTER;
    sock_udp_ep_t remote;

    if (argc == 1) {
        goto help;
    }

    if (strcmp(argv[1], "info") == 0) {
        uint8_t open_reqs = gcoap_op_state();

        if (IS_USED(MODULE_GCOAP_DTLS)) {
            printf("CoAP server is listening on port %u\n", CONFIG_GCOAPS_PORT);
        } else {
            printf("CoAP server is listening on port %u\n", CONFIG_GCOAP_PORT);
        }
#if IS_USED(MODULE_GCOAP_DTLS)
        printf("Connection secured with DTLS\n");
        printf("Free DTLS session slots: %d/%d\n", dsm_get_num_available_slots(),
                dsm_get_num_maximum_slots());
#endif
        printf(" CLI requests sent: %u\n", req_count);
        printf("CoAP open requests: %u\n", open_reqs);
        printf("Configured Proxy: ");
        if (*_proxy_uri) {
            printf("%s\n", _proxy_uri);
        }
        else {
            puts("None");
        }
        return 0;
    }
    else if (strcmp(argv[1], "proxy") == 0) {
        if ((argc == 4) && (strcmp(argv[2], "set") == 0)) {
            if (_uristr2remote(argv[3], &_proxy_remote, NULL, _proxy_uri, sizeof(_proxy_uri))) {
                puts("Could not set proxy");
                return 1;
            }
            return 0;
        }
        if ((argc == 3) && (strcmp(argv[2], "unset") == 0)) {
            memset(&_proxy_remote, 0, sizeof(_proxy_remote));
            memset(_proxy_uri, 0, sizeof(_proxy_uri));
            return 0;
        }
        goto help;
    }

    /* if not 'info' and 'proxy', must be a method code or ping */
    int code_pos = -1;
    for (size_t i = 0; i < ARRAY_SIZE(method_codes) && code_pos == -1; i++) {
        if (strcmp(argv[1], method_codes[i]) == 0) {
            code_pos = i;
        }
    }
    if (code_pos == -1) {
        goto help;
    }

    /* parse options */
    int apos = 2;       /* position of address argument */

    /* For GET requests additional switches allow for registering and
     * deregistering an observe. This example only supports one observe. */
    if (code_pos == COAP_METHOD_GET) {
        if (argc > apos) {
            if (strcmp(argv[apos], "-o") == 0) {
                observe = true;
                apos++;
            }
            else if (strcmp(argv[apos], "-d") == 0) {
                observe = true;
                apos++;
                obs_value = COAP_OBS_DEREGISTER;
            }
        }
    }

    /* ping must be confirmable */
    unsigned msg_type = (!code_pos ? COAP_TYPE_CON : COAP_TYPE_NON);
    if (argc > apos && strcmp(argv[apos], "-c") == 0) {
        msg_type = COAP_TYPE_CON;
        apos++;
    }
    if (apos < argc) {
        const char *path;
        if (_uristr2remote(argv[apos++], &remote, &path, _last_req_uri, sizeof(_last_req_uri))) {
            puts("Could not parse URI");
            goto help;
        }
        gcoap_req_init(&pdu, buf, CONFIG_GCOAP_PDU_BUF_SIZE, code_pos, NULL);

        if (observe) {
            coap_opt_add_uint(&pdu, COAP_OPT_OBSERVE, obs_value);
        }

        if (!*_proxy_uri) {
            /* If the request is not a ping, add uri path option separately
             * (options must be added in order) */
            if (path) {
                coap_opt_add_uri_path(&pdu, path);
            }
        }

        coap_hdr_set_type(pdu.hdr, msg_type);

        size_t paylen = 0;
        if (apos < argc) {
            coap_opt_add_format(&pdu, COAP_FORMAT_TEXT);
            paylen = strlen(argv[apos]);
        }

        if (*_proxy_uri) {
            coap_opt_add_proxy_uri(&pdu, _last_req_uri);
        }

        if (paylen) {
            len = coap_opt_finish(&pdu, COAP_OPT_FINISH_PAYLOAD);
            if (pdu.payload_len >= paylen) {
                memcpy(pdu.payload, argv[apos++], paylen);
                len += paylen;
            }
            else {
                puts("gcoap_cli: msg buffer too small");
                return 1;
            }
        }
        else {
            len = coap_opt_finish(&pdu, COAP_OPT_FINISH_NONE);
        }

        printf("gcoap_cli: sending msg ID %u, %" PRIuSIZE " bytes\n",
                coap_get_id(&pdu), len);
        gcoap_socket_type_t tl = _get_tl(_last_req_uri);
        sock_udp_ep_t *rem = &remote;
        if (*_proxy_uri) {
            rem = &_proxy_remote;
            tl = _get_tl(_proxy_uri);
        }
        if (_send(&buf[0], len, rem, NULL, tl) <= 0) {
            puts("gcoap_cli: msg send failed");
        }
        else {
            if (observe) {
                /* forget last Observe token, as only one can be stored in this example */
                gcoap_obs_req_forget(&obs_remote, obs_req_token, obs_req_tkl);
                if (obs_value == COAP_OBS_REGISTER) {
                    obs_req_tkl = coap_get_token_len(&pdu);
                    /* backup the token of the initial observe registration */
                    memcpy(obs_req_token, coap_get_token(&pdu), obs_req_tkl);
                    obs_remote = remote;
                }
            }
            /* send Observe notification for /cli/stats */
            notify_observers();
        }
        return 0;
    }
help:
    return _print_usage(argv);
}
