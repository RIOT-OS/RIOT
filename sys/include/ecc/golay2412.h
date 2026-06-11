/*
 * SPDX-FileCopyrightText: 2007-2015 Joseph Gaeddert
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-License-Identifier: MIT
 */

#pragma once

/**
 * @ingroup     sys_ecc
 * @{
 *
 * @file
 * @brief       Golay(24,12) half-rate forward error-correction code
 *
 * References:
 *  [Lin:2004] Lin, Shu and Costello, Daniel L. Jr., "Error Control
 *      Coding," Prentice Hall, New Jersey, 2nd edition, 2004.
 *
 * @author      Joseph Gaeddert
 * @author      Peter Kietzmann <peter.kietzmann@haw.hamburg.de>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Encode block of data using Golay(24,12) encoder
 *
 * @pre     @p dec_msg_len and must be a multiple of 3
 *
 * @param[in]   dec_msg_len     decoded message length (number of bytes)
 * @param[in]   msg_dec         decoded message [size: dec_msg_len]
 * @param[out]  msg_enc         encoded message [size: 2 * dec_msg_len]
 */
void golay2412_encode(uint32_t dec_msg_len,
                      const uint8_t msg_dec[dec_msg_len],
                      uint8_t msg_enc[2 * dec_msg_len]);

/**
 * @brief   Decode block of data using Golay(24,12) decoder
 *
 * @pre     @p dec_msg_len and must be a multiple of 3
 *
 * @param[in]   dec_msg_len     decoded message length (number of bytes)
 * @param[in]   msg_enc         decoded message [size: dec_msg_len]
 * @param[out]  msg_dec         encoded message [size: 2 * dec_msg_len]
 */
void golay2412_decode(uint32_t dec_msg_len,
                      const uint8_t msg_enc[2 * dec_msg_len],
                      uint8_t msg_dec[dec_msg_len]);

#ifdef __cplusplus
}
#endif

/** @} */
