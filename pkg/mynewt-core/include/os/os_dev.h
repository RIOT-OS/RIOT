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
 * @brief       Abstraction layer for RIOT adaption
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Device structure.
 */
struct os_dev {
};

/**
 * @brief   Unused define, void cast
 */
#define OS_DEV_SETHANDLERS(__dev, __open, __close)          \
    (void) __dev;                \
    (void) __open;                \
    (void) __close;

#ifdef __cplusplus
}
#endif
