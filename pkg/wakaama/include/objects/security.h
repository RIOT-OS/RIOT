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
 * @experimental This API is considered experimental and may change in future releases without
 *               deprecation process.
 *
 * This implements the LwM2M Security object as specified in the Appendix E1 of
 * the LwM2M specification.
 *
 * So far only NO_SEC, PSK (Pre-shared key) and RPK (Raw public key) modes are available.
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
 * | Server Public Key       |  4 |    Yes    | Opaque  |         |       |    Yes     |
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
 * ### Pre-shared keys
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * // assuming buffers psk_id and psk_key containing credential information
 * // assuming client_data is a valid lwm2m_client_data_t instance
 * // [...]
 *
 *  // prepare instance arguments
 *  lwm2m_obj_security_args_t args = {
 *      .server_id = CONFIG_LWM2M_SERVER_SHORT_ID,
 *      .server_uri = CONFIG_LWM2M_SERVER_URI,
 *      .security_mode = LWM2M_SECURITY_MODE_PRE_SHARED_KEY,
 *      .pub_key_or_id = psk_id,
 *      .pub_key_or_id_len = sizeof(psk_id) - 1,
 *      .secret_key = psk_key,
 *      .secret_key_len = sizeof(psk_key) - 1,
 *      .is_bootstrap = false,
 *      .client_hold_off_time = 5,
 *      .bootstrap_account_timeout = 0
 *  };
 *
 *  // initialize the security object and get handle
 *  lwm2m_object_t *sec_obj = lwm2m_object_security_init(&client_data);
 *
 *  // instantiate a new security object with instance ID 1
 *  int res = lwm2m_object_security_instance_create(&args, 1);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ### Raw public keys
 *
 * To use this security mode the following keys are required:
 *  - server public key (`SubjectPublicKeyInfo` DER encoded, according to RFC5280)
 *  - own public (`SubjectPublicKeyInfo` DER encoded) and private (as a `ECPrivateKey` DER encoded
 *    sequence, according to RFC5915)keys. See below on how they can be generated.
 *
 * It is possible that you may need to increase @ref CONFIG_DTLS_HANDSHAKE_BUFSIZE_EXP when using
 * RPK mode.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * // assuming buffers rpk_pub, rpk_priv, and server_rpk_pub containing the elliptic curve keys
 * // assuming client_data is a valid lwm2m_client_data_t instance
 * // [...]
 *
 *  // prepare instance arguments
 *  lwm2m_obj_security_args_t args = {
 *      .server_id = CONFIG_LWM2M_SERVER_SHORT_ID,
 *      .server_uri = CONFIG_LWM2M_SERVER_URI,
 *      .security_mode = LWM2M_SECURITY_MODE_RAW_PUBLIC_KEY,
 *      .pub_key_or_id = rpk_pub,
 *      .pub_key_or_id_len = sizeof(rpk_pub),
 *      .secret_key = rpk_priv,
 *      .secret_key_len = sizeof(rpk_priv),
 *      .server_pub_key = server_rpk_pub,
 *      .server_pub_key_len = sizeof(server_rpk_pub),
 *      .is_bootstrap = false,
 *      .client_hold_off_time = 5,
 *      .bootstrap_account_timeout = 0
 *  };
 *
 *  // initialize the security object and get handle
 *  lwm2m_object_t *sec_obj = lwm2m_object_security_init(&client_data);
 *
 *  // instantiate a new security object with next available instance ID
 *  int id = lwm2m_object_security_instance_create(&args, -1);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * #### Generating the keys
 *
 * The local key pair can be generated using [OpenSSL](https://www.openssl.org/).
 * 1. Generate the key pair using the secp256r1 parameters:
 * ~~~~~~~~~~~~~~~~~~~~
 *  $ openssl ecparam -name secp256r1 -genkey -outform der -out keys.der
 * ~~~~~~~~~~~~~~~~~~~~
 *
 * 2. Get the public part of the key:
 * ~~~~~~~~~~~~~~~~~~~~
 *  $ openssl ec -in keys.der -inform DER -outform DER -pubout | xxd -i
 * ~~~~~~~~~~~~~~~~~~~~
 *
 * 3. Get the private part of the key:
 * ~~~~~~~~~~~~~~~~~~~~
 *  $ openssl ec -in keys.der -inform DER -no_public -outform DER | xxd -i
 * ~~~~~~~~~~~~~~~~~~~~
 *
 * 4. If your server requires your public key as X-Y coordinates you can dump it as text, remove the
 *    first byte and split the rest in two equally-sized parts. The first part will be X, the
 *    second Y.
 * ~~~~~~~~~~~~~~~~~~~~
 *  $ openssl ec -in keys.der -inform DER -text
 *
 *  [...]
 *  pub:
 *    04:a0:c3:8e:cb:a1:02:eb:5d:25:96:98:bb:60:8e:
 *    28:19:56:06:96:70:15:9b:54:ff:d9:60:32:c3:3e:
 *    89:08:ae:3a:33:2f:54:5f:68:a2:ac:d1:b9:df:2b:
 *    79:65:49:3f:1c:ae:64:7a:32:02:e4:32:8d:6b:22:
 *    67:83:0d:7c:b2
 *  ASN1 OID: prime256v1
 *  NIST CURVE: P-256
 *  [...]
 * ~~~~~~~~~~~~~~~~~~~~
 *
 * Following the example above we have:
 * ~~~~~~~~~~~~~~~~~~~~
 * X : a0c38ecba102eb5d259698bb608e281956069670159b54ffd96032c33e8908ae
 * Y : 3a332f545f68a2acd1b9df2b7965493f1cae647a3202e4328d6b2267830d7cb2
 * ~~~~~~~~~~~~~~~~~~~~
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
#include "lwm2m_client.h"
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

/**
 * @name Resource IDs for the LWM2M Security Object
 * @{
 */
/**
 * @brief Server URI
 */
#define LWM2M_SECURITY_URI_ID                 0

/**
 * @brief Bootstrap server
 */
#define LWM2M_SECURITY_BOOTSTRAP_ID           1

/**
 * @brief Security mode
 */
#define LWM2M_SECURITY_SECURITY_ID            2

/**
 * @brief Public key or ID
 */
#define LWM2M_SECURITY_PUBLIC_KEY_ID          3

/**
 * @brief Server public key
 */
#define LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID   4

/**
 * @brief Secret key
 */
#define LWM2M_SECURITY_SECRET_KEY_ID          5

/**
 * @brief SMS security mode
 */
#define LWM2M_SECURITY_SMS_SECURITY_ID        6

/**
 * @brief SMS binding key parameters
 */
#define LWM2M_SECURITY_SMS_KEY_PARAM_ID       7

/**
 * @brief SMS binding secret keys
 */
#define LWM2M_SECURITY_SMS_SECRET_KEY_ID      8

/**
 * @brief Server SMS number
 */
#define LWM2M_SECURITY_SMS_SERVER_NUMBER_ID   9

/**
 * @brief Short server ID
 */
#define LWM2M_SECURITY_SHORT_SERVER_ID        10

/**
 * @brief Client hold-off time
 */
#define LWM2M_SECURITY_HOLD_OFF_ID            11

/**
 * @brief Bootstrap server account timeout
 */
#define LWM2M_SECURITY_BOOTSTRAP_TIMEOUT_ID   12
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
#define CONFIG_LWM2M_OBJ_SECURITY_PUB_KEY_ID_BUFSIZE         (128)
#endif

/**
 * @brief Buffer size of the server public key resource
 */
#ifndef CONFIG_LWM2M_OBJ_SECURITY_SERVER_PUB_KEY_BUFSIZE
#define CONFIG_LWM2M_OBJ_SECURITY_SERVER_PUB_KEY_BUFSIZE     (128)
#endif

/**
 * @brief Buffer size of the secret key resource
 */
#ifndef CONFIG_LWM2M_OBJ_SECURITY_SEC_KEY_BUFSIZE
#define CONFIG_LWM2M_OBJ_SECURITY_SEC_KEY_BUFSIZE            (64)
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
     * @brief Server's URI the instance is associated to.
     *
     * @note This buffer will be copied internally.
     */
    const char *server_uri;

    /**
     * @brief Security mode to use. For now only @ref LWM2M_SECURITY_MODE_NONE and
     *        @ref LWM2M_SECURITY_MODE_PRE_SHARED_KEY and
     *        @ref LWM2M_SECURITY_MODE_RAW_PUBLIC_KEY are supported.
     */
    uint8_t security_mode;

    /**
     * @brief Pointer to the Key ID when using @ref LWM2M_SECURITY_MODE_PRE_SHARED_KEY.
     *        Pointer to the public key encoded as a `SubjectPublicKeyInfo` sequence when using
     *        @ref LWM2M_SECURITY_MODE_RAW_PUBLIC_KEY (see @ref credman_load_public_key).
     *        May be `NULL` when @ref LWM2M_SECURITY_MODE_NONE is used.
     *
     * @note This buffer will be copied internally.
     */
    const uint8_t *pub_key_or_id;

    /**
     * @brief Length of @ref lwm2m_obj_security_args_t::pub_key_or_id.
     */
    size_t pub_key_or_id_len;

    /**
     * @brief Pointer to the Key when using @ref LWM2M_SECURITY_MODE_PRE_SHARED_KEY.
     *        Pointer to the private key PKCS8 DER encoded when using
     *        @ref LWM2M_SECURITY_MODE_RAW_PUBLIC_KEY (see @ref credman_load_private_key).
     *        May be `NULL` when @ref LWM2M_SECURITY_MODE_NONE is used.
     *
     * @note This buffer will be copied internally.
     */
    const uint8_t *secret_key;

    /**
     * @brief Length of @ref lwm2m_obj_security_args_t::secret_key.
     */
    size_t secret_key_len;

    /**
     * @brief Pointer to the server public key encoded as a `SubjectPublicKeyInfo` sequence when
     *        using @ref LWM2M_SECURITY_MODE_RAW_PUBLIC_KEY (see @ref credman_load_public_key).
     *        May be `NULL` when @ref LWM2M_SECURITY_MODE_NONE or
     *        @ref LWM2M_SECURITY_MODE_PRE_SHARED_KEY are used.
     *
     * @note This buffer will be copied internally.
     */
    const uint8_t *server_pub_key;

    /**
     * @brief Length of @ref lwm2m_obj_security_args_t::server_pub_key.
     */
    size_t server_pub_key_len;

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
 * @brief Initialize the Security object.
 *
 * @param[in] client_data  LwM2M client data.
 *
 * @return Pointer to the Security object on success
 */
lwm2m_object_t *lwm2m_object_security_init(lwm2m_client_data_t *client_data);

/**
 * @brief   Create a new Security instance and add it to the @p object list.
 *
 * @param[in]      args             Initialize structure with the parameter for the instance. May
 *                                  not be NULL.
 * @param[in]      instance_id      ID for the new instance. It must be between 0 and
 *                                  (UINT16_MAX - 1), if -1 the next available ID will be used.
 *
 * @return Instance ID (> 0) on success
 * @return -EINVAL if an invalid @p instance_id is given
 * @return -ENOMEM if no memory is available to create a new instance
 */
int lwm2m_object_security_instance_create(const lwm2m_obj_security_args_t *args,
                                          int32_t instance_id);

/**
 * @brief   Get the credential of a given instance of the security object.
 *
 * @param[in] instance_id                  ID of the instance.
 *
 * @return Credential tag.
 * @retval CREDMAN_TAG_EMPTY when no credential is assigned.
 */
credman_tag_t lwm2m_object_security_get_credential(uint16_t instance_id);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* OBJECTS_SECURITY_H */
