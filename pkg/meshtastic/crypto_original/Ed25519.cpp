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

#include "Ed25519.h"
#include "Curve25519.h"
#include "Crypto.h"
#include "RNG.h"
#include "utility/LimbUtil.h"
#include <string.h>

/**
 * \class Ed25519 Ed25519.h <Ed25519.h>
 * \brief Digital signatures based on the elliptic curve modulo 2^255 - 19.
 *
 * The first step in creating a digital signature with Ed25519 is to
 * generate a key pair:
 *
 * \code
 * uint8_t privateKey[32];
 * uint8_t publicKey[32];
 *
 * Ed25519::generatePrivateKey(privateKey);
 * Ed25519::derivePublicKey(publicKey, privateKey);
 * \endcode
 *
 * The application can store both the private and public key for later
 * signing operations.  Or it can store just the private key and then
 * derive the public key at the point where signing is to occur.
 *
 * Message signing produces a 64-byte signature as follows:
 *
 * \code
 * uint8_t message[N];
 * uint8_t signature[64];
 *
 * Ed25519::sign(signature, privateKey, publicKey, message, N);
 * \endcode
 *
 * And then to verify the signature:
 *
 * \code
 * if (!Ed25519::verify(signature, publicKey, message, N)) {
 *     // The signature is invalid.
 *     ...
 * }
 * \endcode
 *
 * \note The public functions in this class need a substantial amount of
 * stack space to store intermediate results while the curve function is
 * being evaluated.  About 1.5k of free stack space is recommended for safety.
 *
 * References: https://tools.ietf.org/html/draft-irtf-cfrg-eddsa-05
 *
 * \sa Curve25519
 */

/** @cond */

// 37095705934669439343138083508754565189542113879843219016388785533085940283555
static limb_t const numD[NUM_LIMBS_256BIT] PROGMEM = {
    LIMB_PAIR(0x135978A3, 0x75EB4DCA), LIMB_PAIR(0x4141D8AB, 0x00700A4D),
    LIMB_PAIR(0x7779E898, 0x8CC74079), LIMB_PAIR(0x2B6FFE73, 0x52036CEE)
};

// d * 2
static limb_t const numDx2[NUM_LIMBS_256BIT] PROGMEM = {
    LIMB_PAIR(0x26B2F159, 0xEBD69B94), LIMB_PAIR(0x8283B156, 0x00E0149A),
    LIMB_PAIR(0xEEF3D130, 0x198E80F2), LIMB_PAIR(0x56DFFCE7, 0x2406D9DC)
};

// Extended homogenous co-ordinates for the base point.
static limb_t const numBx[NUM_LIMBS_256BIT] PROGMEM = {
    LIMB_PAIR(0x8F25D51A, 0xC9562D60), LIMB_PAIR(0x9525A7B2, 0x692CC760),
    LIMB_PAIR(0xFDD6DC5C, 0xC0A4E231), LIMB_PAIR(0xCD6E53FE, 0x216936D3)
};
static limb_t const numBy[NUM_LIMBS_256BIT] PROGMEM = {
    LIMB_PAIR(0x66666658, 0x66666666), LIMB_PAIR(0x66666666, 0x66666666),
    LIMB_PAIR(0x66666666, 0x66666666), LIMB_PAIR(0x66666666, 0x66666666)
};
static limb_t const numBz[NUM_LIMBS_256BIT] PROGMEM = {
    LIMB_PAIR(0x00000001, 0x00000000), LIMB_PAIR(0x00000000, 0x00000000),
    LIMB_PAIR(0x00000000, 0x00000000), LIMB_PAIR(0x00000000, 0x00000000)
};
static limb_t const numBt[NUM_LIMBS_256BIT] PROGMEM = {
    LIMB_PAIR(0xA5B7DDA3, 0x6DDE8AB3), LIMB_PAIR(0x775152F5, 0x20F09F80),
    LIMB_PAIR(0x64ABE37D, 0x66EA4E8E), LIMB_PAIR(0xD78B7665, 0x67875F0F)
};

// 2^252 + 27742317777372353535851937790883648493
static limb_t const numQ[NUM_LIMBS_256BIT] PROGMEM = {
    LIMB_PAIR(0x5CF5D3ED, 0x5812631A), LIMB_PAIR(0xA2F79CD6, 0x14DEF9DE),
    LIMB_PAIR(0x00000000, 0x00000000), LIMB_PAIR(0x00000000, 0x10000000)
};

