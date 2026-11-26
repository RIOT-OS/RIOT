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

#include "BigNumberUtil.h"
#include "utility/EndianUtil.h"
#include "utility/LimbUtil.h"
#include <string.h>

/**
 * \class BigNumberUtil BigNumberUtil.h <BigNumberUtil.h>
 * \brief Utilities to assist with implementing big number arithmetic.
 *
 * Big numbers are represented as arrays of limb_t words, which may be
 * 8 bits, 16 bits, or 32 bits in size depending upon how the library
 * was configured.  For AVR, 16 bit limbs usually give the best performance.
 *
 * Limb arrays are ordered from the least significant word to the most
 * significant.
 */

/**
 * \brief Unpacks the little-endian byte representation of a big number
 * into a limb array.
 *
 * \param limbs The limb array, starting with the least significant word.
 * \param count The number of elements in the \a limbs array.
 * \param bytes The bytes to unpack.
 * \param len The number of bytes to unpack.
 *
 * If \a len is shorter than the length of \a limbs, then the high bytes
 * will be filled with zeroes.  If \a len is longer than the length of
 * \a limbs, then the high bytes will be truncated and lost.
 *
 * \sa packLE(), unpackBE()
 */
void BigNumberUtil::unpackLE(limb_t *limbs, size_t count,
                             const uint8_t *bytes, size_t len)
{
#if BIGNUMBER_LIMB_8BIT
    if (len < count) {
        memcpy(limbs, bytes, len);
        memset(limbs + len, 0, count - len);
    } else {
        memcpy(limbs, bytes, count);
    }
#elif CRYPTO_LITTLE_ENDIAN
    count *= sizeof(limb_t);
    if (len < count) {
        memcpy(limbs, bytes, len);
        memset(((uint8_t *)limbs) + len, 0, count - len);
    } else {
        memcpy(limbs, bytes, count);
    }
#elif BIGNUMBER_LIMB_16BIT
    while (count > 0 && len >= 2) {
        *limbs++ = ((limb_t)(bytes[0])) |
                  (((limb_t)(bytes[1])) << 8);
        bytes += 2;
        --count;
        len -= 2;
    }
    if (count > 0 && len == 1) {
        *limbs++ = ((limb_t)(bytes[0]));
        --count;
    }
    while (count > 0) {
        *limbs++ = 0;
        --count;
    }
#elif BIGNUMBER_LIMB_32BIT
    while (count > 0 && len >= 4) {
        *limbs++ = ((limb_t)(bytes[0])) |
                  (((limb_t)(bytes[1])) <<  8) |
                  (((limb_t)(bytes[2])) << 16) |
                  (((limb_t)(bytes[3])) << 24);
        bytes += 4;
        --count;
        len -= 4;
    }
    if (count > 0 && len > 0) {
        if (len == 3) {
            *limbs++ = ((limb_t)(bytes[0])) |
                      (((limb_t)(bytes[1])) <<  8) |
                      (((limb_t)(bytes[2])) << 16);
        } else if (len == 2) {
            *limbs++ = ((limb_t)(bytes[0])) |
                      (((limb_t)(bytes[1])) <<  8);
        } else {
            *limbs++ = ((limb_t)(bytes[0]));
        }
        --count;
    }
    while (count > 0) {
        *limbs++ = 0;
        --count;
    }
#elif BIGNUMBER_LIMB_64BIT
    while (count > 0 && len >= 8) {
        *limbs++ = ((limb_t)(bytes[0])) |
                  (((limb_t)(bytes[1])) <<  8) |
                  (((limb_t)(bytes[2])) << 16) |
                  (((limb_t)(bytes[3])) << 24) |
                  (((limb_t)(bytes[4])) << 32) |
                  (((limb_t)(bytes[5])) << 40) |
                  (((limb_t)(bytes[6])) << 48) |
                  (((limb_t)(bytes[7])) << 56);
        bytes += 8;
        --count;
        len -= 8;
    }
    if (count > 0 && len > 0) {
        limb_t word = 0;
        uint8_t shift = 0;
        while (len > 0 && shift < 64) {
            word |= (((limb_t)(*bytes++)) << shift);
            shift += 8;
            --len;
        }
        *limbs++ = word;
        --count;
    }
    while (count > 0) {
        *limbs++ = 0;
        --count;
    }
#endif
}

