/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Test application checking behavior of assert and assert_ng
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#define NDEBUG 1

#include <stdio.h>
#include <stdlib.h>

#include "assert.h"

static int _passed = 1;

__attribute__((unused))
static int func_used_in_assert(void)
{
    puts("ERROR: function call in assert() is still executed with NDEBUG!");
    _passed = 0;
    return 0;
}

int main(void)
{
    assert(func_used_in_assert());

    if (_passed) {
        puts("Test of behavior with NDEBUG: PASSED");
        extern void test_failed_assertion(void);
        test_failed_assertion();
    }
    else {
        puts("TEST FAILED");
    }
    return 0;
}