/** @endcond */

/**
 * \brief Signs a message using a specific Ed25519 private key.
 *
 * \param signature The signature value.
 * \param privateKey The private key to use to sign the message.
 * \param publicKey The public key corresponding to \a privateKey.
 * \param message Points to the message to be signed.
 * \param len The length of the \a message to be signed.
 *
 * \sa verify(), derivePublicKey()
 */
void Ed25519::sign(uint8_t signature[64], const uint8_t privateKey[32],
                   const uint8_t publicKey[32], const void *message, size_t len)
{
    SHA512 hash;
    uint8_t *buf = (uint8_t *)(hash.state.w); // Reuse hash buffer to save memory.
    limb_t a[NUM_LIMBS_256BIT];
    limb_t r[NUM_LIMBS_256BIT];
    limb_t k[NUM_LIMBS_256BIT];
    limb_t t[NUM_LIMBS_512BIT + 1];
    Point rB;

    // Derive the secret scalar a and the message prefix from the private key.
    deriveKeys(&hash, a, privateKey);

    // Hash the prefix and the message to derive r.
    hash.reset();
    hash.update(buf + 32, 32);
    hash.update(message, len);
    hash.finalize(buf, 0);
    reduceQFromBuffer(r, buf, t);

    // Encode rB into the first half of the signature buffer as R.
    mul(rB, r);
    encodePoint(signature, rB);

    // Hash R, A, and the message to get k.
    hash.reset();
    hash.update(signature, 32); // R
    hash.update(publicKey, 32); // A
    hash.update(message, len);
    hash.finalize(buf, 0);
    reduceQFromBuffer(k, buf, t);

    // Compute s = (r + k * a) mod q.
    Curve25519::mulNoReduce(t, k, a);
    t[NUM_LIMBS_512BIT] = 0;
    reduceQ(t, t);
    BigNumberUtil::add(t, t, r, NUM_LIMBS_256BIT);
    BigNumberUtil::reduceQuick_P(t, t, numQ, NUM_LIMBS_256BIT);
    BigNumberUtil::packLE(signature + 32, 32, t, NUM_LIMBS_256BIT);

    // Clean up.
    clean(a);
    clean(r);
    clean(k);
    clean(t);
    clean(rB);
}

/**
 * \brief Determines if the S half of a signature is below the group order q.
 *
 * \param s The 32 bytes of S, little-endian.
 *
 * \return Returns true if S < q.
 *
 * RFC 8032 requires it: S + q, S + 2q, ... verify like S, so without the
 * check one signature has several valid encodings.  The signature is
 * public, so this does not need to run in constant time.
 */
static bool isCanonicalS(const uint8_t s[32])
{
    limb_t S[NUM_LIMBS_256BIT];
    limb_t diff[NUM_LIMBS_256BIT];
    BigNumberUtil::unpackLE(S, NUM_LIMBS_256BIT, s, 32);
    return BigNumberUtil::sub_P(diff, S, numQ, NUM_LIMBS_256BIT) != 0;
}

/**
 * \brief Verifies a signature using a specific Ed25519 public key.
 *
 * \param signature The signature value to be verified.
 * \param publicKey The public key to use to verify the signature.
 * \param message The message whose signature is to be verified.
 * \param len The length of the \a message to be verified.
 *
 * \return Returns true if the \a signature is valid for \a message;
 * or false if the \a signature is not valid.
 *
 * A signature whose S half is not below the group order is not valid
 * (RFC 8032, section 5.1.7).
 *
 * \sa sign()
 */
