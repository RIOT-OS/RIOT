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
 * Code referring to Liquid-DSP
 * https://github.com/jgaeddert/liquid-dsp/blob/master/src/fec/src/fec_golay2412.c
 *
 * @author      Joseph Gaeddert
 * @author      Peter Kietzmann <peter.kietzmann@haw.hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "bitarithm.h"
#include "ecc/golay2412.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* generator matrix transposed [24 x 12] */
static const uint32_t golay2412_Gt[24] = {
    0x08ed, 0x01db, 0x03b5, 0x0769, 0x0ed1, 0x0da3, 0x0b47, 0x068f,
    0x0d1d, 0x0a3b, 0x0477, 0x0ffe, 0x0800, 0x0400, 0x0200, 0x0100,
    0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001
};

/* P matrix [12 x 12] */
static const uint32_t golay2412_P[12] = {
    0x08ed, 0x01db, 0x03b5, 0x0769,
    0x0ed1, 0x0da3, 0x0b47, 0x068f,
    0x0d1d, 0x0a3b, 0x0477, 0x0ffe
};

/* parity check matrix [12 x 24] */
static const uint32_t golay2412_H[12] = {
    0x008008ed, 0x004001db, 0x002003b5, 0x00100769,
    0x00080ed1, 0x00040da3, 0x00020b47, 0x0001068f,
    0x00008d1d, 0x00004a3b, 0x00002477, 0x00001ffe
};

/* print string of bits to standard output */
static inline void liquid_print_bitstring(uint32_t _x,
                                          uint32_t _n)
{
    uint32_t i;

    for (i = 0; i < _n; i++) {
        printf("%" PRIu32, (_x >> (_n - i - 1)) & 1);
    }
}

#ifndef NDEBUG
static uint32_t block_get_enc_msg_len(uint32_t _dec_msg_len,
                                      uint32_t _m,
                                      uint32_t _k)
{
    assert((_m > 0) && (_k >= _m));

    /* compute total number of bits in decoded message  */
    uint32_t num_bits_in = _dec_msg_len * 8;

    /* compute total number of blocks: ceil(num_bits_in/_m) */
    uint32_t num_blocks = num_bits_in / _m + ((num_bits_in % _m) ? 1 : 0);

    /* compute total number of bits out */
    uint32_t num_bits_out = num_blocks * _k;

    /* compute total number of bytes out: ceil(num_bits_out/8) */
    uint32_t num_bytes_out = num_bits_out / 8 + ((num_bits_out % 8) ? 1 : 0);

    DEBUG("block_get_enc_msg_len(%" PRIu32 ",%" PRIu32 ",%" PRIu32 ")\n",
          _dec_msg_len, _m, _k);
    DEBUG("    dec msg len :   %" PRIu32 " bytes\n", _dec_msg_len);
    DEBUG("    m           :   %" PRIu32 " bits\n", _m);
    DEBUG("    k           :   %" PRIu32 " bits\n", _k);
    DEBUG("    num bits in :   %" PRIu32 " bits\n", num_bits_in);
    DEBUG("    num blocks  :   %" PRIu32 "\n", num_blocks);
    DEBUG("    num bits out:   %" PRIu32 " bits\n", num_bits_out);
    DEBUG("    enc msg len :   %" PRIu32 " bytes\n", num_bytes_out);

    return num_bytes_out;
}
#endif

/* multiply input vector with parity check matrix, H */
static uint32_t golay2412_matrix_mul(uint32_t _v,
                                     const uint32_t *_A,
                                     uint32_t _n)
{
    uint32_t x = 0;
    uint32_t i;

    for (i = 0; i < _n; i++) {
        x <<= 1;
        /* same as above, but exploit the fact that vectors are at
         * most 24 bits long */
        uint32_t p = _A[i] & _v;
        uint32_t c = 0;
        c = bitarithm_bits_set_u32(p & 0x00ffffff);

        /* mod 2 */
        x |= c & 0x0001;
    }
    return x;
}
static uint32_t golay2412_encode_symbol(uint32_t _sym_dec, const uint32_t *_A)
{
    /* validate input */
    assert(_sym_dec < (1 << 12));

    /* compute encoded/transmitted message: v = m*G */
    return golay2412_matrix_mul(_sym_dec, _A, 24);
}

/* search for p[i] such that w(v+p[i]) <= 2, return -1 on fail */
static int8_t golay2412_parity_search(uint32_t _v, const uint32_t *_A)
{
    assert(_v < (1 << 12));

    uint8_t i;
    for (i = 0; i < 12; i++) {
        /* same as above but faster, exploiting fact that P has
         * only 12 bits of resolution */
        uint32_t p = _v ^ _A[i];

        if (bitarithm_bits_set_u32(p & 0x00000fff) <= 2) {
            return i;
        }
    }

    /* could not find p[i] to satisfy criteria */
    return -1;
}

