/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

/**
 * @addtogroup unittests
 * @{
 */

/**
 * @file
 * @brief  Unit tests for the unified CoAP framework
 * @author Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#include "embUnit.h"
#include "embUnit/embUnit.h"
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#ifndef DOXYGEN
#  define static_strlen(str) (sizeof(str) - 1)

#  define _TEST_ASSERT_EQUAL_BYTES(a, b, size) \
    TEST_ASSERT_EQUAL_INT(memcmp((uint8_t*)a, (uint8_t*)b, size), 0)

#  define _TEST_ASSERT_EQUAL_BYTES_STRING(a, str) \
    _TEST_ASSERT_EQUAL_BYTES(a, str, static_strlen(str))

#  define _TEST_ASSERT_EQUAL_POINTER(a, b) \
    TEST_ASSERT_EQUAL_INT((uintptr_t)a, (uintptr_t)b)

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
