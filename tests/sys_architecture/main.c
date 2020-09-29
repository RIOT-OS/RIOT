/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for sys/architecture
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "assert.h"
#include "architecture.h"

/* On all but 8bit architectures, at least one of the following should be
 * misaligned */
static char unaligned_a[1];
static char unaligned_b[1];

/* If WORD_ALIGNED is not working correctly, at least one of the following
 * should be misaligned (except for 8bit architectures, on which alignment
 * doesn't matter). */
static char WORD_ALIGNED aligned_a[1];
static char WORD_ALIGNED aligned_b[1];

static inline unsigned get_misalignemnt(const char *ptr)
{
    /* word size is always a power of two, so we can speed up the modulo */
    return (uintptr_t)ptr & (ARCHITECTURE_WORD_BYTES - 1);
}

int main(void)
{
    /* Most can be evaluated at compile time, so we just do this :-)
     * Using multiple static asserts in a single function doesn't play nicely
     * with the non-C11 fallback implementation of static_assert, so we just
     * use a single use statement.
     */
    static_assert(
        (ARCHITECTURE_WORD_BITS == CORRECT_WORD_BITS) &&
        (ARCHITECTURE_WORD_BYTES == CORRECT_WORD_BITS / 8) &&
        (sizeof(uword_t) == ARCHITECTURE_WORD_BYTES) &&
        (sizeof(sword_t) == ARCHITECTURE_WORD_BYTES),
        "word size details are incorrect"
    );

    printf("One word is %u bits or %u bytes in size\n",
           ARCHITECTURE_WORD_BITS, ARCHITECTURE_WORD_BYTES);

    int failed = 0;

    if (ARCHITECTURE_WORD_BYTES == 1) {
        puts("Skipping test for WORD_ALIGNED; every alignment is fine on 8bit "
             "architectures.");
    }
    else {
        printf("%s", "Verifying test works: ");
        if (get_misalignemnt(unaligned_a) || get_misalignemnt(unaligned_b)) {
            puts("OK");
        }
        else {
            puts("FAILED");
            failed = 1;
        }

        printf("%s", "Verifying WORD_ALIGNED works: ");
        if (get_misalignemnt(aligned_a) || get_misalignemnt(aligned_b)) {
            puts("FAILED");
            failed = 1;
        }
        else {
            puts("OK");
        }
    }

    if (failed) {
        puts("TEST FAILED");
    }
    else {
        puts("TEST SUCCEEDED");
    }

    return 0;
}
