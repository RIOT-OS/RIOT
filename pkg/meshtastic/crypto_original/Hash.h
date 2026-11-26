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

#ifndef CRYPTO_HASH_h
#define CRYPTO_HASH_h

#include <inttypes.h>
#include <stddef.h>

class Hash
{
public:
    Hash();
    virtual ~Hash();

    virtual size_t hashSize() const = 0;
    virtual size_t blockSize() const = 0;

    virtual void reset() = 0;
    virtual void update(const void *data, size_t len) = 0;
    virtual void finalize(void *hash, size_t len) = 0;

    virtual void clear() = 0;

    virtual void resetHMAC(const void *key, size_t keyLen) = 0;
    virtual void finalizeHMAC(const void *key, size_t keyLen, void *hash, size_t hashLen) = 0;

protected:
    void formatHMACKey(void *block, const void *key, size_t len, uint8_t pad);
};

template <typename T> void hmac
    (void *out, size_t outLen, const void *key, size_t keyLen,
     const void *data, size_t dataLen)
{
    T context;
    context.resetHMAC(key, keyLen);
    context.update(data, dataLen);
    context.finalizeHMAC(key, keyLen, out, outLen);
}

#endif
