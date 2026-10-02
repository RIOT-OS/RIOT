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
 * @brief       mynewt-core types
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Error codes not abstracted in mynewt-core/kernel/os
 * @{
 */
#define SYS_EINVAL      (-2)
#define SYS_ENOMEM      (-1)
/** @} */

/**
 * @name    Macro to wait forever on events and mutexes
 * @{
 */
#define OS_TIMEOUT_NEVER   (UINT32_MAX)
#define OS_WAIT_FOREVER    (OS_TIMEOUT_NEVER)
/** @} */

/**
 * @brief time type
 */
typedef uint32_t os_time_t;

/**
 * @brief stack buffer type
 */
typedef char os_stack_t;

#ifdef __cplusplus
}
#endif
