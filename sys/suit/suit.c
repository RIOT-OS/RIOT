/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       SUIT secure OTA firmware upgrade implementation for
 *              CBOR based manifests
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <nanocbor/nanocbor.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "log.h"
#include "suit/handlers.h"
#include "suit/policy.h"
#include "suit.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int suit_parse(suit_manifest_t *manifest, const uint8_t *buf,
                  size_t len)
{
    nanocbor_value_t it;

    manifest->buf = buf;
    manifest->len = len;
    nanocbor_decoder_init(&it, buf, len);
    LOG_DEBUG("Starting envelope sequence handler\n");
    return suit_handle_manifest_structure(manifest, &it,
                                          suit_envelope_handlers,
                                          suit_envelope_handlers_len);
}
