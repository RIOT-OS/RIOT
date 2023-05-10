/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the pointer tagging helpers
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>

#include "kernel_defines.h"
#include "ptrtag.h"
#include "test_utils/expect.h"

typedef struct PTRTAG {
    uint8_t a;
} custom_type_t;

/* To rule out that custom type gets aligned by 4 bytes only by chance, we
 * allocate two subsequently. At least one would be unaligned, if the
 * alignment requirement is not increased to at least 4
 */
static custom_type_t a;
static custom_type_t b;
uint8_t PTRTAG c;
uint8_t PTRTAG d;

int main(void)
{
    puts(
        "Test Application for sys/ptrtag\n"
        "=====================================\n"
    );

    void *ptrs[] = {&a, &b, &c, &d};
    for (uint8_t tag = 0; tag < 4; tag++) {
        for (size_t i = 0; i < ARRAY_SIZE(ptrs); i++) {
            void *ptr = ptrs[i];
            void *tagged = ptrtag(ptr, tag);
            expect(ptrtag_ptr(tagged) == ptr);
            expect(ptrtag_tag(tagged) == tag);
        }
    }

    puts("TEST PASSED");

    return 0;
}
