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

#ifndef CRYPTO_ROTATEUTIL_H
#define CRYPTO_ROTATEUTIL_H

#include <inttypes.h>

// Rotation functions that are optimised for best performance on AVR.
// The most efficient rotations are where the number of bits is 1 or a
// multiple of 8, so we compose the efficient rotations to produce all
// other rotation counts of interest.

#if defined(__AVR__)
#define CRYPTO_ROTATE32_COMPOSED 1
#define CRYPTO_ROTATE64_COMPOSED 0
#else
#define CRYPTO_ROTATE32_COMPOSED 0
#define CRYPTO_ROTATE64_COMPOSED 0
#endif

#if CRYPTO_ROTATE32_COMPOSED

// Rotation macros for 32-bit arguments.

// Generic left rotate - best performance when "bits" is 1 or a multiple of 8.
#define leftRotate(a, bits) \
    (__extension__ ({ \
        uint32_t _temp = (a); \
        (_temp << (bits)) | (_temp >> (32 - (bits))); \
    }))

// Generic right rotate - best performance when "bits" is 1 or a multiple of 8.
#define rightRotate(a, bits) \
    (__extension__ ({ \
        uint32_t _temp = (a); \
        (_temp >> (bits)) | (_temp << (32 - (bits))); \
    }))

// Left rotate by 1.
#define leftRotate1(a)  (leftRotate((a), 1))

// Left rotate by 2.
#define leftRotate2(a)  (leftRotate(leftRotate((a), 1), 1))

// Left rotate by 3.
#define leftRotate3(a)  (leftRotate(leftRotate(leftRotate((a), 1), 1), 1))

// Left rotate by 4.
#define leftRotate4(a)  (leftRotate(leftRotate(leftRotate(leftRotate((a), 1), 1), 1), 1))

// Left rotate by 5: Rotate left by 8, then right by 3.
#define leftRotate5(a)  (rightRotate(rightRotate(rightRotate(leftRotate((a), 8), 1), 1), 1))

// Left rotate by 6: Rotate left by 8, then right by 2.
#define leftRotate6(a)  (rightRotate(rightRotate(leftRotate((a), 8), 1), 1))

// Left rotate by 7: Rotate left by 8, then right by 1.
#define leftRotate7(a)  (rightRotate(leftRotate((a), 8), 1))

// Left rotate by 8.
#define leftRotate8(a)  (leftRotate((a), 8))

// Left rotate by 9: Rotate left by 8, then left by 1.
#define leftRotate9(a)  (leftRotate(leftRotate((a), 8), 1))

// Left rotate by 10: Rotate left by 8, then left by 2.
#define leftRotate10(a) (leftRotate(leftRotate(leftRotate((a), 8), 1), 1))

// Left rotate by 11: Rotate left by 8, then left by 3.
#define leftRotate11(a) (leftRotate(leftRotate(leftRotate(leftRotate((a), 8), 1), 1), 1))

// Left rotate by 12: Rotate left by 16, then right by 4.
#define leftRotate12(a) (rightRotate(rightRotate(rightRotate(rightRotate(leftRotate((a), 16), 1), 1), 1), 1))

// Left rotate by 13: Rotate left by 16, then right by 3.
#define leftRotate13(a) (rightRotate(rightRotate(rightRotate(leftRotate((a), 16), 1), 1), 1))

// Left rotate by 14: Rotate left by 16, then right by 2.
#define leftRotate14(a) (rightRotate(rightRotate(leftRotate((a), 16), 1), 1))

// Left rotate by 15: Rotate left by 16, then right by 1.
#define leftRotate15(a) (rightRotate(leftRotate((a), 16), 1))

// Left rotate by 16.
#define leftRotate16(a) (leftRotate((a), 16))

// Left rotate by 17: Rotate left by 16, then left by 1.
#define leftRotate17(a) (leftRotate(leftRotate((a), 16), 1))

// Left rotate by 18: Rotate left by 16, then left by 2.
#define leftRotate18(a) (leftRotate(leftRotate(leftRotate((a), 16), 1), 1))

// Left rotate by 19: Rotate left by 16, then left by 3.
#define leftRotate19(a) (leftRotate(leftRotate(leftRotate(leftRotate((a), 16), 1), 1), 1))

// Left rotate by 20: Rotate left by 16, then left by 4.
#define leftRotate20(a) (leftRotate(leftRotate(leftRotate(leftRotate(leftRotate((a), 16), 1), 1), 1), 1))

// Left rotate by 21: Rotate left by 24, then right by 3.
#define leftRotate21(a) (rightRotate(rightRotate(rightRotate(leftRotate((a), 24), 1), 1), 1))