/**
 * \brief Unpacks the big-endian byte representation of a big number
 * into a limb array.
 *
 * \param limbs The limb array, starting with the least significant word.
 * \param count The number of elements in the \a limbs array.
 * \param bytes The bytes to unpack.
 * \param len The number of bytes to unpack.
 *
 * If \a len is shorter than the length of \a limbs, then the high bytes
 * will be filled with zeroes.  If \a len is longer than the length of
 * \a limbs, then the high bytes will be truncated and lost.
 *
 * \sa packBE(), unpackLE()
 */
void BigNumberUtil::unpackBE(limb_t *limbs, size_t count,
                             const uint8_t *bytes, size_t len)
{
#if BIGNUMBER_LIMB_8BIT
    while (count > 0 && len > 0) {
        --count;
        --len;
        *limbs++ = bytes[len];
    }
    memset(limbs, 0, count);
#elif BIGNUMBER_LIMB_16BIT
    bytes += len;
    while (count > 0 && len >= 2) {
        --count;
        bytes -= 2;
        len -= 2;
        *limbs++ = ((limb_t)(bytes[1])) |
                  (((limb_t)(bytes[0])) << 8);
    }
    if (count > 0 && len == 1) {
        --count;
        --bytes;
        *limbs++ = (limb_t)(bytes[0]);
    }
    memset(limbs, 0, count * sizeof(limb_t));
#elif BIGNUMBER_LIMB_32BIT
    bytes += len;
    while (count > 0 && len >= 4) {
        --count;
        bytes -= 4;
        len -= 4;
        *limbs++ = ((limb_t)(bytes[3])) |
                  (((limb_t)(bytes[2])) << 8) |
                  (((limb_t)(bytes[1])) << 16) |
                  (((limb_t)(bytes[0])) << 24);
    }
    if (count > 0) {
        if (len == 3) {
            --count;
            bytes -= 3;
            *limbs++ = ((limb_t)(bytes[2])) |
                      (((limb_t)(bytes[1])) << 8) |
                      (((limb_t)(bytes[0])) << 16);
        } else if (len == 2) {
            --count;
            bytes -= 2;
            *limbs++ = ((limb_t)(bytes[1])) |
                      (((limb_t)(bytes[0])) << 8);
        } else if (len == 1) {
            --count;
            --bytes;
            *limbs++ = (limb_t)(bytes[0]);
        }
    }
    memset(limbs, 0, count * sizeof(limb_t));
#elif BIGNUMBER_LIMB_64BIT
    bytes += len;
    while (count > 0 && len >= 8) {
        --count;
        bytes -= 8;
        len -= 8;
        *limbs++ = ((limb_t)(bytes[7])) |
                  (((limb_t)(bytes[6])) << 8) |
                  (((limb_t)(bytes[5])) << 16) |
                  (((limb_t)(bytes[4])) << 24) |
                  (((limb_t)(bytes[3])) << 32) |
                  (((limb_t)(bytes[2])) << 40) |
                  (((limb_t)(bytes[1])) << 48) |
                  (((limb_t)(bytes[0])) << 56);
    }
    if (count > 0 && len > 0) {
        limb_t word = 0;
        uint8_t shift = 0;
        while (len > 0 && shift < 64) {
            word |= (((limb_t)(*(--bytes))) << shift);
            shift += 8;
            --len;
        }
        *limbs++ = word;
        --count;
    }
    memset(limbs, 0, count * sizeof(limb_t));
#endif
}

/**
 * \brief Packs the little-endian byte representation of a big number
 * into a byte array.
 *
 * \param bytes The byte array to pack into.
 * \param len The number of bytes in the destination \a bytes array.
 * \param limbs The limb array representing the big number, starting with
 * the least significant word.
 * \param count The number of elements in the \a limbs array.
 *
 * If \a len is shorter than the length of \a limbs, then the number will
 * be truncated to the least significant \a len bytes.  If \a len is longer
 * than the length of \a limbs, then the high bytes will be filled with zeroes.
 *
 * \sa unpackLE(), packBE()
 */
