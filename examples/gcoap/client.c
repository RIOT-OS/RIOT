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
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

#include "fmt.h"
#include "net/gcoap.h"
#include "net/sock/util.h"
#include "net/utils.h"
#include "od.h"

#include "gcoap_example.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if IS_USED(MODULE_GCOAP_DTLS)
#include "net/dsm.h"
#endif

static bool _proxied = false;
static sock_udp_ep_t _proxy_remote;
static char proxy_uri[64];

/* Retain request path to re-request if response includes block. User must not
 * start a new request (with a new path) until any blockwise transfer
 * completes or times out. */
#define _LAST_REQ_PATH_MAX (64)
static char _last_req_path[_LAST_REQ_PATH_MAX];

/* whether this node is currently observing a resource as a client */
static bool observing = false;

/* the token used for observing a remote resource */
static uint8_t obs_req_token[GCOAP_TOKENLEN_MAX];

/* actual length of above token */
static size_t obs_req_tkl = 0;

uint16_t req_count = 0;

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
            if (block.blknum == 0 && !strlen(_last_req_path)) {
                puts("Path too long; can't complete blockwise");
                return;
            }

            if (_proxied) {
                gcoap_req_init(pdu, (uint8_t *)pdu->hdr, CONFIG_GCOAP_PDU_BUF_SIZE,
                               COAP_METHOD_GET, NULL);
            }
            else {
                gcoap_req_init(pdu, (uint8_t *)pdu->hdr, CONFIG_GCOAP_PDU_BUF_SIZE,
                               COAP_METHOD_GET, _last_req_path);
            }

            if (msg_type == COAP_TYPE_ACK) {
                coap_hdr_set_type(pdu->hdr, COAP_TYPE_CON);
            }
            block.blknum++;
            coap_opt_add_block2_control(pdu, &block);

            if (_proxied) {
                coap_opt_add_proxy_uri(pdu, _last_req_path);
            }

            int len = coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
            gcoap_req_send((uint8_t *)pdu->hdr, len, remote,
                           _resp_handler, memo->context,
                           GCOAP_SOCKET_TYPE_UNDEF);
        }
        else {
            puts("--- blockwise complete ---");
        }
    }
}

static size_t _send(uint8_t *buf, size_t len, sock_udp_ep_t *remote)
{
    size_t bytes_sent;

    bytes_sent = gcoap_req_send(buf, len, remote, _resp_handler, NULL,
                                GCOAP_SOCKET_TYPE_UNDEF);
    if (bytes_sent > 0) {
        req_count++;
    }
    return bytes_sent;
}

static int _print_usage(char **argv)
{
    printf("usage: %s <get [-o|-d]|post|put|ping|proxy|info>\n", argv[0]);
    return 1;
}

