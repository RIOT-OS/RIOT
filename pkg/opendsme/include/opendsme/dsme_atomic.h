/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_opendsme
 *
 * @{
 *
 * @file
 *
 * @author      José I. Álamos <jose.alamos@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Begin atomic operations on openDSME.
 *
 * @note Since openDSME runs on one thread, this is not required
 */
#define dsme_atomicBegin()

/**
 * @brief End atomic operations on openDSME.
 *
 * @note Since openDSME runs on one thread, this is not required
 */
#define dsme_atomicEnd()

#ifdef __cplusplus
}
#endif
/** @} */