// Left rotate by 22: Rotate left by 24, then right by 2.
#define leftRotate22(a) (rightRotate(rightRotate(leftRotate((a), 24), 1), 1))

// Left rotate by 23: Rotate left by 24, then right by 1.
#define leftRotate23(a) (rightRotate(leftRotate((a), 24), 1))

// Left rotate by 24.
#define leftRotate24(a) (leftRotate((a), 24))

// Left rotate by 25: Rotate left by 24, then left by 1.
#define leftRotate25(a) (leftRotate(leftRotate((a), 24), 1))

// Left rotate by 26: Rotate left by 24, then left by 2.
#define leftRotate26(a) (leftRotate(leftRotate(leftRotate((a), 24), 1), 1))

// Left rotate by 27: Rotate left by 24, then left by 3.
#define leftRotate27(a) (leftRotate(leftRotate(leftRotate(leftRotate((a), 24), 1), 1), 1))

// Left rotate by 28: Rotate right by 4.
#define leftRotate28(a) (rightRotate(rightRotate(rightRotate(rightRotate((a), 1), 1), 1), 1))

// Left rotate by 29: Rotate right by 3.
#define leftRotate29(a) (rightRotate(rightRotate(rightRotate((a), 1), 1), 1))

// Left rotate by 30: Rotate right by 2.
#define leftRotate30(a) (rightRotate(rightRotate((a), 1), 1))

// Left rotate by 31: Rotate right by 1.
#define leftRotate31(a) (rightRotate((a), 1))

// Define the 32-bit right rotations in terms of left rotations.
#define rightRotate1(a)  (leftRotate31((a)))
#define rightRotate2(a)  (leftRotate30((a)))
#define rightRotate3(a)  (leftRotate29((a)))
#define rightRotate4(a)  (leftRotate28((a)))
#define rightRotate5(a)  (leftRotate27((a)))
#define rightRotate6(a)  (leftRotate26((a)))
#define rightRotate7(a)  (leftRotate25((a)))
#define rightRotate8(a)  (leftRotate24((a)))
#define rightRotate9(a)  (leftRotate23((a)))
#define rightRotate10(a) (leftRotate22((a)))
#define rightRotate11(a) (leftRotate21((a)))
#define rightRotate12(a) (leftRotate20((a)))
#define rightRotate13(a) (leftRotate19((a)))
#define rightRotate14(a) (leftRotate18((a)))
#define rightRotate15(a) (leftRotate17((a)))
#define rightRotate16(a) (leftRotate16((a)))
#define rightRotate17(a) (leftRotate15((a)))
#define rightRotate18(a) (leftRotate14((a)))
#define rightRotate19(a) (leftRotate13((a)))
#define rightRotate20(a) (leftRotate12((a)))
#define rightRotate21(a) (leftRotate11((a)))
#define rightRotate22(a) (leftRotate10((a)))
#define rightRotate23(a) (leftRotate9((a)))
#define rightRotate24(a) (leftRotate8((a)))
#define rightRotate25(a) (leftRotate7((a)))
#define rightRotate26(a) (leftRotate6((a)))
#define rightRotate27(a) (leftRotate5((a)))
#define rightRotate28(a) (leftRotate4((a)))
#define rightRotate29(a) (leftRotate3((a)))
#define rightRotate30(a) (leftRotate2((a)))
#define rightRotate31(a) (leftRotate1((a)))

#else // !CRYPTO_ROTATE32_COMPOSED

// Generic rotation functions.  All bit shifts are considered to have
// similar performance.  Usually true of 32-bit and higher platforms.

// Rotation macros for 32-bit arguments.

// Generic left rotate.
#define leftRotate(a, bits) \
    (__extension__ ({ \
        uint32_t _temp = (a); \
        (_temp << (bits)) | (_temp >> (32 - (bits))); \
    }))

// Generic right rotate.
#define rightRotate(a, bits) \
    (__extension__ ({ \
        uint32_t _temp = (a); \
        (_temp >> (bits)) | (_temp << (32 - (bits))); \
    }))

