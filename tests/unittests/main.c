/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <stdlib.h>
#include "map.h"

#include "embUnit.h"
#include "xtimer.h"
#include "ztimer.h"

#include "test_utils/interactive_sync.h"
#if IS_USED(MODULE_VFS)
#include "vfs.h"
#endif

#define UNCURRY(FUN, ARGS) FUN(ARGS)
#define RUN_TEST_SUITES(...) MAP(RUN_TEST_SUITE, __VA_ARGS__)
#define RUN_TEST_SUITE(TEST_SUITE) \
    do { \
        extern void tests_##TEST_SUITE(void); \
        tests_##TEST_SUITE(); \
    } while (0);

int main(void)
{
    test_utils_interactive_sync();

#if MODULE_ZTIMER_USEC || MODULE_ZTIMER_MSEC || MODULE_ZTIMER_SEC
    /* auto_init is disabled, but some modules depends on this module being initialized */
    ztimer_init();
#endif

#if MODULE_ZTIMER64_USEC || MODULE_ZTIMER64_MSEC || MODULE_ZTIMER64_SEC
    /* auto_init is disabled, but some modules depends on this module being initialized */
    ztimer64_init();
#endif

#if IS_USED(MODULE_XTIMER) && !IS_USED(MODULE_ZTIMER_XTIMER_COMPAT)
    /* auto_init is disabled, but some modules depends on this module being initialized */
    xtimer_init();
#endif

#if IS_USED(MODULE_FS_NATIVE) && IS_USED(MODULE_VFS_DEFAULT)
    extern void auto_init_vfs(void);
    auto_init_vfs();
#endif

#ifdef OUTPUT
    TextUIRunner_setOutputter(OUTPUTTER);
#endif

    TESTS_START();
#ifndef NO_TEST_SUITES
    UNCURRY(RUN_TEST_SUITES, TEST_SUITES)
#endif
    TESTS_END();
    exit(0);
}
