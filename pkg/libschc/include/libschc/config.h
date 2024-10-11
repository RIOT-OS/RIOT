/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_libschc_config libSCHC compile-time configuration
 * @ingroup     pkg_libschc
 * @ingroup     config
 * @brief       Compile-time configuration for libSCHC
 * @{
 *
 * @file
 * @brief       RIOT-side compile-time configuration for libSCHC
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */
#ifndef LIBSCHC_CONFIG_H
#define LIBSCHC_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Static memory buffer length
 *
 * Length of the static memory buffer for fragmentation in bytes.
 */
#ifndef CONFIG_LIBSCHC_STATIC_MEMBUF_LEN
#define CONFIG_LIBSCHC_STATIC_MEMBUF_LEN    1024
#endif

/**
 * @brief   Maximum number of mbuf pool entries
 *
 * Maximum number of entries in the mbuf used for fragment reassembly.
 */
#ifndef CONFIG_LIBSCHC_MBUF_POOL_SIZE
#define CONFIG_LIBSCHC_MBUF_POOL_SIZE       64
#endif

/**
 * @brief   Maximum number of incoming connections
 */
#ifndef CONFIG_LIBSCHC_MAX_RX_CONNS
#define CONFIG_LIBSCHC_MAX_RX_CONNS         1
#endif

/**
 * @brief Maximum transfer unit of the underlying technology
 */
#ifndef CONFIG_LIBSCHC_MAX_MTU_LEN
#define CONFIG_LIBSCHC_MAX_MTU_LEN          242
#endif

/**
 * @brief   Enable debug output
 */
#ifndef CONFIG_LIBSCHC_DEBUG
#define CONFIG_LIBSCHC_DEBUG
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LIBSCHC_CONFIG_H */
