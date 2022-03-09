/*
 A C-program for MT19937, with initialization improved 2002/1/26.
 Coded by Takuji Nishimura and Makoto Matsumoto.

 Before using, initialize the state by using init_genrand(seed)
 or init_by_array(init_key, key_length).

 Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.

 3. The names of its contributors may not be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 Any feedback is very welcome.
 http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
 email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
 */

#include "random.h"

/** Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /** constant vector a */
#define UPPER_MASK 0x80000000UL /** most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /** least significant r bits */

#define MTI_UNINITIALIZED (N + 1)

static uint32_t mt[N]; /** the array for the state vector  */
static uint16_t mti = MTI_UNINITIALIZED;

void random_init(uint32_t s)
{
    mt[0] = s;
    for (int i = 1; i < N; ++i) {
        mt[i] = 1812433253UL * (mt[i - 1] ^ (mt[i - 1] >> 30)) + i;
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
    }

    mti = N;
}

void random_init_by_array(uint32_t *init_key, int key_length)
{
    random_init(19650218UL);
    int i = 1;
    int j = 0;
    for (int k = N > key_length ? N : key_length; k; --k) {
        mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1664525UL))
                + init_key[j] + j; /* non linear */
        ++i;
        ++j;
        if (i >= N) {
            mt[0] = mt[N - 1];
            i = 1;
        }
        if (j >= key_length) {
            j = 0;
        }
    }
    for (int k = N - 1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1566083941UL)) - i; /* non linear */
        i++;
        if (i >= N) {
            mt[0] = mt[N - 1];
            i = 1;
        }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
}

/** generates N words at one time */
static void generate_numbers(void)
{
    if (mti == MTI_UNINITIALIZED) {
        /* if init_genrand() has not been called, a default initial seed is used */
        random_init(5489UL);
    }

    for (int k = 0; k < N; ++k) {
        uint32_t y = (mt[k] & UPPER_MASK) | (mt[(k + 1) % N] & LOWER_MASK);
        mt[k] = mt[(k + M) % N] ^ (y >> 1);
        if (y & 1) {
            mt[k] ^= MATRIX_A;
        }
    }

    mti = 0;
}

uint32_t random_uint32(void)
{
    if (mti >= N) {
        generate_numbers();
    }

    uint32_t y = mt[mti++];
    y ^= y >> 11;
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= y >> 18;
    return y;
}

#if PRNG_FLOAT

#define TWO_POW_6 64.0
#define TWO_POW_26 67108864.0
#define TWO_POW_32_M1 4294967295.0
#define TWO_POW_32 4294967296.0
#define TWO_POW_53 9007199254740992.0

double random_real(void)
{
    return random_uint32() * (1.0 / TWO_POW_32);
}

double random_real_inclusive(void)
{
    return random_uint32() * (1.0 / TWO_POW_32_M1);
}

double random_real_exclusive(void)
{
    return ((double) random_uint32() + 0.5) * (1.0 / TWO_POW_32);
}

double random_res53(void)
{
    double a = random_uint32() * TWO_POW_26;
    double b = random_uint32() * (1.0 / TWO_POW_6);
    return (a + b) * (1.0 / TWO_POW_53);
}

#endif /* PRNG_FLOAT */
