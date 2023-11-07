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
 * @brief       Declarations for the core/xfa unit test
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */
#ifndef TESTS_CORE_XFA_H
#define TESTS_CORE_XFA_H

#include "architecture.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned val;
    const char *text;
} xfatest_t;

typedef struct WORD_ALIGNED {
    const char text[31];
} xfatest_noalign_t;

typedef struct WORD_ALIGNED {
    const char text[33];
} xfatest_noalign_33_t;

#ifdef __cplusplus
}
#endif

#endif /* TESTS_CORE_XFA_H */

/** @} */
