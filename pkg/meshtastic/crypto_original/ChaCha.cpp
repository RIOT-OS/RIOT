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

#include "ChaCha.h"
#include "Crypto.h"
#include "utility/RotateUtil.h"
#include "utility/EndianUtil.h"
#include "utility/ProgMemUtil.h"
#include <string.h>

/**
 * \class ChaCha ChaCha.h <ChaCha.h>
 * \brief ChaCha stream cipher.
 *
 * ChaCha is a stream cipher that takes a key, an 8-byte nonce/IV, and a
 * counter and hashes them to generate a keystream to XOR with the plaintext.
 * Variations on the ChaCha cipher use 8, 12, or 20 rounds of hashing
 * operations with either 128-bit or 256-bit keys.
 *
 * Reference: http://cr.yp.to/chacha.html
 */

/**
 * \brief Constructs a new ChaCha stream cipher.
 *
 * \param numRounds Number of encryption rounds to use; usually 8, 12, or 20.
 */
ChaCha::ChaCha(uint8_t numRounds)
    : rounds(numRounds)
    , posn(64)
{
}

ChaCha::~ChaCha()
{
    clean(block);
    clean(stream);
}

size_t ChaCha::keySize() const
{
    // Default key size is 256-bit, but any key size is allowed.
    return 32;
}

size_t ChaCha::ivSize() const
{
    // We return 8 but we also support 12-byte nonces in setIV().
    return 8;
}

/**
 * \fn uint8_t ChaCha::numRounds() const
 * \brief Returns the number of encryption rounds; usually 8, 12, or 20.
 *
 * \sa setNumRounds()
 */

/**
 * \fn void ChaCha::setNumRounds(uint8_t numRounds)
 * \brief Sets the number of encryption rounds.
 *
 * \param numRounds The number of encryption rounds; usually 8, 12, or 20.
 *
 * \sa numRounds()
 */

bool ChaCha::setKey(const uint8_t *key, size_t len)
{
    static const char tag128[] PROGMEM = "expand 16-byte k";
    static const char tag256[] PROGMEM = "expand 32-byte k";
    if (len <= 16) {
        memcpy_P(block, tag128, 16);
        memcpy(block + 16, key, len);
        memcpy(block + 32, key, len);
        if (len < 16) {
            memset(block + 16 + len, 0, 16 - len);
            memset(block + 32 + len, 0, 16 - len);
        }
    } else {
        if (len > 32)
            len = 32;
        memcpy_P(block, tag256, 16);
        memcpy(block + 16, key, len);
        if (len < 32)
            memset(block + 16 + len, 0, 32 - len);
    }
    posn = 64;
    return true;
}

bool ChaCha::setIV(const uint8_t *iv, size_t len)
{
    // From draft-nir-cfrg-chacha20-poly1305-10.txt, we can use either
    // 64-bit or 96-bit nonces.  The 96-bit nonce consists of the high
    // word of the counter prepended to a regular 64-bit nonce for ChaCha.
    if (len == 8) {
        memset(block + 48, 0, 8);
        memcpy(block + 56, iv, len);
        posn = 64;
        return true;
    } else if (len == 12) {
        memset(block + 48, 0, 4);
        memcpy(block + 52, iv, len);
        posn = 64;
        return true;
    } else {
        return false;
    }
}

/**
 * \brief Sets the starting counter for encryption.
 *
 * \param counter A 4-byte or 8-byte value to use for the starting counter
 * instead of the default value of zero.
 * \param len The length of the counter, which must be 4 or 8.
 * \return Returns false if \a len is not 4 or 8.
 *
 * This function must be called after setIV() and before the first call
 * to encrypt().  It is used to specify a different starting value than
 * zero for the counter portion of the hash input.
 *
 * \sa setIV()
 */
bool ChaCha::setCounter(const uint8_t *counter, size_t len)
{
    // Normally both the IV and the counter are 8 bytes in length.
    // However, if the IV was 12 bytes, then a 4 byte counter can be used.
    if (len == 4 || len == 8) {
        memcpy(block + 48, counter, len);
        posn = 64;
        return true;
    } else {
        return false;
    }
}

