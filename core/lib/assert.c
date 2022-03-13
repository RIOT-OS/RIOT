/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdio.h>

#include "assert.h"
#include "panic.h"

__NORETURN void _assert_failure(const char *file, unsigned line)
{
    printf("%s:%u => ", file, line);
    core_panic(PANIC_ASSERT_FAIL, "FAILED ASSERTION.");
}

__NORETURN void _assert_panic(void)
{
    core_panic(PANIC_ASSERT_FAIL, "FAILED ASSERTION.");
}

/** @} */
