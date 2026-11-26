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

#include "Curve25519.h"
#include "Crypto.h"
#include "RNG.h"
#include "utility/LimbUtil.h"
#include <string.h>

/**
 * \class Curve25519 Curve25519.h <Curve25519.h>
 * \brief Diffie-Hellman key agreement based on the elliptic curve
 * modulo 2^255 - 19.
 *
 * \note The public functions in this class need a substantial amount of
 * stack space to store intermediate results while the curve function is
 * being evaluated.  About 1k of free stack space is recommended for safety.
 *
 * References: http://cr.yp.to/ecdh.html,
 * <a href="http://tools.ietf.org/html/rfc7748">RFC 7748</a>
 *
 * \sa Ed25519
 */

// Global switch to enable/disable AVR inline assembly optimizations.
#if defined(__AVR__)
// Disabled for now - there are issues with newer Arduino compilers.  FIXME
//#define CURVE25519_ASM_AVR 1
#endif

// The overhead of clean() calls in mul(), reduceQuick(), etc can
// add up to a lot of processing time during eval().  Only do such
// cleanups if strict mode has been enabled.  Other implementations
// like curve25519-donna don't do any cleaning at all so the value
// of cleaning up the stack is dubious at best anyway.
#if defined(CURVE25519_STRICT_CLEAN)
#define strict_clean(x)     clean(x)
#else
#define strict_clean(x)     do { ; } while (0)
#endif

/**
 * \brief Evaluates the raw Curve25519 function.
 *
 * \param result The result of evaluating the curve function.
 * \param s The S parameter to the curve function.
 * \param x The X(Q) parameter to the curve function.  If this pointer is
 * NULL then the value 9 is used for \a x.
 *
 * This function is provided to assist with implementating other
 * algorithms with the curve.  Normally applications should use dh1()
 * and dh2() directly instead.
 *
 * \return Returns true if the function was evaluated; false if \a x is
 * not a proper member of the field modulo (2^255 - 19).
 *
 * Reference: <a href="http://tools.ietf.org/html/rfc7748">RFC 7748</a>
 *
 * \sa dh1(), dh2()
 */
bool Curve25519::eval(uint8_t result[32], const uint8_t s[32], const uint8_t x[32])
{
    limb_t x_1[NUM_LIMBS_256BIT];
    limb_t x_2[NUM_LIMBS_256BIT];
    limb_t x_3[NUM_LIMBS_256BIT];
    limb_t z_2[NUM_LIMBS_256BIT];
    limb_t z_3[NUM_LIMBS_256BIT];
    limb_t A[NUM_LIMBS_256BIT];
    limb_t B[NUM_LIMBS_256BIT];
    limb_t C[NUM_LIMBS_256BIT];
    limb_t D[NUM_LIMBS_256BIT];
    limb_t E[NUM_LIMBS_256BIT];
    limb_t AA[NUM_LIMBS_256BIT];
    limb_t BB[NUM_LIMBS_256BIT];
    limb_t DA[NUM_LIMBS_256BIT];
    limb_t CB[NUM_LIMBS_256BIT];
    uint8_t mask;
    uint8_t sposn;
    uint8_t select;
    uint8_t swap;
    bool retval;

    // Unpack the "x" argument into the limb representation
    // which also masks off the high bit.  NULL means 9.
    if (x) {
        // x1 = x
        BigNumberUtil::unpackLE(x_1, NUM_LIMBS_256BIT, x, 32);
        x_1[NUM_LIMBS_256BIT - 1] &= ((((limb_t)1) << (LIMB_BITS - 1)) - 1);
    } else {
        memset(x_1, 0, sizeof(x_1));    // x_1 = 9
        x_1[0] = 9;
    }

    // Check that "x" is within the range of the modulo field.
    // We can do this with a reduction - if there was no borrow
    // then the value of "x" was out of range.  Timing is sensitive
    // here so that we don't reveal anything about the value of "x".
    // If there was a reduction, then continue executing the rest
    // of this function with the (now) in-range "x" value and
    // report the failure at the end.
    retval = (bool)(reduceQuick(x_1) & 0x01);

    // Initialize the other temporary variables.
    memset(x_2, 0, sizeof(x_2));        // x_2 = 1
    x_2[0] = 1;
    memset(z_2, 0, sizeof(z_2));        // z_2 = 0
    memcpy(x_3, x_1, sizeof(x_1));      // x_3 = x
    memcpy(z_3, x_2, sizeof(x_2));      // z_3 = 1

    // Iterate over all 255 bits of "s" from the highest to the lowest.
    // We ignore the high bit of the 256-bit representation of "s".
    mask = 0x40;
    sposn = 31;
    swap = 0;
    for (uint8_t t = 255; t > 0; --t) {
        // Conditional swaps on entry to this bit but only if we
        // didn't swap on the previous bit.
        select = s[sposn] & mask;
        swap ^= select;
        cswap(swap, x_2, x_3);
        cswap(swap, z_2, z_3);

        // Evaluate the curve.
        add(A, x_2, z_2);               // A = x_2 + z_2
        square(AA, A);                  // AA = A^2
        sub(B, x_2, z_2);               // B = x_2 - z_2
        square(BB, B);                  // BB = B^2
        sub(E, AA, BB);                 // E = AA - BB
        add(C, x_3, z_3);               // C = x_3 + z_3
        sub(D, x_3, z_3);               // D = x_3 - z_3
        mul(DA, D, A);                  // DA = D * A
        mul(CB, C, B);                  // CB = C * B
        add(x_3, DA, CB);               // x_3 = (DA + CB)^2
        square(x_3, x_3);
        sub(z_3, DA, CB);               // z_3 = x_1 * (DA - CB)^2
        square(z_3, z_3);
        mul(z_3, z_3, x_1);
        mul(x_2, AA, BB);               // x_2 = AA * BB
        mulA24(z_2, E);                 // z_2 = E * (AA + a24 * E)
        add(z_2, z_2, AA);
        mul(z_2, z_2, E);

        // Move onto the next lower bit of "s".
        mask >>= 1;
        if (!mask) {
            --sposn;
            mask = 0x80;
            swap = select << 7;
        } else {
            swap = select >> 1;
        }
    }

    // Final conditional swaps.
    cswap(swap, x_2, x_3);
    cswap(swap, z_2, z_3);

    // Compute x_2 * (z_2 ^ (p - 2)) where p = 2^255 - 19.
    recip(z_3, z_2);
    mul(x_2, x_2, z_3);

    // Pack the result into the return array.
    BigNumberUtil::packLE(result, 32, x_2, NUM_LIMBS_256BIT);

    // Clean up and exit.
    clean(x_1);
    clean(x_2);
    clean(x_3);
    clean(z_2);
    clean(z_3);
    clean(A);
    clean(B);
    clean(C);
    clean(D);
    clean(E);
    clean(AA);
    clean(BB);
    clean(DA);
    clean(CB);
    return retval;
}

/**
 * \brief Performs phase 1 of a Diffie-Hellman key exchange using Curve25519.
 *
 * \param k The key value to send to the other party as part of the exchange.
 * \param f The generated secret value for this party.  This must not be
 * transmitted to any party or stored in permanent storage.  It only needs
 * to be kept in memory until dh2() is called.
 *
 * The \a f value is generated with \link RNGClass::rand() CryptRNG.rand()\endlink.
 * It is the caller's responsibility to ensure that the global random number
 * pool has sufficient entropy to generate the 32 bytes of \a f safely
 * before calling this function.
 *
 * The following example demonstrates how to perform a full Diffie-Hellman
 * key exchange using dh1() and dh2():
 *
 * \code
 * uint8_t f[32];
 * uint8_t k[32];
 *
 * // Generate the secret value "f" and the public value "k".
 * Curve25519::dh1(k, f);
 *
 * // Send "k" to the other party.
 * ...
 *
 * // Read the "k" value that the other party sent to us.
 * ...
 *
 * // Generate the shared secret in "k" using the previous secret value "f".
 * if (!Curve25519::dh2(k, f)) {
 *     // The received "k" value was invalid - abort the session.
 *     ...
 * }
 *
 * // The "k" value can now be used to generate session keys for encryption.
 * ...
 * \endcode
 *
 * Reference: <a href="http://tools.ietf.org/html/rfc7748">RFC 7748</a>
 *
 * \sa dh2()
 */