// Left rotate by a specific number of bits.
#define leftRotate1(a)  (leftRotate((a), 1))
#define leftRotate2(a)  (leftRotate((a), 2))
#define leftRotate3(a)  (leftRotate((a), 3))
#define leftRotate4(a)  (leftRotate((a), 4))
#define leftRotate5(a)  (leftRotate((a), 5))
#define leftRotate6(a)  (leftRotate((a), 6))
#define leftRotate7(a)  (leftRotate((a), 7))
#define leftRotate8(a)  (leftRotate((a), 8))
#define leftRotate9(a)  (leftRotate((a), 9))
#define leftRotate10(a) (leftRotate((a), 10))
#define leftRotate11(a) (leftRotate((a), 11))
#define leftRotate12(a) (leftRotate((a), 12))
#define leftRotate13(a) (leftRotate((a), 13))
#define leftRotate14(a) (leftRotate((a), 14))
#define leftRotate15(a) (leftRotate((a), 15))
#define leftRotate16(a) (leftRotate((a), 16))
#define leftRotate17(a) (leftRotate((a), 17))
#define leftRotate18(a) (leftRotate((a), 18))
#define leftRotate19(a) (leftRotate((a), 19))
#define leftRotate20(a) (leftRotate((a), 20))
#define leftRotate21(a) (leftRotate((a), 21))
#define leftRotate22(a) (leftRotate((a), 22))
#define leftRotate23(a) (leftRotate((a), 23))
#define leftRotate24(a) (leftRotate((a), 24))
#define leftRotate25(a) (leftRotate((a), 25))
#define leftRotate26(a) (leftRotate((a), 26))
#define leftRotate27(a) (leftRotate((a), 27))
#define leftRotate28(a) (leftRotate((a), 28))
#define leftRotate29(a) (leftRotate((a), 29))
#define leftRotate30(a) (leftRotate((a), 30))
#define leftRotate31(a) (leftRotate((a), 31))

// Right rotate by a specific number of bits.
#define rightRotate1(a)  (rightRotate((a), 1))
#define rightRotate2(a)  (rightRotate((a), 2))
#define rightRotate3(a)  (rightRotate((a), 3))
#define rightRotate4(a)  (rightRotate((a), 4))
#define rightRotate5(a)  (rightRotate((a), 5))
#define rightRotate6(a)  (rightRotate((a), 6))
#define rightRotate7(a)  (rightRotate((a), 7))
#define rightRotate8(a)  (rightRotate((a), 8))
#define rightRotate9(a)  (rightRotate((a), 9))
#define rightRotate10(a) (rightRotate((a), 10))
#define rightRotate11(a) (rightRotate((a), 11))
#define rightRotate12(a) (rightRotate((a), 12))
#define rightRotate13(a) (rightRotate((a), 13))
#define rightRotate14(a) (rightRotate((a), 14))
#define rightRotate15(a) (rightRotate((a), 15))
#define rightRotate16(a) (rightRotate((a), 16))
#define rightRotate17(a) (rightRotate((a), 17))
#define rightRotate18(a) (rightRotate((a), 18))
#define rightRotate19(a) (rightRotate((a), 19))
#define rightRotate20(a) (rightRotate((a), 20))
#define rightRotate21(a) (rightRotate((a), 21))
#define rightRotate22(a) (rightRotate((a), 22))
#define rightRotate23(a) (rightRotate((a), 23))
#define rightRotate24(a) (rightRotate((a), 24))
#define rightRotate25(a) (rightRotate((a), 25))
#define rightRotate26(a) (rightRotate((a), 26))
#define rightRotate27(a) (rightRotate((a), 27))
#define rightRotate28(a) (rightRotate((a), 28))
#define rightRotate29(a) (rightRotate((a), 29))
#define rightRotate30(a) (rightRotate((a), 30))
#define rightRotate31(a) (rightRotate((a), 31))

#endif // !CRYPTO_ROTATE32_COMPOSED

#if CRYPTO_ROTATE64_COMPOSED

// Rotation macros for 64-bit arguments.

// Generic left rotate - best performance when "bits" is 1 or a multiple of 8.
#define leftRotate_64(a, bits) \
    (__extension__ ({ \
        uint64_t _temp = (a); \
        (_temp << (bits)) | (_temp >> (64 - (bits))); \
    }))

// Generic right rotate - best performance when "bits" is 1 or a multiple of 8.
#define rightRotate_64(a, bits) \
    (__extension__ ({ \
        uint64_t _temp = (a); \
        (_temp >> (bits)) | (_temp << (64 - (bits))); \
    }))

// Left rotate by 1.
#define leftRotate1_64(a)  (leftRotate_64((a), 1))

// Left rotate by 2.
#define leftRotate2_64(a)  (leftRotate_64(leftRotate_64((a), 1), 1))

// Left rotate by 3.
#define leftRotate3_64(a)  (leftRotate_64(leftRotate_64(leftRotate_64((a), 1), 1), 1))

// Left rotate by 4.
#define leftRotate4_64(a)  (leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64((a), 1), 1), 1), 1))

// Left rotate by 5: Rotate left by 8, then right by 3.
#define leftRotate5_64(a)  (rightRotate_64(rightRotate_64(rightRotate_64(leftRotate_64((a), 8), 1), 1), 1))

// Left rotate by 6: Rotate left by 8, then right by 2.
#define leftRotate6_64(a)  (rightRotate_64(rightRotate_64(leftRotate_64((a), 8), 1), 1))