void BigNumberUtil::packLE(uint8_t *bytes, size_t len,
                           const limb_t *limbs, size_t count)
{
#if BIGNUMBER_LIMB_8BIT
    if (len <= count) {
        memcpy(bytes, limbs, len);
    } else {
        memcpy(bytes, limbs, count);
        memset(bytes + count, 0, len - count);
    }
#elif CRYPTO_LITTLE_ENDIAN
    count *= sizeof(limb_t);
    if (len <= count) {
        memcpy(bytes, limbs, len);
    } else {
        memcpy(bytes, limbs, count);
        memset(bytes + count, 0, len - count);
    }
#elif BIGNUMBER_LIMB_16BIT
    limb_t word;
    while (count > 0 && len >= 2) {
        word = *limbs++;
        bytes[0] = (uint8_t)word;
        bytes[1] = (uint8_t)(word >> 8);
        --count;
        len -= 2;
        bytes += 2;
    }
    if (count > 0 && len == 1) {
        bytes[0] = (uint8_t)(*limbs);
        --len;
        ++bytes;
    }
    memset(bytes, 0, len);
#elif BIGNUMBER_LIMB_32BIT
    limb_t word;
    while (count > 0 && len >= 4) {
        word = *limbs++;
        bytes[0] = (uint8_t)word;
        bytes[1] = (uint8_t)(word >> 8);
        bytes[2] = (uint8_t)(word >> 16);
        bytes[3] = (uint8_t)(word >> 24);
        --count;
        len -= 4;
        bytes += 4;
    }
    if (count > 0) {
        if (len == 3) {
            word = *limbs;
            bytes[0] = (uint8_t)word;
            bytes[1] = (uint8_t)(word >> 8);
            bytes[2] = (uint8_t)(word >> 16);
            len -= 3;
            bytes += 3;
        } else if (len == 2) {
            word = *limbs;
            bytes[0] = (uint8_t)word;
            bytes[1] = (uint8_t)(word >> 8);
            len -= 2;
            bytes += 2;
        } else if (len == 1) {
            bytes[0] = (uint8_t)(*limbs);
            --len;
            ++bytes;
        }
    }
    memset(bytes, 0, len);
#elif BIGNUMBER_LIMB_64BIT
    limb_t word;
    while (count > 0 && len >= 8) {
        word = *limbs++;
        bytes[0] = (uint8_t)word;
        bytes[1] = (uint8_t)(word >> 8);
        bytes[2] = (uint8_t)(word >> 16);
        bytes[3] = (uint8_t)(word >> 24);
        bytes[4] = (uint8_t)(word >> 32);
        bytes[5] = (uint8_t)(word >> 40);
        bytes[6] = (uint8_t)(word >> 48);
        bytes[7] = (uint8_t)(word >> 56);
        --count;
        len -= 8;
        bytes += 8;
    }
    if (count > 0) {
        word = *limbs;
        while (len > 0) {
            *bytes++ = (uint8_t)word;
            word >>= 8;
            --len;
        }
    }
    memset(bytes, 0, len);
#endif
}

/**
 * \brief Packs the big-endian byte representation of a big number
 * into a byte array.
 *
 * \param bytes The byte array to pack into.
 * \param len The number of bytes in the destination \a bytes array.
 * \param limbs The limb array representing the big number, starting with
 * the least significant word.
 * \param count The number of elements in the \a limbs array.
 *
 * If \a len is shorter than the length of \a limbs, then the number will
 * be truncated to the least significant \a len bytes.  If \a len is longer
 * than the length of \a limbs, then the high bytes will be filled with zeroes.
 *
 * \sa unpackLE(), packBE()
 */
