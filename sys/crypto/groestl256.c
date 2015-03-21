/*
 * C pyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file
 * @brief       Implementation of the Grøstl-256 hash function
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Praveen Gauravaram
 * @author      Lars R. Knudsen
 * @author      Krystian Matusiewicz
 * @author      Florian Mendel
 * @author      Christian Rechberger
 * @author      Martin Schläffer
 * @author      Søren S. Thomsen
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "crypto/groestl256.h"

/* Grøstl-256 S-Box */
static const uint8_t S[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

/* Mix bytes */
static void mixbytes(uint32_t a[8][COLWORDS], uint32_t b[8], int s)
{
    int i;
    uint32_t t0, t1, t2;

    for (i = 0; i < 8; i++) {
        b[i] = a[i][s];
    }

    /* y_i = a_{i+6} */
    for (i = 0; i < 8; i++) {
        a[i][s] = b[(i + 2) % 8];
    }

    /* t_i = a_i + a_{i+1} */
    for (i = 0; i < 7; i++) {
        b[i] ^= b[(i + 1) % 8];
    }

    b[7] ^= a[6][s];

    /* y_i = a_{i+6} + t_i */
    for (i = 0; i < 8; i++) {
        a[i][s] ^= b[(i + 4) % 8];
    }

    /* y_i = y_i + t_{i+2} */
    for (i = 0; i < 8; i++) {
        a[i][s] ^= b[(i + 6) % 8];
    }

    /* x_i = t_i + t_{i+3} */
    t0 = b[0];
    t1 = b[1];
    t2 = b[2];

    for (i = 0; i < 5; i++) {
        b[i] ^= b[(i + 3) % 8];
    }

    b[5] ^= t0;
    b[6] ^= t1;
    b[7] ^= t2;

    /* z_i = 02 * x_i */
    for (i = 0; i < 8; i++) {
        mul2(b[i], t0);
    }

    /* w_i = z_i + y_{i+4} */
    for (i = 0; i < 8; i++) {
        b[i] ^= a[i][s];
    }

    /* v_i = 02 * w_i */
    for (i = 0; i < 8; i++) {
        mul2(b[i], t0);
    }

    /* b_i = v_{i+3} + y_{i+4} */
    for (i = 0; i < 8; i++) {
        a[i][s] ^= b[(i + 3) % 8];
    }
}

/* Permute bytes */
static void permutation(uint32_t *x, int q)
{
    uint32_t tmp[8];
    uint32_t constant;
    int i, j;

    for (constant = 0; constant < (0x01010101 * ROUNDS); constant += 0x01010101) {
        if (q == 0) {
            for (j = 0; j < COLWORDS; j++) {
                x[j] ^= columnconstant[j] ^ constant;
            }
        }
        else {
            for (i = 0; i < STATEWORDS; i++) {
                x[i] = ~x[i];
            }

            for (j = 0; j < COLWORDS; j++) {
                x[STATEWORDS - COLWORDS + j] ^= columnconstant[j] ^ constant;
            }
        }

        for (i = 0; i < 8; i++) {
            for (j = 0; j < COLWORDS; j++) {
                tmp[j] = x[i * COLWORDS + j];
            }

            for (j = 0; j < STATECOLS; j++) {
                ((uint8_t *)x)[i * STATECOLS + j] = S[((uint8_t *)tmp)[(j + shiftvalues[q][i]) % STATECOLS]];
            }
        }

        for (j = 0; j < COLWORDS; j++) {
            mixbytes((uint32_t(*)[COLWORDS])x, tmp, j);
        }
    }
}

/* XOR byte arrays */
static inline void memxor(uint32_t *dest, const uint32_t *src, uint32_t n)
{
    while (n--) {
        *dest ^= *src;
        dest++;
        src++;
    }
}

/* Prepare input message for compression function */
static void setmessage(uint8_t *buffer, const uint8_t *in, groestl256_ctx s,
                       unsigned long long inlen)
{
    int i;

    for (i = 0; i < s.bytes_in_block; i++) {
        buffer[BYTESLICE(i)] = in[i];
    }

    if (s.bytes_in_block != STATEBYTES) {
        if (s.first_padding_block) {
            buffer[BYTESLICE(i)] = 0x80;
            i++;
        }

        for (; i < STATEBYTES; i++) {
            buffer[BYTESLICE(i)] = 0;
        }

        if (s.last_padding_block) {
            inlen /= STATEBYTES;
            inlen += (s.first_padding_block == s.last_padding_block) ? 1 : 2;

            for (i = STATEBYTES - 8; i < STATEBYTES; i++) {
                buffer[BYTESLICE(i)] = (inlen >> 8 * (STATEBYTES - i - 1)) & 0xff;
            }
        }
    }
}

/* Grøstl-256 hash function */
int groestl256(uint8_t *out, const uint8_t *in, uint64_t inlen)
{
    uint32_t ctx[STATEWORDS];
    uint32_t buffer[STATEWORDS];
    uint64_t rlen = inlen;
    groestl256_ctx s = { STATEBYTES, 0, 0 };
    uint8_t i;

    /* Set inital value */
    for (i = 0; i < STATEWORDS; i++) {
        ctx[i] = 0;
    }

    ((uint8_t *)ctx)[BYTESLICE(STATEBYTES - 2)] = ((CRYPTO_BYTES * 8) >> 8) & 0xff;
    ((uint8_t *)ctx)[BYTESLICE(STATEBYTES - 1)] = (CRYPTO_BYTES * 8) & 0xff;

    /* Iterate compression function */
    while (s.last_padding_block == 0) {
        if (rlen < STATEBYTES) {
            if (s.first_padding_block == 0) {
                s.bytes_in_block = rlen;
                s.first_padding_block = 1;
                s.last_padding_block = (s.bytes_in_block < STATEBYTES - 8) ? 1 : 0;
            }
            else {
                s.bytes_in_block = 0;
                s.first_padding_block = 0;
                s.last_padding_block = 1;
            }
        }
        else {
            rlen -= STATEBYTES;
        }

        /* Compression function */
        setmessage((uint8_t *)buffer, in, s, inlen);
        memxor(buffer, ctx, STATEWORDS);
        permutation(buffer, 0);
        memxor(ctx, buffer, STATEWORDS);
        setmessage((uint8_t *)buffer, in, s, inlen);
        permutation(buffer, 1);
        memxor(ctx, buffer, STATEWORDS);

        /* Increase message pointer */
        in += STATEBYTES;
    }

    /* Output transformation */
    for (i = 0; i < STATEWORDS; i++) {
        buffer[i] = ctx[i];
    }

    permutation(buffer, 0);
    memxor(ctx, buffer, STATEWORDS);

    /* Return truncated hash value */
    for (i = STATEBYTES - CRYPTO_BYTES; i < STATEBYTES; i++) {
        out[i - (STATEBYTES - CRYPTO_BYTES)] = ((uint8_t *)ctx)[BYTESLICE(i)];
    }

    return 0;
}
/** @} */
