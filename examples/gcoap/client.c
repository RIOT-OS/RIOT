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
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmt.h"
#include "net/gcoap.h"
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
                           _resp_handler, memo->context);
        }
        else {
            puts("--- blockwise complete ---");
        }
    }
}

static bool _parse_endpoint(sock_udp_ep_t *remote,
                            const char *addr_str, const char *port_str)
{
    netif_t *netif;

    /* parse hostname */
    if (netutils_get_ipv6((ipv6_addr_t *)&remote->addr, &netif, addr_str) < 0) {
        puts("gcoap_cli: unable to parse destination address");
        return false;
    }
    remote->netif = netif ? netif_get_id(netif) : SOCK_ADDR_ANY_NETIF;
    remote->family = AF_INET6;

    /* parse port */
    remote->port = atoi(port_str);
    if (remote->port == 0) {
        puts("gcoap_cli: unable to parse destination port");
        return false;
    }

    return true;
}

static size_t _send(uint8_t *buf, size_t len, char *addr_str, char *port_str)
{
    size_t bytes_sent;
    sock_udp_ep_t *remote;
    sock_udp_ep_t new_remote;

    if (_proxied) {
        remote = &_proxy_remote;
    }
    else {
        if (!_parse_endpoint(&new_remote, addr_str, port_str)) {
            return 0;
        }
        remote = &new_remote;
    }

    bytes_sent = gcoap_req_send(buf, len, remote, _resp_handler, NULL);
    if (bytes_sent > 0) {
        req_count++;
    }
    return bytes_sent;
}

static int _print_usage(char **argv)
{
    printf("usage: %s <get|post|put|ping|proxy|info>\n", argv[0]);
    return 1;
}

int gcoap_cli_cmd(int argc, char **argv)
{
    /* Ordered like the RFC method code numbers, but off by 1. GET is code 0. */
    char *method_codes[] = {"ping", "get", "post", "put"};
    uint8_t buf[CONFIG_GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;
    size_t len;

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
            char addrstr[IPV6_ADDR_MAX_STR_LEN];
            printf("[%s]:%u\n",
                   ipv6_addr_to_str(addrstr,
                                    (ipv6_addr_t *) &_proxy_remote.addr.ipv6,
                                    sizeof(addrstr)),
                   _proxy_remote.port);
        }
        else {
            puts("None");
        }
        return 0;
    }
    else if (strcmp(argv[1], "proxy") == 0) {
        if ((argc == 5) && (strcmp(argv[2], "set") == 0)) {
            if (!_parse_endpoint(&_proxy_remote, argv[3], argv[4])) {
                puts("Could not set proxy");
                return 1;
            }
            _proxied = true;
            return 0;
        }
        if ((argc == 3) && (strcmp(argv[2], "unset") == 0)) {
            memset(&_proxy_remote, 0, sizeof(_proxy_remote));
            _proxied = false;
            return 0;
        }
        printf("usage: %s proxy set <addr>[%%iface] <port>\n", argv[0]);
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
    /* ping must be confirmable */
    unsigned msg_type = (!code_pos ? COAP_TYPE_CON : COAP_TYPE_NON);
    if (argc > apos && strcmp(argv[apos], "-c") == 0) {
        msg_type = COAP_TYPE_CON;
        apos++;
    }

    if (((argc == apos + 2) && (code_pos == 0)) ||    /* ping */
        ((argc == apos + 3) && (code_pos == 1)) ||    /* get */
        ((argc == apos + 3 ||
          argc == apos + 4) && (code_pos > 1))) {     /* post or put */

        char *uri = NULL;
        int uri_len = 0;
        if (code_pos) {
            uri = argv[apos+2];
            uri_len = strlen(argv[apos+2]);
        }

        if (_proxied) {
            uri_len = snprintf(proxy_uri, 64, "coap://[%s]:%s%s", argv[apos], argv[apos+1], uri);
            uri = proxy_uri;

            gcoap_req_init(&pdu, &buf[0], CONFIG_GCOAP_PDU_BUF_SIZE, code_pos, NULL);
        }
        else{
            gcoap_req_init(&pdu, &buf[0], CONFIG_GCOAP_PDU_BUF_SIZE, code_pos, uri);
        }
        coap_hdr_set_type(pdu.hdr, msg_type);

        memset(_last_req_path, 0, _LAST_REQ_PATH_MAX);
        if (uri_len < _LAST_REQ_PATH_MAX) {
            memcpy(_last_req_path, uri, uri_len);
        }

        size_t paylen = (argc == apos + 4) ? strlen(argv[apos+3]) : 0;
        if (paylen) {
            coap_opt_add_format(&pdu, COAP_FORMAT_TEXT);
        }

        if (_proxied) {
            coap_opt_add_proxy_uri(&pdu, uri);
        }

        if (paylen) {
            len = coap_opt_finish(&pdu, COAP_OPT_FINISH_PAYLOAD);
            if (pdu.payload_len >= paylen) {
                memcpy(pdu.payload, argv[apos+3], paylen);
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

        printf("gcoap_cli: sending msg ID %u, %u bytes\n", coap_get_id(&pdu),
               (unsigned) len);
        if (!_send(&buf[0], len, argv[apos], argv[apos+1])) {
            puts("gcoap_cli: msg send failed");
        }
        else {
            /* send Observe notification for /cli/stats */
            notify_observers();
        }
        return 0;
    }
    else {
        printf("usage: %s <get|post|put> [-c] <addr>[%%iface] <port> <path> [data]\n",
               argv[0]);
        printf("       %s ping <addr>[%%iface] <port>\n", argv[0]);
        printf("Options\n");
        printf("    -c  Send confirmably (defaults to non-confirmable)\n");
        return 1;
    }

    return _print_usage(argv);
}
