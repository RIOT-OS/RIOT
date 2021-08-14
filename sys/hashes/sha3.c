/*
   Implementation by the Keccak, Keyak and Ketje Teams, namely, Guido Bertoni,
   Joan Daemen, Michaël Peeters, Gilles Van Assche and Ronny Van Keer, hereby
   denoted as "the implementer".

   For more information, feedback or questions, please refer to our websites:
   http://keccak.noekeon.org/
   http://keyak.noekeon.org/
   http://ketje.noekeon.org/

   To the extent possible under law, the implementer has waived all copyright
   and related or neighboring rights to the source code in this file.
   http://creativecommons.org/publicdomain/zero/1.0/

   RIOT OS adaptations by Mathias Tausig

   This software is released under the Creative Commons CC0 1.0 license.
   To the extent possible under law, the implementer has waived all copyright
   and related or neighboring rights to the source code in this file.
   For more information see: http://creativecommons.org/publicdomain/zero/1.0/
 */

/*
   ================================================================
   The purpose of this source file is to demonstrate a readable and compact
   implementation of all the Keccak instances approved in the FIPS 202 standard,
   including the hash functions and the extendable-output functions (XOFs).

   We focused on clarity and on source-code compactness,
   rather than on the performance.

   The advantages of this implementation are:
 + The source code is compact, after removing the comments, that is. :-)
 + There are no tables with arbitrary constants.
 + For clarity, the comments link the operations to the specifications using
        the same notation as much as possible.
 + There is no restriction in cryptographic features. In particular,
        the SHAKE128 and SHAKE256 XOFs can produce any output length.
 + The code does not use much RAM, as all operations are done in place.

   The drawbacks of this implementation are:
    - There is no message queue. The whole message must be ready in a buffer.
    - It is not optimized for performance.

   The implementation is even simpler on a little endian platform. Just define the
   LITTLE_ENDIAN symbol in that case.

   For a more complete set of implementations, please refer to
   the Keccak Code Package at https://github.com/gvanas/KeccakCodePackage

   For more information, please refer to:
 * [Keccak Reference] http://keccak.noekeon.org/Keccak-reference-3.0.pdf
 * [Keccak Specifications Summary] http://keccak.noekeon.org/specs_summary.html

   This file uses UTF-8 encoding, as some comments use Greek letters.
   ================================================================
 */

#include <hashes/sha3.h>
#include <stdint.h>

/**
 * Function to compute the Keccak[r, c] sponge function over a given input.
 * @param  rate            The value of the rate r.
 * @param  capacity        The value of the capacity c.
 * @param  input           Pointer to the input message.
 * @param  inputByteLen    The number of input bytes provided in the input message.
 * @param  delimitedSuffix Bits that will be automatically appended to the end
 *                         of the input message, as in domain separation.
 *                         This is a byte containing from 0 to 7 bits
 *                         These <i>n</i> bits must be in the least significant bit positions
 *                         and must be delimited with a bit 1 at position <i>n</i>
 *                         (counting from 0=LSB to 7=MSB) and followed by bits 0
 *                         from position <i>n</i>+1 to position 7.
 *                         Some examples:
 *                           - If no bits are to be appended, then @a delimitedSuffix must be 0x01.
 *                           - If the 2-bit sequence 0,1 is to be appended (as for SHA3-*),
 *                              @a delimitedSuffix must be 0x06.
 *                           - If the 4-bit sequence 1,1,1,1 is to be appended (as for SHAKE*),
 *                              @a delimitedSuffix must be 0x1F.
 *                           - If the 7-bit sequence 1,1,0,1,0,0,0 is to be absorbed,
 *                              @a delimitedSuffix must be 0x8B.
 * @param  output          Pointer to the buffer where to store the output.
 * @param  outputByteLen   The number of output bytes desired.
 * @pre    One must have r+c=1600 and the rate a multiple of 8 bits in this implementation.
 */
static void Keccak(unsigned int rate, unsigned int capacity, const unsigned char *input,
                   unsigned long long int inputByteLen, unsigned char delimitedSuffix, unsigned char *output,
                   unsigned long long int outputByteLen);