static uint32_t golay2412_decode_symbol(uint32_t _sym_enc,
                                        const uint32_t *_A,
                                        const uint32_t *_B)
{
    /* validate input */
    assert((_sym_enc) < (1L << 24));

    /* state variables */
    uint32_t s = 0;         /* syndrome vector */
    uint32_t e_hat = 0;     /* estimated error vector */
    uint32_t v_hat = 0;     /* estimated transmitted message */
    uint32_t m_hat = 0;     /* estimated original message */

    /* compute syndrome vector, s = r*H^T = ( H*r^T )^T */
    s = golay2412_matrix_mul(_sym_enc, _B, 12);
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        printf("s (syndrome vector): ");
        liquid_print_bitstring(s, 12);
        printf("\n");
    }

    /* compute weight of s (12 bits) */
    uint8_t ws = bitarithm_bits_set_u32(s & 0x00000fff);
    DEBUG("w(s) = %u\n", ws);

    /* step 2: */
    e_hat = 0;
    if (ws <= 3) {
        DEBUG("    w(s) <= 3: estimating error vector as [s, 0(12)]\n");
        /* set e_hat = [s 0(12)] */
        e_hat = (s << 12) & 0xfff000;
    }
    else {
        /* step 3: search for p[i] s.t. w(s+p[i]) <= 2 */
        DEBUG("    searching for w(s + p_i) <= 2...\n");
        int8_t s_index = golay2412_parity_search(s, _A);

        if (s_index >= 0) {
            /* vector found! */
            DEBUG("    w(s + p[%2i]) <= 2: estimating error vector as "
                  "[s+p[%2i],u[%2i]]\n", s_index, s_index, s_index);

            /* NOTE : uj = 1 << (12-j-1) */
            e_hat = ((s ^ _A[s_index]) << 12) | (1 << (11 - s_index));
        }
        else {
            /* step 4: compute s*P */
            uint32_t sP = golay2412_matrix_mul(s, _A, 12);

            if (IS_ACTIVE(ENABLE_DEBUG)) {
                printf("s*P: ");
                liquid_print_bitstring(sP, 12);
                printf("\n");
            }

            /* compute weight of sP (12 bits) */
            uint8_t wsP = bitarithm_bits_set_u32(sP & 0x00000fff);
            DEBUG("w(s*P) = %u\n", wsP);

            if (wsP == 2 || wsP == 3) {
                /* step 5: set e = [0, s*P] */
                DEBUG("    w(s*P) in [2,3]: estimating error vector as "
                      "[0(12), s*P]\n");
                e_hat = sP;
            }
            else {
                /* step 6: search for p[i] s.t. w(s*P + p[i]) == 2... */
                DEBUG("    searching for w(s*P + p_i) == 2...\n");

                int8_t sP_index = golay2412_parity_search(sP, _A);

                if (sP_index >= 0) {
                    /* vector found! */
                    DEBUG("    w(s*P + p[%2i]) == 2: estimating error vector "
                          "as [u[%2i],s*P+p[%2i]]\n",
                          sP_index, sP_index, sP_index);

                    /* NOTE : uj = 1 << (12-j-1)
                     *      [      uj << 1 2    ] [    sP + p[j]    ] */
                    e_hat = (1L << (23 - sP_index)) | (sP ^ _A[sP_index]);
                }
                else {
                    /* step 7: decoding error */
                    DEBUG("  **** decoding error\n");
                }
            }
        }
    }

    /* step 8: compute estimated transmitted message: v_hat = r + e_hat */
    v_hat = _sym_enc ^ e_hat;

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        printf("r (received vector):            ");
        liquid_print_bitstring(_sym_enc, 24); printf("\n");
        printf("e-hat (estimated error vector): ");
        liquid_print_bitstring(e_hat, 24);    printf("\n");
        printf("v-hat (estimated tx vector):    ");
        liquid_print_bitstring(v_hat, 24);    printf("\n");
    }

    /* compute estimated original message: (last 12 bits of encoded message) */
    m_hat = v_hat & 0x0fff;

    return m_hat;
}

