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
 * @brief       SUIT handler implementations for the manifest Common sections
 *
 * This file contains functions to handle the common info sections of a manifest.
 * This includes components, dependencies and command sequences.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <inttypes.h>
#include <nanocbor/nanocbor.h>

#include "kernel_defines.h"
#include "suit/handlers.h"
#include "suit.h"

#include "log.h"

static int _component_handler(suit_manifest_t *manifest, int key,
                              nanocbor_value_t *it)
{
    (void)manifest;
    (void)key;
    const uint8_t *subcbor;
    size_t sub_size;
    nanocbor_value_t _it;
    nanocbor_get_bstr(it, &subcbor, &sub_size);
    nanocbor_decoder_init(&_it, subcbor, sub_size);

    /* This is a list of lists, something like:
     * [
     *   [ "sda" "firmwareA" ],
     *   [ "sda" "firmwareB" ]
     * ]
     * */
    nanocbor_value_t arr;
    if (nanocbor_enter_array(&_it, &arr) < 0) {
        LOG_DEBUG("components field not an array %d\n", nanocbor_get_type(it));
        return SUIT_ERR_INVALID_MANIFEST;
    }
    unsigned n = 0;
    while (!nanocbor_at_end(&arr)) {
        nanocbor_value_t comp;
        if (nanocbor_enter_array(&arr, &comp) < 0) {
            LOG_DEBUG("component elements field not an array %d\n",
                      nanocbor_get_type(it));
            return SUIT_ERR_INVALID_MANIFEST;
        }
        while (!nanocbor_at_end(&comp)) {
            const uint8_t *identifier;
            size_t id_len;
            if (nanocbor_get_bstr(&comp, &identifier, &id_len) < 0) {
                LOG_DEBUG("Component name not a byte string\n");
                return SUIT_ERR_INVALID_MANIFEST;
            }
        }
        nanocbor_leave_container(&arr, &comp);
        n++;
    }
    if (n > 1) {
        LOG_INFO("More than 1 component found, exiting\n");
        return SUIT_ERR_UNSUPPORTED;
    }
    manifest->state |= SUIT_MANIFEST_HAVE_COMPONENTS;
    return 0;
}

static int _dependencies_handler(suit_manifest_t *manifest, int key,
                                 nanocbor_value_t *it)
{
    (void)manifest;
    (void)key;
    (void)it;
    /* No dependency support */
    return SUIT_ERR_UNSUPPORTED;
}

int _common_sequence_handler(suit_manifest_t *manifest, int key,
                             nanocbor_value_t *it)
{
    (void)key;
    LOG_DEBUG("Starting conditional sequence handler\n");
    return suit_handle_manifest_structure_bstr(manifest, it,
                                               suit_command_sequence_handlers,
                                               suit_command_sequence_handlers_len);
}

/* begin{code-style-ignore} */
const suit_manifest_handler_t suit_common_handlers[] = {
    [SUIT_COMMON_DEPENDENCIES]     = _dependencies_handler,
    [SUIT_COMMON_COMPONENTS]       = _component_handler,
    [SUIT_COMMON_COMMAND_SEQUENCE] = _common_sequence_handler,
};
/* end{code-style-ignore} */

const size_t suit_common_handlers_len = ARRAY_SIZE(suit_common_handlers);
