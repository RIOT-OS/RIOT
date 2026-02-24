/*
 * SPDX-FileCopyrightText: 2016 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_iana_portrange  IANA Port Ranges
 * @ingroup     net
 * @brief       Service Name and Transport Protocol Port Number Registry
 * @{
 *
 * @file
 * @brief   Service Name and Transport Protocol Port Number Registry
 *
 * @author  smlng <s@mlng.net>,
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Port Number Ranges
 * @see <a href="https://tools.ietf.org/html/rfc6335#section-6">
 *          RFC 6335, section 6
 *      </a>
 * @{
 */
#define IANA_SYSTEM_PORTRANGE_MIN   (0U)
#define IANA_SYSTEM_PORTRANGE_MAX   (1023U)
#define IANA_USER_PORTRANGE_MIN     (1024U)
#define IANA_USER_PORTRANGE_MAX     (49151U)
#define IANA_DYNAMIC_PORTRANGE_MIN  (49152U)
#define IANA_DYNAMIC_PORTRANGE_MAX  (65535U)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
