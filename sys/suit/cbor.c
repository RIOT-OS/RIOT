/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
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
 *
 * @}
 */
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "suit/cbor.h"
#include "suit.h"
#include "cbor.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int _advance_x(CborValue *it, unsigned x)
{
    for (unsigned i = 0; i < x; i++) {
        cbor_value_advance(it);
        if (cbor_value_at_end(it)) {
            return -1;
        }
    }
    return 0;
}

static int _parse_manifest_version(CborValue *it,
                                   uint32_t *version)
{
    uint64_t value;

    if (!cbor_value_is_unsigned_integer(it)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    cbor_value_get_uint64(it, &value);
    *version = (uint32_t)value;
    return SUIT_OK;
}

static int _parse_manifest_seq_no(CborValue *it, uint32_t *seq_no)
{
    uint64_t value;

    if (!cbor_value_is_unsigned_integer(it)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    cbor_value_get_uint64(it, &value);
    *seq_no = (uint32_t)value;
    return SUIT_OK;
}

static ssize_t _parse_manifest_condition(CborValue *it, CborValue *cond,
                                         CborValue *props, size_t idx)
{
    size_t condlen = 0;

    if (!cbor_value_is_array(it)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    cbor_value_enter_container(it, cond);

    /* Conditions array is nonzero in length */
    if (cbor_value_at_end(cond)) {
        return SUIT_ERR_COND;
    }

    /* Iterate to idx and check if it is within the array bounds */
    if (_advance_x(cond, idx) < 0) {
        return SUIT_ERR_COND;
    }

    /* Condition must be a valid array of length 2 */
    if (!cbor_value_is_array(cond) ||
        (cbor_value_get_array_length(cond, &condlen) < 0) ||
        condlen != 2) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    cbor_value_enter_container(cond, props);

    /* First element must be an integer */
    if (!cbor_value_is_integer(props)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return SUIT_OK;
}

static ssize_t _parse_manifest_condition_param(CborValue *it, size_t idx,
                                               uint8_t *buf, size_t *len)
{
    CborValue cond, props;

    int res = _parse_manifest_condition(it, &cond, &props, idx);
    if (res < 0) {
        return res;
    }
    /* At this point props points to the condition type of the condition at
     * position idx */

    /* Advance to the argument */
    cbor_value_advance(&props);
    if (!cbor_value_is_byte_string(&props)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    if (cbor_value_copy_byte_string(&props, buf, len, NULL) < 0) {
        return SUIT_ERR_COND;
    }
    return *len;
}

static ssize_t _parse_manifest_condition_type(CborValue *it, size_t idx,
                                              int *type)
{
    CborValue cond, props;

    int res = _parse_manifest_condition(it, &cond, &props, idx);
    if (res < 0) {
        return res;
    }
    /* At this point props points to the condition type of the condition at
     * position idx */

    cbor_value_get_int(&props, type);
    cbor_value_advance(&props);
    if (!cbor_value_is_byte_string(&props)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    size_t arglen = 0;
    cbor_value_get_string_length(&props, &arglen);
    return arglen;
}

static int _parse_payload_size(CborValue *it, uint32_t *size)
{
    uint64_t value;

    if (!cbor_value_is_unsigned_integer(it)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    cbor_value_get_uint64(it, &value);
    *size = (uint32_t)value;
    return SUIT_OK;
}

static int _parse_payload_storage_id(CborValue *it, uint8_t *buf, size_t *len)
{
    if (!cbor_value_is_byte_string(it)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    cbor_value_copy_byte_string(it, buf, len, NULL);
    return SUIT_OK;
}

static int _parse_manifest_digestalgo(CborValue *it,
                                      suit_cbor_digest_t *algo)
{
    CborValue arr;
    uint64_t value;

    if (cbor_value_is_null(it)) {
        *algo = SUIT_DIGEST_NONE;
        return SUIT_OK;
    }
    if (!cbor_value_is_array(it)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    size_t len = 0;
    if (cbor_value_get_array_length(it, &len) || len > 2) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    if (cbor_value_enter_container(it, &arr)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    cbor_value_get_uint64(&arr, &value);
    *algo = (suit_cbor_digest_t)value;
    return SUIT_OK;
}

int _validate_manifest(CborValue *it)
{
    if (!cbor_value_is_array(it)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    size_t len;
    CborError err = cbor_value_get_array_length(it, &len);
    if (err != 0 || len < SUIT_CBOR_MANIFEST_MIN_LENGTH) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return SUIT_OK;
}

int _validate_payloadinfo(CborValue *it)
{
    if (!cbor_value_is_array(it)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    size_t len;
    CborError err = cbor_value_get_array_length(it, &len);
    if (err != 0 || len != SUIT_CBOR_MANIFEST_PAYLOADINFO_LENGTH) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return SUIT_OK;
}

int suit_cbor_parse(suit_cbor_manifest_t *manifest, const uint8_t *buf, size_t len)
{
    CborParser parser;
    CborValue it, arr;
    CborError err = cbor_parser_init(buf, len, SUIT_TINYCBOR_VALIDATION_MODE,
                                     &parser, &it);

    if (err != 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    /* Validate manifest size */
    if (_validate_manifest(&it)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    /* At this point the manifest should be valid enough */
    manifest->buf = buf;
    manifest->len = len;

    err = cbor_value_enter_container(&it, &arr);
    if (err != 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    if (_advance_x(&arr, SUIT_CBOR_MANIFEST_IDX_PAYLOADINFO) < 0) {
        /* If it is impossible to advance to the payloadinfo we assume that
         * there is no payloadinfo. Length validation is already done by
         * _validate_manifest, an invalid array length should not be possible
         * at this point.
         */
        return SUIT_OK;
    }

    if (_validate_payloadinfo(&arr)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return SUIT_OK;
}

static int _init_and_advance(const suit_cbor_manifest_t *manifest,
                             CborParser *parser, CborValue *it,
                             CborValue *arr, size_t offset)
{
    cbor_parser_init(manifest->buf, manifest->len,
                     SUIT_TINYCBOR_VALIDATION_MODE, parser, it);
    cbor_value_enter_container(it, arr);
    if (_advance_x(arr, offset) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return SUIT_OK;
}

static int _init_and_advance_info(const suit_cbor_manifest_t *manifest,
                                  CborParser *parser, CborValue *it,
                                  CborValue *arr, CborValue *payloadinfo,
                                  size_t offset)
{
    if (_init_and_advance(manifest, parser, it,
                          arr, SUIT_CBOR_MANIFEST_IDX_PAYLOADINFO) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    cbor_value_enter_container(arr, payloadinfo);
    if (_advance_x(payloadinfo, offset) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return SUIT_OK;
}

int suit_cbor_get_version(const suit_cbor_manifest_t *manifest,
                          uint32_t *version)
{
    CborParser parser;
    CborValue it, arr;

    cbor_parser_init(manifest->buf, manifest->len,
                     SUIT_TINYCBOR_VALIDATION_MODE, &parser, &it);
    cbor_value_enter_container(&it, &arr);
    return _parse_manifest_version(&arr, version);
}

int suit_cbor_get_seq_no(const suit_cbor_manifest_t *manifest,
                         uint32_t *seq_no)
{
    CborParser parser;
    CborValue it, arr;

    if (_init_and_advance(manifest, &parser, &it,
                          &arr, SUIT_CBOR_MANIFEST_IDX_SEQ_NO) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return _parse_manifest_seq_no(&arr, seq_no);
}

ssize_t suit_cbor_get_condition_type(const suit_cbor_manifest_t *manifest,
                                     unsigned idx, signed *condition)
{
    CborParser parser;
    CborValue it, arr;

    if (_init_and_advance(manifest, &parser, &it,
                          &arr, SUIT_CBOR_MANIFEST_IDX_CONDITIONS) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return _parse_manifest_condition_type(&arr, idx, condition);
}

int suit_cbor_get_condition_parameter(const suit_cbor_manifest_t *manifest,
                                      unsigned idx, uint8_t *buf, size_t *len)
{
    CborParser parser;
    CborValue it, arr;

    if (_init_and_advance(manifest, &parser, &it,
                          &arr, SUIT_CBOR_MANIFEST_IDX_CONDITIONS) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return _parse_manifest_condition_param(&arr, idx, buf, len);
}

int suit_cbor_payload_get_digestalgo(const suit_cbor_manifest_t *manifest,
                                     suit_cbor_digest_t *algo)
{
    CborParser parser;
    CborValue it, arr, payloadinfo;

    if (_init_and_advance_info(manifest, &parser, &it, &arr, &payloadinfo,
                               SUIT_CBOR_PAYLOADINFO_IDX_DIGESTALGO) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return _parse_manifest_digestalgo(&payloadinfo, algo);
}

int suit_cbor_payload_get_digest(const suit_cbor_manifest_t *manifest,
                                 suit_cbor_digest_type_t digest, uint8_t *buf,
                                 size_t *len)
{
    CborParser parser;
    CborValue it, arr, payloadinfo, map;

    if (_init_and_advance_info(manifest, &parser, &it, &arr, &payloadinfo,
                               SUIT_CBOR_PAYLOADINFO_IDX_DIGESTS) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    if (!cbor_value_is_map(&payloadinfo)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    cbor_value_enter_container(&payloadinfo, &map);
    while (!cbor_value_at_end(&map)) {
        int64_t type;
        cbor_value_get_int64(&map, &type);
        cbor_value_advance(&map);
        if (type == digest) {
            cbor_value_copy_byte_string(&map, buf, len, NULL);
            return 1;
        }
        cbor_value_advance(&map);
    }
    return 0;
}

int suit_cbor_payload_get_size(const suit_cbor_manifest_t *manifest,
                               uint32_t *size)
{
    CborParser parser;
    CborValue it, arr, payloadinfo;

    if (_init_and_advance_info(manifest, &parser, &it, &arr, &payloadinfo,
                               SUIT_CBOR_PAYLOADINFO_IDX_SIZE) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return _parse_payload_size(&payloadinfo, size);
}

int suit_cbor_payload_get_storid(const suit_cbor_manifest_t *manifest,
                                 uint8_t *buf, size_t *len)
{
    CborParser parser;
    CborValue it, arr, payloadinfo;

    if (_init_and_advance_info(manifest, &parser, &it, &arr, &payloadinfo,
                               SUIT_CBOR_PAYLOADINFO_IDX_STORID) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return _parse_payload_storage_id(&payloadinfo, buf, len);
}

ssize_t suit_cbor_get_url(const suit_cbor_manifest_t *manifest, char *buf,
                          size_t len)
{
    CborParser parser;
    CborValue it, arr, payloadinfo, urilist, uri;

    if (_init_and_advance_info(manifest, &parser, &it, &arr, &payloadinfo,
                               SUIT_CBOR_PAYLOADINFO_IDX_URIS) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    if (!cbor_value_is_array(&payloadinfo)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    size_t arr_len;
    cbor_value_get_array_length(&payloadinfo, &arr_len);
    if (arr_len == 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    cbor_value_enter_container(&payloadinfo, &urilist);
    if (!cbor_value_is_array(&urilist)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    cbor_value_get_array_length(&urilist, &arr_len);
    if (arr_len != 2) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    cbor_value_enter_container(&urilist, &uri);
    cbor_value_advance(&uri);
    if (!cbor_value_is_text_string(&uri)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    size_t uri_len = 0;
    cbor_value_get_string_length(&uri, &uri_len);
    if (uri_len > len) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    else {
        cbor_value_copy_text_string(&uri, buf, &len, NULL);
        return uri_len;
    }
}
