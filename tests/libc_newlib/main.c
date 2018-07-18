/*
 * Copyright (C) 2018 Freie Universität Berlin
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
 * @brief       Test application for newlib/newlib-nano inclusion
 *
 * @author      Gaëtan Harter <gaetan.harter@fu-berlin.de>
 *
 * @}
 */

/*
 * Make some different functions visible between newlib and newlib-nano
 */
#define _DEFAULT_SOURCE 1

#include <stdio.h>
#include "embUnit.h"


#ifdef MODULE_NEWLIB
#include <newlib.h>

/* Newlib-nano defines the _NANO_FORMATTED_IO macro
 *
 * Source:
 *
 * https://sourceware.org/git/gitweb.cgi?p=newlib-cygwin.git;a=blob;f=newlib/newlib.hin;h=eadafc8a6a51ef7674c004a14777f6a4619115ee;hb=d34336767e45ee801ebb107e40abe4b1acbd3d14#l83
 */
#ifdef MODULE_NEWLIB_NANO
#ifndef _NANO_FORMATTED_IO
#error newlib-nano is enabled but the header is not visible by the build system
#endif /* _NANO_FORMATTED_IO */
#endif /* MODULE_NEWLIB_NANO */

#endif /* MODULE_NEWLIB */


/* Newlib-nano removed the integer only 'iprintf' functions which are now mapped
 * to printf.
 *
 * Source:
 *
 * https://github.com/32bitmicro/newlib-nano-1.0/blob/f157c994b9a2c4bd8d0cfe9fe8fdd9cd54f8c63b/newlib/README.nano#L32
 *
 */
static void test_newlib(void)
{
    /*
     * Be sure `iprintf` and `printf` are used when `newlib` is included as
     * they should be visible in the final elf file for compile time tests
     */

#ifdef MODULE_NEWLIB
#ifdef MODULE_NEWLIB_NANO
    /* Nano maps iprintf to printf */
    TEST_ASSERT(iprintf == printf);
#else
    /* Normal newlib does not */
    TEST_ASSERT(iprintf != printf);
#endif
#endif
}


static Test *tests_newlib(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_newlib),
    };

    EMB_UNIT_TESTCALLER(tests, NULL, NULL, fixtures);
    return (Test *)&tests;
}

int main(void)
{
    puts("Newlib/nano test");
    TESTS_START();
    TESTS_RUN(tests_newlib());
    TESTS_END();

    return 0;
}
