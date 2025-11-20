/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Data elements for the core/xfa unit test
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */
#include "xfa.h"
#include "tests-core-xfa.h"

XFA(xfatest_t, xfatest, 0) _xfatest1 = { .val = 12345, .text = "xfatest1" };
XFA_CONST(xfatest_t, xfatest_const, 0) _xfatest_const1 = { .val = 0xcafe, .text = "xfatest_const1" };

XFA_INIT(xfatest_t, xfatest_use);
XFA_INIT_CONST(xfatest_t, xfatest_use_const);

XFA(xfatest_t, xfatest_use, 0) _xfatest_use1 = { .val = 3333, .text = "xfatest_use1" };
XFA(xfatest_t, xfatest_use, 0) _xfatest_use_again = { .val = 555, .text = "xfatest use again" };
XFA_CONST(xfatest_t, xfatest_use_const, 0) _xfatest_use_const1 =
    { .val = 4444, .text = "xfatest_use_const1" };

int hack1;
/** @} */
