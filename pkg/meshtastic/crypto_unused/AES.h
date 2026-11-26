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

#include "BlockCipher.h"

// Determine which AES implementation to export to applications.
#if defined(ESP32)
#define CRYPTO_AES_ESP32 1
#else
#define CRYPTO_AES_DEFAULT 1
#endif

#if defined(CRYPTO_AES_DEFAULT) || defined(CRYPTO_DOC)

class AESTiny128;
class AESTiny256;
class AESSmall128;
class AESSmall256;

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

    /** @cond aes_internal */
    uint8_t rounds;
    uint8_t *schedule;

    static void subBytesAndShiftRows(uint8_t *output, const uint8_t *input);
    static void inverseShiftRowsAndSubBytes(uint8_t *output, const uint8_t *input);
    static void mixColumn(uint8_t *output, uint8_t *input);
    static void inverseMixColumn(uint8_t *output, const uint8_t *input);
    static void keyScheduleCore(uint8_t *output, const uint8_t *input, uint8_t iteration);
    static void applySbox(uint8_t *output, const uint8_t *input);
    /** @endcond */

    friend class AESTiny128;
    friend class AESTiny256;
    friend class AESSmall128;
    friend class AESSmall256;
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

class AES192 : public AESCommon
{
public:
    AES192();
    virtual ~AES192();

    size_t keySize() const;

    bool setKey(const uint8_t *key, size_t len);

private:
    uint8_t sched[208];
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

class AESTiny256 : public BlockCipher
{
public:
    AESTiny256();
    virtual ~AESTiny256();

    size_t blockSize() const;
    size_t keySize() const;

    bool setKey(const uint8_t *key, size_t len);

    void encryptBlock(uint8_t *output, const uint8_t *input);
    void decryptBlock(uint8_t *output, const uint8_t *input);

    void clear();

private:
    uint8_t schedule[32];
};

class AESSmall256 : public AESTiny256
{
public:
    AESSmall256();
    virtual ~AESSmall256();

    bool setKey(const uint8_t *key, size_t len);

    void decryptBlock(uint8_t *output, const uint8_t *input);

    void clear();

private:
    uint8_t reverse[32];
};

class AESTiny128 : public BlockCipher
{
public:
    AESTiny128();
    virtual ~AESTiny128();

    size_t blockSize() const;
    size_t keySize() const;

    bool setKey(const uint8_t *key, size_t len);

    void encryptBlock(uint8_t *output, const uint8_t *input);
    void decryptBlock(uint8_t *output, const uint8_t *input);

    void clear();

private:
    uint8_t schedule[16];
};

class AESSmall128 : public AESTiny128
{
public:
    AESSmall128();
    virtual ~AESSmall128();

    bool setKey(const uint8_t *key, size_t len);

    void decryptBlock(uint8_t *output, const uint8_t *input);

    void clear();

private:
    uint8_t reverse[16];
};

#endif // CRYPTO_AES_DEFAULT

#if defined(CRYPTO_AES_ESP32)

/** @cond aes_esp_rename */

// The esp32 SDK keeps moving where aes.h is located, so we have to
// declare the API functions ourselves and make the context opaque.
//
// About the only thing the various SDK versions agree on is that the
// first byte is the length of the key in bytes.
//
// Some versions of esp-idf have a 33 byte AES context, and others 34.
// Allocate up to 40 to make space for future expansion.
#define CRYPTO_ESP32_CONTEXT_SIZE 40

// Some of the esp-idf system headers define enumerations for AES128,
// AES192, and AES256 to identify the hardware-accelerated algorithms.
// These can cause conflicts with the names we use in our library.
// Define our class names to something else to work around esp-idf.
#undef AES128
#undef AES192
#undef AES256
#define AES128 AES128_ESP
#define AES192 AES192_ESP
#define AES256 AES256_ESP

/** @endcond */

class AESCommon : public BlockCipher
{
public:
    virtual ~AESCommon();

    size_t blockSize() const;
    size_t keySize() const;

    bool setKey(const uint8_t *key, size_t len);

    void encryptBlock(uint8_t *output, const uint8_t *input);
    void decryptBlock(uint8_t *output, const uint8_t *input);

    void clear();

protected:
    AESCommon(uint8_t keySize);

private:
    uint8_t ctx[CRYPTO_ESP32_CONTEXT_SIZE];
};

class AES128 : public AESCommon
{
public:
    AES128() : AESCommon(16) {}
    virtual ~AES128();
};

class AES192 : public AESCommon
{
public:
    AES192() : AESCommon(24) {}
    virtual ~AES192();
};

class AES256 : public AESCommon
{
public:
    AES256() : AESCommon(32) {}
    virtual ~AES256();
};

// The ESP32 AES context is so small that it already qualifies as "tiny".
typedef AES128 AESTiny128;
typedef AES256 AESTiny256;
typedef AES128 AESSmall128;
typedef AES256 AESSmall256;

#endif // CRYPTO_AES_ESP32

#endif
