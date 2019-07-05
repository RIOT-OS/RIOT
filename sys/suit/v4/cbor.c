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
#include "cbor.h"
#include "cose/sign.h"

#include "public_key.h"

#include "log.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


static suit_manifest_handler_t _manifest_get_auth_wrapper_handler(int key);
typedef suit_manifest_handler_t (*suit_manifest_handler_getter_t)(int key);

int suit_cbor_map_iterate_init(CborValue *map, CborValue *it)
{
    if (!cbor_value_is_map(map)) {
        LOG_INFO("suit_v4_parse(): manifest not an map\n)");
        return SUIT_ERR_INVALID_MANIFEST;
    }

    cbor_value_enter_container(map, it);

    return SUIT_OK;
}

int suit_cbor_map_iterate(CborValue *it, CborValue *key, CborValue *value)
{
    if (cbor_value_at_end(it)) {
        return 0;
    }

    *key = *it;
    cbor_value_advance(it);

    *value = *it;
    cbor_value_advance(it);

    return 1;
}

int suit_cbor_get_int(const CborValue *it, int *out)
{
    if (!cbor_value_is_integer(it)) {
        LOG_DEBUG("expected integer type, got %u\n", cbor_value_get_type(it));
        return SUIT_ERR_INVALID_MANIFEST;
    }

    /* This check tests whether the integer fits into "int", thus the check
     * is platform dependent. This is for lack of specification of actually
     * allowed values, to be made explicit at some point. */
    if (cbor_value_get_int_checked(it, out) == CborErrorDataTooLarge) {
        LOG_DEBUG("integer doesn't fit into int type\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }

    return SUIT_OK;
}

int suit_cbor_get_string(const CborValue *it, const uint8_t **buf, size_t *len)
{
    if (!(cbor_value_is_text_string(it) || cbor_value_is_byte_string(it) || cbor_value_is_length_known(it))) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    CborValue next = *it;
    cbor_value_get_string_length(it, len);
    cbor_value_advance(&next);
    *buf = next.ptr - *len;
    return SUIT_OK;
}

int suit_cbor_get_uint32(const CborValue *it, uint32_t *out)
{
    int res;
    int64_t val;
    if (!cbor_value_is_unsigned_integer(it)) {
        return CborErrorIllegalType;
    }
    if ((res = cbor_value_get_int64_checked(it, &val))) {
        return res;
    }
    if (val > 0xFFFFFFFF) {
        return CborErrorDataTooLarge;
    }
    *out = (val & 0xFFFFFFFF);

    return CborNoError;
}

int suit_cbor_get_uint(const CborValue *it, unsigned *out)
{
    return suit_cbor_get_uint32(it, (uint32_t *)out);
}

int suit_cbor_subparse(CborParser *parser, CborValue *bseq, CborValue *it)
{
    const uint8_t *bytes;
    size_t bytes_len = 0;

    if (!cbor_value_is_byte_string(bseq)) {
        LOG_DEBUG("suit_cbor_subparse(): bseq not a byte string\n");
        return -1;
    }

    suit_cbor_get_string(bseq, &bytes, &bytes_len);

    return cbor_parser_init(bytes, bytes_len, SUIT_TINYCBOR_VALIDATION_MODE, parser,
                            it);
}

static int _v4_parse(suit_v4_manifest_t *manifest, const uint8_t *buf,
                       size_t len, suit_manifest_handler_getter_t getter)
{

    CborParser parser;
    CborValue it, map, key, value;
    CborError err = cbor_parser_init(buf, len, SUIT_TINYCBOR_VALIDATION_MODE,
                                     &parser, &it);

    if (err != 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    map = it;

    if (suit_cbor_map_iterate_init(&map, &it) != SUIT_OK) {
        LOG_DEBUG("manifest not map!\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }

    LOG_DEBUG("jumping into map\n)");

    while (suit_cbor_map_iterate(&it, &key, &value)) {
        int integer_key;
        if (suit_cbor_get_int(&key, &integer_key) != SUIT_OK){
            return SUIT_ERR_INVALID_MANIFEST;
        }
        LOG_DEBUG("got key val=%i\n", integer_key);
        suit_manifest_handler_t handler = getter(integer_key);

        if (handler) {
            int res = handler(manifest, integer_key, &value);
            LOG_DEBUG("handler res=%i\n", res);
            if (res < 0) {
                LOG_INFO("handler returned <0\n)");
                return SUIT_ERR_INVALID_MANIFEST;
            }
        }
        else {
            LOG_DEBUG("no handler found\n");
        }
    }

    cbor_value_leave_container(&map, &it);

    return SUIT_OK;
}

int suit_v4_parse(suit_v4_manifest_t *manifest, const uint8_t *buf,
                       size_t len)
{
    manifest->buf = buf;
    manifest->len = len;
    return _v4_parse(manifest, buf, len, _manifest_get_auth_wrapper_handler);
}

static int _auth_handler(suit_v4_manifest_t *manifest, int key, CborValue *it)
{
    (void)key;
    const uint8_t *cose_buf;
    size_t cose_len = 0;
    int res = suit_cbor_get_string(it, &cose_buf, &cose_len);

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

static int _manifest_handler(suit_v4_manifest_t *manifest, int key, CborValue *it)
{
    (void)key;
    const uint8_t *manifest_buf;
    size_t manifest_len;

    suit_cbor_get_string(it, &manifest_buf, &manifest_len);

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
            &pkey, manifest->validation_buf, SUIT_COSE_BUF_SIZE);
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
static suit_manifest_handler_t _auth_handlers[] = {
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
