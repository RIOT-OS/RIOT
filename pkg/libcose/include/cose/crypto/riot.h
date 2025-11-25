/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef COSE_CRYPTO_RIOT_H
#define COSE_CRYPTO_RIOT_H

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

#endif /* COSE_CRYPTO_RIOT_H */

/** @} */
