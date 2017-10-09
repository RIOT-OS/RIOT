/**
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H

#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Size for module message queue
 */
#define OTA_UPDATE_MSG_QUEUE_SIZE (4)

/**
 * @brief Timeout for a CoAP update request
 */
#define OTA_REQ_TIMEOUT           (5000000UL)

/**
 * @brief Default firmware name update
 */
#ifndef FIRMWARE_NAME
#define FIRMWARE_NAME             "firmware.bin"
#endif

/**
 * s@brief Default updates server address
 */
#ifndef OTA_SERVER_ADDRESS
#define OTA_SERVER_ADDRESS        "fd00:dead:beef::1"
#endif

/**
 * @brief Default CoAP updates server port
 */
#ifndef OTA_SERVER_COAP_PORT
#define OTA_SERVER_COAP_PORT      "5683"
#endif

/**
 * @brief Stack size for OTA module thread
 */
#ifndef OTA_UPDATE_STACK
#define OTA_UPDATE_STACK          ((THREAD_STACKSIZE_DEFAULT*2) + FLASHPAGE_SIZE)
#endif

/**
 * @brief Peiodic request time for update
 */
#ifndef OTA_PERIODIC_REQ_TIME
#define OTA_PERIODIC_REQ_TIME     (120)
#endif

/**
 * @brief Size for module message queue
 */
#define OTA_MSG_QUEUE_SIZE        (4)

/**
 * @brief Length of the firmware string filename
 */
#define FIRMWARE_NAME_LENGTH            (128)

/**
 * @brief Enum for CoAP requests types for the update process
 * @{
 */
typedef enum {
    COAP_GET_VERSION = 0,  /**< Request for the newest available firmware version */
    COAP_GET_NAME = 1,     /**< Request for the newest firmware name */
} ota_request_t;
/** @} */

/**
 * @brief  Init function for the ota_update module
 *
 * @return The kernel PID for the running module
 *
 */
kernel_pid_t ota_update_init(void);

#ifdef __cplusplus
}
#endif

#endif /* OTA_UPDATE_H */
