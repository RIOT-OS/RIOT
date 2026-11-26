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

#ifndef CRYPTO_AES_h
#define CRYPTO_AES_h

//C++ Libraries
#include "BlockCipher.h"

// C Libraries
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "crypto/ciphers.h"

#ifdef __cplusplus
}
#endif

class AESCommon : public BlockCipher
{
public:
    virtual ~AESCommon();

    size_t blockSize() const;

    void encryptBlock(uint8_t *output, const uint8_t *input);
    void decryptBlock(uint8_t *output, const uint8_t *input);

    void clear();

protected:
    AESCommon();

    cipher_t _cipher;
    size_t _key_size = 0;
};

class AES128 : public AESCommon
{
public:
    AES128();
    virtual ~AES128();

    size_t keySize() const;

    bool setKey(const uint8_t *key, size_t len);

private:
    uint8_t sched[176];
};

class AES256 : public AESCommon
{
public:
    AES256();
    virtual ~AES256();

    size_t keySize() const;

    bool setKey(const uint8_t *key, size_t len);

private:
    uint8_t sched[240];
};

class AESTiny256 : public AES256
{
public:
    AESTiny256();
    virtual ~AESTiny256();
};

class AESSmall256 : public AESTiny256
{
public:
    AESSmall256();
    virtual ~AESSmall256();
};


#endif
