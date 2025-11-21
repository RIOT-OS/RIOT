/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "kernel_defines.h"

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
     static_assert((ARCHITECTURE_WORD_BITS == CORRECT_WORD_BITS)
                   && (ARCHITECTURE_WORD_BYTES == CORRECT_WORD_BITS / 8)
                   && (sizeof(uword_t) == ARCHITECTURE_WORD_BYTES)
                   && (sizeof(sword_t) == ARCHITECTURE_WORD_BYTES)
                   && (UWORD_MIN == 0)
                   && ((ARCHITECTURE_WORD_BITS != 8) || (UWORD_MAX == 255))
                   && ((ARCHITECTURE_WORD_BITS != 8) || (SWORD_MIN == -128))
                   && ((ARCHITECTURE_WORD_BITS != 8) || (SWORD_MAX == 127))
                   && ((ARCHITECTURE_WORD_BITS != 16) || (UWORD_MAX == 65535))
                   && ((ARCHITECTURE_WORD_BITS != 16) || (SWORD_MIN == -32768))
                   && ((ARCHITECTURE_WORD_BITS != 16) || (SWORD_MAX == 32767))
                   && ((ARCHITECTURE_WORD_BITS != 32) || (UWORD_MAX == 4294967295))
                   && ((ARCHITECTURE_WORD_BITS != 32) || (SWORD_MIN == -2147483648))
                   && ((ARCHITECTURE_WORD_BITS != 32) || (SWORD_MAX == 2147483647)),
                   "word size details are incorrect");

    DECLARE_CONSTANT(_workaround, HAS_ALIGNMENT_OF((void *)3, 1)
                                  && HAS_ALIGNMENT_OF((void *)8, 2)
                                  && HAS_ALIGNMENT_OF((void *)8, 4)
                                  && HAS_ALIGNMENT_OF((void *)8, 8)
                                  && !HAS_ALIGNMENT_OF((void *)7, 2)
                                  && !HAS_ALIGNMENT_OF((void *)7, 4)
                                  && !HAS_ALIGNMENT_OF((void *)7, 8))
    static_assert(_workaround, "HAS_ALIGNMENT_OF() is not working");

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
