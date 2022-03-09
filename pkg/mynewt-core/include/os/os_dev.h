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
 * @brief       Abstraction layer for RIOT adaption
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */
#ifndef OS_OS_DEV_H
#define OS_OS_DEV_H

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

#endif /* OS_OS_DEV_H */