// Left rotate by 7: Rotate left by 8, then right by 1.
#define leftRotate7_64(a)  (rightRotate_64(leftRotate_64((a), 8), 1))

// Left rotate by 8.
#define leftRotate8_64(a)  (leftRotate_64((a), 8))

// Left rotate by 9: Rotate left by 8, then left by 1.
#define leftRotate9_64(a)  (leftRotate_64(leftRotate_64((a), 8), 1))

// Left rotate by 10: Rotate left by 8, then left by 2.
#define leftRotate10_64(a) (leftRotate_64(leftRotate_64(leftRotate_64((a), 8), 1), 1))

// Left rotate by 11: Rotate left by 8, then left by 3.
#define leftRotate11_64(a) (leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64((a), 8), 1), 1), 1))

// Left rotate by 12: Rotate left by 16, then right by 4.
#define leftRotate12_64(a) (rightRotate_64(rightRotate_64(rightRotate_64(rightRotate_64(leftRotate_64((a), 16), 1), 1), 1), 1))

// Left rotate by 13: Rotate left by 16, then right by 3.
#define leftRotate13_64(a) (rightRotate_64(rightRotate_64(rightRotate_64(leftRotate_64((a), 16), 1), 1), 1))

// Left rotate by 14: Rotate left by 16, then right by 2.
#define leftRotate14_64(a) (rightRotate_64(rightRotate_64(leftRotate_64((a), 16), 1), 1))

// Left rotate by 15: Rotate left by 16, then right by 1.
#define leftRotate15_64(a) (rightRotate_64(leftRotate_64((a), 16), 1))

// Left rotate by 16.
#define leftRotate16_64(a) (leftRotate_64((a), 16))

// Left rotate by 17: Rotate left by 16, then left by 1.
#define leftRotate17_64(a) (leftRotate_64(leftRotate_64((a), 16), 1))

// Left rotate by 18: Rotate left by 16, then left by 2.
#define leftRotate18_64(a) (leftRotate_64(leftRotate_64(leftRotate_64((a), 16), 1), 1))

// Left rotate by 19: Rotate left by 16, then left by 3.
#define leftRotate19_64(a) (leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64((a), 16), 1), 1), 1))

// Left rotate by 20: Rotate left by 16, then left by 4.
#define leftRotate20_64(a) (leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64((a), 16), 1), 1), 1), 1))

// Left rotate by 21: Rotate left by 24, then right by 3.
#define leftRotate21_64(a) (rightRotate_64(rightRotate_64(rightRotate_64(leftRotate_64((a), 24), 1), 1), 1))

// Left rotate by 22: Rotate left by 24, then right by 2.
#define leftRotate22_64(a) (rightRotate_64(rightRotate_64(leftRotate_64((a), 24), 1), 1))

// Left rotate by 23: Rotate left by 24, then right by 1.
#define leftRotate23_64(a) (rightRotate_64(leftRotate_64((a), 24), 1))

// Left rotate by 24.
#define leftRotate24_64(a) (leftRotate_64((a), 24))

// Left rotate by 25: Rotate left by 24, then left by 1.
#define leftRotate25_64(a) (leftRotate_64(leftRotate_64((a), 24), 1))

// Left rotate by 26: Rotate left by 24, then left by 2.
#define leftRotate26_64(a) (leftRotate_64(leftRotate_64(leftRotate_64((a), 24), 1), 1))

// Left rotate by 27: Rotate left by 24, then left by 3.
#define leftRotate27_64(a) (leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64((a), 24), 1), 1), 1))

// Left rotate by 28: Rotate left by 24, then left by 4.
#define leftRotate28_64(a) (leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64((a), 24), 1), 1), 1), 1))

// Left rotate by 29: Rotate left by 32, then right by 3.
#define leftRotate29_64(a) (rightRotate_64(rightRotate_64(rightRotate_64(leftRotate_64((a), 32), 1), 1), 1))

// Left rotate by 30: Rotate left by 32, then right by 2.
#define leftRotate30_64(a) (rightRotate_64(rightRotate_64(leftRotate_64((a), 32), 1), 1))

// Left rotate by 31: Rotate left by 32, then right by 1.
#define leftRotate31_64(a) (rightRotate_64(leftRotate_64((a), 32), 1))

// Left rotate by 32.
#define leftRotate32_64(a) (leftRotate_64((a), 32))

// Left rotate by 33: Rotate left by 32, then left by 1.
#define leftRotate33_64(a) (leftRotate_64(leftRotate_64((a), 32), 1))

// Left rotate by 34: Rotate left by 32, then left by 2.
#define leftRotate34_64(a) (leftRotate_64(leftRotate_64(leftRotate_64((a), 32), 1), 1))

