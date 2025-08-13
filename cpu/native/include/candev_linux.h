/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup drivers_candev_linux SocketCAN driver
 * @ingroup  drivers_can
 * @brief    Implementation of simulated CAN controller driver using SocketCAN on Linux
 * @{
 */

/**
 * @file
 * @brief  Implementation of simulated CAN controller driver using SocketCAN on Linux
 * @author Hermann Lelong <hermann@otakeys.com>
 * @author Aurelien Gonce <aurelien.gonce@altran.com>
 * @author Vincent Dupont <vincent@otakeys.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__linux__) /* SocketCAN is supported only on Linux */ || defined(DOXYGEN)

#  include <stdbool.h>

#  include "can/device.h"
#  include "can/candev.h"
#  include "mutex.h"

/**
 * @brief Maximum size of an interface name
 */
#  define CAN_MAX_SIZE_INTERFACE_NAME (5)

/**
 * @brief Linux candev configuration
 */
typedef struct candev_conf {
    /** local interface name */
    char interface_name[CAN_MAX_SIZE_INTERFACE_NAME + 1];
} can_conf_t;

/** @brief CAN device configuration type can_conf_t is redefined by native CAN */
#  define HAVE_CAN_CONF_T

#  if !defined(CANDEV_LINUX_MAX_FILTERS_RX) || defined(DOXYGEN)
/**  @brief Max number of rx filters which can be set */
#    define CANDEV_LINUX_MAX_FILTERS_RX  (16)
#  endif

#  if !defined(CANDEV_LINUX_DEFAULT_BITRATE) || defined(DOXYGEN)
/**  @brief Default bitrate setup */
#    define CANDEV_LINUX_DEFAULT_BITRATE (500000)
#  endif

#  if !defined(CANDEV_LINUX_DEFAULT_SPT) || defined(DOXYGEN)
/**  @brief Default sampling point setup*/
#    define CANDEV_LINUX_DEFAULT_SPT (875)
#  endif

/**
 * @brief The candev_linux struct
 */
typedef struct candev_linux {
    candev_t candev;                  /**< candev base structure */
    int sock;                         /**< local socket id */
    const can_conf_t *conf;  /**< device configuration */
    /** filter list */
    struct can_filter filters[CANDEV_LINUX_MAX_FILTERS_RX];
} can_t;

/** @brief CAN device type can_t is redefined by native CAN */
#  define HAVE_CAN_T

/**
 * @brief Array containing socketCAN device names
 */
extern can_conf_t candev_conf[CAN_DLL_NUMOF];

#endif /* defined(__linux__) */

#ifdef __cplusplus
}
#endif

/** @} */
