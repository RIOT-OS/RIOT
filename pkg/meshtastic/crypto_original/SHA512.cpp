/*
 * Copyright (C) 2015 Southern Storm Software, Pty Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "SHA512.h"
#include "Crypto.h"
#include "utility/RotateUtil.h"
#include "utility/EndianUtil.h"
#include "utility/ProgMemUtil.h"
#include <string.h>

/**
 * \class SHA512 SHA512.h <SHA512.h>
 * \brief SHA-512 hash algorithm.
 *
 * Reference: http://en.wikipedia.org/wiki/SHA-2
 *
 * \sa SHA224, SHA256, SHA3_512, BLAKE2b
 */

/**
 * \var SHA512::HASH_SIZE
 * \brief Constant for the size of the hash output of SHA512.
 */

/**
 * \var SHA512::BLOCK_SIZE
 * \brief Constant for the block size of SHA512.
 */

/**
 * \brief Constructs a SHA-512 hash object.
 */
SHA512::SHA512()
{
    reset();
}

/**
 * \brief Destroys this SHA-512 hash object after clearing
 * sensitive information.
 */
SHA512::~SHA512()
{
    clean(state);
}

size_t SHA512::hashSize() const
{
    return 64;
}

size_t SHA512::blockSize() const
{
    return 128;
}

void SHA512::reset()
{
    static uint64_t const hashStart[8] PROGMEM = {
        0x6A09E667F3BCC908ULL, 0xBB67AE8584CAA73BULL, 0x3C6EF372FE94F82BULL,
        0xA54FF53A5F1D36F1ULL, 0x510E527FADE682D1ULL, 0x9B05688C2B3E6C1FULL,
        0x1F83D9ABFB41BD6BULL, 0x5BE0CD19137E2179ULL
    };
    memcpy_P(state.h, hashStart, sizeof(hashStart));
    state.chunkSize = 0;
    state.lengthLow = 0;
    state.lengthHigh = 0;
}

void SHA512::update(const void *data, size_t len)
{
    // Update the total length in bits, not bytes.
    uint64_t temp = state.lengthLow;
    state.lengthLow += (((uint64_t)len) << 3);
    state.lengthHigh += (((uint64_t)len) >> 61);
    if (state.lengthLow < temp)
        ++state.lengthHigh;

    // Break the input up into 1024-bit chunks and process each in turn.
    const uint8_t *d = (const uint8_t *)data;
    while (len > 0) {
        uint8_t size = 128 - state.chunkSize;
        if (size > len)
            size = len;
        memcpy(((uint8_t *)state.w) + state.chunkSize, d, size);
        state.chunkSize += size;
        len -= size;
        d += size;
        if (state.chunkSize == 128) {
            processChunk();
            state.chunkSize = 0;
        }
    }
}

void SHA512::finalize(void *hash, size_t len)
{
    // Pad the last chunk.  We may need two padding chunks if there
    // isn't enough room in the first for the padding and length.
    uint8_t *wbytes = (uint8_t *)state.w;
    if (state.chunkSize <= (128 - 17)) {
        wbytes[state.chunkSize] = 0x80;
        memset(wbytes + state.chunkSize + 1, 0x00, 128 - 16 - (state.chunkSize + 1));
        state.w[14] = htobe64(state.lengthHigh);
        state.w[15] = htobe64(state.lengthLow);
        processChunk();
    } else {
        wbytes[state.chunkSize] = 0x80;
        memset(wbytes + state.chunkSize + 1, 0x00, 128 - (state.chunkSize + 1));
        processChunk();
        memset(wbytes, 0x00, 128 - 16);
        state.w[14] = htobe64(state.lengthHigh);
        state.w[15] = htobe64(state.lengthLow);
        processChunk();
    }

    // Convert the result into big endian and return it.
    for (uint8_t posn = 0; posn < 8; ++posn)
        state.w[posn] = htobe64(state.h[posn]);

    // Copy the hash to the caller's return buffer.
    size_t maxHashSize = hashSize();
    if (len > maxHashSize)
        len = maxHashSize;
    memcpy(hash, state.w, len);
}

void SHA512::clear()
{
    clean(state);
    reset();
}

void SHA512::resetHMAC(const void *key, size_t keyLen)
{
    formatHMACKey(state.w, key, keyLen, 0x36);
    state.lengthLow += 128 * 8;
    processChunk();
}

void SHA512::finalizeHMAC(const void *key, size_t keyLen, void *hash, size_t hashLen)
{
    uint8_t temp[64];
    finalize(temp, sizeof(temp));
    formatHMACKey(state.w, key, keyLen, 0x5C);
    state.lengthLow += 128 * 8;
    processChunk();
    update(temp, hashSize());
    finalize(hash, hashLen);
    clean(temp);
}

/**
 * \brief Processes a single 1024-bit chunk with the core SHA-512 algorithm.
 *
 * Reference: http://en.wikipedia.org/wiki/SHA-2
 */
