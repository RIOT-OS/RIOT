/*
 * Copyright (C) 2019 Koen Zandberg
 *               2020 Inria
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
 * @brief       SUIT Handlers for the command sequences in the common section of
 *              a SUIT manifest.
 *
 * This file contains the functions to handle command sequences from a SUIT
 * manifest. This includes both directives and conditions.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <inttypes.h>
#include <nanocbor/nanocbor.h>
#include <assert.h>

#include "hashes/sha256.h"

#include "kernel_defines.h"
#include "suit/conditions.h"
#include "suit/handlers.h"
#include "suit/policy.h"
#include "suit/storage.h"
#include "suit.h"

#ifdef MODULE_SUIT_TRANSPORT_COAP
#include "suit/transport/coap.h"
#endif
#include "suit/transport/mock.h"

#include "log.h"

static int _get_component_size(suit_manifest_t *manifest,
                               suit_component_t *comp,
                               uint32_t *img_size)
{
    nanocbor_value_t param_size;
    if ((suit_param_ref_to_cbor(manifest, &comp->param_size, &param_size) == 0)
            || (nanocbor_get_uint32(&param_size, img_size) < 0)) { return
        SUIT_ERR_INVALID_MANIFEST;
    }
    return SUIT_OK;
}

static suit_component_t *_get_component(suit_manifest_t *manifest)
{
    /* Out-of-bounds check has been done in the _dtv_set_comp_idx, True/False
     * not handled here intentionally */
    assert(manifest->component_current < CONFIG_SUIT_COMPONENT_MAX);
    return &manifest->components[manifest->component_current];
}

