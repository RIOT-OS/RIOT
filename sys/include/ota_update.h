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

/** @brief Size for module message queue */
#define OTA_UPDATE_MSG_QUEUE_SIZE (4)
#define OTA_REQ_TIMEOUT           (5000000UL)

#ifndef FIRMWARE_NAME
#define FIRMWARE_NAME             "firmware.bin"
#endif

#ifndef OTA_SERVER_ADDRESS
#define OTA_SERVER_ADDRESS        "fd00:fc0a:c057:ade1::1"
#endif

#ifndef OTA_SERVER_COAP_PORT
#define OTA_SERVER_COAP_PORT      "5684"
#endif

/** @brief Stack size for module thread */
#ifndef OTA_UPDATE_STACK
#define OTA_UPDATE_STACK          (THREAD_STACKSIZE_DEFAULT + FLASHPAGE_SIZE)
#endif

#ifndef OTA_PERIODIC_REQ_TIME
#define OTA_PERIODIC_REQ_TIME     (120)
#endif

/** @brief Size for module message queue */
#define OTA_MSG_QUEUE_SIZE        (4)

#define FW_NAME_LENGTH            (128)

typedef enum {
    COAP_GET_VERSION = 0,
    COAP_GET_NAME = 1,
} ota_request_t;

kernel_pid_t ota_update_init(void);

#ifdef __cplusplus
}
#endif

#endif /* OTA_UPDATE_H */