void ChaCha::encrypt(uint8_t *output, const uint8_t *input, size_t len)
{
    while (len > 0) {
        if (posn >= 64) {
            // Generate a new encrypted counter block.
            hashCore((uint32_t *)stream, (const uint32_t *)block, rounds);
            posn = 0;

            // Increment the counter, taking care not to reveal
            // any timing information about the starting value.
            // We iterate through the entire counter region even
            // if we could stop earlier because a byte is non-zero.
            uint16_t temp = 1;
            uint8_t index = 48;
            while (index < 56) {
                temp += block[index];
                block[index] = (uint8_t)temp;
                temp >>= 8;
                ++index;
            }
        }
        uint8_t templen = 64 - posn;
        if (templen > len)
            templen = len;
        len -= templen;
        while (templen > 0) {
            *output++ = *input++ ^ stream[posn++];
            --templen;
        }
    }
}

void ChaCha::decrypt(uint8_t *output, const uint8_t *input, size_t len)
{
    encrypt(output, input, len);
}

/**
 * \brief Generates a single block of output direct from the keystream.
 *
 * \param output The output buffer to fill with keystream bytes.
 *
 * Unlike encrypt(), this function does not XOR the keystream with
 * plaintext data.  Instead it generates the keystream directly into
 * the caller-supplied buffer.  This is useful if the caller knows
 * that the plaintext is all-zeroes.
 *
 * \sa encrypt()
 */
void ChaCha::keystreamBlock(uint32_t *output)
{
    // Generate the hash output directly into the caller-supplied buffer.
    hashCore(output, (const uint32_t *)block, rounds);
    posn = 64;

    // Increment the lowest counter byte.  We are assuming that the caller
    // is ChaChaPoly::setKey() and that the previous counter value was zero.
    block[48] = 1;
}

void ChaCha::clear()
{
    clean(block);
    clean(stream);
    posn = 64;
}

// Perform a ChaCha quarter round operation.
#define quarterRound(a, b, c, d)    \
    do { \
        uint32_t _b = (b); \
        uint32_t _a = (a) + _b; \
        uint32_t _d = leftRotate((d) ^ _a, 16); \
        uint32_t _c = (c) + _d; \
        _b = leftRotate12(_b ^ _c); \
        _a += _b; \
        (d) = _d = leftRotate(_d ^ _a, 8); \
        _c += _d; \
        (a) = _a; \
        (b) = leftRotate7(_b ^ _c); \
        (c) = _c; \
    } while (0)

/**
 * \brief Executes the ChaCha hash core on an input memory block.
 *
 * \param output Output memory block, must be at least 16 words in length
 * and must not overlap with \a input.
 * \param input Input memory block, must be at least 16 words in length.
 * \param rounds Number of ChaCha rounds to perform; usually 8, 12, or 20.
 *
 * This function is provided for the convenience of applications that need
 * access to the ChaCha hash core without the higher-level processing that
 * turns the core into a stream cipher.
 */
void ChaCha::hashCore(uint32_t *output, const uint32_t *input, uint8_t rounds)
{
    uint8_t posn;

    // Copy the input buffer to the output prior to the first round
    // and convert from little-endian to host byte order.
    for (posn = 0; posn < 16; ++posn)
        output[posn] = le32toh(input[posn]);

    // Perform the ChaCha rounds in sets of two.
    for (; rounds >= 2; rounds -= 2) {
        // Column round.
        quarterRound(output[0], output[4], output[8],  output[12]);
        quarterRound(output[1], output[5], output[9],  output[13]);
        quarterRound(output[2], output[6], output[10], output[14]);
        quarterRound(output[3], output[7], output[11], output[15]);

        // Diagonal round.
        quarterRound(output[0], output[5], output[10], output[15]);
        quarterRound(output[1], output[6], output[11], output[12]);
        quarterRound(output[2], output[7], output[8],  output[13]);
        quarterRound(output[3], output[4], output[9],  output[14]);
    }

    // Add the original input to the final output, convert back to
    // little-endian, and return the result.
    for (posn = 0; posn < 16; ++posn)
        output[posn] = htole32(output[posn] + le32toh(input[posn]));
}
