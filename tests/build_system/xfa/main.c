/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     test
 * @{
 *
 * @file
 * @brief       cross file array (XFA) test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "xfa.h"

#include "xfatest.h"

XFA_INIT(xfatest_t, xfatest);
XFA_INIT_CONST(xfatest_t, xfatest_const);

int main(void)
{
    puts("Cross file array test");

    unsigned n = XFA_LEN(xfatest_t, xfatest);
    printf("xfatest[%u]:\n", n);
    for (unsigned i = 0; i < n; i++) {
        printf("[%u] = %u, \"%s\", '%c'\n",
                i, xfatest[i].val, xfatest[i].text, xfatest[i].letter);
    }
    n = XFA_LEN(xfatest_t, xfatest_const);
    printf("xfatest_const[%u]:\n", n);
    for (unsigned i = 0; i < n; i++) {
        printf("[%u] = %u, \"%s\", '%c'\n",
                i, xfatest_const[i].val, xfatest_const[i].text, xfatest_const[i].letter);
    }

    return 0;
}
