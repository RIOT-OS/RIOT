/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @{
 * @ingroup     lwm2m_objects_security
 *
 * @file
 * @brief       Security object implementation for LwM2M client using Wakaama
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "liblwm2m.h"
#include "objects/security.h"
#include "lwm2m_client_config.h"
#include "lwm2m_client.h"
#include "kernel_defines.h"
#include "net/credman.h"
#include "mutex.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ENABLE_DEBUG    0
#include "debug.h"

/**
 * @brief Descriptor of a LwM2M Security object instance (Object ID = 0)
 */
typedef struct lwm2m_obj_security_inst {
    /**
     * @brief Linked list handle.
     */
    lwm2m_list_t list;

    /**
     * @brief Server URI.
     */
    char uri[CONFIG_LWM2M_URI_MAX_SIZE];

    /**
     * @brief Indicates if the server associated to the security instance is a Bootstrap-Server.
     */
    bool is_bootstrap;

    /**
     * @brief Security mode to use with the server.
     */
    uint8_t security_mode;

#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
    /**
     * @brief Tag of the credential to use with the server.
     */
    credman_tag_t cred_tag;

    /**
     * @brief Buffer for the public key or ID resource.
     */
    uint8_t pub_key_or_id[CONFIG_LWM2M_OBJ_SECURITY_PUB_KEY_ID_BUFSIZE];

    /**
     * @brief Bytes used in @ref lwm2m_obj_security_inst_t::pub_key_or_id.
     */
    size_t pub_key_or_id_len;

    /**
     * @brief Buffer for the secret or private key resource.
     */
    uint8_t secret_key[CONFIG_LWM2M_OBJ_SECURITY_SEC_KEY_BUFSIZE];

    /**
     * @brief Bytes used in @ref lwm2m_obj_security_inst_t::secret_key.
     */
    size_t secret_key_len;
#endif /* MODULE_WAKAAMA_CLIENT_DTLS */

    /**
     * @brief Short ID to reference the server.
     */
    uint16_t short_id;

    /**
     * @brief Hold off time for registration.
     */
    uint32_t client_hold_off_time;

    /**
     * @brief Timeout for Bootstrap-Server account deletion.
     */
    uint32_t bs_account_timeout;
} lwm2m_obj_security_inst_t;

/**
 * @brief 'Read' callback for the security object.
 *
 * @param[in] instance_id       ID of the instance to read
 * @param[in, out] num_data     Number of resources requested. 0 means all.
 * @param[in, out] data_array   Initialized data array to output the values,
 *                              when @p num_data != 0. Uninitialized otherwise.
 * @param[in] object            Security object pointer
 *
 * @retval COAP_205_CONTENT                 on success
 * @retval COAP_404_NOT_FOUND               when resource can't be found
 * @retval COAP_500_INTERNAL_SERVER_ERROR   otherwise
 */
static uint8_t _read_cb(uint16_t instance_id, int *num_data, lwm2m_data_t **data_array,
                        lwm2m_object_t *object);

/**
 * @brief 'Write' callback for the security object.
 *
 * @param[in] instance_id       ID of the instance to write to
 * @param[in] num_data          Number of resources to write
 * @param[in] data_array        Array of data to write
 * @param[in] object            Security object pointer
 *
 * @retval COAP_204_CHANGED                 on success
 * @retval COAP_400_BAD_REQUEST             otherwise
 */
static uint8_t _write_cb(uint16_t instance_id, int num_data, lwm2m_data_t * data_array,
                         lwm2m_object_t * object);

/**
 * @brief 'Delete' callback for the security object.
 *
 * @param[in] instance_id       ID of the instance to delete
 * @param[in] object            Security object pointer
 *
 * @retval COAP_202_DELETED                 on success
 * @retval COAP_404_NOT_FOUND               when the instance can't be found
 */
static uint8_t _delete_cb(uint16_t instance_id, lwm2m_object_t *object);

/**
 * @brief 'Create' callback for the security object.
 *
 * @param[in] instance_id       ID of the instance to create
 * @param[in] num_data          Number of resources to write
 * @param[in] data_array        Array of data to write
 * @param[in] object            Security object pointer
 *
 * @retval COAP_201_CREATED                 on success
 * @retval COAP_500_INTERNAL_SERVER_ERROR   otherwise
 */
static uint8_t _create_cb(uint16_t instance_id, int num_data, lwm2m_data_t * data_array,
                          lwm2m_object_t * object);

/**
 * @brief Get a value from a security object instance.
 *
 * @param data[in, out]     Data structure indicating the id of the resource
 *                          to get the value of. It will contain the value if
 *                          successful.
 * @param instance[in]      Instance to get the data from.
 * @retval 0 on success
 * @retval <0 otherwise
 */
static int _get_value(lwm2m_data_t *data, lwm2m_obj_security_inst_t *instance);

/**
 * @brief Free an instance from the pool.
 *
 * @param[in] instance      Pointer to the instance to free. Must not be NULL.
 */
static void _free_instance(lwm2m_obj_security_inst_t *instance);

/**
 * @brief Free an instance from the pool.
 *
 * @param[in] instance      Pointer to the instance to free. Must not be NULL.
 */
static void _free_instance(lwm2m_obj_security_inst_t *instance);

/**
 * @brief Implementation of the object interface for the Security Object.
 */
static lwm2m_object_t _security_object = {
    .next           = NULL,
    .objID          = LWM2M_SECURITY_OBJECT_ID,
    .instanceList   = NULL,
    .readFunc       = _read_cb,
    .writeFunc      = _write_cb,
    .createFunc     = _create_cb,
    .deleteFunc     = _delete_cb,
    .executeFunc    = NULL,
    .discoverFunc   = NULL,
    .userData       = NULL,
};

#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
/**
 * @brief Update a credential in the credman registry with the current instance information.
 *
 * @param[in] instance      Instance to update the credential to.
 *
 * @retval 0 on success
 * @retval <0 otherwise
 */
static int _update_credential(lwm2m_obj_security_inst_t *instance);

/**
 * @brief Credential tag counter
 */
static credman_tag_t _tag_count = CONFIG_LWM2M_CREDMAN_TAG_BASE;
#endif /* MODULE_WAKAAMA_CLIENT_DTLS */

/**
 * @brief Pool of object instances.
 */
static lwm2m_obj_security_inst_t _instances[CONFIG_LWM2M_OBJ_SECURITY_INSTANCES_MAX] = { 0 };

/**
 * @brief Mutex for instance pool access.
 */
static mutex_t _mutex = MUTEX_INIT;

static lwm2m_obj_security_inst_t *_get_free_instance(void)
{
    lwm2m_obj_security_inst_t *instance = NULL;
    mutex_lock(&_mutex);

    for (unsigned i = 0; i < CONFIG_LWM2M_OBJ_SECURITY_INSTANCES_MAX; i++) {
        /* we use the server short_id to check if the instance is not used, as 0 is not allowed */
        if (!_instances[i].short_id) {
            instance = &_instances[i];
            instance->short_id = UINT16_MAX; /* temporary ID to mark as used */
            break;
        }
    }

    mutex_unlock(&_mutex);
    return instance;
}

static void _free_instance(lwm2m_obj_security_inst_t *instance)
{
    assert(instance);
    mutex_lock(&_mutex);

    instance->short_id = 0;

    mutex_unlock(&_mutex);
}

#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
static int _update_credential(lwm2m_obj_security_inst_t *instance)
{
    assert(instance);

    credman_credential_t cred;

    /* TODO: add support for RPK */
    credman_delete(instance->cred_tag, CREDMAN_TYPE_PSK);

    DEBUG("[lwm2m:security]: updating credential\n");
    DEBUG("[lwm2m:security]: PSK ID: %.*s\n", instance->pub_key_or_id_len, instance->pub_key_or_id);
    DEBUG("[lwm2m:security]: PSK Key: %.*s\n", instance->secret_key_len, instance->secret_key);

    if (instance->cred_tag == CREDMAN_TAG_EMPTY) {
        instance->cred_tag = ++_tag_count;
    }

    cred.type = CREDMAN_TYPE_PSK;
    cred.tag = instance->cred_tag;
    cred.params.psk.id.s = instance->pub_key_or_id;
    cred.params.psk.id.len = instance->pub_key_or_id_len;
    cred.params.psk.key.s = instance->secret_key;
    cred.params.psk.key.len = instance->secret_key_len;

    lwm2m_client_add_credential(instance->cred_tag);

    return credman_add(&cred) == CREDMAN_OK ? 0 : -1;
}
#endif /* MODULE_WAKAAMA_CLIENT_DTLS */

static int _get_value(lwm2m_data_t *data, lwm2m_obj_security_inst_t *instance)
{
    assert(data);
    assert(instance);

    /* resource IDs are defined by Wakaama in liblwm2m.h */
    switch (data->id) {
        case LWM2M_SECURITY_URI_ID:
            lwm2m_data_encode_string(instance->uri, data);
            break;

        case LWM2M_SECURITY_BOOTSTRAP_ID:
            lwm2m_data_encode_bool(instance->is_bootstrap, data);
            break;

        case LWM2M_SECURITY_SHORT_SERVER_ID:
            lwm2m_data_encode_int(instance->short_id, data);
            break;

        case LWM2M_SECURITY_HOLD_OFF_ID:
            lwm2m_data_encode_int(instance->client_hold_off_time, data);
            break;

        case LWM2M_SECURITY_BOOTSTRAP_TIMEOUT_ID:
            lwm2m_data_encode_int(instance->bs_account_timeout, data);
            break;

        case LWM2M_SECURITY_SECURITY_ID:
            lwm2m_data_encode_int(instance->security_mode, data);
            break;

        case LWM2M_SECURITY_PUBLIC_KEY_ID:
#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
            lwm2m_data_encode_opaque(instance->pub_key_or_id, instance->pub_key_or_id_len, data);
#else
            return COAP_404_NOT_FOUND;
#endif
            break;

        case LWM2M_SECURITY_SECRET_KEY_ID:
#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
            lwm2m_data_encode_opaque(instance->secret_key, instance->secret_key_len, data);
#else
            return COAP_404_NOT_FOUND;
#endif
            break;

        /* not implemented */
        case LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID:
        case LWM2M_SECURITY_SMS_SECURITY_ID:
        case LWM2M_SECURITY_SMS_KEY_PARAM_ID:
        case LWM2M_SECURITY_SMS_SECRET_KEY_ID:
        case LWM2M_SECURITY_SMS_SERVER_NUMBER_ID:
            return COAP_404_NOT_FOUND;

        default:
            return COAP_404_NOT_FOUND;
    }
    return COAP_205_CONTENT;
}

static uint8_t _read_cb(uint16_t instance_id, int *num_data, lwm2m_data_t **data_array,
                        lwm2m_object_t *object)
{
    lwm2m_obj_security_inst_t *instance;
    uint8_t result;
    int i = 0;

    /* try to get the requested instance from the object list */
    instance = (lwm2m_obj_security_inst_t *)lwm2m_list_find(object->instanceList, instance_id);
    if (NULL == instance) {
        result = COAP_404_NOT_FOUND;
        goto out;
    }

    /* if the number of resources is not specified, we need to read all resources */
    if (!*num_data) {
        DEBUG("[security:read] all resources are read\n");

        uint16_t resList[] = {
            LWM2M_SECURITY_URI_ID,
            LWM2M_SECURITY_BOOTSTRAP_ID,
            LWM2M_SECURITY_SECURITY_ID,
#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
            LWM2M_SECURITY_PUBLIC_KEY_ID,
            LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID,
            LWM2M_SECURITY_SECRET_KEY_ID,
#endif
            /* LWM2M_SECURITY_SMS_SECURITY_ID, */
            /* LWM2M_SECURITY_SMS_KEY_PARAM_ID, */
            /* LWM2M_SECURITY_SMS_SECRET_KEY_ID, */
            /* LWM2M_SECURITY_SMS_SERVER_NUMBER_ID, */
            LWM2M_SECURITY_SHORT_SERVER_ID,
            LWM2M_SECURITY_HOLD_OFF_ID,
            LWM2M_SECURITY_BOOTSTRAP_TIMEOUT_ID
        };

        /* try to allocate data structures for all resources */
        int resNum = ARRAY_SIZE(resList);
        *data_array = lwm2m_data_new(resNum);

        if (NULL == data_array) {
            result = COAP_500_INTERNAL_SERVER_ERROR;
            goto out;
        }

        /* indicate the number of resources that are returned */
        *num_data = resNum;

        /* prepare the resource ID of all resources for the request */
        for (i = 0 ; i < resNum ; i++) {
            (*data_array)[i].id = resList[i];
        }
    }

    /* the data structures in data_array contain the IDs of the resources to get the values of */
    i = 0;
    do {
        DEBUG("[security:read] read: %d\n", (*data_array)[i].id);
        result = _get_value(&(*data_array)[i], instance);
        i++;
    } while (i < *num_data && COAP_205_CONTENT == result);

out:
    return result;
}

static uint8_t _write_cb(uint16_t instance_id, int num_data, lwm2m_data_t *data_array,
                         lwm2m_object_t *object)
{
    lwm2m_obj_security_inst_t *instance;
    int64_t value;
    uint8_t result = COAP_404_NOT_FOUND;
    int i = 0;

    DEBUG("[lwm2m:security:write]: looking for instance %d\n", instance_id);

    /* try to get the requested instance from the object list */
    instance = (lwm2m_obj_security_inst_t *)lwm2m_list_find(object->instanceList, instance_id);
    if (!instance) {
        DEBUG("[lwm2m:security:write]: not found\n");
        goto out;
    }

    /* iterate over the array of data to write */
    do {
        switch (data_array[i].id) {
            case LWM2M_SECURITY_URI_ID:
                DEBUG("[lwm2m:security:write]: writing URI\n");
                if (data_array[i].value.asBuffer.length > CONFIG_LWM2M_URI_MAX_SIZE) {
                    result = COAP_400_BAD_REQUEST;
                }

                strncpy(instance->uri, (char *)data_array[i].value.asBuffer.buffer,
                        data_array[i].value.asBuffer.length);
                result = COAP_204_CHANGED;
                break;

            case LWM2M_SECURITY_BOOTSTRAP_ID:
                DEBUG("[lwm2m:security:write]: writing bootstrap\n");
                if (lwm2m_data_decode_bool(&data_array[i], &(instance->is_bootstrap)) == 1) {
                    result = COAP_204_CHANGED;
                }
                else {
                    result = COAP_400_BAD_REQUEST;
                }
                break;

            case LWM2M_SECURITY_SECURITY_ID:
                DEBUG("[lwm2m:security:write]: writing sec. mode\n");
                if (lwm2m_data_decode_int(&data_array[i], &value) == 1) {
                    /* check if it is a valid security mode */
                    if (LWM2M_SECURITY_MODE_NONE == value ||
                        LWM2M_SECURITY_MODE_PRE_SHARED_KEY == value ||
                        LWM2M_SECURITY_MODE_RAW_PUBLIC_KEY == value ||
                        LWM2M_SECURITY_MODE_CERTIFICATE == value) {
                        instance->security_mode = value;
                        result = COAP_204_CHANGED;
                        break;
                    }
                }
                result = COAP_400_BAD_REQUEST;
                break;

            case LWM2M_SECURITY_SHORT_SERVER_ID:
                DEBUG("[lwm2m:security:write]: writing short ID\n");
                if (lwm2m_data_decode_int(&data_array[i], &value) == 1) {
                    /* check valid range of value */
                    if (value > 0 && value < UINT16_MAX) {
                        instance->short_id = value;
                        result = COAP_204_CHANGED;
                        break;
                    }
                }
                result = COAP_400_BAD_REQUEST;
                break;

            case LWM2M_SECURITY_HOLD_OFF_ID:
                DEBUG("[lwm2m:security:write]: writing hold off time\n");
                if (lwm2m_data_decode_int(&data_array[i], &value) == 1) {
                    /* check valid range of value */
                    if (value >= 0 && value <= UINT32_MAX) {
                        instance->client_hold_off_time = value;
                        result = COAP_204_CHANGED;
                        break;
                    }
                }
                result = COAP_400_BAD_REQUEST;
                break;

            case LWM2M_SECURITY_BOOTSTRAP_TIMEOUT_ID:
                DEBUG("[lwm2m:security:write]: writing bootstrap timeout\n");
                if (lwm2m_data_decode_int(&data_array[i], &value) == 1) {
                    /* check valid range of value */
                    if (value >= 0 && value <= UINT32_MAX) {
                        instance->bs_account_timeout = value;
                        result = COAP_204_CHANGED;
                        break;
                    }
                }
                result = COAP_400_BAD_REQUEST;
                break;

            case LWM2M_SECURITY_PUBLIC_KEY_ID:
#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
                DEBUG("[lwm2m:security:write]: writing pub. key or ID\n");
                if (data_array[i].value.asBuffer.length >
                    CONFIG_LWM2M_OBJ_SECURITY_PUB_KEY_ID_BUFSIZE) {
                    result = COAP_500_INTERNAL_SERVER_ERROR;
                }

                /* copy the new value */
                memcpy(instance->pub_key_or_id, data_array[i].value.asBuffer.buffer,
                       data_array[i].value.asBuffer.length);

                /* if the size changed we need to modify the registered credential */
                if (instance->pub_key_or_id_len != data_array[i].value.asBuffer.length) {
                    instance->pub_key_or_id_len = data_array[i].value.asBuffer.length;
                    _update_credential(instance);
                }
#endif /* MODULE_WAKAAMA_CLIENT_DTLS */
                result = COAP_204_CHANGED;
                break;

            case LWM2M_SECURITY_SECRET_KEY_ID:
#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
                DEBUG("[lwm2m:security:write]: writing sec. key\n");
                if (data_array[i].value.asBuffer.length >
                    CONFIG_LWM2M_OBJ_SECURITY_SEC_KEY_BUFSIZE) {
                    result = COAP_500_INTERNAL_SERVER_ERROR;
                }

                /* copy the new value */
                memcpy(instance->secret_key, data_array[i].value.asBuffer.buffer,
                       data_array[i].value.asBuffer.length);

                /* if the size changed we need to modify the registered credential */
                if (instance->secret_key_len != data_array[i].value.asBuffer.length) {
                    instance->secret_key_len = data_array[i].value.asBuffer.length;
                    _update_credential(instance);
                }
#endif /* MODULE_WAKAAMA_CLIENT_DTLS */
                result = COAP_204_CHANGED;
                break;

            /* not implemented, ignore for now */
            case LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID:
            case LWM2M_SECURITY_SMS_SECURITY_ID:
            case LWM2M_SECURITY_SMS_KEY_PARAM_ID:
            case LWM2M_SECURITY_SMS_SECRET_KEY_ID:
            case LWM2M_SECURITY_SMS_SERVER_NUMBER_ID:
                result = COAP_204_CHANGED;
                break;

            default:
                DEBUG("[lwm2m:security:write]: unknown resource %d\n", data_array[i].id);
                result = COAP_404_NOT_FOUND;
        }
        i++;
    } while (i < num_data && result == COAP_204_CHANGED);

    DEBUG("[lwm2m:security:write]: done\n");

out:
    return result;
}

static uint8_t _delete_cb(uint16_t instance_id, lwm2m_object_t *object)
{
    lwm2m_obj_security_inst_t *instance;

    DEBUG("[lwm2m:security:write]: looking for instance %d\n", instance_id);

    /* try to remove the requested instance from the list */
    object->instanceList = LWM2M_LIST_RM(object->instanceList, instance_id, &instance);

    /* check if the instance was found */
    if (NULL == instance) {
        return COAP_404_NOT_FOUND;
    }

#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
    /* if there is an associated credential, de-register */
    if (instance->cred_tag != CREDMAN_TAG_EMPTY) {
        credman_type_t type = instance->security_mode == LWM2M_SECURITY_MODE_PRE_SHARED_KEY ?
                                CREDMAN_TYPE_PSK : CREDMAN_TYPE_ECDSA;
        credman_delete(instance->cred_tag, type);
        lwm2m_client_remove_credential(instance->cred_tag);
    }
#endif /* MODULE_WAKAAMA_CLIENT_DTLS */

    _free_instance(instance);

    return COAP_202_DELETED;
}

static uint8_t _create_cb(uint16_t instance_id, int num_data, lwm2m_data_t * data_array,
                          lwm2m_object_t * object)
{
    lwm2m_obj_security_inst_t *instance;
    uint8_t result;

    /* try to allocate a new instance */
    instance = _get_free_instance();
    if (!instance) {
        DEBUG("[lwm2m:security:create] can't allocate free instance\n");
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    memset(instance, 0, sizeof(lwm2m_obj_security_inst_t));

#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
    instance->cred_tag = CREDMAN_TAG_EMPTY;
#endif

    /* add to the object instance list */
    instance->list.id = instance_id;
    object->instanceList = LWM2M_LIST_ADD(object->instanceList, instance);

    /* write incoming data to the instance */
    result = _write_cb(instance_id, num_data, data_array, object);

    if (result != COAP_204_CHANGED) {
        _delete_cb(instance_id, object);
    }
    else {
        result = COAP_201_CREATED;
    }

    return result;
}

lwm2m_object_t *lwm2m_object_security_get(void)
{
    return &_security_object;
}

int lwm2m_object_security_instance_create(lwm2m_object_t *object, uint16_t instance_id,
                                          lwm2m_obj_security_args_t *args)
{
    assert(object);
    assert(args);

    lwm2m_obj_security_inst_t *instance = NULL;

    /* some sanity checks */
    if (!object || !args || !args->server_id || !args->server_uri ||
        object->objID != LWM2M_SECURITY_OBJECT_ID) {
        goto out;
    }

    DEBUG("[lwm2m:security]: creating new instance\n");

    /* try to allocate an instance */
    instance = _get_free_instance();
    if (!instance) {
        DEBUG("[lwm2m:security]: can't allocate new instance\n");
        goto out;
    }

    memset(instance, 0, sizeof(lwm2m_obj_security_inst_t));

    instance->list.id = instance_id;
    instance->short_id = args->server_id;
    instance->security_mode = args->security_mode;
    instance->is_bootstrap = args->is_bootstrap;
    instance->client_hold_off_time = args->client_hold_off_time;
    instance->bs_account_timeout = args->bootstrap_account_timeout;

#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
    instance->cred_tag = CREDMAN_TAG_EMPTY;
#endif

    /* copy the URI locally */
    if (strlen(args->server_uri) > CONFIG_LWM2M_URI_MAX_SIZE - 1) {
        DEBUG("[lwm2m:security]: can't copy URI, not enough space\n");
        goto free_out;
    }

    strcpy(instance->uri, args->server_uri);

#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
    if (LWM2M_SECURITY_MODE_NONE != args->security_mode) {
        /* we need a valid credential */
        if (!args->cred) {
            DEBUG("[lwm2m:security]: no credential provided\n");
            goto free_out;
        }

        if (LWM2M_SECURITY_MODE_PRE_SHARED_KEY == args->security_mode) {
            if (args->cred->type != CREDMAN_TYPE_PSK) {
                DEBUG("[lwm2m:security]: incorrect credential type for PSK mode\n");
                goto free_out;
            }

            /* copy the credential locally */
            if (args->cred->params.psk.id.len > CONFIG_LWM2M_OBJ_SECURITY_PUB_KEY_ID_BUFSIZE) {
                DEBUG("[lwm2m:security]: PSK ID too long\n");
                goto free_out;
            }
            memcpy(instance->pub_key_or_id, args->cred->params.psk.id.s,
                    args->cred->params.psk.id.len);
            instance->pub_key_or_id_len = args->cred->params.psk.id.len;

            if (args->cred->params.psk.key.len > CONFIG_LWM2M_OBJ_SECURITY_SEC_KEY_BUFSIZE) {
                DEBUG("[lwm2m:security]: PSK key too long\n");
                goto free_out;
            }
            memcpy(instance->secret_key, args->cred->params.psk.key.s,
                    args->cred->params.psk.key.len);
            instance->secret_key_len = args->cred->params.psk.key.len;

            /* assign a credential tag */
            instance->cred_tag = ++_tag_count;

            if (_update_credential(instance) < 0) {
                DEBUG("[lwm2m:security]: could not register the credential\n");
                goto free_out;
            }
        }
        else if (LWM2M_SECURITY_MODE_RAW_PUBLIC_KEY == args->security_mode) {
            // TODO: add support for RPK
            DEBUG("[lwm2m:security]: RPK not supported yet\n");
            goto free_out;
        }
        else {
            DEBUG("[lwm2m:security]: certificate mode not supported\n");
            goto free_out;
        }
    }
#endif /* MODULE_WAKAAMA_CLIENT_DTLS */

    DEBUG("[lwm2m:security]: added instance with URI: %s\n", instance->uri);

    /* add the new instance to the list */
    object->instanceList = LWM2M_LIST_ADD(object->instanceList, instance);
    goto out;

free_out:
    _free_instance(instance);
    instance = NULL;
out:
    return instance == NULL ? -1 : 0;
}

credman_tag_t lwm2m_object_security_get_credential(lwm2m_object_t *object, uint16_t instance_id)
{
    assert(object);

#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
    lwm2m_obj_security_inst_t *instance;

    /* try to get the requested instance from the object list */
    instance = (lwm2m_obj_security_inst_t *)lwm2m_list_find(object->instanceList, instance_id);
    if (NULL == instance) {
        DEBUG("[lwm2m:security]: no instance %d\n", instance_id);
        return CREDMAN_TAG_EMPTY;
    }

    return instance->cred_tag;
#else
    (void) instance_id;
    return CREDMAN_TAG_EMPTY;
#endif /* MODULE_WAKAAMA_CLIENT_DTLS */
}