static int _addrstr2remote(const char *addr_str, sock_udp_ep_t *remote)
{
    if (sock_udp_name2ep(remote, addr_str) != 0) {
        return -1;
    }

    if (remote->port == 0) {
        if (IS_USED(MODULE_GCOAP_DTLS)) {
            remote->port = CONFIG_GCOAPS_PORT;
        }
        else {
            remote->port = CONFIG_GCOAP_PORT;
        }
    }
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
        /* show help for main commands */
        return _print_usage(argv);
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
        if (_proxied) {
#ifdef SOCK_HAS_IPV6
            char addrstr[IPV6_ADDR_MAX_STR_LEN];
#else
            char addrstr[IPV4_ADDR_MAX_STR_LEN];
#endif
            inet_ntop(_proxy_remote.family, &_proxy_remote.addr, addrstr, sizeof(addrstr));

            if (_proxy_remote.family == AF_INET6) {
                printf("[%s]:%u\n", addrstr, _proxy_remote.port);
            }
            else {
                printf("%s:%u\n", addrstr, _proxy_remote.port);
            }
        }
        else {
            puts("None");
        }
        return 0;
    }
    else if (strcmp(argv[1], "proxy") == 0) {
        if ((argc == 4) && (strcmp(argv[2], "set") == 0)) {
            if (sock_udp_name2ep(&_proxy_remote, argv[3]) != 0) {
                puts("Could not set proxy");
                return 1;
            }

            if (_proxy_remote.port == 0) {
                if (IS_USED(MODULE_GCOAP_DTLS)) {
                    _proxy_remote.port = CONFIG_GCOAPS_PORT;
                }
                else {
                    _proxy_remote.port = CONFIG_GCOAP_PORT;
                }
            }

            _proxied = true;
            return 0;
        }
        if ((argc == 3) && (strcmp(argv[2], "unset") == 0)) {
            memset(&_proxy_remote, 0, sizeof(_proxy_remote));
            _proxied = false;
            return 0;
        }
        printf("usage: %s proxy set <host>[:port]\n", argv[0]);
        printf("       %s proxy unset\n", argv[0]);
        return 1;
    }

    /* if not 'info' and 'proxy', must be a method code or ping */
    int code_pos = -1;
    for (size_t i = 0; i < ARRAY_SIZE(method_codes); i++) {
        if (strcmp(argv[1], method_codes[i]) == 0) {
            code_pos = i;
        }
    }
    if (code_pos == -1) {
        return _print_usage(argv);
    }

    /* parse options */
    int apos = 2;       /* position of address argument */

    /* For GET requests additional switches allow for registering and
     * deregistering an observe. This example only supports one observe. */
    if (code_pos == COAP_METHOD_GET) {
        if (argc > apos) {
            if (strcmp(argv[apos], "-o") == 0) {
                if (observing) {
                    puts("Only one observe supported");
                    return 1;
                }
                observe = true;
                apos++;
            } else if (strcmp(argv[apos], "-d") == 0) {
                if (!observing) {
                    puts("Not observing");
                    return 1;
                }
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

    if (((argc == apos + 1) && (code_pos == 0)) ||    /* ping */
        ((argc == apos + 2) && (code_pos == 1)) ||    /* get */
        ((argc == apos + 2 ||
          argc == apos + 3) && (code_pos > 1))) {     /* post or put */

        /* get unproxied endpoint from address string */
        if (_addrstr2remote(argv[apos], &remote)) {
            printf("'%s' is not a valid address\n", argv[apos]);
            return _print_usage(argv);
        }

        char *uri = NULL;
        int uri_len = 0;
        if (code_pos) {
            uri = argv[apos+1];
            uri_len = strlen(argv[apos+1]);
        }

        if (uri && ((uri_len <= 0) || (uri[0] != '/'))) {
            puts("ERROR: URI-Path must start with a \"/\"");
            return _print_usage(argv);
        }

        if (_proxied) {
#ifdef SOCK_HAS_IPV6
            char addrstr[IPV6_ADDR_MAX_STR_LEN];
#else
            char addrstr[IPV4_ADDR_MAX_STR_LEN];
#endif
            inet_ntop(remote.family, &remote.addr, addrstr, sizeof(addrstr));

            if (remote.family == AF_INET6) {
                uri_len = snprintf(proxy_uri, sizeof(proxy_uri), "coap://[%s]:%d%s",
                                   addrstr, remote.port, uri);
            }
            else {
                uri_len = snprintf(proxy_uri, sizeof(proxy_uri), "coap://%s:%d%s",
                                   addrstr, remote.port, uri);
            }

            uri = proxy_uri;
        }

        gcoap_req_init(&pdu, buf, CONFIG_GCOAP_PDU_BUF_SIZE, code_pos, NULL);

        if (observe) {
            uint8_t *token = coap_get_token(&pdu);
            if (obs_value == COAP_OBS_REGISTER) {
                obs_req_tkl = coap_get_token_len(&pdu);
                /* backup the token of the initial observe registration */
                memcpy(obs_req_token, token, obs_req_tkl);
            } else {
                /* use the token of the registration for deregistration
                 * (manually replace the token set by gcoap_req_init) */
                memcpy(token, obs_req_token, obs_req_tkl);
                if (gcoap_obs_req_forget(&remote, obs_req_token, obs_req_tkl)) {
                    printf("could not remove observe request\n");
                    return 1;
                }
            }

            coap_opt_add_uint(&pdu, COAP_OPT_OBSERVE, obs_value);
        }

        if (!_proxied) {
            /* add uri path option separately
             * (options must be added in order) */
            coap_opt_add_uri_path(&pdu, uri);
        }

        coap_hdr_set_type(pdu.hdr, msg_type);

        memset(_last_req_path, 0, _LAST_REQ_PATH_MAX);
        if (uri_len < _LAST_REQ_PATH_MAX) {
            memcpy(_last_req_path, uri, uri_len);
        }

        size_t paylen = (argc == apos + 3) ? strlen(argv[apos+2]) : 0;
        if (paylen) {
            coap_opt_add_format(&pdu, COAP_FORMAT_TEXT);
        }

        if (_proxied) {
            coap_opt_add_proxy_uri(&pdu, uri);
        }

        if (paylen) {
            len = coap_opt_finish(&pdu, COAP_OPT_FINISH_PAYLOAD);
            if (pdu.payload_len >= paylen) {
                memcpy(pdu.payload, argv[apos+2], paylen);
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
        if (!_send(&buf[0], len, _proxied ? &_proxy_remote : &remote)) {
            puts("gcoap_cli: msg send failed");
        }
        else {
            if (observe) {
                /* on successful observe request, store that this node is
                 * observing / not observing anymore */
                observing = obs_value == COAP_OBS_REGISTER;
            }
            /* send Observe notification for /cli/stats */
            notify_observers();
        }
        return 0;
    }
    else {
        printf("usage: %s <get [-o|-d]|post|put> [-c] <host>[:port] <path> [data]\n",
               argv[0]);
        printf("       %s ping <host>[:port]\n", argv[0]);
        printf("Options\n");
        printf("    -c  Send confirmably (defaults to non-confirmable)\n");
        return 1;
    }

    return _print_usage(argv);
}
