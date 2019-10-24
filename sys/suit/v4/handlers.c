/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_suit_v4
 * @{
 *
 * @file
 * @brief       SUIT v4
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <inttypes.h>

#include "suit/coap.h"
#include "suit/conditions.h"
#include "suit/v4/suit.h"
#include "suit/v4/handlers.h"
#include "suit/v4/policy.h"
#include "suit/v4/suit.h"
#include "riotboot/hdr.h"
#include "riotboot/slot.h"
#include <nanocbor/nanocbor.h>

#include "log.h"

#define HELLO_HANDLER_MAX_STRLEN 32

static int _handle_command_sequence(suit_v4_manifest_t *manifest, nanocbor_value_t *it,
        suit_manifest_handler_t handler);
static int _common_handler(suit_v4_manifest_t *manifest, int key, nanocbor_value_t *it);
static int _common_sequence_handler(suit_v4_manifest_t *manifest, int key, nanocbor_value_t *it);

static int _hello_handler(suit_v4_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)manifest;
    (void)key;

    if (nanocbor_get_type(it) == NANOCBOR_TYPE_TSTR) {
        size_t len = HELLO_HANDLER_MAX_STRLEN;
        char buf[HELLO_HANDLER_MAX_STRLEN];
        nanocbor_get_tstr(it, ((const uint8_t **) &buf), &len);
        return SUIT_OK;
    }
    else {
        LOG_DEBUG("_hello_handler(): unexpected value type: %u\n", nanocbor_get_type(it));
        return -1;
    }
}

static int _validate_uuid(suit_v4_manifest_t *manifest, nanocbor_value_t *it, uuid_t *uuid)
{
    (void)manifest;
    const uint8_t *uuid_manifest_ptr;
    size_t len = sizeof(uuid_t);
    char uuid_str[UUID_STR_LEN + 1];
    char uuid_str2[UUID_STR_LEN + 1];
    if (suit_cbor_get_string(it, &uuid_manifest_ptr, &len) != SUIT_OK) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    uuid_to_string((uuid_t *)uuid_manifest_ptr, uuid_str);
    uuid_to_string(uuid, uuid_str2);
    LOG_INFO("Comparing %s to %s from manifest\n", uuid_str2, uuid_str);
    return uuid_equal(uuid, (uuid_t *)uuid_manifest_ptr) ? 0 : -1;
}

static int _cond_vendor_handler(suit_v4_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)key;
    LOG_INFO("validating vendor ID\n");
    int rc = _validate_uuid(manifest, it, suit_get_vendor_id());
    if (rc == SUIT_OK) {
        LOG_INFO("validating vendor ID: OK\n");
        manifest->validated |= SUIT_VALIDATED_VENDOR;
    }
    return rc;
}

static int _cond_class_handler(suit_v4_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)key;
    LOG_INFO("validating class id\n");
    int rc = _validate_uuid(manifest, it, suit_get_class_id());
    if (rc == SUIT_OK) {
        LOG_INFO("validating class id: OK\n");
        manifest->validated |= SUIT_VALIDATED_CLASS;
    }
    return rc;
}

static int _cond_comp_offset(suit_v4_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)manifest;
    (void)key;
    uint32_t offset;
    suit_cbor_get_uint32(it, &offset);
    uint32_t other_offset = (uint32_t)riotboot_slot_get_hdr(riotboot_slot_other()) \
                            - CPU_FLASH_BASE;
    LOG_INFO("Comparing manifest offset %u with other slot offset %u\n",
           (unsigned)offset, (unsigned)other_offset);
    return other_offset == offset ? 0 : -1;
}

static int _dtv_set_comp_idx(suit_v4_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)key;
    if (nanocbor_get_type(it) == NANOCBOR_TYPE_FLOAT) {
        LOG_DEBUG("_dtv_set_comp_idx() ignoring boolean and floats\n)");
        nanocbor_skip(it);
        return 0;
    }
    int res = suit_cbor_get_int32(it, &manifest->component_current);
    if (!res) {
        LOG_DEBUG("Setting component index to %d\n", (int)manifest->component_current);
    }
    return res;
}

static int _dtv_run_seq_cond(suit_v4_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)key;
    LOG_DEBUG("Starting conditional sequence handler\n");
    _handle_command_sequence(manifest, it, _common_sequence_handler);
    return 0;
}