void Curve25519::dh1(uint8_t k[32], uint8_t f[32])
{
    do {
        // Generate a random "f" value and then adjust the value to make
        // it valid as an "s" value for eval().  According to the specification
        // we need to mask off the 3 right-most bits of f[0], mask off the
        // left-most bit of f[31], and set the second to left-most bit of f[31].
        CryptRNG.rand(f, 32);
        f[0] &= 0xF8;
        f[31] = (f[31] & 0x7F) | 0x40;

        // Evaluate the curve function: k = Curve25519::eval(f, 9).
        // We pass NULL to eval() to indicate the value 9.  There is no
        // need to check the return value from eval() because we know
        // that 9 is a valid field element.
        eval(k, f, 0);

        // If "k" is weak for contributory behaviour then reject it,
        // generate another "f" value, and try again.  This case is
        // highly unlikely but we still perform the check just in case.
    } while (isWeakPoint(k));
}

/**
 * \brief Performs phase 2 of a Diffie-Hellman key exchange using Curve25519.
 *
 * \param k On entry, this is the key value that was received from the other
 * party as part of the exchange.  On exit, this will be the shared secret.
 * \param f The secret value for this party that was generated by dh1().
 * The \a f value will be destroyed by this function.
 *
 * \return Returns true if the key exchange was successful, or false if
 * the \a k value is invalid.
 *
 * Reference: <a href="http://tools.ietf.org/html/rfc7748">RFC 7748</a>
 *
 * \sa dh1()
 */
bool Curve25519::dh2(uint8_t k[32], uint8_t f[32])
{
    uint8_t weak;

    // Evaluate the curve function: k = Curve25519::eval(f, k).
    // If "k" is weak for contributory behaviour before or after
    // the curve evaluation, then fail the exchange.  For safety
    // we perform every phase of the weak checks even if we could
    // bail out earlier so that the execution takes the same
    // amount of time for weak and non-weak "k" values.
    weak  = isWeakPoint(k);                     // Is "k" weak before?
    weak |= ((eval(k, f, k) ^ 0x01) & 0x01);    // Is "k" weak during?
    weak |= isWeakPoint(k);                     // Is "k" weak after?
    clean(f, 32);
    return (bool)((weak ^ 0x01) & 0x01);
}

/**
 * \brief Determines if a Curve25519 point is weak for contributory behaviour.
 *
 * \param k The point to check.
 * \return Returns 1 if \a k is weak for contributory behavior or
 * returns zero if \a k is not weak.
 */
uint8_t Curve25519::isWeakPoint(const uint8_t k[32])
{
    // List of weak points from http://cr.yp.to/ecdh.html
    // That page lists some others but they are variants on these
    // of the form "point + i * (2^255 - 19)" for i = 0, 1, 2.
    // Here we mask off the high bit and eval() catches the rest.
    static const uint8_t points[5][32] PROGMEM = {
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0xE0, 0xEB, 0x7A, 0x7C, 0x3B, 0x41, 0xB8, 0xAE,
         0x16, 0x56, 0xE3, 0xFA, 0xF1, 0x9F, 0xC4, 0x6A,
         0xDA, 0x09, 0x8D, 0xEB, 0x9C, 0x32, 0xB1, 0xFD,
         0x86, 0x62, 0x05, 0x16, 0x5F, 0x49, 0xB8, 0x00},
        {0x5F, 0x9C, 0x95, 0xBC, 0xA3, 0x50, 0x8C, 0x24,
         0xB1, 0xD0, 0xB1, 0x55, 0x9C, 0x83, 0xEF, 0x5B,
         0x04, 0x44, 0x5C, 0xC4, 0x58, 0x1C, 0x8E, 0x86,
         0xD8, 0x22, 0x4E, 0xDD, 0xD0, 0x9F, 0x11, 0x57},
        {0xEC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F}
    };

    // Check each of the weak points in turn.  We perform the
    // comparisons carefully so as not to reveal the value of "k"
    // in the instruction timing.  If "k" is indeed weak then
    // we still check everything so as not to reveal which
    // weak point it is.
    uint8_t result = 0;
    for (uint8_t posn = 0; posn < 5; ++posn) {
        const uint8_t *point = points[posn];
        uint8_t check = (pgm_read_byte(&(point[31])) ^ k[31]) & 0x7F;
        for (uint8_t index = 31; index > 0; --index)
            check |= (pgm_read_byte(&(point[index - 1])) ^ k[index - 1]);
        result |= (uint8_t)((((uint16_t)0x0100) - check) >> 8);
    }

    // The "result" variable will be non-zero if there was a match.
    return result;
}

/**
 * \brief Reduces a number modulo 2^255 - 19.
 *
 * \param result The array that will contain the result when the
 * function exits.  Must be NUM_LIMBS_256BIT limbs in size.
 * \param x The number to be reduced, which must be NUM_LIMBS_512BIT
 * limbs in size and less than or equal to square(2^255 - 19 - 1).
 * This array will be modified by the reduction process.
 * \param size The size of the high order half of \a x.  This indicates
 * the size of \a x in limbs.  If it is shorter than NUM_LIMBS_256BIT
 * then the reduction can be performed quicker.
 */