bool Ed25519::verify(const uint8_t signature[64], const uint8_t publicKey[32],
                     const void *message, size_t len)
{
    SHA512 hash;
    Point A;
    Point R;
    Point sB;
    Point kA;
    uint8_t *k = (uint8_t *)(hash.state.w); // Reuse hash buffer to save memory.
    bool result = false;

    // Check S, then decode the public key and the R component of the signature.
    if (isCanonicalS(signature + 32) && decodePoint(A, publicKey) &&
            decodePoint(R, signature)) {
        // Reconstruct the k value from the signing step.
        hash.reset();
        hash.update(signature, 32);
        hash.update(publicKey, 32);
        hash.update(message, len);
        hash.finalize(k, 0);

        // Calculate s * B.  The s value is stored temporarily in kA.t.
        BigNumberUtil::unpackLE(kA.t, NUM_LIMBS_256BIT, signature + 32, 32);
        mul(sB, kA.t, false);

        // Calculate R + k * A.  We don't need sB.t in equal() below,
        // so we reuse that as a temporary buffer when reducing k.
        reduceQFromBuffer(sB.t, k, kA.x);
        mul(kA, sB.t, A, false);
        add(R, kA);

        // Compare s * B and R + k * A for equality.
        result = equal(sB, R);
    }

    // Clean up and exit.
    clean(A);
    clean(R);
    clean(sB);
    clean(kA);
    return result;
}

/**
 * \brief Generates a private key for Ed25519 signing operations.
 *
 * \param privateKey The resulting private key.
 *
 * The private key is generated with \link RNGClass::rand() CryptRNG.rand()\endlink.
 * It is the caller's responsibility to ensure that the global random number
 * pool has sufficient entropy to generate the 32 bytes of the key safely
 * before calling this function.
 *
 * \sa derivePublicKey()
 */
void Ed25519::generatePrivateKey(uint8_t privateKey[32])
{
    CryptRNG.rand(privateKey, 32);
}

/**
 * \brief Derives the public key from a private key.
 *
 * \param publicKey The public key.
 * \param privateKey The private key.
 *
 * \sa generatePrivateKey()
 */
void Ed25519::derivePublicKey(uint8_t publicKey[32], const uint8_t privateKey[32])
{
    SHA512 hash;
    limb_t a[NUM_LIMBS_256BIT];
    Point ptA;

    // Derive the secret scalar a from the private key.
    deriveKeys(&hash, a, privateKey);

    // Compute the point A = aB and encode it.
    mul(ptA, a);
    encodePoint(publicKey, ptA);

    // Clean up and exit.
    clean(a);
    clean(ptA);
}

/**
 * \brief Reduces a number modulo q that was specified in a 512 bit buffer.
 *
 * \param result The result array, which must be NUM_LIMBS_256BIT limbs in size.
 * \param buf The buffer containing the value to reduce in little-endian order.
 * \param temp A temporary buffer of at least NUM_LIMBS_512BIT + 1 in size.
 *
 * \sa reduceQ()
 */
void Ed25519::reduceQFromBuffer(limb_t *result, const uint8_t buf[64], limb_t *temp)
{
    BigNumberUtil::unpackLE(temp, NUM_LIMBS_512BIT, buf, 64);
    temp[NUM_LIMBS_512BIT] = 0;
    reduceQ(result, temp);
}

/**
 * \brief Reduces a number modulo q.
 *
 * \param result The result array, which must be NUM_LIMBS_256BIT limbs in size.
 * \param r The value to reduce, which must be NUM_LIMBS_512BIT + 1
 * limbs in size.
 *
 * The \a r array will be modified by this function as a side effect of
 * the division.  It is allowed for \a result to be the same as \a r.
 *
 * \sa reduceQFromBuffer()
 */
void Ed25519::reduceQ(limb_t *result, limb_t *r)
{
    // Algorithm from: http://en.wikipedia.org/wiki/Barrett_reduction
    //
    // We assume that r is less than or equal to (q - 1)^2.
    //
    // We want to compute result = r mod q.  Find the smallest k such
    // that 2^k > q.  In our case, k = 253.  Then set m = floor(4^k / q)
    // and let r = r - q * floor(m * r / 4^k).  This will be the result
    // or it will be at most one subtraction of q away from the result.
    //
    // Note: 4^k = 4^253 = 2^506 = 2^512/2^6.  We can more easily compute
    // the result we want if we set m = floor(4^k * 2^6 / q) instead and
    // then r = r - q * floor(m * r / 2^512).  Because the slight extra
    // precision in m, r is at most two subtractions of q away from the
    // final result.
    static limb_t const numM[NUM_LIMBS_256BIT + 1] PROGMEM = {
        LIMB_PAIR(0x0A2C131B, 0xED9CE5A3), LIMB_PAIR(0x086329A7, 0x2106215D),
        LIMB_PAIR(0xFFFFFFEB, 0xFFFFFFFF), LIMB_PAIR(0xFFFFFFFF, 0xFFFFFFFF),
        0x0F
    };
    limb_t temp[NUM_LIMBS_512BIT + NUM_LIMBS_256BIT + 1];

    // Multiply r by m.
    BigNumberUtil::mul_P(temp, r, NUM_LIMBS_512BIT, numM, NUM_LIMBS_256BIT + 1);

    // Multiply (m * r) / 2^512 by q and subtract it from r.
    // We can ignore the high words of the subtraction result
    // because they will all turn into zero after the subtraction.
    BigNumberUtil::mul_P(temp, temp + NUM_LIMBS_512BIT, NUM_LIMBS_256BIT + 1,
                         numQ, NUM_LIMBS_256BIT);
    BigNumberUtil::sub(r, r, temp, NUM_LIMBS_256BIT);

    // Perform two subtractions of q from the result to reduce it.
    BigNumberUtil::reduceQuick_P(result, r, numQ, NUM_LIMBS_256BIT);
    BigNumberUtil::reduceQuick_P(result, result, numQ, NUM_LIMBS_256BIT);

    // Clean up and exit.
    clean(temp);
}

