/*
 * SPDX-FileCopyrightText: 2024 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include "CYS/unprotected.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   SHA-256 context, as required by the PSA hash API
 */
typedef CYS_hash_sha256_ctx_t psa_hashes_sha256_ctx_t;

#ifdef __cplusplus
}
#endif
