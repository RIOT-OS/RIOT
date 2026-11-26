/*
 * Copyright (C) 2015,2018 Southern Storm Software, Pty Ltd.
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

#include "AES.h"
#include "Crypto.h"
#include <string.h>

#if defined(CRYPTO_AES_DEFAULT) || defined(CRYPTO_DOC)

/**
 * \class AES256 AES.h <AES.h>
 * \brief AES block cipher with 256-bit keys.
 *
 * \sa AES128, AES192, AESTiny256, AESSmall256
 */

/**
 * \brief Constructs an AES 256-bit block cipher with no initial key.
 *
 * This constructor must be followed by a call to setKey() before the
 * block cipher can be used for encryption or decryption.
 */
AES256::AES256()
{
    rounds = 14;
    schedule = sched;
}

AES256::~AES256()
{
    clean(sched);
}

/**
 * \brief Size of a 256-bit AES key in bytes.
 * \return Always returns 32.
 */
size_t AES256::keySize() const
{
    return 32;
}

bool AES256::setKey(const uint8_t *key, size_t len)
{
    if (len != 32)
        return false;

    // Copy the key itself into the first 32 bytes of the schedule.
    uint8_t *schedule = sched;
    memcpy(schedule, key, 32);

    // Expand the key schedule until we have 240 bytes of expanded key.
    uint8_t iteration = 1;
    uint8_t n = 32;
    uint8_t w = 8;
    while (n < 240) {
        if (w == 8) {
            // Every 32 bytes (8 words) we need to apply the key schedule core.
            keyScheduleCore(schedule + 32, schedule + 28, iteration);
            schedule[32] ^= schedule[0];
            schedule[33] ^= schedule[1];
            schedule[34] ^= schedule[2];
            schedule[35] ^= schedule[3];
            ++iteration;
            w = 0;
        } else if (w == 4) {
            // At the 16 byte mark we need to apply the S-box.
            applySbox(schedule + 32, schedule + 28);
            schedule[32] ^= schedule[0];
            schedule[33] ^= schedule[1];
            schedule[34] ^= schedule[2];
            schedule[35] ^= schedule[3];
        } else {
            // Otherwise just XOR the word with the one 32 bytes previous.
            schedule[32] = schedule[28] ^ schedule[0];
            schedule[33] = schedule[29] ^ schedule[1];
            schedule[34] = schedule[30] ^ schedule[2];
            schedule[35] = schedule[31] ^ schedule[3];
        }

        // Advance to the next word in the schedule.
        schedule += 4;
        n += 4;
        ++w;
    }

    return true;
}

/**
 * \class AESTiny256 AES.h <AES.h>
 * \brief AES block cipher with 256-bit keys and tiny memory usage.
 *
 * This class differs from the AES256 class in the following ways:
 *
 * \li RAM requirements are vastly reduced.  The key is stored directly
 * and then expanded to the full key schedule round by round.  The setKey()
 * method is very fast because of this.
 * \li Performance of encryptBlock() is slower than for AES256 due to
 * expanding the key on the fly rather than ahead of time.
 * \li The decryptBlock() function is not supported, which means that CBC
 * mode cannot be used but the CTR, CFB, OFB, EAX, and GCM modes can be used.
 *
 * This class is useful when RAM is at a premium, CBC mode is not required,
 * and reduced encryption performance is not a hindrance to the application.
 *
 * The companion AESSmall256 class supports decryptBlock() at the cost of
 * some additional memory and slower setKey() times.
 *
 * \sa AESSmall256, AES256
 */

/** @cond */

// Helper macros.
#define LEFT 0
#define RIGHT 16
#define ENCRYPT(phase) \
    do { \
        AESCommon::subBytesAndShiftRows(state2, state1); \
        AESCommon::mixColumn(state1,      state2); \
        AESCommon::mixColumn(state1 + 4,  state2 + 4); \
        AESCommon::mixColumn(state1 + 8,  state2 + 8); \
        AESCommon::mixColumn(state1 + 12, state2 + 12); \
        for (posn = 0; posn < 16; ++posn) \
            state1[posn] ^= schedule[posn + (phase)]; \
    } while (0)
