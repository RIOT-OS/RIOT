/*
 * Copyright (c) 2018 Ken Bannister. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       nanocoap test CLI client
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmt.h"
#include "net/coap.h"
#include "net/gnrc/netif.h"
#include "net/nanocoap.h"
#include "net/nanocoap_sock.h"
#include "net/sock/util.h"
#include "od.h"
#include "shell.h"

#ifdef MODULE_GNRC_IPV6
static ssize_t _send(coap_pkt_t *pkt, size_t len,
                          char *addr_str, const char *port_str)
{
    ipv6_addr_t addr;
    sock_udp_ep_t remote;

    remote.family = AF_INET6;

    /* parse for interface */
    char *iface = ipv6_addr_split_iface(addr_str);
    if (!iface) {
        if (gnrc_netif_numof() == 1) {
            /* assign the single interface found in gnrc_netif_numof() */
            remote.netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
        }
        else {
            remote.netif = SOCK_ADDR_ANY_NETIF;
        }
    }
    else {
        int pid = atoi(iface);
        if (gnrc_netif_get_by_pid(pid) == NULL) {
            puts("nanocli: interface not valid");
            return 0;
        }
        remote.netif = pid;
    }

    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("nanocli: unable to parse destination address");
        return 0;
    }
    if ((remote.netif == SOCK_ADDR_ANY_NETIF) && ipv6_addr_is_link_local(&addr)) {
        puts("nanocli: must specify interface for link local target");
        return 0;
    }
    memcpy(&remote.addr.ipv6[0], &addr.u8[0], sizeof(addr.u8));

    /* parse port */
    remote.port = atoi(port_str);
    if (remote.port == 0) {
        puts("nanocli: unable to parse destination port");
        return 0;
    }

    return nanocoap_request(pkt, NULL, &remote, len);
}

#if MODULE_NANOCOAP_TOKEN_EXT
#  define CLIENT_TOKEN_LENGTH_MAX   16
#else
#  define CLIENT_TOKEN_LENGTH_MAX   COAP_TOKEN_LENGTH_MAX
#endif
static uint8_t _client_token[CLIENT_TOKEN_LENGTH_MAX] = {0xDA, 0xEC};
static uint8_t _client_token_len = 2;

static int _cmd_client(int argc, char **argv)
{
    /* Ordered like the RFC method code numbers, but off by 1. GET is code 0. */
    const char *method_codes[] = {"get", "post", "put"};
    unsigned buflen = 128;
    uint8_t buf[buflen];
    coap_pkt_t pkt;
    size_t len;

    if (argc == 1) {
        /* show help for commands */
        goto end;
    }

    int code_pos = -1;
    for (size_t i = 0; i < ARRAY_SIZE(method_codes); i++) {
        if (strcmp(argv[1], method_codes[i]) == 0) {
            code_pos = i;
        }
    }
    if (code_pos == -1) {
        goto end;
    }

    pkt.hdr = (coap_hdr_t *)buf;

    /* parse options */
    if (argc == 5 || argc == 6) {
        ssize_t hdrlen = coap_build_hdr(pkt.hdr, COAP_TYPE_CON,
                                        _client_token, _client_token_len,
                                        code_pos+1, 1);
        coap_pkt_init(&pkt, &buf[0], buflen, hdrlen);
        coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, argv[4], '/');
        if (argc == 6) {
            coap_opt_add_uint(&pkt, COAP_OPT_CONTENT_FORMAT, COAP_FORMAT_TEXT);
            len = coap_opt_finish(&pkt, COAP_OPT_FINISH_PAYLOAD);

            pkt.payload_len = strlen(argv[5]);
            memcpy(pkt.payload, argv[5], pkt.payload_len);
            len += pkt.payload_len;
        }
        else {
            len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
        }

        printf("nanocli: sending msg ID %u, %" PRIuSIZE " bytes\n", coap_get_id(&pkt),
               len);

        ssize_t res = _send(&pkt, buflen, argv[2], argv[3]);
        if (res < 0) {
            printf("nanocli: msg send failed: %" PRIdSIZE "\n", res);
        }
        else {
            char *class_str = (coap_get_code_class(&pkt) == COAP_CLASS_SUCCESS)
                                    ? "Success" : "Error";
            printf("nanocli: response %s, code %1u.%02u", class_str,
                   coap_get_code_class(&pkt), coap_get_code_detail(&pkt));
            if (pkt.payload_len) {
                unsigned format = coap_get_content_type(&pkt);
                if (format == COAP_FORMAT_TEXT
                        || format == COAP_FORMAT_LINK
                        || coap_get_code_class(&pkt) == COAP_CLASS_CLIENT_FAILURE
                        || coap_get_code_class(&pkt) == COAP_CLASS_SERVER_FAILURE) {
                    /* Expecting diagnostic payload in failure cases */
                    printf(", %u bytes\n%.*s\n", pkt.payload_len, pkt.payload_len,
                                                                  (char *)pkt.payload);
                }
                else {
                    printf(", %u bytes\n", pkt.payload_len);
                    od_hex_dump(pkt.payload, pkt.payload_len, OD_WIDTH_DEFAULT);
                }
            }
            else {
                printf(", empty payload\n");
            }
        }
        return 0;
    }

    end:
    printf("usage: %s <get|post|put> <addr>[%%iface] <port> <path> [data]\n",
           argv[0]);
    return 1;
}
SHELL_COMMAND(client, "CoAP client", _cmd_client);

