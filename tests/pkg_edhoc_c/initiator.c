/*
 * Copyright (C) 2021 Inria
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
 * @brief       EDHOC coap initiator implementation
 *
 * @author      Timothy Claeys <timothy.claeys@inria.fr>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <stdio.h>

#include "net/ipv6.h"
#include "net/nanocoap_sock.h"
#include "shell.h"

#include "edhoc/edhoc.h"
#include "edhoc_keys.h"

#if IS_USED(MODULE_WOLFSSL)
#include "wolfssl/wolfcrypt/sha256.h"
#elif IS_USED(MODULE_TINYCRYPT)
#include "tinycrypt/sha256.h"
#endif

#define ENABLE_DEBUG        0
#include "debug.h"

#define COAP_BUF_SIZE     (256U)

#if IS_ACTIVE(CONFIG_INITIATOR)

extern void print_bstr(const uint8_t *bstr, size_t bstr_len);
extern int edhoc_setup(edhoc_ctx_t *ctx, edhoc_conf_t *conf, edhoc_role_t role,
                       cose_key_t *auth_key, cred_id_t *cred_id, rpk_t *rpk,
                       void *hash_ctx);
extern void edhoc_oscore_exporter(edhoc_ctx_t *ctx, uint8_t *secret, size_t secret_len,
                                  uint8_t *salt, size_t salt_len);

static edhoc_ctx_t _ctx;
static edhoc_conf_t _conf;
static rpk_t _rpk;
static cred_id_t _cred_id;
static cose_key_t _auth_key;
#if IS_USED(MODULE_WOLFSSL)
static wc_Sha256 _sha_i;
#elif IS_USED(MODULE_TINYCRYPT)
struct tc_sha256_state_struct _sha_i;
#endif
static uint8_t _method;
static uint8_t _suite;

static int _parse_ipv6_addr(char *addr_str, ipv6_addr_t *addr, uint16_t *netif)
{
    /* parse for interface */
    char *iface = ipv6_addr_split_iface(addr_str);

    if (!iface) {
        if (gnrc_netif_numof() == 1) {
            /* assign the single interface found in gnrc_netif_numof() */
            *netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
        }
        else {
            *netif = SOCK_ADDR_ANY_NETIF;
        }
    }
    else {
        int pid = atoi(iface);
        if (gnrc_netif_get_by_pid(pid) == NULL) {
            puts("[initiator]: interface not valid");
            return -1;
        }
        *netif = pid;
    }

    /* parse destination address */
    if (ipv6_addr_from_str(addr, addr_str) == NULL) {
        puts("[initiator]: unable to parse destination address");
        return -1;
    }
    if ((*netif == SOCK_ADDR_ANY_NETIF) && ipv6_addr_is_link_local(addr)) {
        puts("[initiator]: must specify interface for link local target");
        return -1;
    }

    return 0;
}

static ssize_t _send(coap_pkt_t *pkt, size_t len, ipv6_addr_t *addr, uint16_t netif, uint16_t port)
{
    sock_udp_ep_t remote;

    remote.family = AF_INET6;
    remote.port = port;
    remote.netif = netif;
    memcpy(&remote.addr.ipv6[0], addr->u8, sizeof(addr->u8));

    return nanocoap_request(pkt, NULL, &remote, len);
}

static ssize_t _build_coap_pkt(coap_pkt_t *pkt, uint8_t *buf, ssize_t buflen,
                               uint8_t *payload, ssize_t payload_len)
{
    uint8_t token[2] = { 0xDA, 0xEC };
    ssize_t len = 0;

    /* set pkt buffer */
    pkt->hdr = (coap_hdr_t *)buf;
    /* build header, confirmed message always post */
    ssize_t hdrlen = coap_build_hdr(pkt->hdr, COAP_TYPE_CON, token,
                                    sizeof(token), COAP_METHOD_POST, 1);

    coap_pkt_init(pkt, buf, buflen, hdrlen);
    coap_opt_add_string(pkt, COAP_OPT_URI_PATH, "/.well-known/edhoc", '/');
    coap_opt_add_uint(pkt, COAP_OPT_CONTENT_FORMAT, COAP_FORMAT_OCTET);
    len = coap_opt_finish(pkt, COAP_OPT_FINISH_PAYLOAD);
    /* copy msg payload */
    pkt->payload_len = payload_len;
    memcpy(pkt->payload, payload, payload_len);
    len += pkt->payload_len;
    return len;
}