#define DECRYPT(phase) \
    do { \
        for (posn = 0; posn < 16; ++posn) \
            state2[posn] ^= schedule[posn + (phase)]; \
        AESCommon::inverseMixColumn(state1,      state2); \
        AESCommon::inverseMixColumn(state1 + 4,  state2 + 4); \
        AESCommon::inverseMixColumn(state1 + 8,  state2 + 8); \
        AESCommon::inverseMixColumn(state1 + 12, state2 + 12); \
        AESCommon::inverseShiftRowsAndSubBytes(state2, state1); \
    } while (0)
#define KCORE(n) \
    do { \
        AESCommon::keyScheduleCore(temp, schedule + 28, (n)); \
        schedule[0] ^= temp[0]; \
        schedule[1] ^= temp[1]; \
        schedule[2] ^= temp[2]; \
        schedule[3] ^= temp[3]; \
    } while (0)
#define KXOR(a, b) \
    do { \
        schedule[(a) * 4] ^= schedule[(b) * 4]; \
        schedule[(a) * 4 + 1] ^= schedule[(b) * 4 + 1]; \
        schedule[(a) * 4 + 2] ^= schedule[(b) * 4 + 2]; \
        schedule[(a) * 4 + 3] ^= schedule[(b) * 4 + 3]; \
    } while (0)
#define KSBOX() \
    do { \
        AESCommon::applySbox(temp, schedule + 12); \
        schedule[16] ^= temp[0]; \
        schedule[17] ^= temp[1]; \
        schedule[18] ^= temp[2]; \
        schedule[19] ^= temp[3]; \
    } while (0)

/** @endcond */

/**
 * \brief Constructs an AES 256-bit block cipher with no initial key.
 *
 * This constructor must be followed by a call to setKey() before the
 * block cipher can be used for encryption or decryption.
 */
AESTiny256::AESTiny256()
{
}

AESTiny256::~AESTiny256()
{
    clean(schedule);
}

/**
 * \brief Size of an AES block in bytes.
 * \return Always returns 16.
 */
size_t AESTiny256::blockSize() const
{
    return 16;
}

/**
 * \brief Size of a 256-bit AES key in bytes.
 * \return Always returns 32.
 */
size_t AESTiny256::keySize() const
{
    return 32;
}

bool AESTiny256::setKey(const uint8_t *key, size_t len)
{
    if (len == 32) {
        // Make a copy of the key - it will be expanded in encryptBlock().
        memcpy(schedule, key, 32);
        return true;
    }
    return false;
}

void AESTiny256::encryptBlock(uint8_t *output, const uint8_t *input)
{
    uint8_t schedule[32];
    uint8_t posn;
    uint8_t round;
    uint8_t state1[16];
    uint8_t state2[16];
    uint8_t temp[4];

    // Start with the key in the schedule buffer.
    memcpy(schedule, this->schedule, 32);

    // Copy the input into the state and perform the first round.
    for (posn = 0; posn < 16; ++posn)
        state1[posn] = input[posn] ^ schedule[posn];
    ENCRYPT(RIGHT);

    // Perform the next 12 rounds of the cipher two at a time.
    for (round = 1; round <= 6; ++round) {
        // Expand the next 32 bytes of the key schedule.
        KCORE(round);
        KXOR(1, 0);
        KXOR(2, 1);
        KXOR(3, 2);
        KSBOX();
        KXOR(5, 4);
        KXOR(6, 5);
        KXOR(7, 6);

        // Encrypt using the left and right halves of the key schedule.
        ENCRYPT(LEFT);
        ENCRYPT(RIGHT);
    }

    // Expand the final 16 bytes of the key schedule.
    KCORE(7);
    KXOR(1, 0);
    KXOR(2, 1);
    KXOR(3, 2);

    // Perform the final round.
    AESCommon::subBytesAndShiftRows(state2, state1);
    for (posn = 0; posn < 16; ++posn)
        output[posn] = state2[posn] ^ schedule[posn];
}

