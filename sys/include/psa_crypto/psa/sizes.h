/*
 * Copyright (C) 2025 TU Dresden
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        sizes.h
 * @brief       Size definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Functions to convert bits to bytes
 *
 * @param   bits
 *
 * @return  Number of bytes contained in bits
 */
#define PSA_BITS_TO_BYTES(bits) (size_t)(((bits) + 7) / 8)

/**
 * @brief   Functions to convert bytes to bits
 *
 * @param   bytes
 *
 * @return  Number of bits contained in bytes
 */
#define PSA_BYTES_TO_BITS(bytes) ((bytes) * 8)

#ifdef __cplusplus
}
#endif

/** @} */
