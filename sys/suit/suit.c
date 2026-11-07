/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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
