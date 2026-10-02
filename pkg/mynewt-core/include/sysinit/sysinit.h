/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_mynewt_core
 * @{
 *
 * @file
 * @brief       sysinit abstraction layer for RIOT adaption
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief assert macro
 */
#define SYSINIT_PANIC_ASSERT(rc)        assert(rc);

/**
 * @brief empty definition
 */
#define SYSINIT_ASSERT_ACTIVE()

#ifdef __cplusplus
}
#endif