static int _cmd_client_token(int argc, char **argv){
    if (argc != 2) {
        printf("Usage: %s <TOKEN_HEX>\n", argv[0]);
        return 1;
    }

    ssize_t tkl = scn_buf_hex(_client_token, sizeof(_client_token),
                              argv[1], strlen(argv[1]));

    if (tkl == -EOVERFLOW) {
        puts("Token too long");
        return 1;
    }

    if (tkl < 0) {
        puts("Failed to parse token");
        return 1;
    }

    _client_token_len = tkl;

    return 0;
}
SHELL_COMMAND(client_token, "Set Token for CoAP client", _cmd_client_token);
#endif

static int _blockwise_cb(void *arg, size_t offset, uint8_t *buf,
                         size_t len, int more)
{
    (void)arg;
    (void)more;

    printf("offset %03" PRIuSIZE ": ", offset);
    for (unsigned i = 0; i < len; ++i) {
        putchar(buf[i]);
    }
    puts("");

    return 0;
}

static int _cmd_url(int argc, char **argv)
{
    /* Ordered like the RFC method code numbers, but off by 1. GET is code 0. */
    const char *method_codes[] = { "get", "post", "put", "delete" };
    int res;

    if (argc < 3) {
        goto error;
    }

    int code_pos = -1;
    for (size_t i = 0; i < ARRAY_SIZE(method_codes); i++) {
        if (strcmp(argv[1], method_codes[i]) == 0) {
            code_pos = i;
            break;
        }
    }
    if (code_pos == -1) {
        goto error;
    }

    switch (code_pos) {
    case COAP_METHOD_GET - 1:
        res = nanocoap_get_blockwise_url(argv[2], COAP_BLOCKSIZE_32,
                                         _blockwise_cb, NULL);
        break;
    case COAP_METHOD_POST - 1:
    case COAP_METHOD_PUT - 1:
        ;
        char response[32];
        nanocoap_sock_t sock;
        res = nanocoap_sock_url_connect(argv[2], &sock);
        if (res) {
            break;
        }
        if (code_pos == 1) {
            res = nanocoap_sock_post(&sock, sock_urlpath(argv[2]),
                                     argv[3], strlen(argv[3]),
                                     response, sizeof(response));
        } else {
            res = nanocoap_sock_put(&sock, sock_urlpath(argv[2]),
                                    argv[3], strlen(argv[3]),
                                    response, sizeof(response));
        }
        nanocoap_sock_close(&sock);
        if (res > 0) {
            response[res] = 0;
            printf("response: %s\n", response);
        }
        break;
    case COAP_METHOD_DELETE - 1:
        res = nanocoap_sock_delete_url(argv[2]);
        break;
    default:
        printf("TODO: implement %s request\n", method_codes[code_pos]);
        return -1;
    }

    if (res) {
        printf("res: %d\n", res);
    }
    return res;

error:
    printf("usage: %s <get|post|put|delete> <url> [data]\n", argv[0]);
    return -1;
}

SHELL_COMMAND(url, "CoAP client URL request", _cmd_url);

static const char song[] =
    "Join us now and share the software;\n"
    "You'll be free, hackers, you'll be free.\n"
    "Join us now and share the software;\n"
    "You'll be free, hackers, you'll be free.\n"
    "\n"
    "Hoarders can get piles of money,\n"
    "That is true, hackers, that is true.\n"
    "But they cannot help their neighbors;\n"
    "That's not good, hackers, that's not good.\n"
    "\n"
    "When we have enough free software\n"
    "At our call, hackers, at our call,\n"
    "We'll kick out those dirty licenses\n"
    "Ever more, hackers, ever more.\n"
    "\n"
    "Join us now and share the software;\n"
    "You'll be free, hackers, you'll be free.\n"
    "Join us now and share the software;\n"
    "You'll be free, hackers, you'll be free.\n";

static int _cmd_put(int argc, char **argv)
{
    int res;
    nanocoap_sock_t sock;
    coap_block_request_t ctx;

    if (argc < 2) {
        printf("usage: %s <url>\n", argv[0]);
        return 1;
    }

    res = nanocoap_block_request_connect_url(&ctx, &sock, argv[1],
                                             COAP_METHOD_PUT, COAP_BLOCKSIZE_32);
    if (res < 0) {
        printf("error: %d\n", res);
        return res;
    }

    const char *pos = song;
    size_t len = sizeof(song);

    while (len) {
        res = nanocoap_sock_block_request(&ctx, pos, len, 0, NULL, NULL);
        if (res <= 0) {
            puts(strerror(-res));
            break;
        }
        len -= res;
        pos += res;
    }

    nanocoap_sock_close(&sock);
    return res;
}

SHELL_COMMAND(put, "experimental put", _cmd_put);

static int _cmd_put_non(int argc, char **argv)
{
    int res;

    if (argc < 3) {
        printf("usage: %s <url> <data>\n", argv[0]);
        return 1;
    }

    nanocoap_sock_t sock;
    nanocoap_sock_url_connect(argv[1], &sock);

    res = nanocoap_sock_put_non(&sock, sock_urlpath(argv[1]), argv[2], strlen(argv[2]),
                                NULL, 0);
    nanocoap_sock_close(&sock);

    return res;
}

SHELL_COMMAND(put_non, "non-confirmable put", _cmd_put_non);

static int _cmd_get_non(int argc, char **argv)
{
    int res;

    uint8_t response[coap_szx2size(CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT)];

    if (argc < 2) {
        printf("usage: %s <url>\n", argv[0]);
        return 1;
    }

    nanocoap_sock_t sock;
    nanocoap_sock_url_connect(argv[1], &sock);

    res = nanocoap_sock_get_non(&sock, sock_urlpath(argv[1]), response, sizeof(response));
    nanocoap_sock_close(&sock);

    if (res >= 0) {
        od_hex_dump(response, res, OD_WIDTH_DEFAULT);
    }
    else {
        printf("error: %d\n", res);
    }
    return res;
}

SHELL_COMMAND(get_non, "non-confirmable get", _cmd_get_non);
