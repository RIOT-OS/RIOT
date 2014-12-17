/*
 * Copyright (C) 2014 René Kijewski
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "tests-lib.h"

void tests_lib(void)
{
    TESTS_RUN(tests_lib_ringbuffer_tests());
}
