/*
 * Copyright (C) 2021 Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_coding_xor  XOR code
 * @ingroup     sys_coding
 * @brief       Simple XOR based coding algorithms
 *
 * @experimental This is a very basic implementation, it can only recover
 *               1 lost block in 3 and only has a 33% chance of recovering
 *               two consecutive lost blocks.
 *               API / Algorithm might change if that means we can do better.
 *
 * @{
 *
 * @file
 * @brief   XOR coding definitions
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef CODING_XOR_H
#define CODING_XOR_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of payload bytes per parity byte
 */
#ifndef CONFIG_CODING_XOR_CHECK_BYTES
#define CONFIG_CODING_XOR_CHECK_BYTES       3U
#endif

/**
 * @brief   Get the size of the needed parity buffer for a given payload size
 *
 * @param   in      Payload length
 */
#define CODING_XOR_PARITY_LEN(in)  (((in) + CONFIG_CODING_XOR_CHECK_BYTES - 1) \
                                          / CONFIG_CODING_XOR_CHECK_BYTES)

/**
 * @brief   Generate parity and mix data buffer
 *
 * This generates parity data to recover one in @ref CONFIG_CODING_XOR_CHECK_BYTES
 * bytes.
 * The data buffer is then mixed to distribute bytes amongst transfer blocks, so
 * that the chance for consecutive bytes to be in the same block is lowered.
 *
 * @param[in,out]  data     The data buffer to be processed
 * @param[in]      len      Size of the data buffer
 * @param[out]     parity   Buffer to hold parity data.
 *                          Must be at least `CODING_XOR_PARITY_LEN(len)` bytes
 */
void coding_xor_generate(void *data, size_t len, uint8_t *parity);

/**
 * @brief   Restore and unmix buffer
 *
 *
 * @param[in, out] data     The data buffer to be restored
 * @param[in] len           Size of the data buffer
 * @param[in,out] parity    Buffer with parity data.
 *                          Must be at least `CODING_XOR_PARITY_LEN(len)` bytes
 * @param[in,out] blocks    Bitfieled to indicate which blocks were received.
 *                          This indicates the presence of both data and parity
 *                          blocks. Parity blocks are appended after the last
 *                          data block.
 *                          If a block was restored it's bit will be set.
 * @param[in]  block_size   Size of a data/payload block
 * @param[in]  recover_parity If true, missing parity blocks will be re-generated
 *                          from data blocks.
 *
 * @return                  True if all data blocks were recovered
 */
bool coding_xor_recover(void *data, size_t len, uint8_t *parity,
                        uint8_t *blocks, size_t block_size, bool recover_parity);
#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CODING_XOR_H */