void SHA512::processChunk()
{
    // Round constants for SHA-512.
    static uint64_t const k[80] PROGMEM = {
        0x428A2F98D728AE22ULL, 0x7137449123EF65CDULL, 0xB5C0FBCFEC4D3B2FULL,
        0xE9B5DBA58189DBBCULL, 0x3956C25BF348B538ULL, 0x59F111F1B605D019ULL,
        0x923F82A4AF194F9BULL, 0xAB1C5ED5DA6D8118ULL, 0xD807AA98A3030242ULL,
        0x12835B0145706FBEULL, 0x243185BE4EE4B28CULL, 0x550C7DC3D5FFB4E2ULL,
        0x72BE5D74F27B896FULL, 0x80DEB1FE3B1696B1ULL, 0x9BDC06A725C71235ULL,
        0xC19BF174CF692694ULL, 0xE49B69C19EF14AD2ULL, 0xEFBE4786384F25E3ULL,
        0x0FC19DC68B8CD5B5ULL, 0x240CA1CC77AC9C65ULL, 0x2DE92C6F592B0275ULL,
        0x4A7484AA6EA6E483ULL, 0x5CB0A9DCBD41FBD4ULL, 0x76F988DA831153B5ULL,
        0x983E5152EE66DFABULL, 0xA831C66D2DB43210ULL, 0xB00327C898FB213FULL,
        0xBF597FC7BEEF0EE4ULL, 0xC6E00BF33DA88FC2ULL, 0xD5A79147930AA725ULL,
        0x06CA6351E003826FULL, 0x142929670A0E6E70ULL, 0x27B70A8546D22FFCULL,
        0x2E1B21385C26C926ULL, 0x4D2C6DFC5AC42AEDULL, 0x53380D139D95B3DFULL,
        0x650A73548BAF63DEULL, 0x766A0ABB3C77B2A8ULL, 0x81C2C92E47EDAEE6ULL,
        0x92722C851482353BULL, 0xA2BFE8A14CF10364ULL, 0xA81A664BBC423001ULL,
        0xC24B8B70D0F89791ULL, 0xC76C51A30654BE30ULL, 0xD192E819D6EF5218ULL,
        0xD69906245565A910ULL, 0xF40E35855771202AULL, 0x106AA07032BBD1B8ULL,
        0x19A4C116B8D2D0C8ULL, 0x1E376C085141AB53ULL, 0x2748774CDF8EEB99ULL,
        0x34B0BCB5E19B48A8ULL, 0x391C0CB3C5C95A63ULL, 0x4ED8AA4AE3418ACBULL,
        0x5B9CCA4F7763E373ULL, 0x682E6FF3D6B2B8A3ULL, 0x748F82EE5DEFB2FCULL,
        0x78A5636F43172F60ULL, 0x84C87814A1F0AB72ULL, 0x8CC702081A6439ECULL,
        0x90BEFFFA23631E28ULL, 0xA4506CEBDE82BDE9ULL, 0xBEF9A3F7B2C67915ULL,
        0xC67178F2E372532BULL, 0xCA273ECEEA26619CULL, 0xD186B8C721C0C207ULL,
        0xEADA7DD6CDE0EB1EULL, 0xF57D4F7FEE6ED178ULL, 0x06F067AA72176FBAULL,
        0x0A637DC5A2C898A6ULL, 0x113F9804BEF90DAEULL, 0x1B710B35131C471BULL,
        0x28DB77F523047D84ULL, 0x32CAAB7B40C72493ULL, 0x3C9EBE0A15C9BEBCULL,
        0x431D67C49C100D4CULL, 0x4CC5D4BECB3E42B6ULL, 0x597F299CFC657E2AULL,
        0x5FCB6FAB3AD6FAECULL, 0x6C44198C4A475817ULL
    };

    // Convert the first 16 words from big endian to host byte order.
    uint8_t index;
    for (index = 0; index < 16; ++index)
        state.w[index] = be64toh(state.w[index]);

    // Initialise working variables to the current hash value.
    uint64_t a = state.h[0];
    uint64_t b = state.h[1];
    uint64_t c = state.h[2];
    uint64_t d = state.h[3];
    uint64_t e = state.h[4];
    uint64_t f = state.h[5];
    uint64_t g = state.h[6];
    uint64_t h = state.h[7];

    // Perform the first 16 rounds of the compression function main loop.
    uint64_t temp1, temp2;
    for (index = 0; index < 16; ++index) {
        temp1 = h + pgm_read_qword(k + index) + state.w[index] +
                (rightRotate14_64(e) ^ rightRotate18_64(e) ^
                 rightRotate41_64(e)) + ((e & f) ^ ((~e) & g));
        temp2 = (rightRotate28_64(a) ^ rightRotate34_64(a) ^
                 rightRotate39_64(a)) + ((a & b) ^ (a & c) ^ (b & c));
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    // Perform the 64 remaining rounds.  We expand the first 16 words to
    // 80 in-place in the "w" array.  This saves 512 bytes of memory
    // that would have otherwise need to be allocated to the "w" array.
    for (; index < 80; ++index) {
        // Expand the next word.
        temp1 = state.w[(index - 15) & 0x0F];
        temp2 = state.w[(index - 2) & 0x0F];
        temp1 = state.w[index & 0x0F] =
            state.w[(index - 16) & 0x0F] + state.w[(index - 7) & 0x0F] +
                (rightRotate1_64(temp1) ^ rightRotate8_64(temp1) ^
                 (temp1 >> 7)) +
                (rightRotate19_64(temp2) ^ rightRotate61_64(temp2) ^
                 (temp2 >> 6));

        // Perform the round.
        temp1 = h + pgm_read_qword(k + index) + temp1 +
                (rightRotate14_64(e) ^ rightRotate18_64(e) ^
                 rightRotate41_64(e)) + ((e & f) ^ ((~e) & g));
        temp2 = (rightRotate28_64(a) ^ rightRotate34_64(a) ^
                 rightRotate39_64(a)) + ((a & b) ^ (a & c) ^ (b & c));
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    // Add the compressed chunk to the current hash value.
    state.h[0] += a;
    state.h[1] += b;
    state.h[2] += c;
    state.h[3] += d;
    state.h[4] += e;
    state.h[5] += f;
    state.h[6] += g;
    state.h[7] += h;

    // Attempt to clean up the stack.
    a = b = c = d = e = f = g = h = temp1 = temp2 = 0;
}