// Left rotate by 35: Rotate left by 32, then left by 3.
#define leftRotate35_64(a) (leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64((a), 32), 1), 1), 1))

// Left rotate by 36: Rotate left by 32, then left by 4.
#define leftRotate36_64(a) (leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64((a), 32), 1), 1), 1), 1))

// Left rotate by 37: Rotate left by 40, then right by 3.
#define leftRotate37_64(a) (rightRotate_64(rightRotate_64(rightRotate_64(leftRotate_64((a), 40), 1), 1), 1))

// Left rotate by 38: Rotate left by 40, then right by 2.
#define leftRotate38_64(a) (rightRotate_64(rightRotate_64(leftRotate_64((a), 40), 1), 1))

// Left rotate by 39: Rotate left by 40, then right by 1.
#define leftRotate39_64(a) (rightRotate_64(leftRotate_64((a), 40), 1))

// Left rotate by 40.
#define leftRotate40_64(a) (leftRotate_64((a), 40))

// Left rotate by 41: Rotate left by 40, then left by 1.
#define leftRotate41_64(a) (leftRotate_64(leftRotate_64((a), 40), 1))

// Left rotate by 42: Rotate left by 40, then left by 2.
#define leftRotate42_64(a) (leftRotate_64(leftRotate_64(leftRotate_64((a), 40), 1), 1))

// Left rotate by 43: Rotate left by 40, then left by 3.
#define leftRotate43_64(a) (leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64((a), 40), 1), 1), 1))

// Left rotate by 44: Rotate left by 40, then left by 4.
#define leftRotate44_64(a) (leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64((a), 40), 1), 1), 1), 1))

// Left rotate by 45: Rotate left by 48, then right by 3.
#define leftRotate45_64(a) (rightRotate_64(rightRotate_64(rightRotate_64(leftRotate_64((a), 48), 1), 1), 1))

// Left rotate by 46: Rotate left by 48, then right by 2.
#define leftRotate46_64(a) (rightRotate_64(rightRotate_64(leftRotate_64((a), 48), 1), 1))

// Left rotate by 47: Rotate left by 48, then right by 1.
#define leftRotate47_64(a) (rightRotate_64(leftRotate_64((a), 48), 1))

// Left rotate by 48.
#define leftRotate48_64(a) (leftRotate_64((a), 48))

// Left rotate by 49: Rotate left by 48, then left by 1.
#define leftRotate49_64(a) (leftRotate_64(leftRotate_64((a), 48), 1))

// Left rotate by 50: Rotate left by 48, then left by 2.
#define leftRotate50_64(a) (leftRotate_64(leftRotate_64(leftRotate_64((a), 48), 1), 1))

// Left rotate by 51: Rotate left by 48, then left by 3.
#define leftRotate51_64(a) (leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64((a), 48), 1), 1), 1))

// Left rotate by 52: Rotate left by 48, then left by 4.
#define leftRotate52_64(a) (leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64((a), 48), 1), 1), 1), 1))

// Left rotate by 53: Rotate left by 56, then right by 3.
#define leftRotate53_64(a) (rightRotate_64(rightRotate_64(rightRotate_64(leftRotate_64((a), 56), 1), 1), 1))

// Left rotate by 54: Rotate left by 56, then right by 2.
#define leftRotate54_64(a) (rightRotate_64(rightRotate_64(leftRotate_64((a), 56), 1), 1))

// Left rotate by 55: Rotate left by 56, then right by 1.
#define leftRotate55_64(a) (rightRotate_64(leftRotate_64((a), 56), 1))

// Left rotate by 56.
#define leftRotate56_64(a) (leftRotate_64((a), 56))

// Left rotate by 57: Rotate left by 56, then left by 1.
#define leftRotate57_64(a) (leftRotate_64(leftRotate_64((a), 56), 1))

// Left rotate by 58: Rotate left by 56, then left by 2.
#define leftRotate58_64(a) (leftRotate_64(leftRotate_64(leftRotate_64((a), 56), 1), 1))

// Left rotate by 59: Rotate left by 56, then left by 3.
#define leftRotate59_64(a) (leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64((a), 56), 1), 1), 1))

// Left rotate by 60: Rotate left by 60, then left by 4.
#define leftRotate60_64(a) (leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64(leftRotate_64((a), 56), 1), 1), 1), 1))

// Left rotate by 61: Rotate right by 3.
#define leftRotate61_64(a) (rightRotate_64(rightRotate_64(rightRotate_64((a), 1), 1), 1))

// Left rotate by 62: Rotate right by 2.
#define leftRotate62_64(a) (rightRotate_64(rightRotate_64((a), 1), 1))

