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

#include "SHA256.h"
#include "Crypto.h"
#include "utility/RotateUtil.h"
#include "utility/EndianUtil.h"
#include "utility/ProgMemUtil.h"
#include <string.h>

/**
 * \class SHA256 SHA256.h <SHA256.h>
 * \brief SHA-256 hash algorithm.
 *
 * Reference: http://en.wikipedia.org/wiki/SHA-2
 *
 * \sa SHA224, SHA384, SHA512, SHA3_256, BLAKE2s
 */

/**
 * \var SHA256::HASH_SIZE
 * \brief Constant for the size of the hash output of SHA256.
 */

/**
 * \var SHA256::BLOCK_SIZE
 * \brief Constant for the block size of SHA256.
 */

/**
 * \brief Constructs a SHA-256 hash object.
 */
SHA256::SHA256()
{
    reset();
}

/**
 * \brief Destroys this SHA-256 hash object after clearing
 * sensitive information.
 */
SHA256::~SHA256()
{
    clean(state);
}

size_t SHA256::hashSize() const
{
    return 32;
}

size_t SHA256::blockSize() const
{
    return 64;
}

void SHA256::reset()
{
    state.h[0] = 0x6a09e667;
    state.h[1] = 0xbb67ae85;
    state.h[2] = 0x3c6ef372;
    state.h[3] = 0xa54ff53a,
    state.h[4] = 0x510e527f;
    state.h[5] = 0x9b05688c;
    state.h[6] = 0x1f83d9ab;
    state.h[7] = 0x5be0cd19;
    state.chunkSize = 0;
    state.length = 0;
}

void SHA256::update(const void *data, size_t len)
{
    // Update the total length (in bits, not bytes).
    state.length += ((uint64_t)len) << 3;

    // Break the input up into 512-bit chunks and process each in turn.
    const uint8_t *d = (const uint8_t *)data;
    while (len > 0) {
        uint8_t size = 64 - state.chunkSize;
        if (size > len)
            size = len;
        memcpy(((uint8_t *)state.w) + state.chunkSize, d, size);
        state.chunkSize += size;
        len -= size;
        d += size;
        if (state.chunkSize == 64) {
            processChunk();
            state.chunkSize = 0;
        }
    }
}

void SHA256::finalize(void *hash, size_t len)
{
    // Pad the last chunk.  We may need two padding chunks if there
    // isn't enough room in the first for the padding and length.
    uint8_t *wbytes = (uint8_t *)state.w;
    if (state.chunkSize <= (64 - 9)) {
        wbytes[state.chunkSize] = 0x80;
        memset(wbytes + state.chunkSize + 1, 0x00, 64 - 8 - (state.chunkSize + 1));
        state.w[14] = htobe32((uint32_t)(state.length >> 32));
        state.w[15] = htobe32((uint32_t)state.length);
        processChunk();
    } else {
        wbytes[state.chunkSize] = 0x80;
        memset(wbytes + state.chunkSize + 1, 0x00, 64 - (state.chunkSize + 1));
        processChunk();
        memset(wbytes, 0x00, 64 - 8);
        state.w[14] = htobe32((uint32_t)(state.length >> 32));
        state.w[15] = htobe32((uint32_t)state.length);
        processChunk();
    }

    // Convert the result into big endian and return it.
    for (uint8_t posn = 0; posn < 8; ++posn)
        state.w[posn] = htobe32(state.h[posn]);

    // Copy the hash to the caller's return buffer.
    size_t maxHashSize = hashSize();
    if (len > maxHashSize)
        len = maxHashSize;
    memcpy(hash, state.w, len);
}

void SHA256::clear()
{
    clean(state);
    reset();
}

void SHA256::resetHMAC(const void *key, size_t keyLen)
{
    formatHMACKey(state.w, key, keyLen, 0x36);
    state.length += 64 * 8;
    processChunk();
}

void SHA256::finalizeHMAC(const void *key, size_t keyLen, void *hash, size_t hashLen)
{
    uint8_t temp[32];
    finalize(temp, sizeof(temp));
    formatHMACKey(state.w, key, keyLen, 0x5C);
    state.length += 64 * 8;
    processChunk();
    update(temp, hashSize());
    finalize(hash, hashLen);
    clean(temp);
}

/**
 * \brief Processes a single 512-bit chunk with the core SHA-256 algorithm.
 *
 * Reference: http://en.wikipedia.org/wiki/SHA-2
 */
void SHA256::processChunk()
{
    // Round constants for SHA-256.
    static uint32_t const k[64] PROGMEM = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    // Convert the first 16 words from big endian to host byte order.
    uint8_t index;
    for (index = 0; index < 16; ++index)
        state.w[index] = be32toh(state.w[index]);

    // Initialise working variables to the current hash value.
    uint32_t a = state.h[0];
    uint32_t b = state.h[1];
    uint32_t c = state.h[2];
    uint32_t d = state.h[3];
    uint32_t e = state.h[4];
    uint32_t f = state.h[5];
    uint32_t g = state.h[6];
    uint32_t h = state.h[7];

    // Perform the first 16 rounds of the compression function main loop.
    uint32_t temp1, temp2;
    for (index = 0; index < 16; ++index) {
        temp1 = h + pgm_read_dword(k + index) + state.w[index] +
                (rightRotate6(e) ^ rightRotate11(e) ^ rightRotate25(e)) +
                ((e & f) ^ ((~e) & g));
        temp2 = (rightRotate2(a) ^ rightRotate13(a) ^ rightRotate22(a)) +
                ((a & b) ^ (a & c) ^ (b & c));
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    // Perform the 48 remaining rounds.  We expand the first 16 words to
    // 64 in-place in the "w" array.  This saves 192 bytes of memory
    // that would have otherwise need to be allocated to the "w" array.
    for (; index < 64; ++index) {
        // Expand the next word.
        temp1 = state.w[(index - 15) & 0x0F];
        temp2 = state.w[(index - 2) & 0x0F];
        temp1 = state.w[index & 0x0F] =
            state.w[(index - 16) & 0x0F] + state.w[(index - 7) & 0x0F] +
                (rightRotate7(temp1) ^ rightRotate18(temp1) ^ (temp1 >> 3)) +
                (rightRotate17(temp2) ^ rightRotate19(temp2) ^ (temp2 >> 10));

        // Perform the round.
        temp1 = h + pgm_read_dword(k + index) + temp1 +
                (rightRotate6(e) ^ rightRotate11(e) ^ rightRotate25(e)) +
                ((e & f) ^ ((~e) & g));
        temp2 = (rightRotate2(a) ^ rightRotate13(a) ^ rightRotate22(a)) +
                ((a & b) ^ (a & c) ^ (b & c));
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