void Curve25519::reduce(limb_t *result, limb_t *x, uint8_t size)
{
    /*
    Note: This explaination is best viewed with a UTF-8 text viewer.

    To help explain what this function is doing, the following describes
    how to efficiently compute reductions modulo a base of the form (2ⁿ - b)
    where b is greater than zero and (b + 1)² <= 2ⁿ.

    Here we are interested in reducing the result of multiplying two
    numbers that are less than or equal to (2ⁿ - b - 1).  That is,
    multiplying numbers that have already been reduced.

    Given some x less than or equal to (2ⁿ - b - 1)², we want to find a
    y less than (2ⁿ - b) such that:

        y ≡ x mod (2ⁿ - b)

    We know that for all integer values of k >= 0:

        y ≡ x - k * (2ⁿ - b)
          ≡ x - k * 2ⁿ + k * b

    In our case we choose k = ⌊x / 2ⁿ⌋ and then let:

        w = (x mod 2ⁿ) + ⌊x / 2ⁿ⌋ * b

    The value w will either be the answer y or y can be obtained by
    repeatedly subtracting (2ⁿ - b) from w until it is less than (2ⁿ - b).
    At most b subtractions will be required.

    In our case b is 19 which is more subtractions than we would like to do,
    but we can handle that by performing the above reduction twice and then
    performing a single trial subtraction:

        w = (x mod 2ⁿ) + ⌊x / 2ⁿ⌋ * b
        y = (w mod 2ⁿ) + ⌊w / 2ⁿ⌋ * b
        if y >= (2ⁿ - b)
            y -= (2ⁿ - b)

    The value y is the answer we want for reducing x modulo (2ⁿ - b).
    */

#if !defined(CURVE25519_ASM_AVR)
    dlimb_t carry;
    uint8_t posn;

    // Calculate (x mod 2^255) + ((x / 2^255) * 19) which will
    // either produce the answer we want or it will produce a
    // value of the form "answer + j * (2^255 - 19)".
    carry = ((dlimb_t)(x[NUM_LIMBS_256BIT - 1] >> (LIMB_BITS - 1))) * 19U;
    x[NUM_LIMBS_256BIT - 1] &= ((((limb_t)1) << (LIMB_BITS - 1)) - 1);
    for (posn = 0; posn < size; ++posn) {
        carry += ((dlimb_t)(x[posn + NUM_LIMBS_256BIT])) * 38U;
        carry += x[posn];
        x[posn] = (limb_t)carry;
        carry >>= LIMB_BITS;
    }
    if (size < NUM_LIMBS_256BIT) {
        // The high order half of the number is short; e.g. for mulA24().
        // Propagate the carry through the rest of the low order part.
        for (posn = size; posn < NUM_LIMBS_256BIT; ++posn) {
            carry += x[posn];
            x[posn] = (limb_t)carry;
            carry >>= LIMB_BITS;
        }
    }

    // The "j" value may still be too large due to the final carry-out.
    // We must repeat the reduction.  If we already have the answer,
    // then this won't do any harm but we must still do the calculation
    // to preserve the overall timing.
    carry *= 38U;
    carry += ((dlimb_t)(x[NUM_LIMBS_256BIT - 1] >> (LIMB_BITS - 1))) * 19U;
    x[NUM_LIMBS_256BIT - 1] &= ((((limb_t)1) << (LIMB_BITS - 1)) - 1);
    for (posn = 0; posn < NUM_LIMBS_256BIT; ++posn) {
        carry += x[posn];
        x[posn] = (limb_t)carry;
        carry >>= LIMB_BITS;
    }

    // At this point "x" will either be the answer or it will be the
    // answer plus (2^255 - 19).  Perform a trial subtraction which
    // is equivalent to adding 19 and subtracting 2^255.  We put the
    // trial answer into the top-most limbs of the original "x" array.
    // We add 19 here; the subtraction of 2^255 occurs in the next step.
    carry = 19U;
    for (posn = 0; posn < NUM_LIMBS_256BIT; ++posn) {
        carry += x[posn];
        x[posn + NUM_LIMBS_256BIT] = (limb_t)carry;
        carry >>= LIMB_BITS;
    }

    // If there was a borrow, then the bottom-most limbs of "x" are the
    // correct answer.  If there was no borrow, then the top-most limbs
    // of "x" are the correct answer.  Select the correct answer but do
    // it in a way that instruction timing will not reveal which value
    // was selected.  Borrow will occur if the high bit of the previous
    // result is 0: turn the high bit into a selection mask.
    limb_t mask = (limb_t)(((slimb_t)(x[NUM_LIMBS_512BIT - 1])) >> (LIMB_BITS - 1));
    limb_t nmask = ~mask;
    x[NUM_LIMBS_512BIT - 1] &= ((((limb_t)1) << (LIMB_BITS - 1)) - 1);
    for (posn = 0; posn < NUM_LIMBS_256BIT; ++posn) {
        result[posn] = (x[posn] & nmask) | (x[posn + NUM_LIMBS_256BIT] & mask);
    }
#else
    __asm__ __volatile__ (
        // Calculate (x mod 2^255) + ((x / 2^255) * 19) which will
        // either produce the answer we want or it will produce a
        // value of the form "answer + j * (2^255 - 19)".
        "ldd r24,Z+31\n"                // Extract the high bit of x[31]
        "mov r25,r24\n"                 // and mask it off
        "andi r25,0x7F\n"
        "std Z+31,r25\n"
        "lsl r24\n"                     // carry = high bit * 19
        "mov r24,__zero_reg__\n"
        "sbc r24,__zero_reg__\n"
        "andi r24,19\n"

        "mov r25,%1\n"                  // load "size" into r25
        "ldi r23,38\n"                  // r23 = 38
        "mov r22,__zero_reg__\n"        // r22 = 0 (we're about to destroy r1)
        "1:\n"
        "ld r16,Z\n"                    // r16 = x[0]
        "ldd r17,Z+32\n"                // r17 = x[32]
        "mul r17,r23\n"                 // r0:r1 = r17 * 38
        "add r0,r24\n"                  // r0:r1 += carry
        "adc r1,r22\n"
        "add r0,r16\n"                  // r0:r1 += r16
        "adc r1,r22\n"
        "st Z+,r0\n"                    // *x++ = r0
        "mov r24,r1\n"                  // carry = r1
        "dec r25\n"                     // if (--r25 != 0) loop
        "brne 1b\n"

        // If the size is short, then we need to continue propagating carries.
        "ldi r25,32\n"
        "cp %1,r25\n"
        "breq 3f\n"
        "sub r25,%1\n"
        "ld __tmp_reg__,Z\n"
        "add __tmp_reg__,r24\n"
        "st Z+,__tmp_reg__\n"
        "dec r25\n"
        "2:\n"
        "ld __tmp_reg__,Z\n"            // *x++ += carry
        "adc __tmp_reg__,r22\n"
        "st Z+,__tmp_reg__\n"
        "dec r25\n"
        "brne 2b\n"
        "mov r24,r22\n"                 // put the carry back into r24
        "adc r24,r22\n"
        "3:\n"
        "sbiw r30,32\n"                 // Point Z back to the start of "x"

        // The "j" value may still be too large due to the final carry-out.
        // We must repeat the reduction.  If we already have the answer,
        // then this won't do any harm but we must still do the calculation
        // to preserve the overall timing.
        "mul r24,r23\n"                 // carry *= 38
        "ldd r24,Z+31\n"                // Extract the high bit of x[31]
        "mov r25,r24\n"                 // and mask it off
        "andi r25,0x7F\n"
        "std Z+31,r25\n"
        "lsl r24\n"                     // carry += high bit * 19
        "mov r24,r22\n"
        "sbc r24,r22\n"
        "andi r24,19\n"
        "add r0,r24\n"
        "adc r1,r22\n"                  // 9-bit carry is now in r0:r1

        // Propagate the carry through the rest of x.
        "ld r24,Z\n"                    // x[0]
        "add r0,r24\n"
        "adc r1,r22\n"
        "st Z+,r0\n"
        "ld r24,Z\n"                    // x[1]
        "add r1,r24\n"
        "st Z+,r1\n"
        "ldi r25,30\n"                  // x[2..31]
        "4:\n"
        "ld r24,Z\n"
        "adc r24,r22\n"
        "st Z+,r24\n"
        "dec r25\n"
        "brne 4b\n"
        "sbiw r30,32\n"                 // Point Z back to the start of "x"

        // We destroyed __zero_reg__ (r1) above, so restore its zero value.
        "mov __zero_reg__,r22\n"

        // At this point "x" will either be the answer or it will be the
        // answer plus (2^255 - 19).  Perform a trial subtraction which
        // is equivalent to adding 19 and subtracting 2^255.  We put the
        // trial answer into the top-most limbs of the original "x" array.
        // We add 19 here; the subtraction of 2^255 occurs in the next step.
        "ldi r24,8\n"               // Loop counter.
        "ldi r25,19\n"              // carry = 19
        "5:\n"
        "ld r16,Z+\n"               // r16:r19:carry = *xx++ + carry
        "ld r17,Z+\n"
        "ld r18,Z+\n"
        "ld r19,Z+\n"
        "add r16,r25\n"             // r16:r19:carry += carry
        "adc r17,__zero_reg__\n"
        "adc r18,__zero_reg__\n"
        "adc r19,__zero_reg__\n"
        "mov r25,__zero_reg__\n"
        "adc r25,r25\n"
        "std Z+28,r16\n"            // *tt++ = r16:r19
        "std Z+29,r17\n"
        "std Z+30,r18\n"
        "std Z+31,r19\n"
        "dec r24\n"
        "brne 5b\n"

        // Subtract 2^255 from x[32..63] which is equivalent to extracting
        // the top bit and then masking it off.  If the top bit is zero
        // then a borrow has occurred and this isn't the answer we want.
        "mov r25,r19\n"
        "andi r19,0x7F\n"
        "std Z+31,r19\n"
        "lsl r25\n"
        "mov r25,__zero_reg__\n"
        "sbc r25,__zero_reg__\n"

        // At this point, r25 is 0 if the original x[0..31] is the answer
        // we want, or 0xFF if x[32..63] is the answer we want.  Essentially
        // we need to do a conditional move of either x[0..31] or x[32..63]
        // into "result".
        "sbiw r30,32\n"             // Point Z back to x[0].
        "ldi r24,8\n"
        "6:\n"
        "ldd r16,Z+32\n"
        "ldd r17,Z+33\n"
        "ldd r18,Z+34\n"
        "ldd r19,Z+35\n"
        "ld r20,Z+\n"
        "ld r21,Z+\n"
        "ld r22,Z+\n"
        "ld r23,Z+\n"
        "eor r16,r20\n"
        "eor r17,r21\n"
        "eor r18,r22\n"
        "eor r19,r23\n"
        "and r16,r25\n"
        "and r17,r25\n"
        "and r18,r25\n"
        "and r19,r25\n"
        "eor r20,r16\n"
        "eor r21,r17\n"
        "eor r22,r18\n"
        "eor r23,r19\n"
        "st X+,r20\n"
        "st X+,r21\n"
        "st X+,r22\n"
        "st X+,r23\n"
        "dec r24\n"
        "brne 6b\n"

        : : "z"(x), "r"((uint8_t)(size * sizeof(limb_t))), "x"(result)
        : "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
          "r24", "r25"
    );
#endif
}

