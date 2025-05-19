/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_eddystone Eddystone
 * @ingroup     net
 * @brief       General values defined by the BLE Eddystone beacon format
 *
 * @see https://github.com/google/eddystone/blob/master/protocol-specification.md
 * @{
 *
 * @file
 * @brief       Constants defined by the Eddystone specification
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Collection of general Eddystone constants
 * @{
 */
#define EDDYSTONE_NAMESPACE_LEN     (10U)
#define EDDYSTONE_INSTANCE_LEN      (6U)
/** @} */

/**
 * @name    URL scheme prefix values
 * @{
 */
#define EDDYSTONE_URL_HTTP_WWW      (0x00)      /**< `http://www.URL` */
#define EDDYSTONE_URL_HTTPS_WWWW    (0x01)      /**< `https://www.URL` */
#define EDDYSTONE_URL_HTTP          (0x02)      /**< `http://URL` */
#define EDDYSTONE_URL_HTTPS         (0x03)      /**< `https://URL` */
/** @} */

/**
 * @name    Eddystone frame types
 * @{
 */
#define EDDYSTONE_UID               (0x00)
#define EDDYSTONE_URL               (0x10)
#define EDDYSTONE_TLM               (0x20)
#define EDDYSTONE_EID               (0x30)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
