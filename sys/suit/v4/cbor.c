/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_suit
 * @{
 *
 * @file
 * @brief       SUIT manifest parser library for CBOR based manifests
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "suit/v4/handlers.h"
#include "suit/v4/suit.h"
#include "suit/v4/policy.h"
#include "nanocbor/nanocbor.h"
#include "cose/sign.h"

#include "public_key.h"

#include "log.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static suit_manifest_handler_t _manifest_get_auth_wrapper_handler(int key);
typedef suit_manifest_handler_t (*suit_manifest_handler_getter_t)(int key);

int suit_cbor_subparse(nanocbor_value_t *bseq, nanocbor_value_t *it)
{
    const uint8_t *bytes;
    size_t bytes_len = 0;
    int res = nanocbor_get_bstr(bseq, &bytes, &bytes_len);
    if (res < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    nanocbor_decoder_init(it, bytes, bytes_len);
    return SUIT_OK;
}

int _v4_parse(suit_v4_manifest_t *manifest, const uint8_t *buf,
              size_t len, suit_manifest_handler_getter_t getter)
{
    nanocbor_value_t it, map;

    nanocbor_decoder_init(&it, buf, len);

    map = it;

    if (nanocbor_enter_map(&map, &it) < 0) {
        LOG_DEBUG("suit _v4_parse(): manifest not a map!\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }

    while (!nanocbor_at_end(&it)) {
        int32_t integer_key;

        if (nanocbor_get_int32(&it, &integer_key) < 0) {
            return SUIT_ERR_INVALID_MANIFEST;
        }
        LOG_DEBUG("got key val=%" PRIi32 "\n", integer_key);
        suit_manifest_handler_t handler = getter(integer_key);

        nanocbor_value_t value = it;
        nanocbor_skip(&it);

        if (handler) {
            int res = handler(manifest, integer_key, &value);
            if (res < 0) {
                LOG_INFO("handler returned <0\n)");
                return SUIT_ERR_INVALID_MANIFEST;
            }
        }
        else {
            LOG_DEBUG("no handler found\n");
        }
    }

    nanocbor_leave_container(&map, &it);

    return SUIT_OK;
}

int suit_v4_parse(suit_v4_manifest_t *manifest, const uint8_t *buf,
                  size_t len)
{
    manifest->buf = buf;
    manifest->len = len;
    return _v4_parse(manifest, buf, len, _manifest_get_auth_wrapper_handler);
}

static int _auth_handler(suit_v4_manifest_t *manifest, int key,
                         nanocbor_value_t *it)
{
    (void)key;
    const uint8_t *cose_buf;
    size_t cose_len = 0;
    int res = nanocbor_get_bstr(it, &cose_buf, &cose_len);

    if (res < 0) {
        LOG_INFO("Unable to get COSE signature\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }
    res = cose_sign_decode(&manifest->verify, cose_buf, cose_len);
    if (res < 0) {
        LOG_INFO("Unable to parse COSE signature\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return 0;
}

static int _manifest_handler(suit_v4_manifest_t *manifest, int key,
                             nanocbor_value_t *it)
{
    (void)key;
    const uint8_t *manifest_buf;
    size_t manifest_len;

    nanocbor_get_bstr(it, &manifest_buf, &manifest_len);

    /* Validate the COSE struct first now that we have the payload */
    cose_sign_decode_set_payload(&manifest->verify, manifest_buf, manifest_len);

    /* Iterate over signatures, should only be a single signature */
    cose_signature_dec_t signature;

    cose_sign_signature_iter_init(&signature);
    if (!cose_sign_signature_iter(&manifest->verify, &signature)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    /* Initialize key from hardcoded public key */
    cose_key_t pkey;
    cose_key_init(&pkey);
    cose_key_set_keys(&pkey, COSE_EC_CURVE_ED25519, COSE_ALGO_EDDSA,
                      public_key, NULL, NULL);

    LOG_INFO("suit: verifying manifest signature...\n");
    int verification = cose_sign_verify(&manifest->verify, &signature,
                                        &pkey, manifest->validation_buf,
                                        SUIT_COSE_BUF_SIZE);
    if (verification != 0) {
        LOG_INFO("Unable to validate signature\n");
        return SUIT_ERR_SIGNATURE;
    }

    return _v4_parse(manifest, manifest_buf,
                     manifest_len, suit_manifest_get_manifest_handler);
}

static suit_manifest_handler_t _suit_manifest_get_handler(int key,
                                                          const suit_manifest_handler_t *handlers,
                                                          size_t len)
{
    if (key < 0 || (size_t)key >= len) {
        return NULL;
    }
    return handlers[key];
}

/* begin{code-style-ignore} */
static const suit_manifest_handler_t _auth_handlers[] = {
    [ 0] = NULL,
    [ 1] = _auth_handler,
    [ 2] = _manifest_handler,
};
/* end{code-style-ignore} */

static const unsigned _auth_handlers_len = ARRAY_SIZE(_auth_handlers);

static suit_manifest_handler_t _manifest_get_auth_wrapper_handler(int key)
{
    return _suit_manifest_get_handler(key, _auth_handlers,
                                      _auth_handlers_len);
}
