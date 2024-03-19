/*
 * Copyright (C) 2003-2005 by Christopher R. Hertel
 *               2015 Freie Universität Berlin
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @ingroup     sys_hashes
 * @{
 *
 * @file
 * @brief       Implements the MD5 hash algorithm, as described in RFC 1321
 *
 * @author      Christopher R. Hertel <crh@ubiqx.mn.org>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "hashes/md5.h"

/**
 * @brief   In round one, the values of k (which are used to index
 *          particular four-byte sequences in the input) are simply
 *          sequential.  In later rounds, however, they are a bit more
 *          varied.  Rather than calculate the values of k (which may
 *          or may not be possible--I haven't though about it) the
 *          values are stored in this array.
 */
static const uint8_t K[3][16] = {
    /* Round 1: skipped (since it is simply sequential). */
    {  1,  6, 11,  0,  5, 10, 15,  4,  9, 14,  3,  8, 13,  2,  7, 12 }, /* R2 */
    {  5,  8, 11, 14,  1,  4,  7, 10, 13,  0,  3,  6,  9, 12, 15,  2 }, /* R3 */
    {  0,  7, 14,  5, 12,  3, 10,  1,  8, 15,  6, 13,  4, 11,  2,  9 }  /* R4 */
};

/**
 * @brief   In each round there is a left rotate operation performed as
 *          part of the 16 permutations.  The number of bits varies in
 *          a repeating pattern. This array keeps track of the patterns
 *          used in each round.
 */
static const uint8_t S[4][4] = {
    { 7, 12, 17, 22 },  /* Round 1 */
    { 5,  9, 14, 20 },  /* Round 2 */
    { 4, 11, 16, 23 },  /* Round 3 */
    { 6, 10, 15, 21 }   /* Round 4 */
};

/**
 * @brief   There are four rounds of 16 permutations for a total of 64.
 *          In each of these 64 permutation operations, a different
 *          constant value is added to the mix.  The constants are
 *          based on the sine function...read RFC 1321 for more detail.
 *          In any case, the correct constants are stored in the T[][]
 *          array.  They're divided up into four groups of 16.
 */
static const uint32_t T[4][16] = {
    { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,   /* Round 1 */
      0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
      0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
      0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 },

    { 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,   /* Round 2 */
      0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
      0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
      0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a },

    { 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,   /* Round 3 */
      0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
      0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
      0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 },

    { 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,   /* Round 4 */
      0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
      0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
      0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 },
};

/**
 * @brief   md5F(), md5G(), md5H(), and md5I() are described in RFC 1321
 *
 * All of these operations are bitwise, and so not impacted by endian-ness.
 * @{
 */
#define md5F( X, Y, Z ) (((X) &(Y)) | ((~(X)) & (Z)))
#define md5G( X, Y, Z ) (((X) &(Z)) | ((Y) &(~(Z))))
#define md5H( X, Y, Z ) ((X) ^ (Y) ^ (Z))
#define md5I( X, Y, Z ) ((Y) ^ ((X) | (~(Z))))
/** @} */

/**
 * @brief   Extract one byte from a 32-bit word
 *
 * A value of 0 for <idx> indicates the lowest order byte, while 3 indicates
 * the highest order byte.
 */
#define GETBYTE(L, idx) ((uint8_t)((L >> (((idx) & 0x03) << 3)) & 0xFF))

/**
 * @brief   Permute the ABCD "registers" using the 64-byte <block> as a driver
 *
 * The MD5 algorithm operates on a set of four longwords stored (conceptually)
 * in four "registers".  It is easy to imagine a simple MD4/5 chip that would
 * operate this way.  In any case, the mangling of the contents of those
 * registers is driven by the input message.  The message is chopped and finally
 * padded into 64-byte chunks and each chunk is used to manipulate the contents
 * of the registers.
 *
 * The MD5 Algorithm calls for padding the input to ensure that it is a multiple
 * of 64 bytes in length. The last 16 bytes of the padding space are used to
 * store the message length (the length of the original message, before padding,
 * expressed in terms of bits). If there is not enough room for 16 bytes worth
 * of bitcount (eg., if the original message was 122 bytes long) then the block
 * is padded to the end with zeros and passed to this function. Then *another*
 * block is filled with zeros except for the last 16 bytes which contain the
 * length.
 *
 * Oh... and the algorithm requires that there be at least one padding byte. The
 * first padding byte has a value of 0x80, and any others are 0x00.
 *
 * @param[in|out] abcd      Pointer to an array of four unsigned longwords
 * @param[in]     block     Array of bytes, must be 64 bytes in size
 */