static int _param_get_uri_list(suit_v4_manifest_t *manifest, nanocbor_value_t *it)
{
    LOG_DEBUG("got url list\n");
    manifest->components[manifest->component_current].url = *it;
    return 0;
}
static int _param_get_digest(suit_v4_manifest_t *manifest, nanocbor_value_t *it)
{
    LOG_DEBUG("got digest\n");
    manifest->components[manifest->component_current].digest = *it;
    return 0;
}

static int _param_get_img_size(suit_v4_manifest_t *manifest, nanocbor_value_t *it)
{
    int res = suit_cbor_get_uint32(it, &manifest->components[0].size);
    if (res) {
        LOG_DEBUG("error getting image size\n");
        return res;
    }
    return res;
}

static int _dtv_set_param(suit_v4_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)key;
    /* `it` points to the entry of the map containing the type and value */
    nanocbor_value_t map;

    nanocbor_enter_map(it, &map);

    while (!nanocbor_at_end(&map)) {
        /* map points to the key of the param */
        int32_t param_key;
        suit_cbor_get_int32(&map, &param_key);
        LOG_DEBUG("Setting component index to %" PRIi32 "\n", manifest->component_current);
        LOG_DEBUG("param_key=%" PRIi32 "\n", param_key);
        int res;
        switch (param_key) {
            case 6: /* SUIT URI LIST */
                res = _param_get_uri_list(manifest, &map);
                break;
            case 11: /* SUIT DIGEST */
                res = _param_get_digest(manifest, &map);
                break;
            case 12: /* SUIT IMAGE SIZE */
                res = _param_get_img_size(manifest, &map);
                break;
            default:
                res = -1;
        }

        nanocbor_skip(&map);

        if (res) {
            return res;
        }
    }
    return SUIT_OK;
}

static int _dtv_fetch(suit_v4_manifest_t *manifest, int key, nanocbor_value_t *_it)
{
    (void)key; (void)_it; (void)manifest;
    LOG_DEBUG("_dtv_fetch() key=%i\n", key);

    const uint8_t *url;
    size_t url_len;

    /* TODO: there must be a simpler way */
    {
        /* the url list is a binary sequence containing a cbor array of
         * (priority, url) tuples (represented as array with length two)
         * */

        nanocbor_value_t it;

        /* open sequence with cbor parser */
        int err = suit_cbor_subparse(&manifest->components[0].url, &it);
        if (err < 0) {
            LOG_DEBUG("subparse failed\n)");
            return err;
        }

        /* confirm the document contains an array */
        if (nanocbor_get_type(&it) != NANOCBOR_TYPE_ARR) {
            LOG_DEBUG("url list no array\n)");
            LOG_DEBUG("type: %u\n", nanocbor_get_type(&it));
        }

        /* enter container, confirm it is an array, too */
        nanocbor_value_t url_it;
        nanocbor_enter_array(&it, &url_it);
        if (nanocbor_get_type(&url_it) != NANOCBOR_TYPE_ARR) {
            LOG_DEBUG("url entry no array\n)");
        }

        /* expect two entries: priority as int, url as byte string. bail out if not. */
        nanocbor_value_t url_value_it;
        nanocbor_enter_array(&url_it, &url_value_it);

        /* check that first array entry is an int (the priotity of the url) */
        if (nanocbor_get_type(&url_value_it) != NANOCBOR_TYPE_UINT) {
            LOG_DEBUG("expected URL priority (int), got %d\n", nanocbor_get_type(&url_value_it));
            return -1;
        }

        /* skip URL priority (currently unused) */
        nanocbor_skip(&url_value_it);

        int res = suit_cbor_get_string(&url_value_it, &url, &url_len);
        if (res) {
            LOG_DEBUG("error parsing URL\n)");
            return -1;
        }
        if (url_len >= manifest->urlbuf_len) {
            LOG_INFO("url too large: %u>%u\n)", (unsigned)url_len, (unsigned)manifest->urlbuf_len);
            return -1;
        }
        memcpy(manifest->urlbuf, url, url_len);
        manifest->urlbuf[url_len] = '\0';

        nanocbor_leave_container(&url_it, &url_value_it);
        nanocbor_leave_container(&it, &url_it);
    }

    LOG_DEBUG("_dtv_fetch() fetching \"%s\" (url_len=%u)\n", manifest->urlbuf, (unsigned)url_len);

    int target_slot = riotboot_slot_other();
    riotboot_flashwrite_init(manifest->writer, target_slot);
    int res = suit_coap_get_blockwise_url(manifest->urlbuf, COAP_BLOCKSIZE_64, suit_flashwrite_helper,
            manifest->writer);

    if (res) {
        LOG_INFO("image download failed\n)");
        return res;
    }

    const uint8_t *digest;
    size_t digest_len;

    res = suit_cbor_get_string(&manifest->components[0].digest, &digest, &digest_len);
    if (res) {
        return res;
    }

    /* "digest" points to a 36 byte string that includes the digest type.
     * riotboot_flashwrite_verify_sha256() is only interested in the 32b digest,
     * so shift the pointer accordingly.
     */
    res = riotboot_flashwrite_verify_sha256(digest + 4, manifest->components[0].size, target_slot);
    if (res) {
        LOG_INFO("image verification failed\n");
        return res;
    }

    manifest->state |= SUIT_MANIFEST_HAVE_IMAGE;

    return SUIT_OK;
}

