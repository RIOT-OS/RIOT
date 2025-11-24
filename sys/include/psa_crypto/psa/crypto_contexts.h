/*
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        crypto_contexts.h
 * @brief       Context definitions for PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"
#include "psa/crypto_includes.h"

#include "aead/types.h"
#include "hash/types.h"
#include "cipher/types.h"
#include "key_derivation/types.h"
#include "mac/types.h"

#ifdef __cplusplus
}
#endif

/** @} */