/**
 * \brief Quickly reduces a number modulo 2^255 - 19.
 *
 * \param x The number to be reduced, which must be NUM_LIMBS_256BIT
 * limbs in size and less than or equal to 2 * (2^255 - 19 - 1).
 * \return Zero if \a x was greater than or equal to (2^255 - 19).
 *
 * The answer is also put into \a x and will consist of NUM_LIMBS_256BIT limbs.
 *
 * This function is intended for reducing the result of additions where
 * the caller knows that \a x is within the described range.  A single
 * trial subtraction is all that is needed to reduce the number.
 */
limb_t Curve25519::reduceQuick(limb_t *x)
{
#if !defined(CURVE25519_ASM_AVR)
    limb_t temp[NUM_LIMBS_256BIT];
    dlimb_t carry;
    uint8_t posn;
    limb_t *xx;
    limb_t *tt;
    
    // Perform a trial subtraction of (2^255 - 19) from "x" which is
    // equivalent to adding 19 and subtracting 2^255.  We add 19 here;
    // the subtraction of 2^255 occurs in the next step.
    carry = 19U;
    xx = x;
    tt = temp;
    for (posn = 0; posn < NUM_LIMBS_256BIT; ++posn) {
        carry += *xx++;
        *tt++ = (limb_t)carry;
        carry >>= LIMB_BITS;
    }

    // If there was a borrow, then the original "x" is the correct answer.
    // If there was no borrow, then "temp" is the correct answer.  Select the
    // correct answer but do it in a way that instruction timing will not
    // reveal which value was selected.  Borrow will occur if the high bit
    // of "temp" is 0: turn the high bit into a selection mask.
    limb_t mask = (limb_t)(((slimb_t)(temp[NUM_LIMBS_256BIT - 1])) >> (LIMB_BITS - 1));
    limb_t nmask = ~mask;
    temp[NUM_LIMBS_256BIT - 1] &= ((((limb_t)1) << (LIMB_BITS - 1)) - 1);
    xx = x;
    tt = temp;
    for (posn = 0; posn < NUM_LIMBS_256BIT; ++posn) {
        *xx = ((*xx) & nmask) | ((*tt++) & mask);
        ++xx;
    }

    // Clean up "temp".
    strict_clean(temp);

    // Return a zero value if we actually subtracted (2^255 - 19) from "x".
    return nmask;
#else // CURVE25519_ASM_AVR
    limb_t temp[NUM_LIMBS_256BIT];
    uint8_t result;
    __asm__ __volatile__ (
        // Subtract (2^255 - 19) from "x", which is the same as adding 19
        // and then subtracting 2^255.
        "ldi r24,8\n"               // Loop counter.
        "ldi r25,19\n"              // carry = 19
        "1:\n"
        "ld r16,Z+\n"               // r16:r19:carry = *xx++ + carry
        "ld r17,Z+\n"
        "ld r18,Z+\n"
        "ld r19,Z+\n"
        "add r16,r25\n"             // r16:r19:carry += carry
        "adc r17,__zero_reg__\n"
        "adc r18,__zero_reg__\n"
        "adc r19,__zero_reg__\n"
        "mov r25,__zero_reg__\n"
        "adc r25,r25\n"
        "st X+,r16\n"               // *tt++ = r16:r19
        "st X+,r17\n"
        "st X+,r18\n"
        "st X+,r19\n"
        "dec r24\n"
        "brne 1b\n"

        // Subtract 2^255 from "temp" which is equivalent to extracting
        // the top bit and then masking it off.  If the top bit is zero
        // then a borrow has occurred and this isn't the answer we want.
        "mov r25,r19\n"
        "andi r19,0x7F\n"
        "st -X,r19\n"
        "lsl r25\n"
        "mov r25,__zero_reg__\n"
        "sbc r25,__zero_reg__\n"

        // At this point, r25 is 0 if the original "x" is the answer
        // we want, or 0xFF if "temp" is the answer we want.  Essentially
        // we need to do a conditional move of "temp" into "x".
        "sbiw r26,31\n"             // Point X back to the start of "temp".
        "sbiw r30,32\n"             // Point Z back to the start of "x".
        "ldi r24,8\n"
        "2:\n"
        "ld r16,X+\n"
        "ld r17,X+\n"
        "ld r18,X+\n"
        "ld r19,X+\n"
        "ld r20,Z\n"
        "ldd r21,Z+1\n"
        "ldd r22,Z+2\n"
        "ldd r23,Z+3\n"
        "eor r16,r20\n"
        "eor r17,r21\n"
        "eor r18,r22\n"
        "eor r19,r23\n"
        "and r16,r25\n"
        "and r17,r25\n"
        "and r18,r25\n"
        "and r19,r25\n"
        "eor r20,r16\n"
        "eor r21,r17\n"
        "eor r22,r18\n"
        "eor r23,r19\n"
        "st Z+,r20\n"
        "st Z+,r21\n"
        "st Z+,r22\n"
        "st Z+,r23\n"
        "dec r24\n"
        "brne 2b\n"
        "mov %0,r25\n"
        : "=r"(result)
        : "x"(temp), "z"(x)
        : "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
          "r24", "r25"
    );
    strict_clean(temp);
    return result;
#endif // CURVE25519_ASM_AVR
}

/**
 * \brief Multiplies two 256-bit values to produce a 512-bit result.
 *
 * \param result The result, which must be NUM_LIMBS_512BIT limbs in size
 * and must not overlap with \a x or \a y.
 * \param x The first value to multiply, which must be NUM_LIMBS_256BIT
 * limbs in size.
 * \param y The second value to multiply, which must be NUM_LIMBS_256BIT
 * limbs in size.
 *
 * \sa mul()
 */