static int _validate_uuid(suit_manifest_t *manifest,
                          suit_param_ref_t *ref,
                          uuid_t *uuid)
{
    const uint8_t *uuid_manifest_ptr;
    size_t len = sizeof(uuid_t);
    nanocbor_value_t it;

    if ((suit_param_ref_to_cbor(manifest, ref, &it) == 0) ||
            (nanocbor_get_bstr(&it, &uuid_manifest_ptr, &len) < 0)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    char uuid_str[UUID_STR_LEN + 1];
    char uuid_str2[UUID_STR_LEN + 1];
    uuid_to_string((uuid_t *)uuid_manifest_ptr, uuid_str);
    uuid_to_string(uuid, uuid_str2);
    LOG_INFO("Comparing %s to %s from manifest\n", uuid_str2, uuid_str);

    return uuid_equal(uuid, (uuid_t *)uuid_manifest_ptr)
           ? SUIT_OK
           : SUIT_ERR_COND;
}

static int _cond_vendor_handler(suit_manifest_t *manifest,
                                int key,
                                nanocbor_value_t *it)
{
    (void)key;
    (void)it;
    LOG_INFO("validating vendor ID\n");
    suit_component_t *comp = _get_component(manifest);
    int rc = _validate_uuid(manifest, &comp->param_vendor_id,
                            suit_get_vendor_id());
    if (rc == SUIT_OK) {
        LOG_INFO("validating vendor ID: OK\n");
        manifest->validated |= SUIT_VALIDATED_VENDOR;
    }
    return rc;
}

static int _cond_class_handler(suit_manifest_t *manifest,
                               int key,
                               nanocbor_value_t *it)
{
    (void)key;
    (void)it;
    LOG_INFO("validating class id\n");
    suit_component_t *comp = _get_component(manifest);
    int rc = _validate_uuid(manifest, &comp->param_class_id,
                            suit_get_class_id());
    if (rc == SUIT_OK) {
        LOG_INFO("validating class id: OK\n");
        manifest->validated |= SUIT_VALIDATED_CLASS;
    }
    return rc;
}

static int _cond_comp_offset(suit_manifest_t *manifest,
                             int key,
                             nanocbor_value_t *it)
{
    (void)manifest;
    (void)key;
    uint32_t offset;
    uint32_t report;

    suit_component_t *comp = _get_component(manifest);

    /* Grab offset from param */
    if (nanocbor_get_uint32(it, &report) < 0) {
        LOG_WARNING("_cond_comp_offset(): expected None param\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }
    nanocbor_value_t param_offset;
    suit_param_ref_to_cbor(manifest, &comp->param_component_offset,
                           &param_offset);
    nanocbor_get_uint32(&param_offset, &offset);

    if (!suit_storage_has_offset(comp->storage_backend)) {
        return SUIT_ERR_COND;
    }

    LOG_INFO("Comparing manifest offset %"PRIx32" with other slot offset\n",
             offset);

    return suit_storage_match_offset(comp->storage_backend, offset) ?
        SUIT_OK : SUIT_ERR_COND;
}

static int _dtv_set_comp_idx(suit_manifest_t *manifest,
                             int key,
                             nanocbor_value_t *it)
{
    (void)key;
    bool index = false;
    uint32_t new_index;

    /* It can be a bool, meaning all or none of the components */
    if (nanocbor_get_bool(it, &index) >= 0) {
        new_index = index ?
            SUIT_MANIFEST_COMPONENT_ALL : SUIT_MANIFEST_COMPONENT_NONE;
    }
    /* It can be a positive integer, meaning one of the components */
    else if (nanocbor_get_uint32(it, &new_index) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    /* And if it is an integer it must be within the allowed bounds */
    else if (new_index >= CONFIG_SUIT_COMPONENT_MAX) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    suit_component_t *component = &manifest->components[new_index];
    char name[CONFIG_SUIT_COMPONENT_MAX_NAME_LEN];

    suit_storage_t *storage = component->storage_backend;
    char separator = suit_storage_get_separator(storage);

    /* Done this before in the component stage, shouldn't be different now */
    suit_component_name_to_string(manifest, component,
                                  separator, name, sizeof(name));

    suit_storage_set_active_location(storage, name);

    /* Update the manifest context */
    manifest->component_current = new_index;

    LOG_INFO("Setting component index to %d\n",
              (int)manifest->component_current);
    return 0;
}

static int _dtv_run_seq_cond(suit_manifest_t *manifest,
                             int key,
                             nanocbor_value_t *it)
{
    (void)key;
    LOG_DEBUG("Starting conditional sequence handler\n");
    return suit_handle_manifest_structure_bstr(manifest, it,
            suit_command_sequence_handlers, suit_command_sequence_handlers_len);
}

static int _dtv_try_each(suit_manifest_t *manifest,
                         int key, nanocbor_value_t *it)
{
    (void)key;
    LOG_DEBUG("Starting suit-directive-try-each handler\n");
    nanocbor_value_t container;

    if ((nanocbor_enter_array(it, &container) < 0) &&
        (nanocbor_enter_map(it, &container) < 0)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    int res = SUIT_ERR_COND;
    while (!nanocbor_at_end(&container)) {
        nanocbor_value_t _container = container;
        /* `_container` should be CBOR _bstr wrapped according to the spec, but
         * it is not */
        res = suit_handle_manifest_structure_bstr(manifest, &_container,
                suit_command_sequence_handlers,
                suit_command_sequence_handlers_len);

        nanocbor_skip(&container);

        if (res != SUIT_ERR_COND) {
            break;
        }
    }

    return res;
}

static int _dtv_set_param(suit_manifest_t *manifest, int key,
                          nanocbor_value_t *it)
{
    (void)key;
    /* `it` points to the entry of the map containing the type and value */
    nanocbor_value_t map;

    nanocbor_enter_map(it, &map);

    suit_component_t *comp = _get_component(manifest);

    while (!nanocbor_at_end(&map)) {
        /* map points to the key of the param */
        int32_t param_key;
        if (nanocbor_get_int32(&map, &param_key) < 0) {
            return SUIT_ERR_INVALID_MANIFEST;
        }
        LOG_DEBUG("param_key=%" PRIi32 "\n", param_key);
        unsigned int type = nanocbor_get_type(&map);
        /* Filter 'complex' types and only allow int, nint, bstr and tstr types
         * for parameter values */
        if (type > NANOCBOR_TYPE_TSTR) {
            return SUIT_ERR_INVALID_MANIFEST;
        }
        suit_param_ref_t *ref;
        switch (param_key) {
            case SUIT_PARAMETER_VENDOR_IDENTIFIER:
                ref = &comp->param_vendor_id;
                break;
            case SUIT_PARAMETER_CLASS_IDENTIFIER:
                ref = &comp->param_class_id;
                break;
            case SUIT_PARAMETER_IMAGE_DIGEST:
                ref = &comp->param_digest;
                break;
            case SUIT_PARAMETER_COMPONENT_OFFSET:
                ref = &comp->param_component_offset;
                break;
            case SUIT_PARAMETER_IMAGE_SIZE:
                ref = &comp->param_size;
                break;
            case SUIT_PARAMETER_URI:
                ref = &comp->param_uri;
                break;
            default:
                LOG_DEBUG("Unsupported parameter %" PRIi32 "\n", param_key);
                return SUIT_ERR_UNSUPPORTED;
        }

        suit_param_cbor_to_ref(manifest, ref, &map);

        /* Simple skip is sufficient to skip non-complex types */
        nanocbor_skip(&map);
    }
    return SUIT_OK;
}

static int _start_storage(suit_manifest_t *manifest, suit_component_t *comp)
{
    uint32_t img_size = 0;
    char name[CONFIG_SUIT_COMPONENT_MAX_NAME_LEN];
    char separator = suit_storage_get_separator(comp->storage_backend);

    if (_get_component_size(manifest, comp, &img_size) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    /* Done this before in the component stage, shouldn't be different now */
    suit_component_name_to_string(manifest, comp,
                                  separator, name, sizeof(name));

    suit_storage_set_active_location(comp->storage_backend, name);

    return suit_storage_start(comp->storage_backend, manifest, img_size);
}

static int _dtv_fetch(suit_manifest_t *manifest, int key,
                      nanocbor_value_t *_it)
{
    (void)key; (void)_it;
    LOG_DEBUG("_dtv_fetch() key=%i\n", key);

    const uint8_t *url;
    size_t url_len;

    /* Check the policy before fetching anything */
    int res = suit_policy_check(manifest);
    if (res) {
        return SUIT_ERR_POLICY_FORBIDDEN;
    }

    suit_component_t *comp = _get_component(manifest);

    /* Deny the fetch if the component was already fetched before */
    if (suit_component_check_flag(comp, SUIT_COMPONENT_STATE_FETCHED)) {
        LOG_ERROR("Component already fetched before\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }

    nanocbor_value_t param_uri;
    suit_param_ref_to_cbor(manifest, &comp->param_uri,
                           &param_uri);
    int err = nanocbor_get_tstr(&param_uri, &url, &url_len);
    if (err < 0) {
        LOG_DEBUG("URL parsing failed\n)");
        return err;
    }
    memcpy(manifest->urlbuf, url, url_len);
    manifest->urlbuf[url_len] = '\0';

    LOG_DEBUG("_dtv_fetch() fetching \"%s\" (url_len=%u)\n", manifest->urlbuf,
              (unsigned)url_len);

    if (_start_storage(manifest, comp) < 0) {
        LOG_ERROR("Unable to start storage backend\n");
        return SUIT_ERR_STORAGE;
    }

    res = -1;

    if (0) {}
#ifdef MODULE_SUIT_TRANSPORT_COAP
    else if (strncmp(manifest->urlbuf, "coap://", 7) == 0) {
        res = suit_coap_get_blockwise_url(manifest->urlbuf, CONFIG_SUIT_COAP_BLOCKSIZE,
                                          suit_storage_helper,
                                          manifest);
    }
#endif
#ifdef MODULE_SUIT_TRANSPORT_MOCK
    else if (strncmp(manifest->urlbuf, "test://", 7) == 0) {
        res = suit_transport_mock_fetch(manifest);
    }
#endif
    else {
        LOG_WARNING("suit: unsupported URL scheme!\n)");
        return res;
    }

    suit_component_set_flag(comp, SUIT_COMPONENT_STATE_FETCHED);

    if (res) {
        suit_component_set_flag(comp, SUIT_COMPONENT_STATE_FETCH_FAILED);
        /* TODO: The leftover data from a failed fetch should be purged. It
         * could contain potential malicious data from an attacker */
        LOG_INFO("image download failed with code %i\n", res);
        return res;
    }

    LOG_DEBUG("Update OK\n");
    return SUIT_OK;
}

static int _get_digest(nanocbor_value_t *bstr, const uint8_t **digest, size_t
                       *digest_len)
{
    /* Bstr is a byte string with a cbor array containing the type and the
     * digest */

    const uint8_t *digest_struct;
    size_t digest_struct_len;
    uint32_t digest_type;
    nanocbor_value_t digest_it;
    nanocbor_value_t arr_it;

    nanocbor_get_bstr(bstr, &digest_struct, &digest_struct_len);

    nanocbor_decoder_init(&digest_it, digest_struct, digest_struct_len);
    nanocbor_enter_array(&digest_it, &arr_it);
    nanocbor_get_uint32(&arr_it, &digest_type);
    return nanocbor_get_bstr(&arr_it, digest, digest_len);
}

static int _validate_payload(suit_component_t *component, const uint8_t *digest,
                             size_t payload_size)
{
    uint8_t payload_digest[SHA256_DIGEST_LENGTH];
    suit_storage_t *storage = component->storage_backend;

    if (suit_storage_has_readptr(storage)) {
        /* Direct read possible */
        const uint8_t *payload = NULL;
        size_t payload_len = 0;

        suit_storage_read_ptr(storage, &payload, &payload_len);
        if (payload_size != payload_len) {
            return SUIT_ERR_STORAGE_EXCEEDED;
        }
        sha256(payload, payload_len, payload_digest);
    }
    else {
        /* Piecewise feeding */
        sha256_context_t ctx;
        sha256_init(&ctx);
        size_t pos = 0;
        while (pos < payload_size) {
            uint8_t buf[64];

            size_t read_len = (payload_size - pos) > sizeof(buf) ?
                              sizeof(buf) : payload_size - pos;

            suit_storage_read(storage, buf, pos, read_len);
            sha256_update(&ctx, buf, read_len);

            pos += read_len;
        }
        sha256_final(&ctx, payload_digest);
    }

    return (memcmp(digest, payload_digest, SHA256_DIGEST_LENGTH) == 0) ?
        SUIT_OK : SUIT_ERR_DIGEST_MISMATCH;
}

static int _dtv_verify_image_match(suit_manifest_t *manifest, int key,
                                   nanocbor_value_t *_it)
{
    (void)key; (void)_it;
    LOG_DEBUG("dtv_image_match\n");
    const uint8_t *digest;
    size_t digest_len;
    suit_component_t *comp = _get_component(manifest);

    uint32_t img_size;
    if (_get_component_size(manifest, comp, &img_size) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    /* Only check the component if it is fetched, but not failed */
    if (!suit_component_check_flag(comp, SUIT_COMPONENT_STATE_FETCHED) ||
            suit_component_check_flag(comp,
                                      SUIT_COMPONENT_STATE_FETCH_FAILED)) {
        LOG_ERROR("Fetch failed, or nothing fetched, nothing to check: %u\n",
                  comp->state);
        return SUIT_ERR_INVALID_MANIFEST;
    }

    LOG_INFO("Verifying image digest\n");
    nanocbor_value_t _v;
    if (suit_param_ref_to_cbor(manifest, &comp->param_digest, &_v) == 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    int res = _get_digest(&_v, &digest, &digest_len);

    if (res < 0) {
        LOG_DEBUG("Unable to parse digest structure\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }

    /* TODO: replace with generic verification (not only sha256) */
    LOG_INFO("Starting digest verification against image\n");
    res = _validate_payload(comp, digest, img_size);
    if (res == SUIT_OK) {
        LOG_INFO("Install correct payload\n");
        suit_storage_install(comp->storage_backend, manifest);
    }
    else {
        LOG_INFO("Erasing bad payload\n");
        if (comp->storage_backend->driver->erase) {
            suit_storage_erase(comp->storage_backend);
        }
    }
    return res;
}

/* begin{code-style-ignore} */
const suit_manifest_handler_t suit_command_sequence_handlers[] = {
    [SUIT_COND_VENDOR_ID]        = _cond_vendor_handler,
    [SUIT_COND_CLASS_ID]         = _cond_class_handler,
    [SUIT_COND_IMAGE_MATCH]      = _dtv_verify_image_match,
    [SUIT_COND_COMPONENT_OFFSET] = _cond_comp_offset,
    [SUIT_DIR_SET_COMPONENT_IDX] = _dtv_set_comp_idx,
    [SUIT_DIR_TRY_EACH]          = _dtv_try_each,
    [SUIT_DIR_SET_PARAM]         = _dtv_set_param,
    [SUIT_DIR_OVERRIDE_PARAM]    = _dtv_set_param,
    [SUIT_DIR_FETCH]             = _dtv_fetch,
    [SUIT_DIR_RUN_SEQUENCE]      = _dtv_run_seq_cond,
};
/* end{code-style-ignore} */

const size_t suit_command_sequence_handlers_len =
        ARRAY_SIZE(suit_command_sequence_handlers);