// Left rotate by 63: Rotate right by 1.
#define leftRotate63_64(a) (rightRotate_64((a), 1))

// Define the 64-bit right rotations in terms of left rotations.
#define rightRotate1_64(a)  (leftRotate63_64((a)))
#define rightRotate2_64(a)  (leftRotate62_64((a)))
#define rightRotate3_64(a)  (leftRotate61_64((a)))
#define rightRotate4_64(a)  (leftRotate60_64((a)))
#define rightRotate5_64(a)  (leftRotate59_64((a)))
#define rightRotate6_64(a)  (leftRotate58_64((a)))
#define rightRotate7_64(a)  (leftRotate57_64((a)))
#define rightRotate8_64(a)  (leftRotate56_64((a)))
#define rightRotate9_64(a)  (leftRotate55_64((a)))
#define rightRotate10_64(a) (leftRotate54_64((a)))
#define rightRotate11_64(a) (leftRotate53_64((a)))
#define rightRotate12_64(a) (leftRotate52_64((a)))
#define rightRotate13_64(a) (leftRotate51_64((a)))
#define rightRotate14_64(a) (leftRotate50_64((a)))
#define rightRotate15_64(a) (leftRotate49_64((a)))
#define rightRotate16_64(a) (leftRotate48_64((a)))
#define rightRotate17_64(a) (leftRotate47_64((a)))
#define rightRotate18_64(a) (leftRotate46_64((a)))
#define rightRotate19_64(a) (leftRotate45_64((a)))
#define rightRotate20_64(a) (leftRotate44_64((a)))
#define rightRotate21_64(a) (leftRotate43_64((a)))
#define rightRotate22_64(a) (leftRotate42_64((a)))
#define rightRotate23_64(a) (leftRotate41_64((a)))
#define rightRotate24_64(a) (leftRotate40_64((a)))
#define rightRotate25_64(a) (leftRotate39_64((a)))
#define rightRotate26_64(a) (leftRotate38_64((a)))
#define rightRotate27_64(a) (leftRotate37_64((a)))
#define rightRotate28_64(a) (leftRotate36_64((a)))
#define rightRotate29_64(a) (leftRotate35_64((a)))
#define rightRotate30_64(a) (leftRotate34_64((a)))
#define rightRotate31_64(a) (leftRotate33_64((a)))
#define rightRotate32_64(a) (leftRotate32_64((a)))
#define rightRotate33_64(a) (leftRotate31_64((a)))
#define rightRotate34_64(a) (leftRotate30_64((a)))
#define rightRotate35_64(a) (leftRotate29_64((a)))
#define rightRotate36_64(a) (leftRotate28_64((a)))
#define rightRotate37_64(a) (leftRotate27_64((a)))
#define rightRotate38_64(a) (leftRotate26_64((a)))
#define rightRotate39_64(a) (leftRotate25_64((a)))
#define rightRotate40_64(a) (leftRotate24_64((a)))
#define rightRotate41_64(a) (leftRotate23_64((a)))
#define rightRotate42_64(a) (leftRotate22_64((a)))
#define rightRotate43_64(a) (leftRotate21_64((a)))
#define rightRotate44_64(a) (leftRotate20_64((a)))
#define rightRotate45_64(a) (leftRotate19_64((a)))
#define rightRotate46_64(a) (leftRotate18_64((a)))
#define rightRotate47_64(a) (leftRotate17_64((a)))
#define rightRotate48_64(a) (leftRotate16_64((a)))
#define rightRotate49_64(a) (leftRotate15_64((a)))
#define rightRotate50_64(a) (leftRotate14_64((a)))
#define rightRotate51_64(a) (leftRotate13_64((a)))
#define rightRotate52_64(a) (leftRotate12_64((a)))
#define rightRotate53_64(a) (leftRotate11_64((a)))
#define rightRotate54_64(a) (leftRotate10_64((a)))
#define rightRotate55_64(a) (leftRotate9_64((a)))
#define rightRotate56_64(a) (leftRotate8_64((a)))
#define rightRotate57_64(a) (leftRotate7_64((a)))
#define rightRotate58_64(a) (leftRotate6_64((a)))
#define rightRotate59_64(a) (leftRotate5_64((a)))
#define rightRotate60_64(a) (leftRotate4_64((a)))
#define rightRotate61_64(a) (leftRotate3_64((a)))
#define rightRotate62_64(a) (leftRotate2_64((a)))
#define rightRotate63_64(a) (leftRotate1_64((a)))

#else // !CRYPTO_ROTATE64_COMPOSED

// Rotation macros for 64-bit arguments.

