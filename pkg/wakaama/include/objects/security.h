/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     lwm2m_objects
 * @defgroup    lwm2m_objects_security Security LwM2M object
 * @brief       Security object implementation for LwM2M client using Wakaama
 *
 * This implements the LwM2M Security object as specified in the Appendix E1 of
 * the LwM2M specification.
 *
 * So far only NO_SEC and PSK (Pre-shared key) modes are available.
 *
 * ## Resources
 *
 * For an XML description of the object see
 * https://raw.githubusercontent.com/OpenMobileAlliance/lwm2m-registry/prod/version_history/0-1_0.xml.
 *
 * |         Name            | ID | Mandatory |  Type   |  Range  | Units | Implemented |
 * | ----------------------- | -- | --------- | ------- | ------- | ----- | ---------- |
 * | Server URI              |  0 |    Yes    | String  |         |       |    Yes     |
 * | Bootstrap Server        |  1 |    Yes    | Boolean |         |       |    Yes     |
 * | Security Mode           |  2 |    Yes    | Integer |   0-3   |       |    Yes     |
 * | Public Key or ID        |  3 |    Yes    | Opaque  |         |       |    Yes     |
 * | Server Public Key or ID |  4 |    Yes    | Opaque  |         |       |     No     |
 * | Secret Key              |  5 |    Yes    | Opaque  |         |       |    Yes     |
 * | SMS Security Mode       |  6 |    No     | Integer |  0-255  |       |     No     |
 * | SMS Binding Key Param.  |  7 |    No     | Opaque  |   6 B   |       |     No     |
 * | SMS Binding Secret Keys |  8 |    No     | Opaque  | 32-48 B |       |     No     |
 * | Server SMS Number       |  9 |    No     | String  |         |       |     No     |
 * | Short Server ID         | 10 |    No     | Integer | 1-65535 |       |     Yes    |
 * | Client Hold Off Time    | 11 |    No     | Integer |         |   s   |     Yes    |
 * | BS Account Timeout      | 12 |    No     | Integer |         |   s   |     Yes    |
 *
 * ## Usage
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * // assuming buffers psk_id and psk_key containing credential information
 * // [...]
 *
 * // create credential
 * credman_credential_t cred = {
 *      .type = CREDMAN_TYPE_PSK,
 *      .params = {
 *          .psk = {
 *              .key = { .s = psk_key, .len = sizeof(psk_key) - 1 },
 *              .id = { .s = psk_id, .len = sizeof(psk_id) - 1 },
 *          }
 *      }
 * }
 *
 *  // prepare instance arguments
 *  lwm2m_obj_security_args_t args = {
 *      .server_id = 1,
 *      .server_uri = "coap://[fd00:dead:beef::1]",
 *      .security_mode = LWM2M_SECURITY_MODE_NONE,
 *      .cred = &cred,
 *      .is_bootstrap = false,
 *      .client_hold_off_time = 5,
 *      .bootstrap_account_timeout = 0
 *  };
 *
 *  // get the security object handle
 *  lwm2m_object_t *sec_obj = lwm2m_object_security_get();
 *
 *  // instantiate a new security object
 *  int res = lwm2m_object_security_instance_create(sec_obj, 1, &args);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef OBJECTS_SECURITY_H
#define OBJECTS_SECURITY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "liblwm2m.h"
#include "net/credman.h"
#include "lwm2m_client_config.h"

/* these are defined in liblwm2m.h, and are reproduced here for documentation purposes */
#ifdef DOXYGEN
/**
 * @name LwM2M Security object security modes
 * @{
 */
/**
 * @brief Pre-Shared keys mode
 */
#define LWM2M_SECURITY_MODE_PRE_SHARED_KEY  0

/**
 * @brief Raw public keys mode
 */
#define LWM2M_SECURITY_MODE_RAW_PUBLIC_KEY  1

/**
 * @brief Certificate mode
 */
#define LWM2M_SECURITY_MODE_CERTIFICATE     2

/**
 * @brief No security mode
 */
#define LWM2M_SECURITY_MODE_NONE            3
/** @} */
#endif /* DOXYGEN */

