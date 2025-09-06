/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * @brief       Unittests for thread safe ringbuffer
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Entry point of the test suite
 */
void tests_tsrb(void);

#ifdef __cplusplus
}
#endif

/** @} */