// Generic left rotate.
#define leftRotate_64(a, bits) \
    (__extension__ ({ \
        uint64_t _temp = (a); \
        (_temp << (bits)) | (_temp >> (64 - (bits))); \
    }))

// Generic right rotate.
#define rightRotate_64(a, bits) \
    (__extension__ ({ \
        uint64_t _temp = (a); \
        (_temp >> (bits)) | (_temp << (64 - (bits))); \
    }))

// Left rotate by a specific number of bits.
#define leftRotate1_64(a)  (leftRotate_64((a), 1))
#define leftRotate2_64(a)  (leftRotate_64((a), 2))
#define leftRotate3_64(a)  (leftRotate_64((a), 3))
#define leftRotate4_64(a)  (leftRotate_64((a), 4))
#define leftRotate5_64(a)  (leftRotate_64((a), 5))
#define leftRotate6_64(a)  (leftRotate_64((a), 6))
#define leftRotate7_64(a)  (leftRotate_64((a), 7))
#define leftRotate8_64(a)  (leftRotate_64((a), 8))
#define leftRotate9_64(a)  (leftRotate_64((a), 9))
#define leftRotate10_64(a) (leftRotate_64((a), 10))
#define leftRotate11_64(a) (leftRotate_64((a), 11))
#define leftRotate12_64(a) (leftRotate_64((a), 12))
#define leftRotate13_64(a) (leftRotate_64((a), 13))
#define leftRotate14_64(a) (leftRotate_64((a), 14))
#define leftRotate15_64(a) (leftRotate_64((a), 15))
#define leftRotate16_64(a) (leftRotate_64((a), 16))
#define leftRotate17_64(a) (leftRotate_64((a), 17))
#define leftRotate18_64(a) (leftRotate_64((a), 18))
#define leftRotate19_64(a) (leftRotate_64((a), 19))
#define leftRotate20_64(a) (leftRotate_64((a), 20))
#define leftRotate21_64(a) (leftRotate_64((a), 21))
#define leftRotate22_64(a) (leftRotate_64((a), 22))
#define leftRotate23_64(a) (leftRotate_64((a), 23))
#define leftRotate24_64(a) (leftRotate_64((a), 24))
#define leftRotate25_64(a) (leftRotate_64((a), 25))
#define leftRotate26_64(a) (leftRotate_64((a), 26))
#define leftRotate27_64(a) (leftRotate_64((a), 27))
#define leftRotate28_64(a) (leftRotate_64((a), 28))
#define leftRotate29_64(a) (leftRotate_64((a), 29))
#define leftRotate30_64(a) (leftRotate_64((a), 30))
#define leftRotate31_64(a) (leftRotate_64((a), 31))
#define leftRotate32_64(a) (leftRotate_64((a), 32))
#define leftRotate33_64(a) (leftRotate_64((a), 33))
#define leftRotate34_64(a) (leftRotate_64((a), 34))
#define leftRotate35_64(a) (leftRotate_64((a), 35))
#define leftRotate36_64(a) (leftRotate_64((a), 36))
#define leftRotate37_64(a) (leftRotate_64((a), 37))
#define leftRotate38_64(a) (leftRotate_64((a), 38))
#define leftRotate39_64(a) (leftRotate_64((a), 39))
#define leftRotate40_64(a) (leftRotate_64((a), 40))
#define leftRotate41_64(a) (leftRotate_64((a), 41))
#define leftRotate42_64(a) (leftRotate_64((a), 42))
#define leftRotate43_64(a) (leftRotate_64((a), 43))
#define leftRotate44_64(a) (leftRotate_64((a), 44))
#define leftRotate45_64(a) (leftRotate_64((a), 45))
#define leftRotate46_64(a) (leftRotate_64((a), 46))
#define leftRotate47_64(a) (leftRotate_64((a), 47))
#define leftRotate48_64(a) (leftRotate_64((a), 48))
#define leftRotate49_64(a) (leftRotate_64((a), 49))
#define leftRotate50_64(a) (leftRotate_64((a), 50))
#define leftRotate51_64(a) (leftRotate_64((a), 51))
#define leftRotate52_64(a) (leftRotate_64((a), 52))
#define leftRotate53_64(a) (leftRotate_64((a), 53))
#define leftRotate54_64(a) (leftRotate_64((a), 54))
#define leftRotate55_64(a) (leftRotate_64((a), 55))
#define leftRotate56_64(a) (leftRotate_64((a), 56))
#define leftRotate57_64(a) (leftRotate_64((a), 57))
#define leftRotate58_64(a) (leftRotate_64((a), 58))
#define leftRotate59_64(a) (leftRotate_64((a), 59))
#define leftRotate60_64(a) (leftRotate_64((a), 60))
#define leftRotate61_64(a) (leftRotate_64((a), 61))
#define leftRotate62_64(a) (leftRotate_64((a), 62))
#define leftRotate63_64(a) (leftRotate_64((a), 63))

