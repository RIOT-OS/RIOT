/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_sock_util_conf SOCK utility functions compile configurations
 * @ingroup     net_sock_conf
 * @{
 *
 * @brief       sock utility configuration values
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief maximum length of the scheme part for sock_urlsplit.
 *
 * Ensures a hard limit on the string iterator
 * */
#ifndef CONFIG_SOCK_SCHEME_MAXLEN
#define CONFIG_SOCK_SCHEME_MAXLEN      (16U)
#endif

/**
 * @brief maximum length of host:port part for sock_urlsplit()
 */
#ifndef CONFIG_SOCK_HOSTPORT_MAXLEN
#define CONFIG_SOCK_HOSTPORT_MAXLEN    (64U)
#endif

/**
 * @brief maximum length path for sock_urlsplit()
 */
#ifndef CONFIG_SOCK_URLPATH_MAXLEN
#define CONFIG_SOCK_URLPATH_MAXLEN     (64U)
#endif

/**
 * @brief   Timeout in milliseconds for sock_dtls_establish_session()
 */
#ifndef CONFIG_SOCK_DTLS_TIMEOUT_MS
#define CONFIG_SOCK_DTLS_TIMEOUT_MS    (1000U)
#endif

/**
 * @brief   Number of DTLS handshake retries for sock_dtls_establish_session()
 */
#ifndef CONFIG_SOCK_DTLS_RETRIES
#define CONFIG_SOCK_DTLS_RETRIES       (2)
#endif

#ifdef __cplusplus
}
#endif

/** @} */