/**
 *  Function to compute SHA3-256 on the input message. The output length is fixed to 32 bytes.
 */
static void FIPS202_SHA3_256(const unsigned char *input, unsigned int inputByteLen, unsigned char *output)
{
    Keccak(1088, 512, input, inputByteLen, 0x06, output, SHA3_256_DIGEST_LENGTH);
}

void sha3_256(void *digest, const void *data, size_t len)
{
    FIPS202_SHA3_256(data, len, digest);
}

void sha3_256_init(keccak_state_t *ctx)
{
    Keccak_init(ctx, 1088, 512, 0x06);
}

void sha3_update(keccak_state_t *ctx, const void *data, size_t len)
{
    Keccak_update(ctx, data, len);
}

void sha3_256_final(keccak_state_t *ctx, void *digest)
{
    Keccak_final(ctx, digest, SHA3_256_DIGEST_LENGTH);
}

/**
 *  Function to compute SHA3-384 on the input message. The output length is fixed to 48 bytes.
 */
static void FIPS202_SHA3_384(const unsigned char *input, unsigned int inputByteLen, unsigned char *output)
{
    Keccak(832, 768, input, inputByteLen, 0x06, output, 48);
}

void sha3_384(void *digest, const void *data, size_t len)
{
    FIPS202_SHA3_384(data, len, digest);
}

void sha3_384_init(keccak_state_t *ctx)
{
    Keccak_init(ctx, 832, 768, 0x06);
}

void sha3_384_final(keccak_state_t *ctx, void *digest)
{
    Keccak_final(ctx, digest, SHA3_384_DIGEST_LENGTH);
}

/**
 *  Function to compute SHA3-512 on the input message. The output length is fixed to 64 bytes.
 */
static void FIPS202_SHA3_512(const unsigned char *input, unsigned int inputByteLen, unsigned char *output)
{
    Keccak(576, 1024, input, inputByteLen, 0x06, output, 64);
}

void sha3_512(void *digest, const void *data, size_t len)
{
    FIPS202_SHA3_512(data, len, digest);
}

void sha3_512_init(keccak_state_t *ctx)
{
    Keccak_init(ctx, 576, 1024, 0x06);
}

void sha3_512_final(keccak_state_t *ctx, void *digest)
{
    Keccak_final(ctx, digest, SHA3_512_DIGEST_LENGTH);
}

/*
   ================================================================
   Technicalities
   ================================================================
 */

typedef uint8_t UINT8;
typedef uint64_t UINT64;
typedef UINT64 tKeccakLane;

#if __BYTE_ORDER__ == __ORDER__LITTLE_ENDIAN__
#define LITTLE_ENDIAN
#endif

#ifndef LITTLE_ENDIAN
/** Function to load a 64-bit value using the little-endian (LE) convention.
 * On a LE platform, this could be greatly simplified using a cast.
 */
static UINT64 load64(const UINT8 *x)
{
    int i;
    UINT64 u = 0;

    for (i = 7; i >= 0; --i) {
        u <<= 8;
        u |= x[i];
    }
    return u;
}

/** Function to store a 64-bit value using the little-endian (LE) convention.
 * On a LE platform, this could be greatly simplified using a cast.
 */
static void store64(UINT8 *x, UINT64 u)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        x[i] = u;
        u >>= 8;
    }
}

/** Function to XOR into a 64-bit value using the little-endian (LE) convention.
 * On a LE platform, this could be greatly simplified using a cast.
 */
static void xor64(UINT8 *x, UINT64 u)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        x[i] ^= u;
        u >>= 8;
    }
}
#endif

/*
   ================================================================
   A readable and compact implementation of the Keccak-f[1600] permutation.
   ================================================================
 */

#define ROL64(a, offset) ((((UINT64)a) << offset) ^ (((UINT64)a) >> (64 - offset)))
#define i(x, y) ((x) + 5 * (y))

#ifdef LITTLE_ENDIAN
    #define readLane(x, y)          (((tKeccakLane *)state)[i(x, y)])
    #define writeLane(x, y, lane)   (((tKeccakLane *)state)[i(x, y)]) = (lane)
    #define XORLane(x, y, lane)     (((tKeccakLane *)state)[i(x, y)]) ^= (lane)
