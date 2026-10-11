/*
 * SPDX-FileCopyrightText: 2026 Carl Seifert <carl.seifert@tu-dresden.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the `tlv` module
 *
 * @author      Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The entry point of this test suite.
 */
void tests_tlv(void);

/**
 * @brief Generates tests for timex
 *
 * @return embUnit tests if successful, NULL if not.
 */
Test *tests_tlv_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