/**
 * \brief Multiplies a value by a curve point.
 *
 * \param result The result of the multiplication.
 * \param s The value, which must be NUM_LIMBS_256BIT limbs in size.
 * \param p The curve point, which will be modified by this function.
 * \param constTime Set to true if the evaluation must be constant-time
 * because \a s is a secret value.
 */
void Ed25519::mul(Point &result, const limb_t *s, Point &p, bool constTime)
{
    Point q;
    limb_t A[NUM_LIMBS_256BIT];
    limb_t B[NUM_LIMBS_256BIT];
    limb_t C[NUM_LIMBS_256BIT];
    limb_t D[NUM_LIMBS_256BIT];
    limb_t mask, select;
    uint8_t sposn, t;

    // Initialize the result to (0, 1, 1, 0).
    memset(&result, 0, sizeof(Point));
    result.y[0] = 1;
    result.z[0] = 1;

    // Iterate over the 255 bits of "s" to calculate "s * p".
    mask = 1;
    sposn = 0;
    for (t = 255; t > 0; --t) {
        // Add p to the result to produce q.  The specification refers
        // to temporary variables A to H.  We can dispense with E to H
        // by using B, D, q.z, and q.t to hold those values temporarily.
        select = s[sposn] & mask;
        if (constTime || select) {
            Curve25519::sub(A, result.y, result.x);
            Curve25519::sub(C, p.y, p.x);
            Curve25519::mul(A, A, C);
            Curve25519::add(B, result.y, result.x);
            Curve25519::add(C, p.y, p.x);
            Curve25519::mul(B, B, C);
            Curve25519::mul(C, result.t, p.t);
            Curve25519::mul_P(C, C, numDx2);
            Curve25519::mul(D, result.z, p.z);
            Curve25519::add(D, D, D);
            Curve25519::sub(q.t, B, A);             // E = B - A
            Curve25519::sub(q.z, D, C);             // F = D - C
            Curve25519::add(D, D, C);               // G = D + C
            Curve25519::add(B, B, A);               // H = B + A
            if (constTime) {
                // Put the intermediate value into q.
                Curve25519::mul(q.x, q.t, q.z);         // q.x = E * F
                Curve25519::mul(q.y, D, B);             // q.y = G * H
                Curve25519::mul(q.z, q.z, D);           // q.z = F * G
                Curve25519::mul(q.t, q.t, B);           // q.t = E * H

                // Copy q into the result if the current bit of s is 1.
                Curve25519::cmove(select, result.x, q.x);
                Curve25519::cmove(select, result.y, q.y);
                Curve25519::cmove(select, result.z, q.z);
                Curve25519::cmove(select, result.t, q.t);
            } else {
                // Put the intermediate value directly into the result.
                Curve25519::mul(result.x, q.t, q.z);     // q.x = E * F
                Curve25519::mul(result.y, D, B);         // q.y = G * H
                Curve25519::mul(result.z, q.z, D);       // q.z = F * G
                Curve25519::mul(result.t, q.t, B);       // q.t = E * H
            }
        }

        // Double p for the next iteration.
        Curve25519::sub(A, p.y, p.x);
        Curve25519::square(A, A);
        Curve25519::add(B, p.y, p.x);
        Curve25519::square(B, B);
        Curve25519::square(C, p.t);
        Curve25519::mul_P(C, C, numDx2);
        Curve25519::square(D, p.z);
        Curve25519::add(D, D, D);
        Curve25519::sub(p.t, B, A);             // E = B - A
        Curve25519::sub(p.z, D, C);             // F = D - C
        Curve25519::add(D, D, C);               // G = D + C
        Curve25519::add(B, B, A);               // H = B + A
        Curve25519::mul(p.x, p.t, p.z);         // p.x = E * F
        Curve25519::mul(p.y, D, B);             // p.y = G * H
        Curve25519::mul(p.z, p.z, D);           // p.z = F * G
        Curve25519::mul(p.t, p.t, B);           // p.t = E * H

        // Move onto the next bit of s from lowest to highest.
        if (mask != (((limb_t)1) << (LIMB_BITS - 1))) {
            mask <<= 1;
        } else {
            ++sposn;
            mask = 1;
        }
    }

    // Clean up.
    clean(q);
    clean(A);
    clean(B);
    clean(C);
    clean(D);
}