static int _version_handler(suit_v4_manifest_t *manifest, int key,
                            nanocbor_value_t *it)
{
    (void)manifest;
    (void)key;
    /* Validate manifest version */
    int32_t version = -1;
    if ((nanocbor_get_type(it) == NANOCBOR_TYPE_UINT) &&
        (nanocbor_get_int32(it, &version) >= 0)) {
        if (version == SUIT_VERSION) {
            manifest->validated |= SUIT_VALIDATED_VERSION;
            LOG_INFO("suit: validated manifest version\n)");
            return 0;
        }
        else {
            return -1;
        }
    }
    return -1;
}

static int _seq_no_handler(suit_v4_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)manifest;
    (void)key;
    (void)it;

    int32_t seq_nr;

    if ((nanocbor_get_type(it) == NANOCBOR_TYPE_UINT)) {
        nanocbor_get_int32(it, &seq_nr);
        const riotboot_hdr_t *hdr = riotboot_slot_get_hdr(riotboot_slot_current());
        if (seq_nr <= (int32_t)hdr->version) {
            LOG_INFO("%"PRId32" <= %"PRId32"\n", seq_nr, hdr->version);
            LOG_INFO("seq_nr <= running image\n)");
            return -1;
        }

        hdr = riotboot_slot_get_hdr(riotboot_slot_other());
        if (riotboot_hdr_validate(hdr) == 0) {
            if (seq_nr<= (int32_t)hdr->version) {
                LOG_INFO("%"PRIu32" <= %"PRIu32"\n", seq_nr, hdr->version);
                LOG_INFO("seq_nr <= other image\n)");
                return -1;
            }
        }
        LOG_INFO("suit: validated sequence number\n)");
        manifest->validated |= SUIT_VALIDATED_SEQ_NR;
        return 0;
    }
    LOG_INFO("Unable to get sequence number\n");
    return -1;
}

static int _dependencies_handler(suit_v4_manifest_t *manifest, int key,
                                 nanocbor_value_t *it)
{
    (void)manifest;
    (void)key;
    (void)it;
    /* No dependency support */
    return 0;
}

static int _component_handler(suit_v4_manifest_t *manifest, int key,
                              nanocbor_value_t *it)
{
    (void)manifest;
    (void)key;

    nanocbor_value_t arr;

    LOG_DEBUG("storing components\n)");
    if (nanocbor_get_type(it) != NANOCBOR_TYPE_ARR) {
        LOG_DEBUG("components field not an array\n");
        return -1;
    }
    nanocbor_enter_array(it, &arr);

    unsigned n = 0;
    while (!nanocbor_at_end(&arr)) {
        nanocbor_value_t map, key, value;
        if (n < SUIT_V4_COMPONENT_MAX) {
            manifest->components_len += 1;
        }
        else {
            LOG_DEBUG("too many components\n)");
            return SUIT_ERR_INVALID_MANIFEST;
        }

        suit_cbor_map_iterate_init(&arr, &map);

        suit_v4_component_t *current = &manifest->components[n];

        while (suit_cbor_map_iterate(&map, &key, &value)) {
            /* handle key, value */
            int32_t integer_key;
            if (suit_cbor_get_int32(&key, &integer_key)) {
                return SUIT_ERR_INVALID_MANIFEST;
            }

            switch (integer_key) {
                case SUIT_COMPONENT_IDENTIFIER:
                    current->identifier = value;
                    break;
                case SUIT_COMPONENT_SIZE:
                    LOG_DEBUG("skipping SUIT_COMPONENT_SIZE");
                    break;
                case SUIT_COMPONENT_DIGEST:
                    current->digest = value;
                    break;
                default:
                    LOG_DEBUG("ignoring unexpected component data (nr. %" PRIi32 ")\n", integer_key);
            }

            LOG_DEBUG("component %u parsed\n", n);
        }
        nanocbor_leave_container(&arr, &map);
        n++;
    }

    manifest->state |= SUIT_MANIFEST_HAVE_COMPONENTS;

    nanocbor_leave_container(it, &arr);

    LOG_DEBUG("storing components done\n)");

    return 0;
}

