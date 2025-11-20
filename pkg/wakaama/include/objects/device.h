/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     lwm2m_objects
 * @defgroup    lwm2m_objects_device Device LwM2M object
 * @brief       Device object implementation for LwM2M client using Wakaama
 *
 * | Name                        | ID | Mandatory |   Type  | Range | Units | Implemented |
 * |-----------------------------|:--:|:---------:|:-------:|:-----:|:-----:|:-----------:|
 * | Manufacturer                |  0 |     No    |  String |   -   |   -   |     Yes     |
 * | Model Number                |  1 |     No    |  String |   -   |   -   |     Yes     |
 * | Serial Number               |  2 |     No    |  String |   -   |   -   |     Yes     |
 * | Firmware Version            |  3 |     No    |  String |   -   |   -   |     Yes     |
 * | Reboot                      |  4 |    Yes    |    -    |   -   |   -   |     Yes     |
 * | Factory Reset               |  5 |     No    |    -    |   -   |   -   |      No     |
 * | Available Power Sources     |  6 |     No    | Integer |  0-7  |   -   |      No     |
 * | Power Source Voltage        |  7 |     No    | Integer |   -   |   mV  |      No     |
 * | Power Source Current        |  8 |     No    | Integer |   -   |   mA  |      No     |
 * | Battery Level               |  9 |     No    | Integer | 0-100 |   %   |      No     |
 * | Memory Free                 | 10 |     No    | Integer |   -   |   KB  |      No     |
 * | Error Code                  | 11 |    Yes    | Integer |  0-8  |   -   |      No     |
 * | Reset Error Code            | 12 |     No    |    -    |   -   |   -   |      No     |
 * | Current Time                | 13 |     No    |   Time  |   -   |   -   |      No     |
 * | UTC Offset                  | 14 |     No    |  String |   -   |   -   |      No     |
 * | Timezone                    | 15 |     No    |  String |   -   |   -   |      No     |
 * | Supported Binding and Modes | 16 |    Yes    |  String |   -   |   -   |     Yes     |
 * | Device Type                 | 17 |     No    |  String |   -   |   -   |     Yes     |
 * | Hardware Version            | 18 |     No    |  String |   -   |   -   |     Yes     |
 * | Software Version            | 19 |     No    |  String |   -   |   -   |     Yes     |
 * | Battery Status              | 20 |     No    | Integer |  0-6  |   -   |      No     |
 * | Memory Total                | 21 |     No    | Integer |   -   |   -   |      No     |
 * | ExtDevInfo                  | 22 |     No    |  Objlnk |   -   |   -   |      No     |
 *
 * @{
 *
 * @file
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liblwm2m.h"
#include "lwm2m_client.h"
#include "lwm2m_client_config.h"

/**
 * @brief Resources of the LwM2M device object instance
 *
 * @see http://www.openmobilealliance.org/tech/profiles/LWM2M_Device-v1_0_3.xml
 */
enum lwm2m_device_resources {
    LWM2M_RES_MANUFACTURER = 0, /**< Human readable manufacturer name */
    LWM2M_RES_MODEL_NO,         /**< Model identifier (manufacturer specified string) */
    LWM2M_RES_SERIAL,           /**< Serial number */
    LWM2M_RES_FW_VER,           /**< Current firmware version of the device */
    LWM2M_RES_REBOOT,           /**< Reboot the device */
    LWM2M_RES_FRESET,           /**< Perform a factory reset of the device */
    LWM2M_RES_POWER_SRC,        /**< Available power sources */
    LWM2M_RES_POWER_VOL,        /**< Present voltage for each power source */
    LWM2M_RES_POWER_AMP,        /**< Present current for each power source */
    LWM2M_RES_BATTERY_LEVEL,    /**< Current battery level as a percentage */
    LWM2M_RES_MEM_FREE,         /**< Estimated current available storage (kB) */
    LWM2M_RES_ERROR_CODE,       /**< Last error code */
    LWM2M_RES_ERROR_CODE_RESET, /**< Delete all error code instances */
    LWM2M_RES_TIME,             /**< Current UNIX time of the client */
    LWM2M_RES_TIME_OFFSET,      /**< Indicated the UTC offset for the device */
    LWM2M_RES_TIME_ZONE,        /**< Indicates the time zone of the device */
    LWM2M_RES_BINDINGS,         /**< Indicates supported bindings and modes on the client */
    LWM2M_RES_TYPE,             /**< Type of device */
    LWM2M_RES_HW_VERSION,       /**< Current hardware version of the device */
    LWM2M_RES_SW_VERSION,       /**< Current software version on the device */
    LWM2M_RES_BATTERY_STATUS,   /**< Battery status when internal battery is present */
    LWM2M_RES_MEM_TOTAL,        /**< Total amount of storage space in the device (kB*/
    LWM2M_RES_EXT_DEV_INFO,     /**< External device object instance */
    LWM2M_DEVICE_RESOURCES      /**< Number of resources */
};

/**
 * @brief Error codes for the
 * @ref lwm2m_device_resources::LWM2M_RES_ERROR_CODE "Error" resource in the
 * device object of LwM2M
 */
enum lwm2m_device_error_codes {
    LWM2M_DEVICE_ERR_NO_ERR     = 0, /**< No error */
    LWM2M_DEVICE_ERR_LOW_BATT   = 1, /**< Low battery power */
    LWM2M_DEVICE_ERR_EXT_OFF    = 2, /**< External power supply off */
    LWM2M_DEVICE_ERR_GPS_ERR    = 3, /**< GPS module failure */
    LWM2M_DEVICE_ERR_LOW_SIGNAL = 4, /**< Low received signal strength */
    LWM2M_DEVICE_ERR_NO_MEM     = 5, /**< Out of memory */
    LWM2M_DEVICE_ERR_SMS_ERR    = 6, /**< SMS failure */
    LWM2M_DEVICE_ERR_IP_ERR     = 7, /**< IP connectivity failure */
    LWM2M_DEVICE_ERR_PERIPH_ERR = 8  /**< Peripheral malfunction */
};

/**
 * @defgroup lwm2m_objects_device_config LwM2M Device Object configuration
 * @ingroup lwm2m_client_config
 *
 * @brief Configuration options for the LwM2M Device Object.
 * @{
 */
/**
 * @brief Device name used to register at the LwM2M server
 */
#ifndef CONFIG_LWM2M_DEVICE_NAME
#define CONFIG_LWM2M_DEVICE_NAME "testRIOTDevice"
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
 *
 * @note Select using CONFIG_LWM2M_DEVICE_BINDING_*
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

/**
 * @brief Initialize the Device object.
 *
 * @param[in] client_data  LwM2M client data.
 *
 * @return Pointer to the Device object on success
 */
lwm2m_object_t *lwm2m_object_device_init(lwm2m_client_data_t *client_data);

/**
 * @brief Determines if a reboot request has been issued to the device by a
 *        server.
 *
 * @return true reboot has been requested
 * @return false reboot has not been requested
 */
bool lwm2m_device_reboot_requested(void);

#ifdef __cplusplus
}
#endif

/** @} */
