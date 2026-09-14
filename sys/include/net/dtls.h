/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_dtls DTLS
 * @ingroup     net
 * @brief       DTLS support in RIOT
 * @see         <a href="https://tools.ietf.org/html/rfc6347">
 *                  RFC 6347
 *              </a>
 *
 * There are two ways of using DTLS in RIOT. First is directly using
 * the third party libraries available offering DTLS implementation
 * with the pkg system. Supported DTLS implementations are:
 *
 * - @ref pkg_tinydtls "TinyDTLS"
 * - @ref pkg_wolfssl "WolfSSL"
 *
 * The other way of using DTLS is through the @ref net_sock_dtls
 * "DTLS sock API".
 *
 * For further instructions, please refer to the corresponding page.
 *
 * @{
 *
 * @file
 * @brief   DTLS
 *
 * @author  Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 */

#include "modules.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The maximum number DTLS peers (i.e. sessions)
 */
#ifndef CONFIG_DTLS_PEER_MAX
#if IS_USED(MODULE_GCOAP_DTLS)
#define CONFIG_DTLS_PEER_MAX       (2)
#else
#define CONFIG_DTLS_PEER_MAX       (1)
#endif
#endif

#ifdef __cplusplus
}
#endif

/** @} */
