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
 * @brief Default port for the local LwM2M CoAPs server
 */
#ifndef CONFIG_LWM2M_LOCAL_DTLS_PORT
#define CONFIG_LWM2M_LOCAL_DTLS_PORT    "5684"
#endif

/**
 * @brief Lifetime of the device object on the LwM2M server
 */
#ifndef CONFIG_LWM2M_DEVICE_TTL
#define CONFIG_LWM2M_DEVICE_TTL 300
#endif

/**
 * @brief Alternate path to place LwM2M resources
 */
#ifndef CONFIG_LWM2M_ALT_PATH
#define CONFIG_LWM2M_ALT_PATH  NULL
#endif

/**
 * @brief Define to 1 to add bootstrap server support
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
 * @brief Number to use as base for assigning tags to @ref net_credman credentials.
 */
#ifndef CONFIG_LWM2M_CREDMAN_TAG_BASE
#define CONFIG_LWM2M_CREDMAN_TAG_BASE   (10U)
#endif

/**
 * @brief Maximum length of an URI allowed.
 */
#ifndef CONFIG_LWM2M_URI_MAX_SIZE
#define CONFIG_LWM2M_URI_MAX_SIZE       64
#endif

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* LWM2M_CLIENT_CONFIG_H */
