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
 * @brief       EDHOC coap responder implementation
 *
 * @author      Timothy Claeys <timothy.claeys@inria.fr>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
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

#if IS_ACTIVE(CONFIG_RESPONDER)

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
static wc_Sha256 _sha_r;
#elif IS_USED(MODULE_TINYCRYPT)
struct tc_sha256_state_struct _sha_r;
#endif

ssize_t _edhoc_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, void *context)
{
    (void)context;
    ssize_t msg_len = 0;

    printf("[responder]: received an EDHOC message (len %d):\n", pkt->payload_len);
    print_bstr(pkt->payload, pkt->payload_len);

    if (_ctx.state == EDHOC_FINALIZED || _ctx.state == EDHOC_FAILED) {
        _ctx.state = EDHOC_WAITING;
    }

    if (_ctx.state == EDHOC_WAITING) {
        uint8_t msg[COAP_BUF_SIZE];
        if ((msg_len =
                 edhoc_create_msg2(&_ctx, pkt->payload, pkt->payload_len, msg, sizeof(msg))) >= 0) {
            printf("[responder]: sending msg2 (%d bytes):\n", (int) msg_len);
            print_bstr(msg, msg_len);
            msg_len = coap_reply_simple(pkt, COAP_CODE_204, buf, len, COAP_FORMAT_OCTET, msg,
                                        msg_len);
        }
        else {
            puts("[responder]: failed to create msg2");
            coap_reply_simple(pkt, COAP_CODE_404, buf, len, COAP_FORMAT_TEXT, NULL, 0);
            return -1;
        }
    }
    else if (_ctx.state == EDHOC_SENT_MESSAGE_2) {
        puts("[responder]: finalize exchange");
        edhoc_resp_finalize(&_ctx, pkt->payload, pkt->payload_len, false, NULL, 0);
        msg_len = coap_reply_simple(pkt, COAP_CODE_204, buf, len, COAP_FORMAT_OCTET, NULL, 0);
    }

    if (_ctx.state == EDHOC_FINALIZED) {
        puts("[responder]: handshake successfully completed");
    }

    return msg_len;
}

/* must be sorted by path (ASCII order) */
const coap_resource_t coap_resources[] = {
    COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER,
    { "/.well-known/edhoc", COAP_POST, _edhoc_handler, NULL },
};

const unsigned coap_resources_numof = ARRAY_SIZE(coap_resources);

int responder_cmd(int argc, char **argv)
{
    if (argc < 2) {
        puts("Usage:");
        puts("\tresp oscore: derive OSCORE secret and salt");
        return -1;
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

int responder_cli_init(void)
{
    if (edhoc_setup(&_ctx, &_conf, EDHOC_IS_RESPONDER, &_auth_key, &_cred_id,
                    &_rpk, &_sha_r)) {
        puts("[responder]: error during setup");
        return -1;
    }

    /* use fixed values only for testing purposes */
    puts("[responder]: load ephemeral key: ONLY FOR TESTING");
    if (edhoc_load_ephkey(&_ctx, resp_cbor_eph_key, sizeof(resp_cbor_eph_key)) != 0) {
        return -1;
    }
    puts("[responder]: preset cid: ONLY FOR TESTING");
    if (edhoc_session_preset_cidr(&_ctx, resp_cid, sizeof(resp_cid)) != 0) {
        return -1;
    }

    return 0;
}

#endif /* CONFIG_RESPONDER */