void golay2412_encode(uint32_t _dec_msg_len,
                      unsigned char *_msg_dec,
                      unsigned char *_msg_enc)
{
    uint32_t i = 0;         /* decoded byte counter */
    uint32_t j = 0;         /* encoded byte counter */
    uint32_t m0;            /* first 12-bit symbol (uncoded) */
    uint32_t v0;            /* first 24-bit symbol (encoded) */
    unsigned char s0;       /* first 8-bit symbol */

    /* determine remainder of input length / 3 */
    uint32_t r = _dec_msg_len % 3;

    for (i = 0; i < _dec_msg_len - r; i += 3) {
        uint32_t m1;            /* second 12-bit symbol (uncoded) */
        uint32_t v1;            /* second 24-bit symbol (encoded) */
        unsigned char s1, s2;   /* second and third 8-bit symbols */

        /* strip three input bytes (two uncoded symbols) */
        s0 = _msg_dec[i + 0];
        s1 = _msg_dec[i + 1];
        s2 = _msg_dec[i + 2];

        /* pack into two 12-bit symbols */
        m0 = ((s0 << 4) & 0x0ff0) | ((s1 >> 4) & 0x000f);
        m1 = ((s1 << 8) & 0x0f00) | ((s2) & 0x00ff);

        /* encode each 12-bit symbol into a 24-bit symbol */
        v0 = golay2412_encode_symbol(m0, golay2412_Gt);
        v1 = golay2412_encode_symbol(m1, golay2412_Gt);

        /* unpack two 24-bit symbols into six 8-bit bytes
         * retaining order of bits in output */
        _msg_enc[j + 0] = (v0 >> 16) & 0xff;
        _msg_enc[j + 1] = (v0 >>  8) & 0xff;
        _msg_enc[j + 2] = (v0) & 0xff;
        _msg_enc[j + 3] = (v1 >> 16) & 0xff;
        _msg_enc[j + 4] = (v1 >>  8) & 0xff;
        _msg_enc[j + 5] = (v1) & 0xff;

        j += 6;
    }

    /* if input length isn't divisible by 3, encode last 1 or two bytes */
    for (i = _dec_msg_len - r; i < _dec_msg_len; i++) {
        /* strip last input symbol */
        s0 = _msg_dec[i];

        /* extend as 12-bit symbol */
        m0 = s0;

        /* encode into 24-bit symbol */
        v0 = golay2412_encode_symbol(m0, golay2412_Gt);

        /* unpack one 24-bit symbol into three 8-bit bytes, and
         * append to output array */
        _msg_enc[j + 0] = (v0 >> 16) & 0xff;
        _msg_enc[j + 1] = (v0 >>  8) & 0xff;
        _msg_enc[j + 2] = (v0) & 0xff;

        j += 3;
    }

    assert( j == block_get_enc_msg_len(_dec_msg_len, 12, 24));
    assert( i == _dec_msg_len);
}

void golay2412_decode(uint32_t _dec_msg_len,
                      unsigned char *_msg_enc,
                      unsigned char *_msg_dec)
{
    uint32_t i = 0;                     /* decoded byte counter */
    uint32_t j = 0;                     /* encoded byte counter */
    uint32_t v0;                        /* first 24-bit encoded symbol */
    uint32_t m0_hat;                    /* first 12-bit decoded symbol */
    unsigned char r0, r1, r2;           /* first three 8-bit bytes */

    /* determine remainder of input length / 3 */
    uint32_t r = _dec_msg_len % 3;

    for (i = 0; i < _dec_msg_len - r; i += 3) {
        uint32_t v1;                        /* second 24-bit encoded symbol */
        uint32_t m1_hat;                    /* second 12-bit decoded symbol */
        unsigned char r3, r4, r5;           /* last three 8-bit bytes */

        /* strip six input bytes (two encoded symbols) */
        r0 = _msg_enc[j + 0];
        r1 = _msg_enc[j + 1];
        r2 = _msg_enc[j + 2];
        r3 = _msg_enc[j + 3];
        r4 = _msg_enc[j + 4];
        r5 = _msg_enc[j + 5];

        /* pack six 8-bit symbols into two 24-bit symbols */
        v0 =  (((uint32_t)r0 << 16) & 0xff0000)
             | (((uint32_t)r1 <<  8) & 0x00ff00)
             | (((uint32_t)r2 <<  0) & 0x0000ff);

        v1 =  (((uint32_t)r3 << 16) & 0xff0000)
             | (((uint32_t)r4 <<  8) & 0x00ff00)
             | (((uint32_t)r5 <<  0) & 0x0000ff);

        /* decode each symbol into a 12-bit symbol */
        m0_hat = golay2412_decode_symbol(v0, golay2412_P, golay2412_H);
        m1_hat = golay2412_decode_symbol(v1, golay2412_P, golay2412_H);

        /* unpack two 12-bit symbols into three 8-bit bytes */
        _msg_dec[i + 0] = ((m0_hat >> 4) & 0xff);
        _msg_dec[i + 1] = ((m0_hat << 4) & 0xf0) | ((m1_hat >> 8) & 0x0f);
        _msg_dec[i + 2] = ((m1_hat) & 0xff);

        j += 6;
    }

    /* if input length isn't divisible by 3, decode last 1 or two bytes */
    for (i = _dec_msg_len - r; i < _dec_msg_len; i++) {
        /* strip last input symbol (three bytes) */
        r0 = _msg_enc[j + 0];
        r1 = _msg_enc[j + 1];
        r2 = _msg_enc[j + 2];

        /* pack three 8-bit symbols into one 24-bit symbol */
        v0 =  (((uint32_t)r0 << 16) & 0xff0000)
             | (((uint32_t)r1 <<  8) & 0x00ff00)
             | (((uint32_t)r2 <<  0) & 0x0000ff);

        /* decode into a 12-bit symbol */
        m0_hat = golay2412_decode_symbol(v0, golay2412_P, golay2412_H);

        /* retain last 8 bits of 12-bit symbol */
        _msg_dec[i] = m0_hat & 0xff;

        j += 3;
    }

    assert( j == block_get_enc_msg_len(_dec_msg_len, 12, 24));
    assert( i == _dec_msg_len);
}
