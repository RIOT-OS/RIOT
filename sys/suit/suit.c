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
 * @brief       SUIT parser library
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "suit.h"
#include "cbor.h"

static int parse_manifest_version(suit_manifest_t *manifest, CborValue *it)
{
    uint64_t value;
    if (!cbor_value_is_unsigned_integer(it)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    cbor_value_get_uint64(it, &value);
    manifest->version = (uint32_t)value;
    return SUIT_OK;
}

static int parse_manifest_size(suit_manifest_t *manifest, CborValue *it)
{
    uint64_t value;
    if (!cbor_value_is_unsigned_integer(it)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    cbor_value_get_uint64(it, &value);
    manifest->size = (uint32_t)value;
    return SUIT_OK;
}

static int parse_manifest_timestamp(suit_manifest_t *manifest, CborValue *it)
{
    uint64_t value;
    if (!cbor_value_is_unsigned_integer(it)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    cbor_value_get_uint64(it, &value);
    manifest->timestamp = (uint32_t)value;
    return SUIT_OK;
}

static int parse_manifest_digestalgo(suit_manifest_t *manifest, CborValue *it)
{
    CborValue arr;
    uint64_t value;
    if (cbor_value_is_null(it)) {
        manifest->algo = SUIT_DIGEST_NONE;
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
    manifest->algo = (suit_digest_t)value;
    return SUIT_OK;
}

int _check_manifest(CborValue *it)
{
    if (!cbor_value_is_array(it)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    size_t len;
    CborError err = cbor_value_get_array_length(it, &len);
    if (err != 0 || len < SUIT_MANIFEST_MIN_LENGTH) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return SUIT_OK;
}

int _check_payloadinfo(CborValue *it)
{
    if (!cbor_value_is_array(it))
    {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    size_t len;
    CborError err = cbor_value_get_array_length(it, &len);
    if (err != 0 || len != SUIT_MANIFEST_PAYLOADINFO_LENGTH) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return SUIT_OK;
}

int suit_parse(suit_manifest_t *manifest, const uint8_t *buf, size_t len)
{
    manifest->buf = buf;
    manifest->len = len;
    CborParser parser;
    CborValue it, arr, pi;
    manifest->size = 0;
    CborError err = cbor_parser_init(buf, len, 0, &parser, &it);
    if (err != 0) {
        return err;
    }
    if (_check_manifest(&it)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    err = cbor_value_enter_container(&it, &arr);
    if (err != 0) {
        return err;
    }

    int res = parse_manifest_version(manifest, &arr);
    if (res < 0) {
        return res;
    }

    /* skip text and nonce */
    cbor_value_advance(&arr);
    cbor_value_advance(&arr);

    cbor_value_advance(&arr);
    res = parse_manifest_timestamp(manifest, &arr);
    if (res < 0) {
        return res;
    }

    /* TODO: parse conditionals */
    cbor_value_advance(&arr);

    /* Ignore directives, aliases, dependencies and extensions for now */
    cbor_value_advance(&arr);
    cbor_value_advance(&arr);
    cbor_value_advance(&arr);
    cbor_value_advance(&arr);


    if (cbor_value_at_end(&arr)) {
        /* Manifest parsed, no payloadinfo, but is valid */
        return SUIT_OK;
    }
    cbor_value_advance(&arr);
    /* arr at payloadinfo */
    if (_check_payloadinfo(&arr))
    {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    /* Parse payload info */
    cbor_value_enter_container(&arr, &pi);

    /* Skip format */
    cbor_value_advance(&pi);
    parse_manifest_size(manifest, &pi);
    cbor_value_advance(&pi);
    cbor_value_advance(&pi);
    cbor_value_advance(&pi);
    parse_manifest_digestalgo(manifest, &pi);
    cbor_value_advance(&pi);
    manifest->digests = pi.ptr;
    cbor_value_advance(&pi);
    manifest->digest_len = pi.ptr - manifest->digests;
    return SUIT_OK;
}

uint32_t suit_get_version(const suit_manifest_t *manifest)
{
    return manifest->version;
}

uint32_t suit_get_timestamp(const suit_manifest_t *manifest)
{
    return manifest->timestamp;
}

int suit_payload_get_digest(const suit_manifest_t *manifest,
            suit_digest_type_t digest, uint8_t *buf, size_t *len)
{
    (void)digest;
    (void)buf;
    (void)len;
    CborParser parser;
    CborValue it, map;
    CborError err = cbor_parser_init(manifest->digests, manifest->digest_len, 0, &parser, &it);
    if (err != 0) {
        return err;
    }
    cbor_value_enter_container(&it, &map);
    while(!cbor_value_at_end(&map)) {
        int64_t type;
        cbor_value_get_int64(&map, &type);
        cbor_value_advance(&map);
        if (type == digest) {
            cbor_value_copy_byte_string(&map, buf, len, NULL);
            return 1;
        }

    }
    return 0;
}

int suit_manifest_get_storid(suit_manifest_t *manifest, uint8_t *buf, size_t len)
{
    (void)manifest;
    (void)buf;
    (void)len;
//    if (!cbor_value_is_byte_string(it)) {
//        return SUIT_ERR_INVALID_MANIFEST;
//    }
//    cbor_
    return SUIT_OK;
}
