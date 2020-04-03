/*
 * Copyright (C) 2019 Koen Zandberg
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
 * @brief       SUIT content handler helper functions
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <inttypes.h>
#include <nanocbor/nanocbor.h>

#include "suit/handlers.h"
#include "suit.h"

#include "log.h"

static suit_manifest_handler_t _get_handler(int key,
                                            const suit_manifest_handler_t *map,
                                            size_t len)
{
    if (key < 0 || (size_t)key >= len) {
        return NULL;
    }
    return map[key];
}

int suit_handle_manifest_structure(suit_manifest_t *manifest,
                                   nanocbor_value_t *it,
                                   const suit_manifest_handler_t *handlers,
                                   size_t handlers_len)
{
    LOG_DEBUG("Handling command sequence\n");
    nanocbor_value_t container;

    if ((nanocbor_enter_array(it, &container) < 0) &&
        (nanocbor_enter_map(it, &container) < 0)) {
        LOG_DEBUG("Neither array nor map: %d\n", nanocbor_get_type(it));
        return SUIT_ERR_INVALID_MANIFEST;
    }

    while (!nanocbor_at_end(&container)) {
        int32_t key;
        if (nanocbor_get_int32(&container, &key) < 0) {
            LOG_DEBUG("No key found: %d\n", nanocbor_get_type(&container));
            return SUIT_ERR_INVALID_MANIFEST;
        }
        nanocbor_value_t value = container;
        LOG_DEBUG("Executing handler with key %" PRIi32 "\n", key);
        suit_manifest_handler_t handler = _get_handler(key, handlers,
                                                       handlers_len);
        if (!handler) {
            return SUIT_ERR_UNSUPPORTED;
        }
        int res = handler(manifest, key, &value);
        if (res < 0) {
            LOG_DEBUG("Sequence handler error\n");
            return res;
        }
        nanocbor_skip(&container);
    }
    nanocbor_leave_container(it, &container);
    LOG_DEBUG("Leaving sequence handler\n");

    return 0;
}

int suit_handle_manifest_structure_bstr(suit_manifest_t *manifest,
                                        nanocbor_value_t *bseq,
                                        const suit_manifest_handler_t *handlers,
                                        size_t handlers_len)
{
    const uint8_t *buf;
    size_t len;

    LOG_DEBUG("Handling command sequence starting with CBOR type %d\n",
              nanocbor_get_type(bseq));
    if (nanocbor_get_bstr(bseq, &buf, &len) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    nanocbor_value_t it;
    nanocbor_decoder_init(&it, buf, len);
    return suit_handle_manifest_structure(manifest, &it, handlers,
                                          handlers_len);
}
