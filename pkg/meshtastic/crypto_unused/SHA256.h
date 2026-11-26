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

#ifndef CRYPTO_SHA256_h
#define CRYPTO_SHA256_h

#include "Hash.h"

class SHA256 : public Hash
{
public:
    SHA256();
    virtual ~SHA256();

    size_t hashSize() const;
    size_t blockSize() const;

    void reset();
    void update(const void *data, size_t len);
    void finalize(void *hash, size_t len);

    void clear();

    void resetHMAC(const void *key, size_t keyLen);
    void finalizeHMAC(const void *key, size_t keyLen, void *hash, size_t hashLen);

    static const size_t HASH_SIZE  = 32;
    static const size_t BLOCK_SIZE = 64;

protected:
    struct {
        uint32_t h[8];
        uint32_t w[16];
        uint64_t length;
        uint8_t chunkSize;
    } state;

    void processChunk();
};

#endif