void Curve25519::mulNoReduce(limb_t *result, const limb_t *x, const limb_t *y)
{
#if !defined(CURVE25519_ASM_AVR)
    uint8_t i, j;
    dlimb_t carry;
    limb_t word;
    const limb_t *yy;
    limb_t *rr;

    // Multiply the lowest word of x by y.
    carry = 0;
    word = x[0];
    yy = y;
    rr = result;
    for (i = 0; i < NUM_LIMBS_256BIT; ++i) {
        carry += ((dlimb_t)(*yy++)) * word;
        *rr++ = (limb_t)carry;
        carry >>= LIMB_BITS;
    }
    *rr = (limb_t)carry;

    // Multiply and add the remaining words of x by y.
    for (i = 1; i < NUM_LIMBS_256BIT; ++i) {
        word = x[i];
        carry = 0;
        yy = y;
        rr = result + i;
        for (j = 0; j < NUM_LIMBS_256BIT; ++j) {
            carry += ((dlimb_t)(*yy++)) * word;
            carry += *rr;
            *rr++ = (limb_t)carry;
            carry >>= LIMB_BITS;
        }
        *rr = (limb_t)carry;
    }
#else
    __asm__ __volatile__ (
        // Save Y and copy the "result" pointer into it.
        "push r28\n"
        "push r29\n"
        "mov r28,%A2\n"
        "mov r29,%B2\n"

        // Multiply the first byte of "x" by y[0..31].
        "ldi r25,8\n"               // loop 8 times: 4 bytes of y each time
        "clr r24\n"                 // carry = 0
        "clr r22\n"                 // r22 = 0 to replace __zero_reg__
        "ld r23,X+\n"               // r23 = *x++
        "1:\n"
        "ld r16,Z\n"                // r16 = y[0]
        "mul r16,r23\n"             // r8:r9 = y[0] * r23
        "movw r8,r0\n"
        "ldd r16,Z+2\n"             // r16 = y[2]
        "mul r16,r23\n"             // r10:r11 = y[2] * r23
        "movw r10,r0\n"
        "ldd r16,Z+1\n"             // r16 = y[1]
        "mul r16,r23\n"             // r9:r10:r11 += y[1] * r23
        "add r9,r0\n"
        "adc r10,r1\n"
        "adc r11,r22\n"
        "ldd r16,Z+3\n"             // r16 = y[3]
        "mul r16,r23\n"             // r11:r1 += y[3] * r23
        "add r11,r0\n"
        "adc r1,r22\n"
        "add r8,r24\n"              // r8:r9:r10:r11:r1 += carry
        "adc r9,r22\n"
        "adc r10,r22\n"
        "adc r11,r22\n"
        "adc r1,r22\n"
        "mov r24,r1\n"              // carry = r1
        "st Y+,r8\n"                // *rr++ = r8:r9:r10:r11
        "st Y+,r9\n"
        "st Y+,r10\n"
        "st Y+,r11\n"
        "adiw r30,4\n"
        "dec r25\n"
        "brne 1b\n"
        "st Y+,r24\n"               // *rr++ = carry
        "sbiw r28,32\n"             // rr -= 32
        "sbiw r30,32\n"             // Point Z back to the start of y

        // Multiply and add the remaining bytes of "x" by y[0..31].
        "ldi r21,31\n"              // 31 more bytes of x to go.
        "2:\n"
        "ldi r25,8\n"               // loop 8 times: 4 bytes of y each time
        "clr r24\n"                 // carry = 0
        "ld r23,X+\n"               // r23 = *x++
        "3:\n"
        "ld r16,Z\n"                // r16 = y[0]
        "mul r16,r23\n"             // r8:r9 = y[0] * r23
        "movw r8,r0\n"
        "ldd r16,Z+2\n"             // r16 = y[2]
        "mul r16,r23\n"             // r10:r11 = y[2] * r23
        "movw r10,r0\n"
        "ldd r16,Z+1\n"             // r16 = y[1]
        "mul r16,r23\n"             // r9:r10:r11 += y[1] * r23
        "add r9,r0\n"
        "adc r10,r1\n"
        "adc r11,r22\n"
        "ldd r16,Z+3\n"             // r16 = y[3]
        "mul r16,r23\n"             // r11:r1 += y[3] * r23
        "add r11,r0\n"
        "adc r1,r22\n"
        "add r8,r24\n"              // r8:r9:r10:r11:r1 += carry
        "adc r9,r22\n"
        "adc r10,r22\n"
        "adc r11,r22\n"
        "adc r1,r22\n"
        "ld r16,Y\n"                // r8:r9:r10:r11:r1 += rr[0..3]
        "add r8,r16\n"
        "ldd r16,Y+1\n"
        "adc r9,r16\n"
        "ldd r16,Y+2\n"
        "adc r10,r16\n"
        "ldd r16,Y+3\n"
        "adc r11,r16\n"
        "adc r1,r22\n"
        "mov r24,r1\n"              // carry = r1
        "st Y+,r8\n"                // *rr++ = r8:r9:r10:r11
        "st Y+,r9\n"
        "st Y+,r10\n"
        "st Y+,r11\n"
        "adiw r30,4\n"
        "dec r25\n"
        "brne 3b\n"
        "st Y+,r24\n"               // *r++ = carry
        "sbiw r28,32\n"             // rr -= 32
        "sbiw r30,32\n"             // Point Z back to the start of y
        "dec r21\n"
        "brne 2b\n"

        // Restore Y and __zero_reg__.
        "pop r29\n"
        "pop r28\n"
        "clr __zero_reg__\n"
        : : "x"(x), "z"(y), "r"(result)
        : "r8", "r9", "r10", "r11", "r16", "r20", "r21", "r22",
          "r23", "r24", "r25"
    );
#endif
}

/**
 * \brief Multiplies two values and then reduces the result modulo 2^255 - 19.
 *
 * \param result The result, which must be NUM_LIMBS_256BIT limbs in size
 * and can be the same array as \a x or \a y.
 * \param x The first value to multiply, which must be NUM_LIMBS_256BIT limbs
 * in size and less than 2^255 - 19.
 * \param y The second value to multiply, which must be NUM_LIMBS_256BIT limbs
 * in size and less than 2^255 - 19.  This can be the same array as \a x.
 */
void Curve25519::mul(limb_t *result, const limb_t *x, const limb_t *y)
{
    limb_t temp[NUM_LIMBS_512BIT];
    mulNoReduce(temp, x, y);
    reduce(result, temp, NUM_LIMBS_256BIT);
    strict_clean(temp);
    crypto_feed_watchdog();
}

/**
 * \fn void Curve25519::square(limb_t *result, const limb_t *x)
 * \brief Squares a value and then reduces it modulo 2^255 - 19.
 *
 * \param result The result, which must be NUM_LIMBS_256BIT limbs in size and
 * can be the same array as \a x.
 * \param x The value to square, which must be NUM_LIMBS_256BIT limbs in size
 * and less than 2^255 - 19.
 */

/**
 * \brief Multiplies a value by the a24 constant and then reduces the result
 * modulo 2^255 - 19.
 *
 * \param result The result, which must be NUM_LIMBS_256BIT limbs in size
 * and can be the same array as \a x.
 * \param x The value to multiply by a24, which must be NUM_LIMBS_256BIT
 * limbs in size and less than 2^255 - 19.
 */
