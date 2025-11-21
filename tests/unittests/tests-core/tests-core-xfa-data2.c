/*
 * SPDX-FileCopyrightText: 2018 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
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

XFA(xfatest_t, xfatest, 0) _xfatest2 = { .val = 0xbeef, .text = "another test string" };
XFA_CONST(xfatest_t, xfatest_const, 0) _xfatest_const2 =
    { .val = 32444, .text = "const string xfa 2" };
XFA(xfatest_t, xfatest_use, 0) _xfatest_use2 = { .val = 11111, .text = "xfatest_use2" };
XFA_CONST(xfatest_t, xfatest_use_const, 0) _xfatest_use_const2 =
    { .val = 22222, .text = "xfatest_use_const2" };

/** @} */
