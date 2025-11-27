/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
