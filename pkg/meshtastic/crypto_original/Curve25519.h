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

#ifndef CRYPTO_CURVE25519_h
#define CRYPTO_CURVE25519_h

#include "BigNumberUtil.h"

class Ed25519;

class Curve25519
{
public:
    static bool eval(uint8_t result[32], const uint8_t s[32], const uint8_t x[32]);

    static void dh1(uint8_t k[32], uint8_t f[32]);
    static bool dh2(uint8_t k[32], uint8_t f[32]);

#if defined(TEST_CURVE25519_FIELD_OPS)
public:
#else
private:
#endif
    static uint8_t isWeakPoint(const uint8_t k[32]);

    static void reduce(limb_t *result, limb_t *x, uint8_t size);
    static limb_t reduceQuick(limb_t *x);

    static void mulNoReduce(limb_t *result, const limb_t *x, const limb_t *y);

    static void mul(limb_t *result, const limb_t *x, const limb_t *y);
    static void square(limb_t *result, const limb_t *x)
    {
        mul(result, x, x);
    }

    static void mulA24(limb_t *result, const limb_t *x);

    static void mul_P(limb_t *result, const limb_t *x, const limb_t *y);

    static void add(limb_t *result, const limb_t *x, const limb_t *y);
    static void sub(limb_t *result, const limb_t *x, const limb_t *y);

    static void cswap(limb_t select, limb_t *x, limb_t *y);
    static void cmove(limb_t select, limb_t *x, const limb_t *y);

    static void pow250(limb_t *result, const limb_t *x);
    static void recip(limb_t *result, const limb_t *x);
    static bool sqrt(limb_t *result, const limb_t *x);

    // Constructor and destructor are private - cannot instantiate this class.
    Curve25519() {}
    ~Curve25519() {}

    friend class Ed25519;
    friend class XEdDSA;
};

#endif