void Curve25519::mulA24(limb_t *result, const limb_t *x)
{
#if !defined(CURVE25519_ASM_AVR)
    // The constant a24 = 121665 (0x1DB41) as a limb array.
#if BIGNUMBER_LIMB_8BIT
    static limb_t const a24[3] PROGMEM = {0x41, 0xDB, 0x01};
#elif BIGNUMBER_LIMB_16BIT
    static limb_t const a24[2] PROGMEM = {0xDB41, 0x0001};
#elif BIGNUMBER_LIMB_32BIT || BIGNUMBER_LIMB_64BIT
    static limb_t const a24[1] PROGMEM = {0x0001DB41};
#else
    #error "limb_t must be 8, 16, 32, or 64 bits in size"
#endif
    #define NUM_A24_LIMBS   (sizeof(a24) / sizeof(limb_t))

    // Multiply the lowest limb of a24 by x and zero-extend into the result.
    limb_t temp[NUM_LIMBS_512BIT];
    uint8_t i, j;
    dlimb_t carry = 0;
    limb_t word = pgm_read_limb(&(a24[0]));
    const limb_t *xx = x;
    limb_t *tt = temp;
    for (i = 0; i < NUM_LIMBS_256BIT; ++i) {
        carry += ((dlimb_t)(*xx++)) * word;
        *tt++ = (limb_t)carry;
        carry >>= LIMB_BITS;
    }
    *tt = (limb_t)carry;

    // Multiply and add the remaining limbs of a24.
    for (i = 1; i < NUM_A24_LIMBS; ++i) {
        word = pgm_read_limb(&(a24[i]));
        carry = 0;
        xx = x;
        tt = temp + i;
        for (j = 0; j < NUM_LIMBS_256BIT; ++j) {
            carry += ((dlimb_t)(*xx++)) * word;
            carry += *tt;
            *tt++ = (limb_t)carry;
            carry >>= LIMB_BITS;
        }
        *tt = (limb_t)carry;
    }
#else
    limb_t temp[NUM_LIMBS_512BIT];
    #define NUM_A24_LIMBS   ((3 + sizeof(limb_t) - 1) / sizeof(limb_t))
    __asm__ __volatile__ (
        // Load the two low bytes of a24 into r16 and r17.
        // The third byte is 0x01 which we can deal with implicitly.
        "ldi r16,0x41\n"
        "ldi r17,0xDB\n"

        // Iterate over the bytes of "x" and multiply each with a24.
        "ldi r25,32\n"              // 32 bytes in "x"
        "clr r22\n"                 // r22 = 0
        "clr r18\n"                 // r18:r19:r11 = 0 (carry)
        "clr r19\n"
        "clr r11\n"
        "1:\n"
        "ld r21,X+\n"               // r21 = *x++
        "mul r21,r16\n"             // r8:r9 = r21 * a24[0]
        "movw r8,r0\n"
        "mul r21,r17\n"             // r9:r1 += r21 * a24[1]
        "add r9,r0\n"
        "adc r1,r21\n"              // r1:r10 += r21 * a24[2] (implicitly 1)
        "mov r10,r22\n"
        "adc r10,r22\n"
        "add r8,r18\n"              // r8:r9:r1:r10 += carry
        "adc r9,r19\n"
        "adc r1,r11\n"
        "adc r10,r22\n"
        "st Z+,r8\n"                // *tt++ = r8
        "mov r18,r9\n"              // carry = r9:r1:r10
        "mov r19,r1\n"
        "mov r11,r10\n"
        "dec r25\n"
        "brne 1b\n"
        "st Z,r18\n"                // *tt = carry
        "std Z+1,r19\n"
        "std Z+2,r11\n"
#if BIGNUMBER_LIMB_16BIT || BIGNUMBER_LIMB_32BIT
        "std Z+3,r22\n"             // Zero pad to a limb boundary
#endif

        // Restore __zero_reg__
        "clr __zero_reg__\n"

        : : "x"(x), "z"(temp)
        : "r8", "r9", "r10", "r11", "r16", "r17", "r18", "r19",
          "r20", "r21", "r22", "r25"
    );
#endif

    // Reduce the intermediate result modulo 2^255 - 19.
    reduce(result, temp, NUM_A24_LIMBS);
    strict_clean(temp);
}

/**
 * \brief Multiplies two values and then reduces the result modulo 2^255 - 19,
 * where one of the values is in program memory.
 *
 * \param result The result, which must be NUM_LIMBS_256BIT limbs in size
 * and can be the same array as \a x or \a y.
 * \param x The first value to multiply, which must be NUM_LIMBS_256BIT limbs
 * in size and less than 2^255 - 19.
 * \param y The second value to multiply, which must be NUM_LIMBS_256BIT limbs
 * in size and less than 2^255 - 19.  This array must be in program memory.
 */
void Curve25519::mul_P(limb_t *result, const limb_t *x, const limb_t *y)
{
    limb_t temp[NUM_LIMBS_512BIT];
    uint8_t i, j;
    dlimb_t carry;
    limb_t word;
    const limb_t *xx;
    limb_t *tt;

    // Multiply the lowest word of y by x.
    carry = 0;
    word = pgm_read_limb(&(y[0]));
    xx = x;
    tt = temp;
    for (i = 0; i < NUM_LIMBS_256BIT; ++i) {
        carry += ((dlimb_t)(*xx++)) * word;
        *tt++ = (limb_t)carry;
        carry >>= LIMB_BITS;
    }
    *tt = (limb_t)carry;

    // Multiply and add the remaining words of y by x.
    for (i = 1; i < NUM_LIMBS_256BIT; ++i) {
        word = pgm_read_limb(&(y[i]));
        carry = 0;
        xx = x;
        tt = temp + i;
        for (j = 0; j < NUM_LIMBS_256BIT; ++j) {
            carry += ((dlimb_t)(*xx++)) * word;
            carry += *tt;
            *tt++ = (limb_t)carry;
            carry >>= LIMB_BITS;
        }
        *tt = (limb_t)carry;
    }

    // Reduce the intermediate result modulo 2^255 - 19.
    reduce(result, temp, NUM_LIMBS_256BIT);
    strict_clean(temp);
}

/**
 * \brief Adds two values and then reduces the result modulo 2^255 - 19.
 *
 * \param result The result, which must be NUM_LIMBS_256BIT limbs in size
 * and can be the same array as \a x or \a y.
 * \param x The first value to multiply, which must be NUM_LIMBS_256BIT
 * limbs in size and less than 2^255 - 19.
 * \param y The second value to multiply, which must be NUM_LIMBS_256BIT
 * limbs in size and less than 2^255 - 19.
 */
void Curve25519::add(limb_t *result, const limb_t *x, const limb_t *y)
{
#if !defined(CURVE25519_ASM_AVR)
    dlimb_t carry = 0;
    uint8_t posn;
    limb_t *rr = result;

    // Add the two arrays to obtain the intermediate result.
    for (posn = 0; posn < NUM_LIMBS_256BIT; ++posn) {
        carry += *x++;
        carry += *y++;
        *rr++ = (limb_t)carry;
        carry >>= LIMB_BITS;
    }
#else // CURVE25519_ASM_AVR
    __asm__ __volatile__ (
        // Save Y and copy the "result" pointer into it.
        "push r28\n"
        "push r29\n"
        "mov r28,%A2\n"
        "mov r29,%B2\n"

        // Unroll the loop to operate on 4 bytes at a time (8 iterations).
        "ldi r24,8\n"               // Loop counter.
        "clr r25\n"                 // carry = 0
        "1:\n"
        "ld r16,X+\n"               // r16:r19 = *x++
        "ld r17,X+\n"
        "ld r18,X+\n"
        "ld r19,X+\n"
        "ld r20,Z+\n"               // r20:r23 = *y++
        "ld r21,Z+\n"
        "ld r22,Z+\n"
        "ld r23,Z+\n"
        "add r16,r25\n"             // r16:r19:carry += carry
        "adc r17,__zero_reg__\n"
        "adc r18,__zero_reg__\n"
        "adc r19,__zero_reg__\n"
        "mov r25,__zero_reg__\n"
        "adc r25,r25\n"
        "add r16,r20\n"             // r16:r19:carry += r20:r23
        "adc r17,r21\n"
        "adc r18,r22\n"
        "adc r19,r23\n"
        "adc r25,__zero_reg__\n"
        "st Y+,r16\n"               // *rr++ = r16:r23
        "st Y+,r17\n"
        "st Y+,r18\n"
        "st Y+,r19\n"
        "dec r24\n"
        "brne 1b\n"

        // Restore Y.
        "pop r29\n"
        "pop r28\n"
        : : "x"(x), "z"(y), "r"(result)
        : "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
          "r24", "r25"
    );
#endif // CURVE25519_ASM_AVR

    // Reduce the result using the quick trial subtraction method.
    reduceQuick(result);
}

/**
 * \brief Subtracts two values and then reduces the result modulo 2^255 - 19.
 *
 * \param result The result, which must be NUM_LIMBS_256BIT limbs in size
 * and can be the same array as \a x or \a y.
 * \param x The first value to multiply, which must be NUM_LIMBS_256BIT
 * limbs in size and less than 2^255 - 19.
 * \param y The second value to multiply, which must be NUM_LIMBS_256BIT
 * limbs in size and less than 2^255 - 19.
 */
