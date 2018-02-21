/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Configurable parameters for Wakaama example client.
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 *
 * @}
 */


#ifndef LWM2MCONFIG_H
#define LWM2MCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Default ports
 */
#define LWM2M_STANDARD_PORT_STR "5683"
#define LWM2M_STANDARD_PORT      5683
#define LWM2M_DTLS_PORT_STR     "5684"
#define LWM2M_DTLS_PORT          5684
#define LWM2M_BSSERVER_PORT_STR "5685"
#define LWM2M_BSSERVER_PORT      5685

/*
 * The name the device registers at the LwM2M server.
 */
#define LWM2M_DEVICE_NAME "testRIOTDevice"

/*
 * Device object lifetime on the server.
 */
#define LWM2M_DEVICE_TTL 300

/*
 * LwM2M server URI to register/bootstrap with.
 * The host part of the URI MUST be a valid IPv6 address. Host names
 * can not be resolved at this time.
 */
#ifndef LWM2M_SERVER_URI
#define LWM2M_SERVER_URI "coaps://[fd00:dead:beef::1]"
#endif /* LWM2M_SERVER_URI */

/*
 * Numeric ID of LWM2M_SERVER_URI
 */
#define LWM2M_SERVER_ID 10

/*
 * Set to a true value if LWM2M_SERVER_URI is a bootstrap server.
 */
#define LWM2M_SERVER_IS_BOOTSTRAP 0


#ifdef MODULE_TINYDTLS

/*
 * DTLS debug log level. One of the following:
 *    DTLS_LOG_EMERG
 *    DTLS_LOG_ALERT
 *    DTLS_LOG_CRIT
 *    DTLS_LOG_WARN
 *    DTLS_LOG_NOTICE
 *    DTLS_LOG_INFO
 *    DTLS_LOG_DEBUG
 */
#ifndef DTLS_LOG_LEVEL
#define DTLS_LOG_LEVEL DTLS_LOG_CRIT
#endif

/*
 * PSK ID of LWM2M_SERVER_URI.
 */
#define LWM2M_SERVER_PSK_ID "Client_identity"

/*
 * PSK KEY of LWM2M_SERVER_URI in hexadecimal.
 * Set to NULL to not encrypt traffic.
 */
#define LWM2M_SERVER_PSK_KEY "73656372657450534b"

#endif /* MODULE_TINYDTLS */

/*
 * The following defines set descriptor strings as defined for the
 * mandatory LwM2M device object. See appendix E. of the OMA LwM2M technical
 * specification.
 */

/*
 * Manufacturer string.
 * Defaults to "A RIOT maker"
 */
/* #define LWM2M_DEVICE_MANUFACTURER */

/*
 * Model string.
 * Defaults to the RIOT_BOARD define.
 */
/* #define LWM2M_DEVICE_MODEL */

/*
 * Serial number (string).
 * Defaults to "undefined"
 */
/* #define LWM2M_DEVICE_SERIAL */

/*
 * Firmware version string.
 * Defaults to the RIOT_VERSION define.
 */
/* #define LWM2M_DEVICE_FW_VERSION */

/*
 * LwM2M device bindings (see specification for more information).
 * Defaults to "U" (unqueued UDP).
 */
/* #define LWM2M_DEVICE_BINDINGS */

/*
 * Device type string.
 * Defaults to "RIOT device"
 */
/* #define LWM2M_DEVICE_TYPE */

/*
 * Hardware version string.
 * Defaults to the RIOT_BOARD define.
 */
/* #define LWM2M_DEVICE_HW_VERSION */

/*
 * Software version string.
 * Defaults to the RIOT_VERSION define.
 */
/* #define LWM2M_DEVICE_SW_VERSION */

#ifdef __cplusplus
}
#endif
#endif /* LWM2MCONFIG_H */