void AESTiny256::decryptBlock(uint8_t *output, const uint8_t *input)
{
    // Decryption is not supported by AESTiny256.
}

void AESTiny256::clear()
{
    clean(schedule);
}

/**
 * \class AESSmall256 AES.h <AES.h>
 * \brief AES block cipher with 256-bit keys and reduced memory usage.
 *
 * This class differs from the AES256 class in that the RAM requirements are
 * vastly reduced.  The key schedule is expanded round by round instead of
 * being generated and stored by setKey().  The performance of encryption
 * and decryption is slightly less because of this.
 *
 * This class is useful when RAM is at a premium and reduced encryption
 * performance is not a hindrance to the application.
 *
 * The companion AESTiny256 class uses even less RAM but only supports the
 * encryptBlock() operation.  Block cipher modes like CTR, EAX, and GCM
 * do not need the decryptBlock() operation, so AESTiny256 may be a better
 * option than AESSmall256 for many applications.
 *
 * \sa AESTiny256, AES256
 */

/**
 * \brief Constructs an AES 256-bit block cipher with no initial key.
 *
 * This constructor must be followed by a call to setKey() before the
 * block cipher can be used for encryption or decryption.
 */
AESSmall256::AESSmall256()
{
}

AESSmall256::~AESSmall256()
{
    clean(reverse);
}

bool AESSmall256::setKey(const uint8_t *key, size_t len)
{
    uint8_t *schedule;
    uint8_t round;
    uint8_t temp[4];

    // Set the encryption key first.
    if (!AESTiny256::setKey(key, len))
        return false;

    // Expand the key schedule up to the last round which gives
    // us the round keys to use for the final two rounds.  We can
    // then work backwards from there in decryptBlock().
    schedule = reverse;
    memcpy(schedule, key, 32);
    for (round = 1; round <= 6; ++round) {
        KCORE(round);
        KXOR(1, 0);
        KXOR(2, 1);
        KXOR(3, 2);
        KSBOX();
        KXOR(5, 4);
        KXOR(6, 5);
        KXOR(7, 6);
    }
    KCORE(7);
    KXOR(1, 0);
    KXOR(2, 1);
    KXOR(3, 2);

    // Key is ready to go.
    return true;
}

void AESSmall256::decryptBlock(uint8_t *output, const uint8_t *input)
{
#if defined(CRYPTO_AES_NO_DECRYPT)
    // Decryption compiled out; zero the output so callers never see stale data.
    (void)input;
    memset(output, 0, 16);
#else
    uint8_t schedule[32];
    uint8_t round;
    uint8_t posn;
    uint8_t state1[16];
    uint8_t state2[16];
    uint8_t temp[4];

    // Start with the end of the decryption schedule.
    memcpy(schedule, reverse, 32);

    // Copy the input into the state and reverse the final round.
    for (posn = 0; posn < 16; ++posn)
        state1[posn] = input[posn] ^ schedule[posn];
    AESCommon::inverseShiftRowsAndSubBytes(state2, state1);
    KXOR(3, 2);
    KXOR(2, 1);
    KXOR(1, 0);
    KCORE(7);

    // Perform the next 12 rounds of the decryption process two at a time.
    for (round = 6; round >= 1; --round) {
        // Decrypt using the right and left halves of the key schedule.
        DECRYPT(RIGHT);
        DECRYPT(LEFT);

        // Expand the next 32 bytes of the key schedule in reverse.
        KXOR(7, 6);
        KXOR(6, 5);
        KXOR(5, 4);
        KSBOX();
        KXOR(3, 2);
        KXOR(2, 1);
        KXOR(1, 0);
        KCORE(round);
    }

    // Reverse the initial round and create the output words.
    DECRYPT(RIGHT);
    for (posn = 0; posn < 16; ++posn)
        output[posn] = state2[posn] ^ schedule[posn];
#endif
}

void AESSmall256::clear()
{
    clean(reverse);
    AESTiny256::clear();
}

#endif // CRYPTO_AES_DEFAULT