int _handshake_cmd(int argc, char **argv)
{
    uint8_t buf[COAP_BUF_SIZE] = { 0 };
    coap_pkt_t pkt;
    ssize_t len = 0;
    uint16_t port = COAP_PORT;
    uint8_t msg[COAP_BUF_SIZE];
    ssize_t msg_len = 0;

    /* correlation value is transport specific */
    corr_t corr = CORR_1_2;

    if (argc < 2) {
        printf("usage: %s <addr>[%%iface] <port>\n", argv[0]);
        return -1;
    }
    if (argc == 3) {
        port = atoi(argv[2]);
    }

    /* parse address */
    uint16_t netif;
    ipv6_addr_t addr;
    if (_parse_ipv6_addr(argv[1], &addr, &netif)) {
        return -1;
    }

    /* reset state */
    _ctx.state = EDHOC_WAITING;

    if ((msg_len = edhoc_create_msg1(&_ctx, corr, _method, _suite, msg, sizeof(msg))) > 0) {
        printf("[initiator]: sending msg1 (%d bytes):\n", (int)msg_len);
        print_bstr(msg, msg_len);
        _build_coap_pkt(&pkt, buf, sizeof(buf), msg, msg_len);
        len = _send(&pkt, COAP_BUF_SIZE, &addr, netif, port);
    }
    else {
        puts("[initiator]: failed to create msg1");
        return -1;
    }
    if (len < 0) {
        puts("[initiator]: failed to send msg1");
        return -1;
    }

    printf("[initiator]: received a message (%d bytes):\n", pkt.payload_len);
    print_bstr(pkt.payload, pkt.payload_len);

    if ((msg_len = edhoc_create_msg3(&_ctx, pkt.payload, pkt.payload_len, msg, sizeof(msg))) > 0) {
        printf("[initiator]: sending msg3 (%d bytes):\n", (int)msg_len);
        print_bstr(msg, msg_len);
        _build_coap_pkt(&pkt, buf, sizeof(buf), msg, msg_len);
        len = _send(&pkt, COAP_BUF_SIZE, &addr, netif, port);
    }
    else {
        puts("[initiator]: failed to create msg3");
        return -1;
    }

    if (edhoc_init_finalize(&_ctx)) {
        puts("[initiator]: handshake failed");
        return -1;
    }

    puts("[initiator]: handshake successfully completed");

    printf("[initiator]: Transcript hash 4 (%d bytes):\n", EDHOC_DIGEST_SIZE);
    print_bstr(_ctx.session.th4, EDHOC_DIGEST_SIZE);

    _ctx.state = EDHOC_FINALIZED;

    return 0;
}

static void _print_initiator_usage(void)
{
    puts("Usage:");
    puts("\tinit set method <sgn-sgn|sgn-stat|stat-sgn|stat-stat>:"
         " choose authentication method [initiator-responder]");
    puts("\tinit set suite <0|1>: choose cypher-suit, only 0 and 1 support");
    puts("\tinit handshake <addr>[%%iface] <port>: start handshake");
    puts("\tinit oscore: derive OSCORE secret and salt");
}

static int _set_cmd(int argc, char **argv)
{
    if (argc < 2) {
        _print_initiator_usage();
        return -1;
    }

    if (!strcmp(argv[1], "method")) {
        if (!strcmp(argv[2], "sgn-sgn")) {
            _method = EDHOC_AUTH_SIGN_SIGN;
        }
        else if (!strcmp(argv[2], "sgn-stat")) {
            _method = EDHOC_AUTH_SIGN_STATIC;
        }
        else if (!strcmp(argv[2], "stat-sgn")) {
            _method = EDHOC_AUTH_STATIC_SIGN;
        }
        else if (!strcmp(argv[2], "stat-stat")) {
            _method = EDHOC_AUTH_STATIC_STATIC;
        }
        else {
            printf("error: invalid method %s, sgn-sgn|sgn-stat|stat-sgn|stat-stat\n", argv[1]);
            return -1;
        }
    }

    if (!strcmp(argv[1], "suite")) {
        uint8_t suite = atoi(argv[2]);
        if (suite > 1) {
            puts("error: only cypher suits 0 and 1 are supported");
            return -1;
        }
        _suite = suite;
    }

    return 0;
}

int initiator_cmd(int argc, char **argv)
{
    if (argc < 2) {
        _print_initiator_usage();
        return -1;
    }

    if (!strcmp(argv[1], "set")) {
        return _set_cmd(argc - 1, &argv[1]);
    }

    if (!strcmp(argv[1], "handshake")) {
        return _handshake_cmd(argc - 1, &argv[1]);
    }

    if (!strcmp(argv[1], "oscore")) {
        if (_ctx.state == EDHOC_FINALIZED) {
            uint8_t secret[16];
            uint8_t salt[8];
            edhoc_oscore_exporter(&_ctx, secret, sizeof(secret), salt, sizeof(salt));
        }
        else {
            puts("error: perform edhoc handshake first");
        }
    }

    return 0;
}

int initiator_cli_init(void)
{
    /* default to static-static (method 3) since we are using RPK keys */
    _method = EDHOC_AUTH_STATIC_STATIC;
    _suite = EDHOC_CIPHER_SUITE_0;
    if (edhoc_setup(&_ctx, &_conf, EDHOC_IS_INITIATOR, &_auth_key, &_cred_id,
                    &_rpk, &_sha_i)) {
        puts("[initiator]: error during setup");
        return -1;
    }
    /* use fixed values only for testing purposes */
    puts("[initiator]: load ephemeral key: ONLY FOR TESTING");
    if (edhoc_load_ephkey(&_ctx, init_cbor_eph_key, sizeof(init_cbor_eph_key)) != 0) {
        return -1;
    }
    puts("[initiator]: preset cid: ONLY FOR TESTING");
    if (edhoc_session_preset_cidi(&_ctx, init_cid, sizeof(init_cid)) != 0) {
        return -1;
    }

    return 0;
}

#endif /* CONFIG_INITIATOR */