/**
 * \brief Multiplies a value by the base point of the curve.
 *
 * \param result The result of the multiplication.
 * \param s The value, which must be NUM_LIMBS_256BIT limbs in size.
 * \param constTime Set to true if the evaluation must be constant-time
 * because \a s is a secret values.
 */
void Ed25519::mul(Point &result, const limb_t *s, bool constTime)
{
    Point P;
    memcpy_P(P.x, numBx, sizeof(P.x));
    memcpy_P(P.y, numBy, sizeof(P.y));
    memcpy_P(P.z, numBz, sizeof(P.z));
    memcpy_P(P.t, numBt, sizeof(P.t));
    mul(result, s, P, constTime);
    clean(P);
}

/**
 * \brief Adds two curve points.
 *
 * \param p The first point and the result.
 * \param q The second point.
 */
void Ed25519::add(Point &p, const Point &q)
{
    limb_t A[NUM_LIMBS_256BIT];
    limb_t B[NUM_LIMBS_256BIT];
    limb_t C[NUM_LIMBS_256BIT];
    limb_t D[NUM_LIMBS_256BIT];

    Curve25519::sub(A, p.y, p.x);
    Curve25519::sub(C, q.y, q.x);
    Curve25519::mul(A, A, C);
    Curve25519::add(B, p.y, p.x);
    Curve25519::add(C, q.y, q.x);
    Curve25519::mul(B, B, C);
    Curve25519::mul(C, p.t, q.t);
    Curve25519::mul_P(C, C, numDx2);
    Curve25519::mul(D, p.z, q.z);
    Curve25519::add(D, D, D);
    Curve25519::sub(p.t, B, A);             // E = B - A
    Curve25519::sub(p.z, D, C);             // F = D - C
    Curve25519::add(D, D, C);               // G = D + C
    Curve25519::add(B, B, A);               // H = B + A
    Curve25519::mul(p.x, p.t, p.z);         // p.x = E * F
    Curve25519::mul(p.y, D, B);             // p.y = G * H
    Curve25519::mul(p.z, p.z, D);           // p.z = F * G
    Curve25519::mul(p.t, p.t, B);           // p.t = E * H

    clean(A);
    clean(B);
    clean(C);
    clean(D);
}

/**
 * \brief Determine if two curve points are equal.
 *
 * \param p The first curve point.
 * \param q The second curve point.
 *
 * \return Returns true if \a p and \a q are equal; false otherwise.
 */
bool Ed25519::equal(const Point &p, const Point &q)
{
    limb_t a[NUM_LIMBS_256BIT];
    limb_t b[NUM_LIMBS_256BIT];
    bool result = true;

    Curve25519::mul(a, p.x, q.z);
    Curve25519::mul(b, q.x, p.z);
    result &= secure_compare(a, b, sizeof(a));

    Curve25519::mul(a, p.y, q.z);
    Curve25519::mul(b, q.y, p.z);
    result &= secure_compare(a, b, sizeof(a));

    clean(a);
    clean(b);
    return result;
}

/**
 * \brief Encodes a curve point into a 32-byte buffer.
 *
 * \param buf The buffer to encode into.
 * \param point The curve point to encode.  This value will be modified
 * the function and effectively destroyed.
 *
 * \sa decodePoint()
 */
