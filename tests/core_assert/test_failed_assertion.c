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

#include <stdio.h>
#include <stdlib.h>

#include "assert.h"

void test_failed_assertion(void)
{
    puts("Testing if assert(0) blows up without NDEBUG:");
    assert(0);
}
