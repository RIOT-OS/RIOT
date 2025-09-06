/*
 * Copyright (C) 2015 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``inet_csum`` module
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_inet_csum(void);

#ifdef __cplusplus
}
#endif

/** @} */
