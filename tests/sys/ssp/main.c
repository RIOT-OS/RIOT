/*
 * SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       ssp test application
 *
 * This test should crash badly when *not* using the ssp module, and panic if
 * using it.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

__attribute__((weak)) void
_prevent_memset_lto(void *const  s, int c, const size_t n)
{
    (void) s;
    (void) c;
    (void) n;
}

void test_func(void)
{
    char buf[16];

    /* clang will detect the buffer overflow
     * and throw an error if we use `buf` directly */
    void *buffer_to_confuse_compiler = buf;

    /* cppcheck-suppress bufferAccessOutOfBounds
     * (reason: deliberately overflowing stack) */
    memset(buffer_to_confuse_compiler, 0, 32);

    /* prevent that memset call is optimized out by LTO */
    _prevent_memset_lto(buf, 0, 32);
}

int main(void)
{
    puts("calling stack corruption function");

    test_func();

    puts("back to main");

    return 0;
}
