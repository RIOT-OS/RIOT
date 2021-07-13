/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_nimble_advertise nimBLE advertising
 * @ingroup     sys
 *
 * @brief       Automatic advertising for defined bluetooth services
 *
 * @{
 * @file
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

#ifndef AUTO_NIMBLE_ADVERTISE_H
#define AUTO_NIMBLE_ADVERTISE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief Bluetooth device name to be displayed when advertising
 *       Can be configured by setting AUTO_NIMBLE_ADVERTISE_DEVICE_NAME in CFLAGS
*/
#ifdef AUTO_NIMBLE_ADVERTISE_DEVICE_NAME
static const char *device_name = AUTO_NIMBLE_ADVERTISE_DEVICE_NAME;
#else
static const char *device_name = "RIOT OS device";
#endif

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* AUTO_NIMBLE_ADVERTISE_H */
