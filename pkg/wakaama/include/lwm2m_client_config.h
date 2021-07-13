/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *               2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup      pkg_wakaama
 * @ingroup      config
 * @defgroup     lwm2m_client_config Wakaama LwM2M Client configuration
 *
 * @brief        Configuration options for the LwM2M client implementation
 *               based on the Wakaama package.
 * @{
 *
 * @file
 * @brief       LwM2M client configurations
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef LWM2M_CLIENT_CONFIG_H
#define LWM2M_CLIENT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CoAP default port of the LwM2M server
 */
#ifndef CONFIG_LWM2M_STANDARD_PORT
#define CONFIG_LWM2M_STANDARD_PORT "5683"
#endif

/**
 * @brief CoAPS default port of the LwM2M server
 */
#ifndef CONFIG_LWM2M_DTLS_PORT
#define CONFIG_LWM2M_DTLS_PORT     "5684"
#endif

/**
 * @brief CoAP default port of the LwM2M bootstrap server
 */
#ifndef CONFIG_LWM2M_BSSERVER_PORT
#define CONFIG_LWM2M_BSSERVER_PORT "5685"
#endif

/**
 * @brief Default port for the local LwM2M instance
 */
#ifndef CONFIG_LWM2M_LOCAL_PORT
#define CONFIG_LWM2M_LOCAL_PORT    "5683"
#endif

/**
 * @brief Device name used to register at the LwM2M server
 */
#ifndef CONFIG_LWM2M_DEVICE_NAME
#define CONFIG_LWM2M_DEVICE_NAME "testRIOTDevice"
#endif

/**
 * @brief Lifetime of the device object on the LwM2M server
 */
#ifndef CONFIG_LWM2M_DEVICE_TTL
#define CONFIG_LWM2M_DEVICE_TTL 300
#endif

/**
 * @brief LwM2M server URI to register/bootstrap with
 *
 * @note The host part of the URI MUST be a valid IPv6 address. Host names can
 * not be resolved at this time.
 */
#ifndef CONFIG_LWM2M_SERVER_URI
#define CONFIG_LWM2M_SERVER_URI "coap://[fd00:dead:beef::1]"
#endif

/**
 * @brief Numeric ID of CONFIG_LWM2M_SERVER_URI
 */
#ifndef CONFIG_LWM2M_SERVER_ID
#define CONFIG_LWM2M_SERVER_ID 10
#endif

/**
 * @brief Alternate path to place LwM2M resources
 */
#ifndef CONFIG_LWM2M_ALT_PATH
#define CONFIG_LWM2M_ALT_PATH  NULL
#endif

/**
 * @brief Define to 1 to specify that @ref CONFIG_LWM2M_SERVER_URI is a bootstrap server
 *
 * To define just add it to your `CFLAGS` in your application's Makefile:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * CFLAGS += -DCONFIG_LWM2M_BOOTSTRAP=1
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#ifdef DOXYGEN
#define CONFIG_LWM2M_BOOTSTRAP
#endif

/**
 * @brief Device object manufacturer string
 */
#ifndef CONFIG_LWM2M_DEVICE_MANUFACTURER
#define CONFIG_LWM2M_DEVICE_MANUFACTURER "A RIOT maker"
#endif

/**
 * @brief Device object model.
 *
 * @note Defaults to the board name
 */
#ifndef CONFIG_LWM2M_DEVICE_MODEL
#define CONFIG_LWM2M_DEVICE_MODEL RIOT_BOARD
#endif

/**
 * @brief Device object serial number
 */
#ifndef CONFIG_LWM2M_DEVICE_SERIAL
#define CONFIG_LWM2M_DEVICE_SERIAL "undefined"
#endif

/**
 * @brief Device object firmware version
 *
 * @note Defaults to the running RIOT version
 */
#ifndef CONFIG_LWM2M_DEVICE_FW_VERSION
#define CONFIG_LWM2M_DEVICE_FW_VERSION RIOT_VERSION
#endif

/**
 * @{
 * @name Device bindings and queue modes
 *
 * This options are meant to be set either via Kconfig or CFLAGS:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * CFLAGS += -DCONFIG_LWM2M_DEVICE_BINDING_UQ
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note Only one option should be selected. If more than one is defined the
 *       priority follows this order. By default
 *       @ref CONFIG_LWM2M_DEVICE_BINDING_U is assumed.
 */
#ifdef DOXYGEN
/**
 * @brief UDP binding
 */
#define CONFIG_LWM2M_DEVICE_BINDING_U

/**
 * @brief UDP binding with Queue mode
 */
#define CONFIG_LWM2M_DEVICE_BINDING_UQ

/**
 * @brief SMS binding
 */
#define CONFIG_LWM2M_DEVICE_BINDING_S

/**
 * @brief SMS binding with Queue mode
 */
#define CONFIG_LWM2M_DEVICE_BINDING_SQ

/**
 * @brief UDP and SMS bindings
 */
#define CONFIG_LWM2M_DEVICE_BINDING_US

/**
 * @brief UDP and SMS bindings with Queue mode
 */
#define CONFIG_LWM2M_DEVICE_BINDING_UQS
#endif
/** @} */

/**
 * @brief Device object device type
 */
#ifndef CONFIG_LWM2M_DEVICE_TYPE
#define CONFIG_LWM2M_DEVICE_TYPE "RIOT device"
#endif

/**
 * @brief Device object hardware version
 *
 * @note Defaults to the board name
 */
#ifndef CONFIG_LWM2M_DEVICE_HW_VERSION
#define CONFIG_LWM2M_DEVICE_HW_VERSION RIOT_BOARD
#endif

/**
 * @brief Device object software version
 *
 * @note Defaults to the running RIOT version
 */
#ifndef CONFIG_LWM2M_DEVICE_SW_VERSION
#define CONFIG_LWM2M_DEVICE_SW_VERSION RIOT_VERSION
#endif
/** @} */

/**
 * @brief Device binding and queue mode
 */
#if defined(CONFIG_LWM2M_DEVICE_BINDING_U)
#define CONFIG_LWM2M_DEVICE_BINDINGS "U"
#elif defined(CONFIG_LWM2M_DEVICE_BINDING_UQ)
#define CONFIG_LWM2M_DEVICE_BINDINGS "UQ"
#elif defined(CONFIG_LWM2M_DEVICE_BINDING_S)
#define CONFIG_LWM2M_DEVICE_BINDINGS "S"
#elif defined(CONFIG_LWM2M_DEVICE_BINDING_SQ)
#define CONFIG_LWM2M_DEVICE_BINDINGS "SQ"
#elif defined(CONFIG_LWM2M_DEVICE_BINDING_US)
#define CONFIG_LWM2M_DEVICE_BINDINGS "US"
#elif defined(CONFIG_LWM2M_DEVICE_BINDING_UQS)
#define CONFIG_LWM2M_DEVICE_BINDINGS "UQS"
#else
#define CONFIG_LWM2M_DEVICE_BINDINGS "U"
#endif

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* LWM2M_CLIENT_CONFIG_H */
