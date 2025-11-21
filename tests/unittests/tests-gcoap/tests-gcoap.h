/*
 * SPDX-FileCopyrightText: 2016 Ken Bannister
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unit tests for the gcoap module
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 */

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 *
 * Includes simple success case tests to get a resource from a server, and
 * to provide a resource for a client.
 */
void tests_gcoap(void);

#ifdef __cplusplus
}
#endif

/** @} */
