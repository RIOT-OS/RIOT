/*
 * SPDX-FileCopyrightText: 2015 Lucas Jenß
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_ecc
 * @brief
 * @{
 *
 * @brief       Hamming Code implementation for 256byte data segments
 * @author      Lucas Jenß <lucas@x3ro.de>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**  No bit errors were detected in the message */
#define Hamming_ERROR_NONE 0

/**  A single bit was incorrect but has been recovered. */
#define Hamming_ERROR_SINGLEBIT         1

/** The original code has been corrupted. */
#define Hamming_ERROR_ECC               2

/** Multiple bits are incorrect in the data and they cannot be corrected. */
#define Hamming_ERROR_MULTIPLEBITS      3

/**
 *  @brief Computes 3-bytes hamming codes for a data block whose size is multiple of
 *  256 bytes. Each 256 bytes block gets its own code.
 *
 *  @param[in]  data  Data to compute code for.
 *  @param[in]  size  Data size in bytes.
 *  @param[out] code  Codes buffer.
 */
void hamming_compute256x( const uint8_t *data, uint32_t size, uint8_t *code );

/**
 *  @brief Verifies 3-bytes hamming codes for a data block whose size is multiple of
 *  256 bytes. Each 256-bytes block is verified with its own code.
 *
 *  @return Hamming_ERROR_NONE if the data is correct, Hamming_ERROR_SINGLEBIT if one or more
 *  block(s) have had a single bit corrected, or either Hamming_ERROR_ECC
 *  or Hamming_ERROR_MULTIPLEBITS.
 *
 *  @param[in] data  Data buffer to verify.
 *  @param[in] size  Size of the data in bytes.
 *  @param[in] code  Original codes.
 */
uint8_t hamming_verify256x( uint8_t *data, uint32_t size, const uint8_t *code );

#ifdef __cplusplus
}
#endif

/** @} */