/**
 * @defgroup lwm2m_objects_security_config LwM2M Security object compile configurations
 * @ingroup lwm2m_client_config
 * @{
 */
/**
 * @brief Maximum number of instances of the Security object
 */
#ifndef CONFIG_LWM2M_OBJ_SECURITY_INSTANCES_MAX
#define CONFIG_LWM2M_OBJ_SECURITY_INSTANCES_MAX              (2)
#endif

/**
 * @brief Buffer size of the public key or ID resource
 */
#ifndef CONFIG_LWM2M_OBJ_SECURITY_PUB_KEY_ID_BUFSIZE
#define CONFIG_LWM2M_OBJ_SECURITY_PUB_KEY_ID_BUFSIZE         (32)
#endif

/**
 * @brief Buffer size of the server public key resource
 */
#ifndef CONFIG_LWM2M_OBJ_SECURITY_SERVER_PUB_KEY_BUFSIZE
#define CONFIG_LWM2M_OBJ_SECURITY_SERVER_PUB_KEY_BUFSIZE     (32)
#endif

/**
 * @brief Buffer size of the secret key resource
 */
#ifndef CONFIG_LWM2M_OBJ_SECURITY_SEC_KEY_BUFSIZE
#define CONFIG_LWM2M_OBJ_SECURITY_SEC_KEY_BUFSIZE            (32)
#endif
/** @} */

/**
 * @brief Arguments for a new Security object instance creation
 *        (@ref lwm2m_object_security_instance_create).
 */
typedef struct lwm2m_obj_security_args {
    /**
     * @brief Server's short ID the instance is associated to.
     */
    uint16_t server_id;

    /**
     * @brief Server's URI the instance is associated to. Note that a copy of the URI will be kept
     *        locally.
     */
    const char *server_uri;

    /**
     * @brief Security mode to use. For now only @ref LWM2M_SECURITY_MODE_NONE and
     *        @ref LWM2M_SECURITY_MODE_PRE_SHARED_KEY are supported.
     */
    uint8_t security_mode;

    /**
     * @brief Pointer to an initialized credential when a security mode other than
     *        @ref LWM2M_SECURITY_MODE_NONE is used. The tag is ignored. For now only
     *        @ref CREDMAN_TYPE_PSK type is supported.
     */
    const credman_credential_t *cred;

    /**
     * @brief When `true` the security instance is associated to the Bootstrap-Server.
     */
    bool is_bootstrap;

    /**
     * @brief Time, in seconds, to wait before initiating a 'Client Initiated Bootstrap', after it
     *        has been determined that it should be initiated.
     */
    uint32_t client_hold_off_time;

    /**
     * @brief Time, in seconds, that the client waits before it purges the Bootstrap-Server's
     *        account. 0 means never.
     */
    uint32_t bootstrap_account_timeout;
} lwm2m_obj_security_args_t;

/**
 * @brief   Get the LwM2M Security object handle
 *
 * @return Pointer to the global handle of the security object.
 */
lwm2m_object_t *lwm2m_object_security_get(void);

/**
 * @brief   Create a new LwM2M Security object instance and add it to the @p object list.
 *
 * @param[in, out] object                  Security object handle.
 * @param[in] instance_id                  ID for the new instance.
 * @param[in] args                         Arguments for the instance.
 *
 * @return 0 on success
 * @return <0 otherwise
 */
int lwm2m_object_security_instance_create(lwm2m_object_t *object, uint16_t instance_id,
                                          lwm2m_obj_security_args_t *args);

/**
 * @brief   Get the credential of a given instance of the security object.
 *
 * @param[in] object                       Security object handle.
 * @param[in] instance_id                  ID of the instance.
 *
 * @return Credential tag.
 * @retval CREDMAN_TAG_EMPTY when no credential is assigned.
 */
credman_tag_t lwm2m_object_security_get_credential(lwm2m_object_t *object, uint16_t instance_id);

#ifdef __cplusplus
}
#endif

#endif /* OBJECTS_SECURITY_H */
/** @} */
