/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 * Copyright (c) 2018        HAW Hamburg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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

#ifndef ECC_REPETITION_H
#define ECC_REPETITION_H

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
#endif /* ECC_REPETITION_H */
