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

#include "CTR.h"
#include "Crypto.h"
#include <string.h>

/**
 * \class CTRCommon CTR.h <CTR.h>
 * \brief Concrete base class to assist with implementing CTR mode for
 * 128-bit block ciphers.
 *
 * Reference: http://en.wikipedia.org/wiki/Block_cipher_mode_of_operation
 *
 * \sa CTR
 */

/**
 * \brief Constructs a new cipher in CTR mode.
 *
 * This constructor should be followed by a call to setBlockCipher().
 */
CTRCommon::CTRCommon()
    : blockCipher(0)
    , posn(16)
    , counterStart(0)
{
}

CTRCommon::~CTRCommon()
{
    // It is assumed that the subclass will clear sensitive
    // information in the block cipher.
    clean(counter);
    clean(state);
}

size_t CTRCommon::keySize() const
{
    return blockCipher->keySize();
}

size_t CTRCommon::ivSize() const
{
    return 16;
}

/**
 * \brief Sets the counter size for the IV.
 *
 * \param size The number of bytes on the end of the counter block
 * that are relevant when incrementing, between 1 and 16.
 * \return Returns false if the \a size value is not between 1 and 16.
 *
 * When the counter is incremented during encrypt(), only the last
 * \a size bytes are considered relevant.  This can be useful
 * to improve performance when the higher level protocol specifies that
 * only the least significant N bytes "count".  The high level protocol
 * should explicitly generate a new initial counter value and key long
 * before the \a size bytes overflow and wrap around.
 *
 * By default, the counter size is 16 which is the same as the block size
 * of the underlying block cipher.
 *
 * \sa setIV()
 */
bool CTRCommon::setCounterSize(size_t size)
{
    if (size < 1 || size > 16)
        return false;
    counterStart = 16 - size;
    return true;
}

bool CTRCommon::setKey(const uint8_t *key, size_t len)
{
    // Verify the cipher's block size, just in case.
    if (blockCipher->blockSize() != 16)
        return false;

    // Set the key on the underlying block cipher.
    return blockCipher->setKey(key, len);
}

/**
 * \brief Sets the initial counter value to use for future encryption and
 * decryption operations.
 *
 * \param iv The initial counter value which must contain exactly 16 bytes.
 * \param len The length of the counter value, which mut be 16.
 * \return Returns false if \a len is not exactly 16.
 *
 * The precise method to generate the initial counter is not defined by
 * this class.  Usually higher level protocols like SSL/TLS and SSH
 * specify how to construct the initial counter value.  This class merely
 * increments the counter every time a new block of keystream data is needed.
 *
 * \sa encrypt(), setCounterSize()
 */
bool CTRCommon::setIV(const uint8_t *iv, size_t len)
{
    if (len != 16)
        return false;
    memcpy(counter, iv, len);
    posn = 16;
    return true;
}

void CTRCommon::encrypt(uint8_t *output, const uint8_t *input, size_t len)
{
    while (len > 0) {
        if (posn >= 16) {
            // Generate a new encrypted counter block.
            blockCipher->encryptBlock(state, counter);
            posn = 0;

            // Increment the counter, taking care not to reveal
            // any timing information about the starting value.
            // We iterate through the entire counter region even
            // if we could stop earlier because a byte is non-zero.
            uint16_t temp = 1;
            uint8_t index = 16;
            while (index > counterStart) {
                --index;
                temp += counter[index];
                counter[index] = (uint8_t)temp;
                temp >>= 8;
            }
        }
        uint8_t templen = 16 - posn;
        if (templen > len)
            templen = len;
        len -= templen;
        while (templen > 0) {
            *output++ = *input++ ^ state[posn++];
            --templen;
        }
    }
}

void CTRCommon::decrypt(uint8_t *output, const uint8_t *input, size_t len)
{
    encrypt(output, input, len);
}

void CTRCommon::clear()
{
    blockCipher->clear();
    clean(counter);
    clean(state);
    posn = 16;
}

/**
 * \fn void CTRCommon::setBlockCipher(BlockCipher *cipher)
 * \brief Sets the block cipher to use for this CTR object.
 *
 * \param cipher The block cipher to use to implement CTR mode,
 * which must have a block size of 16 bytes (128 bits).
 *
 * \note This class only works with block ciphers whose block size is
 * 16 bytes (128 bits).  If the \a cipher has a different block size,
 * then setKey() will fail and return false.
 */

/**
 * \class CTR CTR.h <CTR.h>
 * \brief Implementation of the Counter (CTR) mode for 128-bit block ciphers.
 *
 * Counter mode converts a block cipher into a stream cipher.  The specific
 * block cipher is passed as the template parameter T and the key is
 * specified via the setKey() function.
 *
 * Keystream blocks are generated by encrypting an increasing counter value
 * and XOR'ing it with each byte of input.  The encrypt() and decrypt()
 * operations are identical.
 *
 * The template parameter T must be a concrete subclass of BlockCipher
 * indicating the specific block cipher to use.  For example, the following
 * creates a CTR object using AES256 as the underlying cipher:
 *
 * \code
 * CTR<AES256> ctr;
 * ctr.setKey(key, 32);
 * ctr.setIV(iv, 16);
 * ctr.setCounterSize(4);
 * ctr.encrypt(output, input, len);
 * \endcode
 *
 * In this example, the last 4 bytes of the IV are incremented to count
 * blocks.  The remaining bytes are left unchanged from block to block.
 *
 * Reference: http://en.wikipedia.org/wiki/Block_cipher_mode_of_operation
 *
 * \sa CFB, OFB, CBC
 */

/**
 * \fn CTR::CTR()
 * \brief Constructs a new CTR object for the 128-bit block cipher T.
 */
