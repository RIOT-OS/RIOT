/*
 * SPDX-FileCopyrightText: 2020 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 *
 * @file
 * @brief       Test cases for the SHA1PRNG pseudo random number generator
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 */

#include <stdio.h>
#include <string.h>

#include "kernel_defines.h"
#include "random.h"

/**
 * @brief expected sequence for seed=1. This sequence was generated running the
 *        following java program (openjdk 11.0.7) as a reference.
 *
 *~~~~
 * import java.security.SecureRandom;
 *
 * public class SHA1PRNGTEST {
 *   public static void main(String args[]) throws Exception {
 *     SecureRandom random = SecureRandom.getInstance("SHA1PRNG");
 *     random.setSeed(1);
 *     int number = 0;
 *     for (int i = 0; i < 20; i++) {
 *         number = random.nextInt();
 *         System.out.print(Integer.toUnsignedString(number) + " ");
 *     }
 *     System.out.println("");
 *   }
 * }
 *~~~~
 */
static const uint32_t seq_seed1[] =
   {2529905901, 3336014406, 1714755920, 3709666991, 1432426612,  554064022,
    1614405352,  861636861, 3689098857, 3893737371, 3138964692,  506954022,
    3469584855, 4144207589, 2031557795, 3248917850, 2384338299, 3341545824,
    2454801916, 3985646079};

static void test_prng_sha1prng_java_u32(void)
{
    uint32_t seed[2] = {1, 0};

    uint32_t test32[ARRAY_SIZE(seq_seed1)];

    /* seed the generator with 8 bytes similar to the java reference
     * implementation
     */
    random_init_by_array(seed, ARRAY_SIZE(seed));

    /* request random samples */
    for (unsigned i = 0; i < ARRAY_SIZE(seq_seed1); i++) {
        test32[i] = random_uint32();
    }

    /* compare generator output and reference */
    if (!(memcmp(test32, seq_seed1, sizeof(seq_seed1)))) {
        printf("%s:SUCCESS\n", __func__);
    }
    else {
        printf("%s:FAILURE\n", __func__);
    }
}

int main(void)
{

    test_prng_sha1prng_java_u32();

    return 0;
}
