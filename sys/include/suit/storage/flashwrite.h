/*
 * SPDX-FileCopyrightText: 2020 Koen Zandberg
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_suit_storage_flashwrite  riotboot flashwrite storage backend
 * @ingroup     sys_suit_storage
 * @brief       SUIT riotboot firmware storage backend
 *
 * @{
 *
 * @brief       riotboot Flashwrite storage backend functions for SUIT manifests
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include "suit.h"
#include "riotboot/flashwrite.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief riotboot flashwrite SUIT storage context
 */
typedef struct {
    suit_storage_t storage;       /**< parent struct */
    riotboot_flashwrite_t writer; /**< Riotboot flashwriter */
} suit_storage_flashwrite_t;

#ifdef __cplusplus
}
#endif

/** @} */
