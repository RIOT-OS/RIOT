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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief encode block of data using Golay(24,12) encoder
 *
 * @pre _dec_msg_len and sizeof(_msg_dec) accordingly have to be multiples of 3
 *
 * @param[in] _dec_msg_len     decoded message length (number of bytes)
 * @param[in] _msg_dec         decoded message [size: 1 x _dec_msg_len]
 * @param[in] _msg_enc         encoded message [size: 1 x 2*_dec_msg_len]
 */
void golay2412_encode(uint32_t _dec_msg_len,
                      unsigned char *_msg_dec,
                      unsigned char *_msg_enc);

/**
 * @brief decode block of data using Golay(24,12) decoder
 *
 * @param[in] _dec_msg_len     decoded message length (number of bytes)
 * @param[in] _msg_dec         encoded message [size: 1 x 2*_dec_msg_len]
 * @param[in] _msg_enc         decoded message [size: 1 x _dec_msg_len]
 */
void golay2412_decode(uint32_t _dec_msg_len,
                      unsigned char *_msg_enc,
                      unsigned char *_msg_dec);

#ifdef __cplusplus
}
#endif

/** @} */