void Ed25519::encodePoint(uint8_t *buf, Point &point)
{
    // Convert the homogeneous coordinates into plain (x, y) coordinates:
    //      zinv = z^(-1) mod p
    //      x = x * zinv  mod p
    //      y = y * zinv  mod p
    // We don't need the t coordinate, so use that to store zinv temporarily.
    Curve25519::recip(point.t, point.z);
    Curve25519::mul(point.x, point.x, point.t);
    Curve25519::mul(point.y, point.y, point.t);

    // Copy the lowest bit of x to the highest bit of y.
    point.y[NUM_LIMBS_256BIT - 1] |= (point.x[0] << (LIMB_BITS - 1));

    // Convert y into little-endian in the return buffer.
    BigNumberUtil::packLE(buf, 32, point.y, NUM_LIMBS_256BIT);
}

/**
 * \brief Decodes a curve point from a 32-byte buffer.
 *
 * \param point The curve point that was decoded from the buffer.
 * \param buf The buffer to decode.
 *
 * \return Returns true if the point was decoded or false if the contents
 * of the buffer do not correspond to a legitimate curve point.
 *
 * \note This function is not constant time so it should only be used
 * on publicly-known values.
 */
bool Ed25519::decodePoint(Point &point, const uint8_t *buf)
{
    limb_t temp[NUM_LIMBS_256BIT];

    // Convert the input buffer from little-endian into the limbs of y.
    BigNumberUtil::unpackLE(point.y, NUM_LIMBS_256BIT, buf, 32);

    // The high bit of y is the sign bit for x.
    limb_t sign = point.y[NUM_LIMBS_256BIT - 1] >> (LIMB_BITS - 1);
    point.y[NUM_LIMBS_256BIT - 1] &= ~(((limb_t)1) << (LIMB_BITS - 1));

    // Set z to 1.
    memcpy_P(point.z, numBz, sizeof(point.z));

    // Compute t = (y * y - 1) * modinv(d * y * y + 1).
    Curve25519::square(point.t, point.y);
    Curve25519::sub(point.x, point.t, point.z);
    Curve25519::mul_P(point.t, point.t, numD);
    Curve25519::add(point.t, point.t, point.z);
    Curve25519::recip(temp, point.t);
    Curve25519::mul(point.t, point.x, temp);
    clean(temp);

    // Check for t = 0.
    limb_t check = point.t[0];
    for (uint8_t posn = 1; posn < NUM_LIMBS_256BIT; ++posn)
        check |= point.t[posn];
    if (!check) {
        // If the sign bit is set, then decoding has failed.
        // Otherwise x is zero and we're done.
        if (sign)
            return false;
        memset(point.x, 0, sizeof(point.x));
        return true;
    }

    // Recover x by taking the sqrt of t and flipping the sign if necessary.
    if (!Curve25519::sqrt(point.x, point.t))
        return false;
    if (sign != (point.x[0] & ((limb_t)1))) {
        // The signs are different so we want the other square root.
        memset(point.t, 0, sizeof(point.t));
        Curve25519::sub(point.x, point.t, point.x);
    }

    // Finally, t = x * y.
    Curve25519::mul(point.t, point.x, point.y);
    return true;
}

/**
 * \brief Derive key material from a 32-byte private key.
 *
 * \param hash SHA512 hash object from the caller for use in this function.
 * The 64-byte output buffer within this hash object will contain the
 * hash prefix on exit.
 * \param a The secret scalar derived from \a privateKey.  This must be
 * NUM_LIMBS_256BIT limbs in size.
 * \param privateKey The 32-byte private key to derive all other values from.
 */
void Ed25519::deriveKeys(SHA512 *hash, limb_t *a, const uint8_t privateKey[32])
{
    // Hash the private key to get the "a" scalar and the message prefix.
    uint8_t *buf = (uint8_t *)(hash->state.w); // Reuse hash buffer to save memory.
    hash->reset();
    hash->update(privateKey, 32);
    hash->finalize(buf, 0);
    buf[0]  &= 0xF8;
    buf[31] &= 0x7F;
    buf[31] |= 0x40;

    // Unpack the first half of the hash value into "a".
    BigNumberUtil::unpackLE(a, NUM_LIMBS_256BIT, buf, 32);
}
