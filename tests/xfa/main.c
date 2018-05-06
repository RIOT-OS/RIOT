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
#include <stdint.h>

#include "xfa.h"

#include "xfatest.h"

XFA_INIT(xfatest_t, xfatest);

/* hack to force inclusion of otherwise unused compilation units */
extern int hack1;
extern int hack2;

int main(void)
{
    unsigned n = XFA_LEN(xfatest_t, xfatest);
    for (unsigned i = 0; i < n; i++) {
        printf("%u %s\n", xfatest[i].val, xfatest[i].text);
    }

    hack1 = hack2;

    return 0;
}
