/*
 * SPDX-FileCopyrightText: 2022 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_libcose
 *
 * @{
 *
 * @file
 * @brief       Crypto function api for glueing RIOT crypto libraries
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AUTO_INIT_PRIO_MOD_LIBCOSE
/**
 * @brief   libCOSE init priority
 */
#define AUTO_INIT_PRIO_MOD_LIBCOSE                      1050
#endif

/**
 * @name list of provided algorithms
 *
 * @{
 */
#define HAVE_ALGO_CHACHA20POLY1305
/** @} */

/**
 * @brief   Initialize libCOSE RIOT crypto backend
 *
 * @note    Automatically called if 'auto_init_libcose_crypt_riot' is included
 *
 */
void libcose_crypt_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
