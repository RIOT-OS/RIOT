/*
 * Copyright (C) 2023 Freie Universität Berlin
 *               2023 Inria
 *               2023 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */
#include "xfa.h"
#include "tests-core-xfa.h"

XFA_CONST(xfatest_noalign_const, 2) xfatest_noalign_t _xfatest_noalign_const_1 =
    { .text = "noalign const 1" };

XFA_CONST(xfatest_noalign_const, 1) xfatest_noalign_t _xfatest_noalign_const_2 =
    { .text = "noalign const 2" };

XFA_CONST(xfatest_noalign_const, 0) xfatest_noalign_t _xfatest_noalign_const_3 =
    { .text = "noalign const 3" };

XFA_CONST(xfatest_noalign_const_sameprio, 0) xfatest_noalign_t _xfatest_noalign_const_sameprio_1 =
    { .text = "noalign const sameprio 1" };

XFA_CONST(xfatest_noalign_const_sameprio, 0) xfatest_noalign_t _xfatest_noalign_const_sameprio_2 =
    { .text = "noalign const sameprio 2" };

XFA_CONST(xfatest_noalign_const_sameprio, 0) xfatest_noalign_t _xfatest_noalign_const_sameprio_3 =
    { .text = "noalign const sameprio 3" };

XFA_CONST(xfatest_noalign_33_const, 2) xfatest_noalign_33_t _xfatest_noalign_33_const_1 =
    { .text = "noalign 33 const 1" };

XFA_CONST(xfatest_noalign_33_const, 1) xfatest_noalign_33_t _xfatest_noalign_33_const_2 =
    { .text = "noalign 33 const 2" };

XFA_CONST(xfatest_noalign_33_const, 0) xfatest_noalign_33_t _xfatest_noalign_33_const_3 =
    { .text = "noalign 33 const 3" };

XFA_CONST(xfatest_noalign_33_const_sameprio, 0) xfatest_noalign_33_t _xfatest_noalign_33_const_sameprio_1 =
    { .text = "noalign 33 const sameprio 1" };

XFA_CONST(xfatest_noalign_33_const_sameprio, 0) xfatest_noalign_33_t _xfatest_noalign_33_const_sameprio_2 =
    { .text = "noalign 33 const sameprio 2" };

XFA_CONST(xfatest_noalign_33_const_sameprio, 0) xfatest_noalign_33_t _xfatest_noalign_33_const_sameprio_3 =
    { .text = "noalign 33 const sameprio 3" };

/** @} */
