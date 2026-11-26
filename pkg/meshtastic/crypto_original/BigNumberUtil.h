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

#ifndef CRYPTO_BIGNUMBERUTIL_h
#define CRYPTO_BIGNUMBERUTIL_h

#include <inttypes.h>
#include <stddef.h>

// Define exactly one of these to 1 to set the size of the basic limb type.
#if defined(__AVR__) || defined(ESP8266)
// 16-bit limbs seem to give the best performance on 8-bit AVR micros.
// They also seem to give better performance on ESP8266 as well.
#define BIGNUMBER_LIMB_8BIT  0
#define BIGNUMBER_LIMB_16BIT 1
#define BIGNUMBER_LIMB_32BIT 0
#define BIGNUMBER_LIMB_64BIT 0
#elif defined(__GNUC__) && __WORDSIZE == 64
// 64-bit system with 128-bit double limbs.
#define BIGNUMBER_LIMB_8BIT  0
#define BIGNUMBER_LIMB_16BIT 0
#define BIGNUMBER_LIMB_32BIT 0
#define BIGNUMBER_LIMB_64BIT 1
#else
// On all other platforms, assume 32-bit is best.
#define BIGNUMBER_LIMB_8BIT  0
#define BIGNUMBER_LIMB_16BIT 0
#define BIGNUMBER_LIMB_32BIT 1
#define BIGNUMBER_LIMB_64BIT 0
#endif

// Define the limb types to use on this platform.
#if BIGNUMBER_LIMB_8BIT
typedef uint8_t limb_t;
typedef int8_t slimb_t;
typedef uint16_t dlimb_t;
#elif BIGNUMBER_LIMB_16BIT
typedef uint16_t limb_t;
typedef int16_t slimb_t;
typedef uint32_t dlimb_t;
#elif BIGNUMBER_LIMB_32BIT
typedef uint32_t limb_t;
typedef int32_t slimb_t;
typedef uint64_t dlimb_t;
#elif BIGNUMBER_LIMB_64BIT
typedef uint64_t limb_t;
typedef int64_t slimb_t;
typedef unsigned __int128 dlimb_t;
#else
#error "limb_t must be 8, 16, 32, or 64 bits in size"
#endif

class BigNumberUtil
{
public:
    static void unpackLE(limb_t *limbs, size_t count,
                         const uint8_t *bytes, size_t len);
    static void unpackBE(limb_t *limbs, size_t count,
                         const uint8_t *bytes, size_t len);
    static void packLE(uint8_t *bytes, size_t len,
                       const limb_t *limbs, size_t count);
    static void packBE(uint8_t *bytes, size_t len,
                       const limb_t *limbs, size_t count);

    static limb_t add(limb_t *result, const limb_t *x,
                      const limb_t *y, size_t size);
    static limb_t sub(limb_t *result, const limb_t *x,
                      const limb_t *y, size_t size);
    static void mul(limb_t *result, const limb_t *x, size_t xcount,
                    const limb_t *y, size_t ycount);
    static void reduceQuick(limb_t *result, const limb_t *x,
                            const limb_t *y, size_t size);

    static limb_t add_P(limb_t *result, const limb_t *x,
                        const limb_t *y, size_t size);
    static limb_t sub_P(limb_t *result, const limb_t *x,
                        const limb_t *y, size_t size);
    static void mul_P(limb_t *result, const limb_t *x, size_t xcount,
                      const limb_t *y, size_t ycount);
    static void reduceQuick_P(limb_t *result, const limb_t *x,
                              const limb_t *y, size_t size);

    static limb_t isZero(const limb_t *x, size_t size);

private:
    // Constructor and destructor are private - cannot instantiate this class.
    BigNumberUtil() {}
    ~BigNumberUtil() {}
};

#endif