/* begin{code-style-ignore} */
static suit_manifest_handler_t global_handlers[] = {
    [ 0] = _hello_handler,
    [ 1] = _version_handler,
    [ 2] = _seq_no_handler,
    [ 3] = _dependencies_handler,
    [ 4] = _component_handler,
    [ 5] = NULL,
    [ 6] = _common_handler,
    [ 9] = _common_handler,
};
/* end{code-style-ignore} */

static const unsigned global_handlers_len = ARRAY_SIZE(global_handlers);

/* begin{code-style-ignore} */
static suit_manifest_handler_t _sequence_handlers[] = {
    [ 0] = NULL,
    [ 1] = _cond_vendor_handler,
    [ 2] = _cond_class_handler,
    [10] = _cond_comp_offset,
    /* Directives */
    [11] = _dtv_set_comp_idx,
    /* [12] = _dtv_set_man_idx, */
    /* [13] = _dtv_run_seq, */
    [14] = _dtv_run_seq_cond,
    [16] = _dtv_set_param,
    [20] = _dtv_fetch,
    /* [22] = _dtv_run, */
};
/* end{code-style-ignore} */

static const unsigned _sequence_handlers_len = ARRAY_SIZE(_sequence_handlers);

static suit_manifest_handler_t _suit_manifest_get_handler(int key,
                                                   const suit_manifest_handler_t *handlers,
                                                   size_t len)
{
    if (key < 0 || (size_t)key >= len) {
        return NULL;
    }
    return handlers[key];
}

suit_manifest_handler_t suit_manifest_get_manifest_handler(int key)
{
    return _suit_manifest_get_handler(key, global_handlers,
                                      global_handlers_len);
}

static int _common_sequence_handler(suit_v4_manifest_t *manifest, int key, nanocbor_value_t *it)
{

    suit_manifest_handler_t handler = _suit_manifest_get_handler(key, _sequence_handlers, _sequence_handlers_len);
    LOG_DEBUG("Handling handler with key %d at %p\n", key, handler);
    if (handler) {
        return handler(manifest, key, it);
    }
    else {
        LOG_DEBUG("Sequence handler not implemented, ID: %d\n", key);
        return -1;
    }
}

static int _common_handler(suit_v4_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)key;
    return _handle_command_sequence(manifest, it, _common_sequence_handler);
}

int _handle_command_sequence(suit_v4_manifest_t *manifest, nanocbor_value_t *bseq,
        suit_manifest_handler_t handler)
{

    LOG_DEBUG("Handling command sequence\n");
    nanocbor_value_t it, arr;

    int err = suit_cbor_subparse(bseq, &it);
    if (err < 0) {
        return err;
    }

    if (nanocbor_get_type(&it) != NANOCBOR_TYPE_ARR) {
        return -1;
    }
    nanocbor_enter_array(&it, &arr);

    while (!nanocbor_at_end(&arr)) {
        nanocbor_value_t map;
        if (nanocbor_get_type(&arr) != NANOCBOR_TYPE_MAP) {
            return SUIT_ERR_INVALID_MANIFEST;
        }
        nanocbor_enter_map(&arr, &map);
        int32_t integer_key;
        if (suit_cbor_get_int32(&map, &integer_key)) {
            return SUIT_ERR_INVALID_MANIFEST;
        }
        int res = handler(manifest, integer_key, &map);
        if (res < 0) {
            LOG_DEBUG("Sequence handler error\n");
            return res;
        }
        nanocbor_leave_container(&arr, &map);
    }
    nanocbor_leave_container(&it, &arr);

    return 0;
}
