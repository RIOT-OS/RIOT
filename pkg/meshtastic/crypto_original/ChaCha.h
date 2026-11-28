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

#ifndef CRYPTO_CHACHA_h
#define CRYPTO_CHACHA_h

#include "Cipher.h"

class ChaChaPoly;

class ChaCha : public Cipher
{
public:
    explicit ChaCha(uint8_t numRounds = 20);
    virtual ~ChaCha();

    size_t keySize() const;
    size_t ivSize() const;

    uint8_t numRounds() const { return rounds; }
    void setNumRounds(uint8_t numRounds) { rounds = numRounds; }

    bool setKey(const uint8_t *key, size_t len);
    bool setIV(const uint8_t *iv, size_t len);
    bool setCounter(const uint8_t *counter, size_t len);

    void encrypt(uint8_t *output, const uint8_t *input, size_t len);
    void decrypt(uint8_t *output, const uint8_t *input, size_t len);

    void clear();

    static void hashCore(uint32_t *output, const uint32_t *input, uint8_t rounds);

private:
    uint8_t block[64];
    uint8_t stream[64];
    uint8_t rounds;
    uint8_t posn;

    void keystreamBlock(uint32_t *output);

    friend class ChaChaPoly;
};

#endif