// Right rotate by a specific number of bits.
#define rightRotate1_64(a)  (rightRotate_64((a), 1))
#define rightRotate2_64(a)  (rightRotate_64((a), 2))
#define rightRotate3_64(a)  (rightRotate_64((a), 3))
#define rightRotate4_64(a)  (rightRotate_64((a), 4))
#define rightRotate5_64(a)  (rightRotate_64((a), 5))
#define rightRotate6_64(a)  (rightRotate_64((a), 6))
#define rightRotate7_64(a)  (rightRotate_64((a), 7))
#define rightRotate8_64(a)  (rightRotate_64((a), 8))
#define rightRotate9_64(a)  (rightRotate_64((a), 9))
#define rightRotate10_64(a) (rightRotate_64((a), 10))
#define rightRotate11_64(a) (rightRotate_64((a), 11))
#define rightRotate12_64(a) (rightRotate_64((a), 12))
#define rightRotate13_64(a) (rightRotate_64((a), 13))
#define rightRotate14_64(a) (rightRotate_64((a), 14))
#define rightRotate15_64(a) (rightRotate_64((a), 15))
#define rightRotate16_64(a) (rightRotate_64((a), 16))
#define rightRotate17_64(a) (rightRotate_64((a), 17))
#define rightRotate18_64(a) (rightRotate_64((a), 18))
#define rightRotate19_64(a) (rightRotate_64((a), 19))
#define rightRotate20_64(a) (rightRotate_64((a), 20))
#define rightRotate21_64(a) (rightRotate_64((a), 21))
#define rightRotate22_64(a) (rightRotate_64((a), 22))
#define rightRotate23_64(a) (rightRotate_64((a), 23))
#define rightRotate24_64(a) (rightRotate_64((a), 24))
#define rightRotate25_64(a) (rightRotate_64((a), 25))
#define rightRotate26_64(a) (rightRotate_64((a), 26))
#define rightRotate27_64(a) (rightRotate_64((a), 27))
#define rightRotate28_64(a) (rightRotate_64((a), 28))
#define rightRotate29_64(a) (rightRotate_64((a), 29))
#define rightRotate30_64(a) (rightRotate_64((a), 30))
#define rightRotate31_64(a) (rightRotate_64((a), 31))
#define rightRotate32_64(a) (rightRotate_64((a), 32))
#define rightRotate33_64(a) (rightRotate_64((a), 33))
#define rightRotate34_64(a) (rightRotate_64((a), 34))
#define rightRotate35_64(a) (rightRotate_64((a), 35))
#define rightRotate36_64(a) (rightRotate_64((a), 36))
#define rightRotate37_64(a) (rightRotate_64((a), 37))
#define rightRotate38_64(a) (rightRotate_64((a), 38))
#define rightRotate39_64(a) (rightRotate_64((a), 39))
#define rightRotate40_64(a) (rightRotate_64((a), 40))
#define rightRotate41_64(a) (rightRotate_64((a), 41))
#define rightRotate42_64(a) (rightRotate_64((a), 42))
#define rightRotate43_64(a) (rightRotate_64((a), 43))
#define rightRotate44_64(a) (rightRotate_64((a), 44))
#define rightRotate45_64(a) (rightRotate_64((a), 45))
#define rightRotate46_64(a) (rightRotate_64((a), 46))
#define rightRotate47_64(a) (rightRotate_64((a), 47))
#define rightRotate48_64(a) (rightRotate_64((a), 48))
#define rightRotate49_64(a) (rightRotate_64((a), 49))
#define rightRotate50_64(a) (rightRotate_64((a), 50))
#define rightRotate51_64(a) (rightRotate_64((a), 51))
#define rightRotate52_64(a) (rightRotate_64((a), 52))
#define rightRotate53_64(a) (rightRotate_64((a), 53))
#define rightRotate54_64(a) (rightRotate_64((a), 54))
#define rightRotate55_64(a) (rightRotate_64((a), 55))
#define rightRotate56_64(a) (rightRotate_64((a), 56))
#define rightRotate57_64(a) (rightRotate_64((a), 57))
#define rightRotate58_64(a) (rightRotate_64((a), 58))
#define rightRotate59_64(a) (rightRotate_64((a), 59))
#define rightRotate60_64(a) (rightRotate_64((a), 60))
#define rightRotate61_64(a) (rightRotate_64((a), 61))
#define rightRotate62_64(a) (rightRotate_64((a), 62))
#define rightRotate63_64(a) (rightRotate_64((a), 63))

#endif // !CRYPTO_ROTATE64_COMPOSED

#endif
