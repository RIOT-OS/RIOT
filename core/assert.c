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

#include "assert.h"
#include "fmt.h"

#ifdef DEBUG_ASSERT_VERBOSE
NORETURN void _assert_failure(const char *file, unsigned line)
{
    print_str(file);
    print_str(":");
    print_u32_dec(line);
    print_str(" => ");
    core_panic(PANIC_ASSERT_FAIL, assert_crash_message); \
}
#else
typedef int dont_be_pedantic;
#endif

/** @} */
