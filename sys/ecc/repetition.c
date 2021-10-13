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
 * @brief       Repetition code
 *
 * Code referring to Liquid-DSP
 * https://github.com/jgaeddert/liquid-dsp/blob/master/src/fec/src/repetition.c
 *
 * @author      Joseph Gaeddert
 * @author      Peter Kietzmann <peter.kietzmann@haw.hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ecc/repetition.h"

void repetition_encode(unsigned int _dec_msg_len,
                       unsigned char *_msg_dec,
                       unsigned char *_msg_enc)
{
    unsigned int i;

    for (i = 0; i < ECC_REPETITION_COUNT; i++) {
        memcpy(&_msg_enc[i * _dec_msg_len], _msg_dec, _dec_msg_len);
    }
}

void repetition_decode(unsigned int _dec_msg_len,
                       unsigned char *_msg_enc,
                       unsigned char *_msg_dec)
{
    unsigned char s[ECC_REPETITION_COUNT];
    unsigned char bitcnt = 0;

    for (unsigned i = 0; i < _dec_msg_len; i++) {
        /* get all repetitions of a symbol */
        for (int k = 0; k < ECC_REPETITION_COUNT; k++) {
            s[k] = _msg_enc[i + k * _dec_msg_len];
        }

        /* sum all ones for each bit position in a byte */
        for (int j = 0; j < 8; j++) {
            for (int k = 0; k < ECC_REPETITION_COUNT; k++) {
                bitcnt += ((s[k] >> j) & 0x01);
            }
            /* decide for a one if majority says one */
            if (bitcnt > DECODER_MAJORITY) {
                _msg_dec[i] |= (1 << (j));
            }
            else {
                _msg_dec[i] &= ~(1 << (j));
            }
            bitcnt = 0;
        }
    }
}
