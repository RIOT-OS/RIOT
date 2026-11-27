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

#ifndef CRYPTO_ED25519_h
#define CRYPTO_ED25519_h

#include "BigNumberUtil.h"
#include "SHA512.h"

class Ed25519
{
public:
    static void sign(uint8_t signature[64], const uint8_t privateKey[32],
                     const uint8_t publicKey[32], const void *message,
                     size_t len);
    static bool verify(const uint8_t signature[64], const uint8_t publicKey[32],
                       const void *message, size_t len);

    static void generatePrivateKey(uint8_t privateKey[32]);
    static void derivePublicKey(uint8_t publicKey[32], const uint8_t privateKey[32]);

protected:
    // Constructor and destructor are private - cannot instantiate this class.
    Ed25519();
    ~Ed25519();

    // Curve point represented in extended homogeneous coordinates.
    struct Point
    {
        limb_t x[32 / sizeof(limb_t)];
        limb_t y[32 / sizeof(limb_t)];
        limb_t z[32 / sizeof(limb_t)];
        limb_t t[32 / sizeof(limb_t)];
    };

    static void reduceQFromBuffer(limb_t *result, const uint8_t buf[64], limb_t *temp);
    static void reduceQ(limb_t *result, limb_t *r);

    static void mul(Point &result, const limb_t *s, Point &p, bool constTime = true);
    static void mul(Point &result, const limb_t *s, bool constTime = true);

    static void add(Point &p, const Point &q);

    static bool equal(const Point &p, const Point &q);

    static void encodePoint(uint8_t *buf, Point &point);
    static bool decodePoint(Point &point, const uint8_t *buf);

    static void deriveKeys(SHA512 *hash, limb_t *a, const uint8_t privateKey[32]);
};

#endif
