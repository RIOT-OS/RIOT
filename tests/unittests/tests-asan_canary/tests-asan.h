/*
 * Copyright (C) 2024 Christian Amsüss
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
 * @brief   Unittests canary for address sanitizer violations
 *
 * @author  Christian Amsüss <chrysn@fsfe.org>
 */
#ifndef TESTS_ASAN_H
#define TESTS_ASAN_H

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 *
 * If this test passes, the address sanitizer was not active.
 */
void tests_asan(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_ASAN_H */
/** @} */
