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
 * @brief       Golay(24,12) half-rate forward error-correction code
 *
 * References:
 *  [Lin:2004] Lin, Shu and Costello, Daniel L. Jr., "Error Control
 *      Coding," Prentice Hall, New Jersey, 2nd edition, 2004.
 *
 * @author      Joseph Gaeddert
 * @author      Peter Kietzmann <peter.kietzmann@haw.hamburg.de>
 */

#ifndef ECC_GOLAY2412_H
#define ECC_GOLAY2412_H

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

#endif /* ECC_GOLAY2412_H */
/** @} */