void BigNumberUtil::packBE(uint8_t *bytes, size_t len,
                           const limb_t *limbs, size_t count)
{
#if BIGNUMBER_LIMB_8BIT
    if (len > count) {
        size_t size = len - count;
        memset(bytes, 0, size);
        len -= size;
        bytes += size;
    } else if (len < count) {
        count = len;
    }
    limbs += count;
    while (count > 0) {
        --count;
        *bytes++ = *(--limbs);
    }
#elif BIGNUMBER_LIMB_16BIT
    size_t countBytes = count * sizeof(limb_t);
    limb_t word;
    if (len >= countBytes) {
        size_t size = len - countBytes;
        memset(bytes, 0, size);
        len -= size;
        bytes += size;
        limbs += count;
    } else {
        count = len / sizeof(limb_t);
        limbs += count;
        if ((len & 1) != 0)
            *bytes++ = (uint8_t)(*limbs);
    }
    while (count > 0) {
        --count;
        word = *(--limbs);
        *bytes++ = (uint8_t)(word >> 8);
        *bytes++ = (uint8_t)word;
    }
#elif BIGNUMBER_LIMB_32BIT
    size_t countBytes = count * sizeof(limb_t);
    limb_t word;
    if (len >= countBytes) {
        size_t size = len - countBytes;
        memset(bytes, 0, size);
        len -= size;
        bytes += size;
        limbs += count;
    } else {
        count = len / sizeof(limb_t);
        limbs += count;
        if ((len & 3) == 3) {
            word = *limbs;
            *bytes++ = (uint8_t)(word >> 16);
            *bytes++ = (uint8_t)(word >> 8);
            *bytes++ = (uint8_t)word;
        } else if ((len & 3) == 2) {
            word = *limbs;
            *bytes++ = (uint8_t)(word >> 8);
            *bytes++ = (uint8_t)word;
        } else if ((len & 3) == 1) {
            *bytes++ = (uint8_t)(*limbs);
        }
    }
    while (count > 0) {
        --count;
        word = *(--limbs);
        *bytes++ = (uint8_t)(word >> 24);
        *bytes++ = (uint8_t)(word >> 16);
        *bytes++ = (uint8_t)(word >> 8);
        *bytes++ = (uint8_t)word;
    }
#elif BIGNUMBER_LIMB_64BIT
    size_t countBytes = count * sizeof(limb_t);
    limb_t word;
    if (len >= countBytes) {
        size_t size = len - countBytes;
        memset(bytes, 0, size);
        len -= size;
        bytes += size;
        limbs += count;
    } else {
        count = len / sizeof(limb_t);
        limbs += count;
        uint8_t size = len & 7;
        uint8_t shift = size * 8;
        word = *limbs;
        while (size > 0) {
            shift -= 8;
            *bytes++ = (uint8_t)(word >> shift);
            --size;
        }
    }
    while (count > 0) {
        --count;
        word = *(--limbs);
        *bytes++ = (uint8_t)(word >> 56);
        *bytes++ = (uint8_t)(word >> 48);
        *bytes++ = (uint8_t)(word >> 40);
        *bytes++ = (uint8_t)(word >> 32);
        *bytes++ = (uint8_t)(word >> 24);
        *bytes++ = (uint8_t)(word >> 16);
        *bytes++ = (uint8_t)(word >> 8);
        *bytes++ = (uint8_t)word;
    }
#endif
}

/**
 * \brief Adds two big numbers.
 *
 * \param result The result of the addition.  This can be the same
 * as either \a x or \a y.
 * \param x The first big number.
 * \param y The second big number.
 * \param size The size of the values in limbs.
 *
 * \return Returns 1 if there was a carry out or 0 if there was no carry out.
 *
 * \sa sub(), mul()
 */
limb_t BigNumberUtil::add(limb_t *result, const limb_t *x,
                          const limb_t *y, size_t size)
{
    dlimb_t carry = 0;
    while (size > 0) {
        carry += *x++;
        carry += *y++;
        *result++ = (limb_t)carry;
        carry >>= LIMB_BITS;
        --size;
    }
    return (limb_t)carry;
}

