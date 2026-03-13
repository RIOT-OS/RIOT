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
 * @brief       ECC repetition code
 *
 * @author      Joseph Gaeddert
 * @author      Peter Kietzmann <peter.kietzmann@haw.hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**  Number of repetitions of a byte */
#ifndef ECC_REPETITION_COUNT
#define ECC_REPETITION_COUNT    (3)
#endif

/** Calculates the majority according to the number of repetitions */
#define DECODER_MAJORITY        ((ECC_REPETITION_COUNT - 1) / 2)

/**
 * @brief encode block of data with @ref ECC_REPETITION_COUNT number of repetitions
 *
 * @param[in]  _dec_msg_len  decoded message length (number of bytes)
 * @param[in]  _msg_dec      decoded message [size: 1 x _dec_msg_len]
 * @param[out] _msg_enc      encoded message [size: 1 x ECC_REPETITION_COUNT*_dec_msg_len]
 */
void repetition_encode(unsigned int _dec_msg_len,
                       unsigned char *_msg_dec,
                       unsigned char *_msg_enc);

/**
 * @brief decode block of data with @ref ECC_REPETITION_COUNT repetitions
 *
 * @param[in]  _dec_msg_len  decoded message length (number of bytes)
 * @param[in]  _msg_enc      encoded message [size: 1 x ECC_REPETITION_COUNT*_dec_msg_len]
 * @param[out] _msg_dec      decoded message [size: 1 x _dec_msg_len]
 */
void repetition_decode(unsigned int _dec_msg_len,
                       unsigned char *_msg_enc,
                       unsigned char *_msg_dec);

#ifdef __cplusplus
}
#endif

/** @} */