void Curve25519::sub(limb_t *result, const limb_t *x, const limb_t *y)
{
#if !defined(CURVE25519_ASM_AVR)
    dlimb_t borrow;
    uint8_t posn;
    limb_t *rr = result;

    // Subtract y from x to generate the intermediate result.
    borrow = 0;
    for (posn = 0; posn < NUM_LIMBS_256BIT; ++posn) {
        borrow = ((dlimb_t)(*x++)) - (*y++) - ((borrow >> LIMB_BITS) & 0x01);
        *rr++ = (limb_t)borrow;
    }

    // If we had a borrow, then the result has gone negative and we
    // have to add 2^255 - 19 to the result to make it positive again.
    // The top bits of "borrow" will be all 1's if there is a borrow
    // or it will be all 0's if there was no borrow.  Easiest is to
    // conditionally subtract 19 and then mask off the high bit.
    rr = result;
    borrow = (borrow >> LIMB_BITS) & 19U;
    borrow = ((dlimb_t)(*rr)) - borrow;
    *rr++ = (limb_t)borrow;
    for (posn = 1; posn < NUM_LIMBS_256BIT; ++posn) {
        borrow = ((dlimb_t)(*rr)) - ((borrow >> LIMB_BITS) & 0x01);
        *rr++ = (limb_t)borrow;
    }
    *(--rr) &= ((((limb_t)1) << (LIMB_BITS - 1)) - 1);
#else // CURVE25519_ASM_AVR
    __asm__ __volatile__ (
        // Save Y and copy the "result" pointer into it.
        "push r28\n"
        "push r29\n"
        "mov r28,%A2\n"
        "mov r29,%B2\n"

        // Unroll the sub loop to operate on 4 bytes at a time (8 iterations).
        "ldi r24,8\n"               // Loop counter.
        "clr r25\n"                 // borrow = 0
        "1:\n"
        "ld r16,X+\n"               // r16:r19 = *x++
        "ld r17,X+\n"
        "ld r18,X+\n"
        "ld r19,X+\n"
        "ld r20,Z+\n"               // r20:r23 = *y++
        "ld r21,Z+\n"
        "ld r22,Z+\n"
        "ld r23,Z+\n"
        "sub r16,r25\n"             // r16:r19:borrow -= borrow
        "sbc r17,__zero_reg__\n"
        "sbc r18,__zero_reg__\n"
        "sbc r19,__zero_reg__\n"
        "mov r25,__zero_reg__\n"
        "sbc r25,__zero_reg__\n"
        "sub r16,r20\n"             // r16:r19:borrow -= r20:r23
        "sbc r17,r21\n"
        "sbc r18,r22\n"
        "sbc r19,r23\n"
        "sbc r25,__zero_reg__\n"
        "st Y+,r16\n"               // *rr++ = r16:r23
        "st Y+,r17\n"
        "st Y+,r18\n"
        "st Y+,r19\n"
        "andi r25,1\n"              // Only need the bottom bit of the borrow
        "dec r24\n"
        "brne 1b\n"

        // If there was a borrow, then we need to add 2^255 - 19 back.
        // We conditionally subtract 19 and then mask off the high bit.
        "neg r25\n"                 // borrow = mask(borrow) & 19
        "andi r25,19\n"
        "sbiw r28,32\n"             // Point Y back to the start of "result"
        "ldi r24,8\n"
        "2:\n"
        "ld r16,Y\n"                // r16:r19 = *rr
        "ldd r17,Y+1\n"
        "ldd r18,Y+2\n"
        "ldd r19,Y+3\n"
        "sub r16,r25\n"
        "sbc r17,__zero_reg__\n"    // r16:r19:borrow -= borrow
        "sbc r18,__zero_reg__\n"
        "sbc r19,__zero_reg__\n"
        "mov r25,__zero_reg__\n"
        "sbc r25,__zero_reg__\n"
        "andi r25,1\n"
        "st Y+,r16\n"               // *r++ = r16:r19
        "st Y+,r17\n"
        "st Y+,r18\n"
        "st Y+,r19\n"
        "dec r24\n"
        "brne 2b\n"
        "andi r19,0x7F\n"           // Mask off the high bit in the last byte
        "sbiw r28,1\n"
        "st Y,r19\n"

        // Restore Y.
        "pop r29\n"
        "pop r28\n"
        : : "x"(x), "z"(y), "r"(result)
        : "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
          "r24", "r25"
    );
#endif // CURVE25519_ASM_AVR
}

/**
 * \brief Conditionally swaps two values if a selection value is non-zero.
 *
 * \param select Non-zero to swap \a x and \a y, zero to leave them unchanged.
 * \param x The first value to conditionally swap.
 * \param y The second value to conditionally swap.
 *
 * The swap is performed in a way that it should take the same amount of
 * time irrespective of the value of \a select.
 *
 * \sa cmove()
 */
void Curve25519::cswap(limb_t select, limb_t *x, limb_t *y)
{
#if !defined(CURVE25519_ASM_AVR)
    uint8_t posn;
    limb_t dummy;
    limb_t sel;

    // Turn "select" into an all-zeroes or all-ones mask.  We don't care
    // which bit or bits is set in the original "select" value.
    sel = (limb_t)(((((dlimb_t)1) << LIMB_BITS) - select) >> LIMB_BITS);
    --sel;

    // Swap the two values based on "select".  Algorithm from:
    // http://tools.ietf.org/html/rfc7748
    for (posn = 0; posn < NUM_LIMBS_256BIT; ++posn) {
        dummy = sel & (x[posn] ^ y[posn]);
        x[posn] ^= dummy;
        y[posn] ^= dummy;
    }
#else // CURVE25519_ASM_AVR
    __asm__ __volatile__ (
        // Combine all bytes from "select" into one and then turn
        // that byte into the "sel" mask in r24.
        "clr r24\n"
#if BIGNUMBER_LIMB_8BIT
        "sub r24,%2\n"
#elif BIGNUMBER_LIMB_16BIT
        "or %A2,%B2\n"
        "sub r24,%A2\n"
#elif BIGNUMBER_LIMB_32BIT
        "or %A2,%B2\n"
        "or %A2,%C2\n"
        "or %A2,%D2\n"
        "sub r24,%A2\n"
#endif
        "mov r24,__zero_reg__\n"
        "sbc r24,r24\n"

        // Perform the conditional swap 4 bytes at a time.
        "ldi r25,8\n"
        "1:\n"
        "ld r16,X+\n"           // r16:r19 = *x
        "ld r17,X+\n"
        "ld r18,X+\n"
        "ld r19,X\n"
        "ld r20,Z\n"            // r20:r23 = *y
        "ldd r21,Z+1\n"
        "ldd r22,Z+2\n"
        "ldd r23,Z+3\n"
        "mov r12,r16\n"         // r12:r15 = (r16:r19 ^ r20:r23) & sel
        "mov r13,r17\n"
        "mov r14,r18\n"
        "mov r15,r19\n"
        "eor r12,r20\n"
        "eor r13,r21\n"
        "eor r14,r22\n"
        "eor r15,r23\n"
        "and r12,r24\n"
        "and r13,r24\n"
        "and r14,r24\n"
        "and r15,r24\n"
        "eor r16,r12\n"         // r16:r19 ^= r12:r15
        "eor r17,r13\n"
        "eor r18,r14\n"
        "eor r19,r15\n"
        "eor r20,r12\n"         // r20:r23 ^= r12:r15
        "eor r21,r13\n"
        "eor r22,r14\n"
        "eor r23,r15\n"
        "st X,r19\n"            // *x++ = r16:r19
        "st -X,r18\n"
        "st -X,r17\n"
        "st -X,r16\n"
        "adiw r26,4\n"
        "st Z+,r20\n"           // *y++ = r20:r23
        "st Z+,r21\n"
        "st Z+,r22\n"
        "st Z+,r23\n"
        "dec r25\n"
        "brne 1b\n"

        : : "x"(x), "z"(y), "r"(select)
        : "r12", "r13", "r14", "r15", "r16", "r17", "r18", "r19",
          "r20", "r21", "r22", "r23", "r24", "r25"
    );
#endif // CURVE25519_ASM_AVR
}