/**
 * \brief Subtracts one big number from another.
 *
 * \param result The result of the subtraction.  This can be the same
 * as either \a x or \a y.
 * \param x The first big number.
 * \param y The second big number to subtract from \a x.
 * \param size The size of the values in limbs.
 *
 * \return Returns 1 if there was a borrow, or 0 if there was no borrow.
 *
 * \sa add(), mul()
 */
limb_t BigNumberUtil::sub(limb_t *result, const limb_t *x,
                          const limb_t *y, size_t size)
{
    dlimb_t borrow = 0;
    while (size > 0) {
        borrow = ((dlimb_t)(*x++)) - (*y++) - ((borrow >> LIMB_BITS) & 0x01);
        *result++ = (limb_t)borrow;
        --size;
    }
    return ((limb_t)(borrow >> LIMB_BITS)) & 0x01;
}

/**
 * \brief Multiplies two big numbers.
 *
 * \param result The result of the multiplication.  The array must be
 * \a xcount + \a ycount limbs in size.
 * \param x Points to the first value to multiply.
 * \param xcount The number of limbs in \a x.
 * \param y Points to the second value to multiply.
 * \param ycount The number of limbs in \a y.
 *
 * \sa mul_P()
 */
void BigNumberUtil::mul(limb_t *result, const limb_t *x, size_t xcount,
                        const limb_t *y, size_t ycount)
{
    size_t i, j;
    dlimb_t carry;
    limb_t word;
    const limb_t *xx;
    limb_t *rr;

    // Multiply the lowest limb of y by x.
    carry = 0;
    word = y[0];
    xx = x;
    rr = result;
    for (i = 0; i < xcount; ++i) {
        carry += ((dlimb_t)(*xx++)) * word;
        *rr++ = (limb_t)carry;
        carry >>= LIMB_BITS;
    }
    *rr = (limb_t)carry;

    // Multiply and add the remaining limbs of y by x.
    for (i = 1; i < ycount; ++i) {
        word = y[i];
        carry = 0;
        xx = x;
        rr = result + i;
        for (j = 0; j < xcount; ++j) {
            carry += ((dlimb_t)(*xx++)) * word;
            carry += *rr;
            *rr++ = (limb_t)carry;
            carry >>= LIMB_BITS;
        }
        *rr = (limb_t)carry;
    }
}

/**
 * \brief Reduces \a x modulo \a y using subtraction.
 *
 * \param result The result of the reduction.  This can be the
 * same as \a x.
 * \param x The number to be reduced.
 * \param y The base to use for the modulo reduction.
 * \param size The size of the values in limbs.
 *
 * It is assumed that \a x is less than \a y * 2 so that a single
 * conditional subtraction will bring it down below \a y.  The reduction
 * is performed in constant time.
 *
 * \sa reduceQuick_P()
 */
void BigNumberUtil::reduceQuick(limb_t *result, const limb_t *x,
                                const limb_t *y, size_t size)
{
    // Subtract "y" from "x" and turn the borrow into an AND mask.
    limb_t mask = sub(result, x, y, size);
    mask = (~mask) + 1;

    // Add "y" back to the result if the mask is non-zero.
    dlimb_t carry = 0;
    while (size > 0) {
        carry += *result;
        carry += (*y++ & mask);
        *result++ = (limb_t)carry;
        carry >>= LIMB_BITS;
        --size;
    }
}

/**
 * \brief Adds two big numbers where one of them is in program memory.
 *
 * \param result The result of the addition.  This can be the same as \a x.
 * \param x The first big number.
 * \param y The second big number.  This must point into program memory.
 * \param size The size of the values in limbs.
 *
 * \return Returns 1 if there was a carry out or 0 if there was no carry out.
 *
 * \sa sub_P(), mul_P()
 */
limb_t BigNumberUtil::add_P(limb_t *result, const limb_t *x,
                            const limb_t *y, size_t size)
{
    dlimb_t carry = 0;
    while (size > 0) {
        carry += *x++;
        carry += pgm_read_limb(y++);
        *result++ = (limb_t)carry;
        carry >>= LIMB_BITS;
        --size;
    }
    return (limb_t)carry;
}

