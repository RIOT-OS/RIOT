/*
 * SPDX-FileCopyrightText: 2024-2025 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup unittests
 * @{
 */

/**
 * @file
 * @brief  Unit tests for the unified CoAP framework
 * @author Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include "embUnit.h"
#include "embUnit/embUnit.h"

#ifndef DOXYGEN
#  define static_strlen(str) (sizeof(str) - 1)

#  define _TEST_ASSERT_EQUAL_BYTES(_exp, _act, size) \
      TEST_ASSERT_EQUAL_INT(0, memcmp((uint8_t *)_exp, (uint8_t *)_act, size))

#  define _TEST_ASSERT_EQUAL_BYTES_STRING(_exp_str, _act) \
      _TEST_ASSERT_EQUAL_BYTES(_act, _exp_str, static_strlen(_exp_str))

#  define _TEST_ASSERT_EQUAL_POINTER(_exp, _act) \
      TEST_ASSERT_EQUAL_INT((uintptr_t)_exp, (uintptr_t)_act)

#  define _TEST_ASSERT_TRUE(a) \
      TEST_ASSERT_EQUAL_INT(true, a)

#  define _TEST_ASSERT_FALSE(a) \
      TEST_ASSERT_EQUAL_INT(false, a)

#  define _BYTES(...) ((uint8_t[]){ __VA_ARGS__ })

#  define _UINT4_MAX  (12)
#  define _UINT12_MAX (13 + 0xff)
#  define _UINT20_MAX (14 + 0xff + 0xffff)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The entry point of this test suite
 */
void tests_unicoap(void);

#ifdef __cplusplus
}
#endif

/** @} */