#else
    #define readLane(x, y)          load64((UINT8 *)state + sizeof(tKeccakLane) * i(x, y))
    #define writeLane(x, y, lane)   store64((UINT8 *)state + sizeof(tKeccakLane) * i(x, y), lane)
    #define XORLane(x, y, lane)     xor64((UINT8 *)state + sizeof(tKeccakLane) * i(x, y), lane)
#endif

/**
 * Function that computes the linear feedback shift register (LFSR) used to
 * define the round constants (see [Keccak Reference, Section 1.2]).
 */
static int LFSR86540(UINT8 *LFSR)
{
    int result = ((*LFSR) & 0x01) != 0;

    if (((*LFSR) & 0x80) != 0) {
        /* Primitive polynomial over GF(2): x^8+x^6+x^5+x^4+1 */
        (*LFSR) = ((*LFSR) << 1) ^ 0x71;
    }
    else {
        (*LFSR) <<= 1;
    }
    return result;
}

/**
 * Function that computes the Keccak-f[1600] permutation on the given state.
 */
static void KeccakF1600_StatePermute(void *state)
{
    unsigned int round, x, y, j, t;
    UINT8 LFSRstate = 0x01;

    for (round = 0; round < 24; round++) {
        {   /* === θ step (see [Keccak Reference, Section 2.3.2]) === */
            tKeccakLane C[5];

            /* Compute the parity of the columns */
            for (x = 0; x < 5; x++)
                C[x] = readLane(x, 0) ^ readLane(x, 1) ^ readLane(x, 2) ^ readLane(x, 3) ^
                       readLane(x, 4);
            for (x = 0; x < 5; x++) {
                /* Compute the θ effect for a given column */
                tKeccakLane D = C[(x + 4) % 5] ^ ROL64(C[(x + 1) % 5], 1);
                /* Add the θ effect to the whole column */
                for (y = 0; y < 5; y++)
                    XORLane(x, y, D);
            }
        }

        {   /* === ρ and π steps (see [Keccak Reference, Sections 2.3.3 and 2.3.4]) === */
            tKeccakLane current;
            /* Start at coordinates (1 0) */
            x = 1; y = 0;
            current = readLane(x, y);
            /* Iterate over ((0 1)(2 3))^t * (1 0) for 0 ≤ t ≤ 23 */
            for (t = 0; t < 24; t++) {
                /* Compute the rotation constant r = (t+1)(t+2)/2 */
                unsigned int r = ((t + 1) * (t + 2) / 2) % 64;
                /* Compute ((0 1)(2 3)) * (x y) */
                unsigned int Y = (2 * x + 3 * y) % 5; x = y; y = Y;
                /* Swap current and state(x,y), and rotate */
                tKeccakLane temp = readLane(x, y);
                writeLane(x, y, ROL64(current, r));
                current = temp;
            }
        }

        {   /* === χ step (see [Keccak Reference, Section 2.3.1]) === */
            tKeccakLane temp[5];
            for (y = 0; y < 5; y++) {
                /* Take a copy of the plane */
                for (x = 0; x < 5; x++)
                    temp[x] = readLane(x, y);
                /* Compute χ on the plane */
                for (x = 0; x < 5; x++)
                    writeLane(x, y, temp[x] ^ ((~temp[(x + 1) % 5]) & temp[(x + 2) % 5]));
            }
        }

        {
            /* === ι step (see [Keccak Reference, Section 2.3.5]) === */
            for (j = 0; j < 7; j++) {
                unsigned int bitPosition = (1 << j) - 1;    /* 2^j-1 */
                if (LFSR86540(&LFSRstate)) {
                    XORLane(0, 0, (tKeccakLane)1 << bitPosition);
                }
            }
        }
    }
}

/*
   ================================================================
   A readable and compact implementation of the Keccak sponge functions
   that use the Keccak-f[1600] permutation.
   ================================================================
 */

