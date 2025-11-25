/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef OS_OS_TYPES_H
#define OS_OS_TYPES_H

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

#endif /* OS_OS_TYPES_H */