/**
 * \brief Subtracts one big number from another where one is in program memory.
 *
 * \param result The result of the subtraction.  This can be the same as \a x.
 * \param x The first big number.
 * \param y The second big number to subtract from \a x.  This must point
 * into program memory.
 * \param size The size of the values in limbs.
 *
 * \return Returns 1 if there was a borrow, or 0 if there was no borrow.
 *
 * \sa add_P(), mul_P()
 */
limb_t BigNumberUtil::sub_P(limb_t *result, const limb_t *x,
                            const limb_t *y, size_t size)
{
    dlimb_t borrow = 0;
    while (size > 0) {
        borrow = ((dlimb_t)(*x++)) - pgm_read_limb(y++) - ((borrow >> LIMB_BITS) & 0x01);
        *result++ = (limb_t)borrow;
        --size;
    }
    return ((limb_t)(borrow >> LIMB_BITS)) & 0x01;
}

/**
 * \brief Multiplies two big numbers where one is in program memory.
 *
 * \param result The result of the multiplication.  The array must be
 * \a xcount + \a ycount limbs in size.
 * \param x Points to the first value to multiply.
 * \param xcount The number of limbs in \a x.
 * \param y Points to the second value to multiply.  This must point
 * into program memory.
 * \param ycount The number of limbs in \a y.
 *
 * \sa mul()
 */
void BigNumberUtil::mul_P(limb_t *result, const limb_t *x, size_t xcount,
                          const limb_t *y, size_t ycount)
{
    size_t i, j;
    dlimb_t carry;
    limb_t word;
    const limb_t *xx;
    limb_t *rr;

    // Multiply the lowest limb of y by x.
    carry = 0;
    word = pgm_read_limb(&(y[0]));
    xx = x;
    rr = result;
    for (i = 0; i < xcount; ++i) {
        carry += ((dlimb_t)(*xx++)) * word;
        *rr++ = (limb_t)carry;
        carry >>= LIMB_BITS;
    }
    *rr = (limb_t)carry;

    // Multiply and add the remaining limb of y by x.
    for (i = 1; i < ycount; ++i) {
        word = pgm_read_limb(&(y[i]));
        carry = 0;
        xx = x;
        rr = result + i;
        for (j = 0; j < xcount; ++j) {
            carry += ((dlimb_t)(*xx++)) * word;
            carry += *rr;
            *rr++ = (limb_t)carry;
            carry >>= LIMB_BITS;
        }
        *rr = (limb_t)carry;
    }
}

/**
 * \brief Reduces \a x modulo \a y using subtraction where \a y is
 * in program memory.
 *
 * \param result The result of the reduction.  This can be the
 * same as \a x.
 * \param x The number to be reduced.
 * \param y The base to use for the modulo reduction.  This must point
 * into program memory.
 * \param size The size of the values in limbs.
 *
 * It is assumed that \a x is less than \a y * 2 so that a single
 * conditional subtraction will bring it down below \a y.  The reduction
 * is performed in constant time.
 *
 * \sa reduceQuick()
 */
void BigNumberUtil::reduceQuick_P(limb_t *result, const limb_t *x,
                                  const limb_t *y, size_t size)
{
    // Subtract "y" from "x" and turn the borrow into an AND mask.
    limb_t mask = sub_P(result, x, y, size);
    mask = (~mask) + 1;

    // Add "y" back to the result if the mask is non-zero.
    dlimb_t carry = 0;
    while (size > 0) {
        carry += *result;
        carry += (pgm_read_limb(y++) & mask);
        *result++ = (limb_t)carry;
        carry >>= LIMB_BITS;
        --size;
    }
}

/**
 * \brief Determine if a big number is zero.
 *
 * \param x Points to the number to test.
 * \param size The number of limbs in \a x.
 * \return Returns 1 if \a x is zero or 0 otherwise.
 *
 * This function attempts to make the determination in constant time.
 */
limb_t BigNumberUtil::isZero(const limb_t *x, size_t size)
{
    limb_t word = 0;
    while (size > 0) {
        word |= *x++;
        --size;
    }
    return (limb_t)(((((dlimb_t)1) << LIMB_BITS) - word) >> LIMB_BITS);
}