#include <string.h>
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static void Keccak(unsigned int rate, unsigned int capacity, const unsigned char *input,
                   unsigned long long int inputByteLen, unsigned char delimitedSuffix,
                   unsigned char *output, unsigned long long int outputByteLen)
{
    UINT8 state[200];
    unsigned int rateInBytes = rate / 8;
    unsigned int blockSize = 0;
    unsigned int i;

    if (((rate + capacity) != 1600) || ((rate % 8) != 0)) {
        return;
    }

    /* === Initialize the state === */
    memset(state, 0, sizeof(state));

    /* === Absorb all the input blocks === */
    while (inputByteLen > 0) {
        blockSize = MIN(inputByteLen, rateInBytes);
        for (i = 0; i < blockSize; i++)
            state[i] ^= input[i];
        input += blockSize;
        inputByteLen -= blockSize;

        if (blockSize == rateInBytes) {
            KeccakF1600_StatePermute(state);
            blockSize = 0;
        }
    }

    /* === Do the padding and switch to the squeezing phase === */
    /* Absorb the last few bits and add the first bit of padding (which coincides with the
       delimiter in delimitedSuffix) */
    state[blockSize] ^= delimitedSuffix;
    /* If the first bit of padding is at position rate-1, we need a whole new block for the
       second bit of padding */
    if (((delimitedSuffix & 0x80) != 0) && (blockSize == (rateInBytes - 1))) {
        KeccakF1600_StatePermute(state);
    }
    /* Add the second bit of padding */
    state[rateInBytes - 1] ^= 0x80;
    /* Switch to the squeezing phase */
    KeccakF1600_StatePermute(state);

    /* === Squeeze out all the output blocks === */
    while (outputByteLen > 0) {
        blockSize = MIN(outputByteLen, rateInBytes);
        memcpy(output, state, blockSize);
        output += blockSize;
        outputByteLen -= blockSize;

        if (outputByteLen > 0) {
            KeccakF1600_StatePermute(state);
        }
    }
}

void Keccak_init(keccak_state_t *ctx, unsigned int rate, unsigned int capacity,
                 unsigned char delimitedSuffix)
{
    ctx->rateInBytes = rate / 8;

    if (((rate + capacity) != 1600) || ((rate % 8) != 0)) {
        return;
    }

    /* === Initialize the state === */
    memset(ctx->state, 0, sizeof(ctx->state));
    ctx->i = 0;

    ctx->rate = rate;
    ctx->capacity = capacity;
    ctx->delimitedSuffix = delimitedSuffix;
}

void Keccak_update(keccak_state_t *ctx, const unsigned char *input,
                   unsigned long long int inputByteLen)
{
    /* === Absorb all the input blocks === */
    while (inputByteLen > 0) {
        unsigned int blockSize = MIN(inputByteLen + ctx->i, ctx->rateInBytes);
        while (ctx->i < blockSize) {
            ctx->state[ctx->i] ^= *input;
            ++(ctx->i);
            input++;
            --inputByteLen;
        }

        if (blockSize == ctx->rateInBytes) {
            KeccakF1600_StatePermute(ctx->state);
            ctx->i = 0;
        }
    }
}

void Keccak_final(keccak_state_t *ctx, unsigned char *output, unsigned long long int outputByteLen)
{

    /* === Do the padding and switch to the squeezing phase === */
    /* Absorb the last few bits and add the first bit of padding (which coincides with the
       delimiter in delimitedSuffix) */
    ctx->state[ctx->i] ^= ctx->delimitedSuffix;
    /* If the first bit of padding is at position rate-1, we need a whole new block for the
       second bit of padding */
    if (((ctx->delimitedSuffix & 0x80) != 0) && (ctx->i == (ctx->rateInBytes - 1))) {
        KeccakF1600_StatePermute(ctx->state);
    }
    /* Add the second bit of padding */
    ctx->state[ctx->rateInBytes - 1] ^= 0x80;
    /* Switch to the squeezing phase */
    KeccakF1600_StatePermute(ctx->state);

    /* === Squeeze out all the output blocks === */
    while (outputByteLen > 0) {
        unsigned int blockSize = MIN(outputByteLen, ctx->rateInBytes);
        memcpy(output, ctx->state, blockSize);
        output += blockSize;
        outputByteLen -= blockSize;

        if (outputByteLen > 0) {
            KeccakF1600_StatePermute(ctx->state);
        }
    }
}