/**
 * \brief Conditionally moves \a y into \a x if a selection value is non-zero.
 *
 * \param select Non-zero to move \a y into \a x, zero to leave \a x unchanged.
 * \param x The destination to move into.
 * \param y The value to conditionally move.
 *
 * The move is performed in a way that it should take the same amount of
 * time irrespective of the value of \a select.
 *
 * \sa cswap()
 */
void Curve25519::cmove(limb_t select, limb_t *x, const limb_t *y)
{
#if !defined(CURVE25519_ASM_AVR)
    uint8_t posn;
    limb_t dummy;
    limb_t sel;

    // Turn "select" into an all-zeroes or all-ones mask.  We don't care
    // which bit or bits is set in the original "select" value.
    sel = (limb_t)(((((dlimb_t)1) << LIMB_BITS) - select) >> LIMB_BITS);
    --sel;

    // Move y into x based on "select".  Similar to conditional swap above.
    for (posn = 0; posn < NUM_LIMBS_256BIT; ++posn) {
        dummy = sel & (x[posn] ^ y[posn]);
        x[posn] ^= dummy;
    }
#else // CURVE25519_ASM_AVR
    __asm__ __volatile__ (
        // Combine all bytes from "select" into one and then turn
        // that byte into the "sel" mask in r24.
        "clr r24\n"
#if BIGNUMBER_LIMB_8BIT
        "sub r24,%2\n"
#elif BIGNUMBER_LIMB_16BIT
        "or %A2,%B2\n"
        "sub r24,%A2\n"
#elif BIGNUMBER_LIMB_32BIT
        "or %A2,%B2\n"
        "or %A2,%C2\n"
        "or %A2,%D2\n"
        "sub r24,%A2\n"
#endif
        "mov r24,__zero_reg__\n"
        "sbc r24,r24\n"

        // Perform the conditional move 4 bytes at a time.
        "ldi r25,8\n"
        "1:\n"
        "ld r16,X+\n"           // r16:r19 = *x
        "ld r17,X+\n"
        "ld r18,X+\n"
        "ld r19,X\n"
        "ld r20,Z+\n"           // r20:r23 = *y++
        "ld r21,Z+\n"
        "ld r22,Z+\n"
        "ld r23,Z+\n"
        "eor r20,r16\n"         // r20:r23 = (r16:r19 ^ r20:r23) & sel
        "eor r21,r17\n"
        "eor r22,r18\n"
        "eor r23,r19\n"
        "and r20,r24\n"
        "and r21,r24\n"
        "and r22,r24\n"
        "and r23,r24\n"
        "eor r16,r20\n"         // r16:r19 ^= r20:r23
        "eor r17,r21\n"
        "eor r18,r22\n"
        "eor r19,r23\n"
        "st X,r19\n"            // *x++ = r16:r19
        "st -X,r18\n"
        "st -X,r17\n"
        "st -X,r16\n"
        "adiw r26,4\n"
        "dec r25\n"
        "brne 1b\n"

        : : "x"(x), "z"(y), "r"(select)
        : "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
          "r24", "r25"
    );
#endif // CURVE25519_ASM_AVR
}

/**
 * \brief Raise x to the power of (2^250 - 1).
 *
 * \param result The result array, which must be NUM_LIMBS_256BIT limbs in size.
 * \param x The value to raise.
 */
void Curve25519::pow250(limb_t *result, const limb_t *x)
{
    limb_t t1[NUM_LIMBS_256BIT];
    uint8_t i, j;

    // The big-endian hexadecimal expansion of (2^250 - 1) is:
    // 03FFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF
    //
    // The naive implementation needs to do 2 multiplications per 1 bit and
    // 1 multiplication per 0 bit.  We can improve upon this by creating a
    // pattern 0000000001 ... 0000000001.  If we square and multiply the
    // pattern by itself we can turn the pattern into the partial results
    // 0000000011 ... 0000000011, 0000000111 ... 0000000111, etc.
    // This averages out to about 1.1 multiplications per 1 bit instead of 2.

    // Build a pattern of 250 bits in length of repeated copies of 0000000001.
    #define RECIP_GROUP_SIZE 10
    #define RECIP_GROUP_BITS 250    // Must be a multiple of RECIP_GROUP_SIZE.
    square(t1, x);
    for (j = 0; j < (RECIP_GROUP_SIZE - 1); ++j)
        square(t1, t1);
    mul(result, t1, x);
    for (i = 0; i < ((RECIP_GROUP_BITS / RECIP_GROUP_SIZE) - 2); ++i) {
        for (j = 0; j < RECIP_GROUP_SIZE; ++j)
            square(t1, t1);
        mul(result, result, t1);
    }

    // Multiply bit-shifted versions of the 0000000001 pattern into
    // the result to "fill in" the gaps in the pattern.
    square(t1, result);
    mul(result, result, t1);
    for (j = 0; j < (RECIP_GROUP_SIZE - 2); ++j) {
        square(t1, t1);
        mul(result, result, t1);
    }

    // Clean up and exit.
    clean(t1);
}

/**
 * \brief Computes the reciprocal of a number modulo 2^255 - 19.
 *
 * \param result The result as a array of NUM_LIMBS_256BIT limbs in size.
 * This cannot be the same array as \a x.
 * \param x The number to compute the reciprocal for.
 */
void Curve25519::recip(limb_t *result, const limb_t *x)
{
    // The reciprocal is the same as x ^ (p - 2) where p = 2^255 - 19.
    // The big-endian hexadecimal expansion of (p - 2) is:
    // 7FFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFEB
    // Start with the 250 upper bits of the expansion of (p - 2).
    pow250(result, x);

    // Deal with the 5 lowest bits of (p - 2), 01011, from highest to lowest.
    square(result, result);
    square(result, result);
    mul(result, result, x);
    square(result, result);
    square(result, result);
    mul(result, result, x);
    square(result, result);
    mul(result, result, x);
}

/**
 * \brief Computes the square root of a number modulo 2^255 - 19.
 *
 * \param result The result as a array of NUM_LIMBS_256BIT limbs in size.
 * This must not overlap with \a x.
 * \param x The number to compute the square root for.
 *
 * For any number \a x, there are two square roots: positive and negative.
 * For example, both 2 and -2 are square roots of 4 because 2 * 2 = -2 * -2.
 * This function will return one or the other.  Callers must determine which
 * square root they are interested in and invert the result as necessary.
 *
 * \note This function is not constant time so it should only be used
 * on publicly-known values.
 */
bool Curve25519::sqrt(limb_t *result, const limb_t *x)
{
    // sqrt(-1) mod (2^255 - 19).
    static limb_t const numSqrtM1[NUM_LIMBS_256BIT] PROGMEM = {
        LIMB_PAIR(0x4A0EA0B0, 0xC4EE1B27), LIMB_PAIR(0xAD2FE478, 0x2F431806),
        LIMB_PAIR(0x3DFBD7A7, 0x2B4D0099), LIMB_PAIR(0x4FC1DF0B, 0x2B832480)
    };
    limb_t y[NUM_LIMBS_256BIT];

    // Algorithm from: http://tools.ietf.org/html/rfc7748

    // Compute a candidate root: result = x^((p + 3) / 8) mod p.
    // (p + 3) / 8 = (2^252 - 2) which is 251 one bits followed by a zero:
    // 0FFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE
    pow250(result, x);
    square(result, result);
    mul(result, result, x);
    square(result, result);

    // Did we get the square root immediately?
    square(y, result);
    if (memcmp(x, y, sizeof(y)) == 0) {
        clean(y);
        return true;
    }

    // Multiply the result by sqrt(-1) and check again.
    mul_P(result, result, numSqrtM1);
    square(y, result);
    if (memcmp(x, y, sizeof(y)) == 0) {
        clean(y);
        return true;
    }

    // The number does not have a square root.
    clean(y);
    return false;
}
