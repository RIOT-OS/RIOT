/*
 * Copyright (C) 2019 Koen Zandberg
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Inria
 *               2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_suit_v4
 * @brief       SUIT v4 manifest handlers
 *
 * @experimental
 *
 * @{
 *
 * @brief       Handler functions for SUIT manifests
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef SUIT_V4_HANDLERS_H
#define SUIT_V4_HANDLERS_H

#include <stddef.h>
#include <stdint.h>

#include "suit/v4/suit.h"
#include "uuid.h"
#include "nanocbor/nanocbor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief suit handler prototype
 *
 * @param manifest  SUIT v4 manifest context
 * @param it        nanocbor_value_t iterator to the content the handler must handle
 *
 * @return          1 on success
 * @return          negative on error
 */
typedef int (*suit_manifest_handler_t)(suit_v4_manifest_t *manifest, int key, nanocbor_value_t *it);

/**
 * @brief    Get suit manifest handler for given integer key
 *
 * @param[in]   key: integer key
 *
 * @return      ptr to handler function
 * @return      NULL (if handler unavailable or key out of range)
 */
suit_manifest_handler_t suit_manifest_get_manifest_handler(int key);

#ifdef __cplusplus
}
#endif

#endif /* SUIT_V4_HANDLERS_H */
/** @} */
