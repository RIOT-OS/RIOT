/*
 * Copyright (C) 2020 Koen Zandberg
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
