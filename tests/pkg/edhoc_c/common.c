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
 * @brief       EDHOC initiator/responder common setup code
 *
 * @author      Timothy Claeys <timothy.claeys@inria.fr>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <stdio.h>
#include <string.h>

#include "kernel_defines.h"
#include "edhoc/edhoc.h"
#include "edhoc_keys.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define CRED_DB_SIZE        ARRAY_SIZE(cred_db)

int _cred_cb(const uint8_t *k, size_t k_len, const uint8_t **o, size_t *o_len)
{
    for (uint8_t i = 0; i < (uint8_t)CRED_DB_SIZE; i++) {
        if (cred_db[i].id_len == k_len) {
            if (memcmp(cred_db[i].id, k, k_len) == 0) {
                *o = cred_db[i].cred;
                *o_len = cred_db[i].cred_len;
                return 0;
            }
        }
    }
    *o = NULL;
    *o_len = 0;
    return EDHOC_ERR_INVALID_CRED_ID;
}

void print_bstr(const uint8_t *bstr, size_t bstr_len)
{
    for (size_t i = 0; i < bstr_len; i++) {
        if ((i + 1) % 8 == 0) {
            printf("0x%02x \n", bstr[i]);
        }
        else {
            printf("0x%02x ", bstr[i]);
        }
    }
    printf("\n");
}

int edhoc_setup(edhoc_ctx_t *ctx, edhoc_conf_t *conf, edhoc_role_t role,
                cose_key_t *auth_key, cred_id_t *cred_id, rpk_t *rpk,
                void *hash_ctx)
{
    /* clear/init context and configuration */
    edhoc_ctx_init(ctx);
    edhoc_conf_init(conf);
    cred_id_init(cred_id);
    cred_rpk_init(rpk);
    cose_key_init(auth_key);

    /* only for testing load preset keys for role */
    const uint8_t *cbor_auth_key = NULL;
    const uint8_t *cbor_rpk = NULL;
    const uint8_t *cbor_rpk_id = NULL;
    size_t cbor_auth_key_len = 0;
    size_t cbor_rpk_len = 0;
    size_t cbor_rpk_id_len;

    if (role == EDHOC_IS_RESPONDER) {
        DEBUG_PUTS("[edhoc]: setting up responder");
        cbor_auth_key = resp_cbor_auth_key;
        cbor_auth_key_len = sizeof(resp_cbor_auth_key);
        cbor_rpk = resp_cbor_rpk;
        cbor_rpk_len = sizeof(resp_cbor_rpk);
        cbor_rpk_id = resp_cbor_rpk_id;
        cbor_rpk_id_len = sizeof(resp_cbor_rpk_id);
    }
    else {
        DEBUG_PUTS("[edhoc]: setting up initiator");
        cbor_auth_key = init_cbor_auth_key;
        cbor_auth_key_len = sizeof(init_cbor_auth_key);
        cbor_rpk = init_cbor_rpk;
        cbor_rpk_len = sizeof(init_cbor_rpk);
        cbor_rpk_id = init_cbor_rpk_id;
        cbor_rpk_id_len = sizeof(init_cbor_rpk_id);
    }

    DEBUG_PUTS("[edhoc]: load private authentication key");
    if (cose_key_from_cbor(auth_key, cbor_auth_key, cbor_auth_key_len) != 0) {
        return -1;
    }

    DEBUG_PUTS("[edhoc]: load and set CBOR RPK");
    if (cred_rpk_from_cbor(rpk, cbor_rpk, cbor_rpk_len) != 0) {
        return -1;
    }

    DEBUG_PUTS("[edhoc]: load credential identifier information");
    if (cred_id_from_cbor(cred_id, cbor_rpk_id, cbor_rpk_id_len) != 0) {
        return -1;
    }

    DEBUG_PUTS("[edhoc]: set up EDHOC callbacks and role");
    edhoc_conf_setup_ad_callbacks(conf, NULL, NULL, NULL);
    if (edhoc_conf_setup_role(conf, role) != 0) {
        return -1;
    }

    DEBUG_PUTS("[edhoc]: set up EDHOC credentials");
    if (edhoc_conf_setup_credentials(conf, auth_key, CRED_TYPE_RPK, rpk, cred_id, _cred_cb) != 0) {
        return -1;
    }

    DEBUG_PUTS("[edhoc]: EDHOC context setup");
    edhoc_ctx_setup(ctx, conf, hash_ctx);

    return 0;
}

void edhoc_oscore_exporter(edhoc_ctx_t *ctx, uint8_t *secret, size_t secret_len,
                           uint8_t *salt, size_t salt_len)
{
    edhoc_exporter(ctx, "OSCORE secret", secret_len, secret, secret_len);
    edhoc_exporter(ctx, "OSCORE salt", salt_len, salt, salt_len);

    puts("OSCORE secret:");
    print_bstr(secret, secret_len);
    puts("OSCORE salt:");
    print_bstr(salt, salt_len);
}