static void permute(uint32_t abcd[4], const uint8_t block[64] )
{
    uint8_t s;
    uint32_t a, b, c, d;
    uint32_t keep_abcd[4];
    uint32_t x[16];

    /* Store the current ABCD values for later reuse */
    for (int i = 0; i < 4; i++) {
        keep_abcd[i] = abcd[i];
    }

    /* Convert the input block into an array of unsigned longs, taking care
     * to read the block in Little Endian order (the algorithm assumes this).
     * The uint32_t values are then handled in host order. */
    for (int i = 0, j = 0; i < 16; i++) {
        x[i]  =  (uint32_t)block[j++];
        x[i] |= ((uint32_t)block[j++] << 8);
        x[i] |= ((uint32_t)block[j++] << 16);
        x[i] |= ((uint32_t)block[j++] << 24);
    }

    /* This loop performs the four rounds of permutations.
     * The rounds are each very similar.  The differences are in three areas:
     *   - The function (F, G, H, or I) used to perform bitwise permutations
     *     on the registers,
     *   - The order in which values from X[] are chosen.
     *   - Changes to the number of bits by which the registers are rotated.
     * This implementation uses a switch statement to deal with some of the
     * differences between rounds.  Other differences are handled by storing
     * values in arrays and using the round number to select the correct set
     * of values.
     *
     * (My implementation appears to be a poor compromise between speed, size,
     * and clarity.  Ugh.  [crh]) */
    for (int round = 0; round < 4; round++) {
        for (int i = 0; i < 16; i++) {
            /* <j> handles the rotation of ABCD */
            int j = (4 - (i % 4)) & 0x3;
            /* <s> is the bit shift for this iteration */
            s = S[round][i % 4];

            /* Copy the b,c,d values per ABCD rotation. This isn't really
             * necessary, it just looks clean & will hopefully be optimized
             * away. */
            b = abcd[(j + 1) & 0x3];
            c = abcd[(j + 2) & 0x3];
            d = abcd[(j + 3) & 0x3];

            /* The actual perumation function.
             * This is broken out to minimize the code within the switch(). */
            switch (round) {
                case 0:         /* round 1 */
                    a = md5F( b, c, d ) + x[i];
                    break;
                case 1:         /* round 2 */
                    a = md5G( b, c, d ) + x[ K[0][i] ];
                    break;
                case 2:         /* round 3 */
                    a = md5H( b, c, d ) + x[ K[1][i] ];
                    break;
                default:        /* round 4 */
                    a = md5I( b, c, d ) + x[ K[2][i] ];
                    break;
            }
            a = 0xFFFFFFFF & (abcd[j] + a + T[round][i]);
            abcd[j] = b + (0xFFFFFFFF & ((a << s) | (a >> (32 - s))));
        }
    }

    /* Use the stored original A, B, C, D values to perform
     * one last convolution. */
    for (int i = 0; i < 4; i++) {
        abcd[i] = (abcd[i] + keep_abcd[i]);
    }
}

void md5_init(md5_ctx_t *ctx)
{
    ctx->len     = 0;
    ctx->b_used  = 0;

    /* The array ABCD[] contains the four 4-byte "registers" that are
     * manipulated to produce the MD5 digest. The input acts upon the registers,
     * not the other way 'round. The initial values are thosegiven in RFC 1321
     * (pg. 4). Note, however, that RFC 1321 provides these values as bytes, not
     * as longwords, and the bytes are arranged in little-endian order as if
     * they were the bytes of (little endian) 32-bit ints.  That's confusing as
     * all getout (to me, anyway). The values given here are provided as 32-bit
     * values in C language format, so they are endian-agnostic.  */
    ctx->abcd[0] = 0x67452301;
    ctx->abcd[1] = 0xefcdab89;
    ctx->abcd[2] = 0x98badcfe;
    ctx->abcd[3] = 0x10325476;
}

void md5_update(md5_ctx_t *ctx, const void *data, size_t len)
{
    /* Add the new block's length to the total length. */
    ctx->len += (uint32_t)len;

    /* Copy the new block's data into the context block.
     * Call the permute() function whenever the context block is full. */
    for (size_t i = 0; i < len; i++) {
        const uint8_t *d = data;

        ctx->block[ctx->b_used] = d[i];
        (ctx->b_used)++;
        if (64 == ctx->b_used) {
            permute(ctx->abcd, ctx->block);
            ctx->b_used = 0;
        }
    }
}

void md5_final(md5_ctx_t *ctx, void *digest)
{
    uint32_t l;

    /* Add the required 0x80 padding initiator byte.
     * The md5_update() function always permutes and resets the context
     * block when it gets full, so we know that there must be at least one
     * free byte in the context block.
     */
    ctx->block[ctx->b_used] = 0x80;
    (ctx->b_used)++;

    /* Zero out any remaining free bytes in the context block. */
    for (int i = ctx->b_used; i < 64; i++) {
        ctx->block[i] = 0;
    }

    /* We need 8 bytes to store the length field.
     * If we don't have 8, call permute() and reset the context block. */
    if (56 < ctx->b_used) {
        permute(ctx->abcd, ctx->block);
        for (int i = 0; i < 64; i++) {
            ctx->block[i] = 0;
        }
    }

    /* Add the total length and perform the final perumation.
     * Note:  The 60'th byte is read from the *original* <ctx->len> value
     *        and shifted to the correct position.  This neatly avoids
     *        any MAXINT numeric overflow issues. */
    l = ctx->len << 3;
    for (int i = 0; i < 4; i++) {
        ctx->block[56 + i] |= GETBYTE(l, i);
    }
    ctx->block[60] = ((GETBYTE(ctx->len, 3) & 0xE0) >> 5);  /* See Above! */
    permute(ctx->abcd, ctx->block);

    /* Now copy the result into the output buffer and we're done */
    for (int i = 0; i < 4; i++) {
        uint8_t *d = digest;

        d[ 0 + i] = GETBYTE(ctx->abcd[0], i);
        d[ 4 + i] = GETBYTE(ctx->abcd[1], i);
        d[ 8 + i] = GETBYTE(ctx->abcd[2], i);
        d[12 + i] = GETBYTE(ctx->abcd[3], i);
    }
}

void md5(void *digest, const void *data, size_t len)
{
    md5_ctx_t ctx;

    md5_init(&ctx);
    md5_update(&ctx, data, len);
    md5_final(&ctx, digest);
}
