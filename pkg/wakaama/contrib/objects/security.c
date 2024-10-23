/*
 * Copyright (C) 2024 HAW Hamburg
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

#define _USED_INSTANCES(_obj) (_obj.wakaama_object.instanceList)
#define _FREE_INSTANCES(_obj) (_obj.free_instances)

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
     * @brief Buffer for the server public key resource.
     */
    uint8_t server_pub_key[CONFIG_LWM2M_OBJ_SECURITY_SERVER_PUB_KEY_BUFSIZE];

    /**
     * @brief Bytes used in @ref lwm2m_obj_security_inst_t::server_pub_key.
     */
    size_t server_pub_key_len;

    /**
     * @brief Credman public key structure for the server.
     */
    ecdsa_public_key_t server_credman_pub_key;

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
 * @param[in] context           LWM2M Context
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
static uint8_t _read_cb(lwm2m_context_t * context, uint16_t instance_id, int * num_data, 
                        lwm2m_data_t ** data_array, lwm2m_object_t * object);

/**
 * @brief 'Write' callback for the security object.

 * @param[in] context           LWM2M Context
 * @param[in] instance_id       ID of the instance to write to
 * @param[in] num_data          Number of resources to write
 * @param[in] data_array        Array of data to write
 * @param[in] object            Security object pointer
 *
 * @retval COAP_204_CHANGED                 on success
 * @retval COAP_400_BAD_REQUEST             otherwise
 */
static uint8_t _write_cb(lwm2m_context_t * context, uint16_t instance_id, int num_data,
                         lwm2m_data_t * data_array, lwm2m_object_t * object,
                         lwm2m_write_type_t write_type);

/**
 * @brief 'Delete' callback for the security object.
 *
 * @param[in] context           LWM2M Context
 * @param[in] instance_id       ID of the instance to delete
 * @param[in] object            Security object pointer
 *
 * @retval COAP_202_DELETED                 on success
 * @retval COAP_404_NOT_FOUND               when the instance can't be found
 */
static uint8_t _delete_cb(lwm2m_context_t * context, uint16_t instance_id,
                          lwm2m_object_t * object);

/**
 * @brief 'Create' callback for the security object.
 *
 * @param[in] context           LWM2M Context
 * @param[in] instance_id       ID of the instance to create
 * @param[in] num_data          Number of resources to write
 * @param[in] data_array        Array of data to write
 * @param[in] object            Security object pointer
 *
 * @retval COAP_201_CREATED                 on success
 * @retval COAP_500_INTERNAL_SERVER_ERROR   otherwise
 */
static uint8_t _create_cb(lwm2m_context_t * context, uint16_t instance_id, int num_data,
                          lwm2m_data_t * data_array, lwm2m_object_t * object);

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
 * @brief Initialize a new instance with the given arguments.
 *
 * @param instance[out]     Instance to initialize.
 * @param instance_id[in]   ID of the instance.
 * @param args[in]          Arguments to initialize the instance with.
 *
 * @retval 0 on success
 * @retval -ENOMEM if there is no memory available to copy credentials
 * @retval -EINVAL if the arguments are invalid
 */
static int _initialize_new_instance(lwm2m_obj_security_inst_t *instance, uint16_t instance_id,
                                    const lwm2m_obj_security_args_t *args);

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
#endif /* MODULE_WAKAAMA_CLIENT_DTLS */

struct lwm2m_security_object {
    lwm2m_object_t wakaama_object;              /**< Wakaama internal object */
    mutex_t lock;                               /**< mutex for the instances access */
    lwm2m_obj_security_inst_t *free_instances;  /**< list of free instances */
    lwm2m_obj_security_inst_t instances[CONFIG_LWM2M_OBJ_SECURITY_INSTANCES_MAX]; /**< instances */
#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
    credman_tag_t tag_count;                    /**< counter for the credential tags */
#endif
};

/**
 * @brief Implementation of the object interface for the Security Object.
 */
static struct lwm2m_security_object _security_object = {
    .lock = MUTEX_INIT,
#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
    .tag_count= CONFIG_LWM2M_CREDMAN_TAG_BASE,
#endif
    .wakaama_object = {
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
    }
};

#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
static int _update_credential(lwm2m_obj_security_inst_t *instance)
{
    assert(instance);

    credman_credential_t cred;
    credman_type_t type;

    if (instance->security_mode == LWM2M_SECURITY_MODE_PRE_SHARED_KEY) {
        type = CREDMAN_TYPE_PSK;
    }
    else {
        type = CREDMAN_TYPE_ECDSA;
    }

    credman_delete(instance->cred_tag, type);

    if (instance->cred_tag == CREDMAN_TAG_EMPTY) {
        instance->cred_tag = ++_security_object.tag_count;
    }

    cred.type = type;
    cred.tag = instance->cred_tag;

    DEBUG("[lwm2m:security]: updating credential with tag %d\n", cred.tag);
    if (type == CREDMAN_TYPE_PSK) {
        DEBUG("[lwm2m:security]: PSK ID: %.*s\n", (unsigned)instance->pub_key_or_id_len, instance->pub_key_or_id);
        DEBUG("[lwm2m:security]: PSK Key: %.*s\n", (unsigned)instance->secret_key_len, instance->secret_key);

        cred.params.psk.id.s = instance->pub_key_or_id;
        cred.params.psk.id.len = instance->pub_key_or_id_len;
        cred.params.psk.key.s = instance->secret_key;
        cred.params.psk.key.len = instance->secret_key_len;
    }
    else {
        DEBUG("[lwm2m:security]: Server pub. key: [");
        for (size_t i = 0; i < instance->server_pub_key_len; i++) {
            DEBUG("0x%02x ", instance->server_pub_key[i]);
        }
        DEBUG("]\n");

        DEBUG("[lwm2m:security]: Pub. key: [");
        for (size_t i = 0; i < instance->pub_key_or_id_len; i++) {
            DEBUG("0x%02x ", instance->pub_key_or_id[i]);
        }
        DEBUG("]\n");

        DEBUG("[lwm2m:security]: Priv. key: [");
        for (size_t i = 0; i < instance->secret_key_len; i++) {
            DEBUG("0x%02x ", instance->secret_key[i]);
        }
        DEBUG("]\n");

        int res = credman_load_public_key(instance->server_pub_key, instance->server_pub_key_len,
                                          &instance->server_credman_pub_key);
        if (res != CREDMAN_OK) {
            DEBUG("[lwm2m:security]: error loading server public key (%d)\n", res);
            return -1;
        }

        cred.params.ecdsa.client_keys = &instance->server_credman_pub_key;
        cred.params.ecdsa.client_keys_size = 1;

        res = credman_load_private_ecc_key(instance->secret_key, instance->secret_key_len, &cred);
        if (res != CREDMAN_OK) {
            DEBUG("[lwm2m:security]: error loading private key (%d)\n", res);
            return -1;
        }

        res = credman_load_public_key(instance->pub_key_or_id, instance->pub_key_or_id_len,
                                      &(cred.params.ecdsa.public_key));
        if (res != CREDMAN_OK) {
            DEBUG("[lwm2m:security]: error loading own public key (%d)\n", res);
            return -1;
        }
    }

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

        case LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID:
#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
            lwm2m_data_encode_opaque(instance->server_pub_key, instance->server_pub_key_len, data);
#else
            return COAP_404_NOT_FOUND;
#endif
            break;

        /* not implemented */
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

static uint8_t _read_cb(lwm2m_context_t * context, uint16_t instance_id, int * num_data, 
                        lwm2m_data_t ** data_array, lwm2m_object_t * object)
{
    (void)context;
    
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

        const uint16_t resList[] = {
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

        if (NULL == *data_array) {
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

static uint8_t _write_cb(lwm2m_context_t * context, uint16_t instance_id, int num_data,
                         lwm2m_data_t * data_array, lwm2m_object_t * object,
                         lwm2m_write_type_t write_type)
{
    (void)context;
    (void)write_type;
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
        lwm2m_data_t *data = &data_array[i];

        switch (data->id) {
            case LWM2M_SECURITY_URI_ID:
                DEBUG("[lwm2m:security:write]: writing URI\n");
                if (data->value.asBuffer.length > CONFIG_LWM2M_URI_MAX_SIZE - 1) {
                    result = COAP_400_BAD_REQUEST;
                }

                strncpy(instance->uri, (char *)data->value.asBuffer.buffer,
                        data->value.asBuffer.length);
                instance->uri[data->value.asBuffer.length] = '\0';
                result = COAP_204_CHANGED;
                break;

            case LWM2M_SECURITY_BOOTSTRAP_ID:
                DEBUG("[lwm2m:security:write]: writing bootstrap\n");
                if (lwm2m_data_decode_bool(data, &(instance->is_bootstrap)) == 1) {
                    result = COAP_204_CHANGED;
                }
                else {
                    result = COAP_400_BAD_REQUEST;
                }
                break;

            case LWM2M_SECURITY_SECURITY_ID:
                DEBUG("[lwm2m:security:write]: writing sec. mode\n");
                if (lwm2m_data_decode_int(data, &value) == 1) {
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
                if (lwm2m_data_decode_int(data, &value) == 1) {
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
                if (lwm2m_data_decode_int(data, &value) == 1) {
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
                if (lwm2m_data_decode_int(data, &value) == 1) {
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
                if (data->value.asBuffer.length >
                    CONFIG_LWM2M_OBJ_SECURITY_PUB_KEY_ID_BUFSIZE) {
                    result = COAP_500_INTERNAL_SERVER_ERROR;
                }

                /* copy the new value */
                memcpy(instance->pub_key_or_id, data->value.asBuffer.buffer,
                       data->value.asBuffer.length);

                /* if the size changed we need to modify the registered credential */
                if (instance->pub_key_or_id_len != data->value.asBuffer.length) {
                    instance->pub_key_or_id_len = data->value.asBuffer.length;
                    _update_credential(instance);
                }
#endif /* MODULE_WAKAAMA_CLIENT_DTLS */
                result = COAP_204_CHANGED;
                break;

            case LWM2M_SECURITY_SECRET_KEY_ID:
#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
                DEBUG("[lwm2m:security:write]: writing sec. key\n");
                if (data->value.asBuffer.length >
                    CONFIG_LWM2M_OBJ_SECURITY_SEC_KEY_BUFSIZE) {
                    result = COAP_500_INTERNAL_SERVER_ERROR;
                }

                /* copy the new value */
                memcpy(instance->secret_key, data->value.asBuffer.buffer,
                       data->value.asBuffer.length);

                /* if the size changed we need to modify the registered credential */
                if (instance->secret_key_len != data->value.asBuffer.length) {
                    instance->secret_key_len = data->value.asBuffer.length;
                    _update_credential(instance);
                }
#endif /* MODULE_WAKAAMA_CLIENT_DTLS */
                result = COAP_204_CHANGED;
                break;

            case LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID:
#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
                DEBUG("[lwm2m:security:write]: writing server pub. key\n");
                if (data->value.asBuffer.length >
                    CONFIG_LWM2M_OBJ_SECURITY_SERVER_PUB_KEY_BUFSIZE) {
                    result = COAP_500_INTERNAL_SERVER_ERROR;
                }

                /* copy the new value */
                memcpy(instance->server_pub_key, data->value.asBuffer.buffer,
                       data->value.asBuffer.length);

                /* if the size changed we need to modify the registered credential */
                if (instance->server_pub_key_len != data->value.asBuffer.length) {
                    instance->server_pub_key_len = data->value.asBuffer.length;
                    _update_credential(instance);
                }
#endif /* MODULE_WAKAAMA_CLIENT_DTLS */
                result = COAP_204_CHANGED;
                break;

            /* not implemented, ignore for now */
            case LWM2M_SECURITY_SMS_SECURITY_ID:
            case LWM2M_SECURITY_SMS_KEY_PARAM_ID:
            case LWM2M_SECURITY_SMS_SECRET_KEY_ID:
            case LWM2M_SECURITY_SMS_SERVER_NUMBER_ID:
                result = COAP_204_CHANGED;
                break;

            default:
                DEBUG("[lwm2m:security:write]: unknown resource %d\n", data->id);
                result = COAP_404_NOT_FOUND;
        }
        i++;
    } while (i < num_data && result == COAP_204_CHANGED);

    DEBUG("[lwm2m:security:write]: done\n");

out:
    return result;
}

static uint8_t _delete_cb(lwm2m_context_t * context, uint16_t instance_id,
                          lwm2m_object_t * object)
{
    (void)context;
    uint8_t result = COAP_404_NOT_FOUND;
    lwm2m_obj_security_inst_t *instance;

    mutex_lock(&_security_object.lock);
    DEBUG("[lwm2m:security:write]: looking for instance %d\n", instance_id);

    /* try to remove the requested instance from the list */
    object->instanceList = LWM2M_LIST_RM(object->instanceList, instance_id, &instance);

    /* check if the instance was found */
    if (NULL == instance) {
        result = COAP_404_NOT_FOUND;
        goto free_out;
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

    /* add instance to free instances list */
    instance->list.id = UINT16_MAX;
    _FREE_INSTANCES(_security_object) = (lwm2m_obj_security_inst_t *) LWM2M_LIST_ADD(
        _FREE_INSTANCES(_security_object), instance);
    result = COAP_202_DELETED;

free_out:
    mutex_unlock(&_security_object.lock);
    return result;
}

static uint8_t _create_cb(lwm2m_context_t * context, uint16_t instance_id, int num_data,
                          lwm2m_data_t * data_array, lwm2m_object_t * object)
{
    (void)context;

    lwm2m_obj_security_inst_t *instance;
    uint8_t result;

    mutex_lock(&_security_object.lock);

    /* check that the ID is free to use */
    if (LWM2M_LIST_FIND(_USED_INSTANCES(_security_object), instance_id ) != NULL) {
        DEBUG("[lwm2m:security]: instance ID %" PRIu16 " already in use\n", instance_id);
        result = COAP_400_BAD_REQUEST;
        goto free_out;
    }

    /* try to allocate an instance, by popping a free node from the list */
    _FREE_INSTANCES(_security_object) = (lwm2m_obj_security_inst_t *) lwm2m_list_remove(
        (lwm2m_list_t *) _FREE_INSTANCES(_security_object),
        UINT16_MAX,
        (lwm2m_list_t **) &instance
    );

    if (!instance) {
        DEBUG("[lwm2m:security:create] can't allocate free instance\n");
        result = COAP_500_INTERNAL_SERVER_ERROR;
        goto free_out;
    }

    memset(instance, 0, sizeof(lwm2m_obj_security_inst_t));

#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
    instance->cred_tag = CREDMAN_TAG_EMPTY;
#endif

    /* add to the object instance list */
    instance->list.id = instance_id;
    object->instanceList = LWM2M_LIST_ADD(object->instanceList, instance);

    /* write incoming data to the instance */
    result = _write_cb(context, instance_id, num_data, data_array, object, LWM2M_WRITE_PARTIAL_UPDATE);

    if (result != COAP_204_CHANGED) {
        _delete_cb(context,instance_id, object);
    }
    else {
        result = COAP_201_CREATED;
        goto free_out;
    }

free_out:
    mutex_unlock(&_security_object.lock);
    return result;
}

lwm2m_object_t *lwm2m_object_security_init(lwm2m_client_data_t *client_data)
{
    /* initialize the instances */
    for (unsigned i = 0; i < CONFIG_LWM2M_OBJ_SECURITY_INSTANCES_MAX; i++) {
        _security_object.instances[i].list.next = NULL;
        _security_object.instances[i].list.id = UINT16_MAX;

        _FREE_INSTANCES(_security_object) = (lwm2m_obj_security_inst_t *) LWM2M_LIST_ADD(
            _FREE_INSTANCES(_security_object), &(_security_object.instances[i]));
    }

    _security_object.wakaama_object.userData = client_data;
    return &(_security_object.wakaama_object);
}

static int _initialize_new_instance(lwm2m_obj_security_inst_t *instance, uint16_t instance_id,
                             const lwm2m_obj_security_args_t *args)
{
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
    size_t uri_len = strlen(args->server_uri);
    if (uri_len > CONFIG_LWM2M_URI_MAX_SIZE - 1) {
        DEBUG("[lwm2m:security]: can't copy URI, not enough space\n");
        return -ENOMEM;
    }

    strncpy(instance->uri, args->server_uri, uri_len);
    instance->uri[uri_len] = '\0';

#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
    if (LWM2M_SECURITY_MODE_NONE != args->security_mode) {
        if (LWM2M_SECURITY_MODE_CERTIFICATE == args->security_mode) {
            DEBUG("[lwm2m:security]: certificate mode not supported\n");
            return -EINVAL;
        }

        /* copy keys locally */
        if (LWM2M_SECURITY_MODE_RAW_PUBLIC_KEY == args->security_mode) {
            if (!args->server_pub_key ||
                args->server_pub_key_len > CONFIG_LWM2M_OBJ_SECURITY_SERVER_PUB_KEY_BUFSIZE) {
                DEBUG("[lwm2m:security]: Invalid server public key\n");
                return -EINVAL;
            }
            memcpy(instance->server_pub_key, args->server_pub_key, args->server_pub_key_len);
            instance->server_pub_key_len = args->server_pub_key_len;
        }

        if (!args->pub_key_or_id ||
            args->pub_key_or_id_len > CONFIG_LWM2M_OBJ_SECURITY_PUB_KEY_ID_BUFSIZE) {
            DEBUG("[lwm2m:security]: Invalid public key or ID\n");
            return -EINVAL;
        }
        memcpy(instance->pub_key_or_id, args->pub_key_or_id, args->pub_key_or_id_len);
        instance->pub_key_or_id_len = args->pub_key_or_id_len;

        if (!args->secret_key ||
            args->secret_key_len > CONFIG_LWM2M_OBJ_SECURITY_SEC_KEY_BUFSIZE) {
            DEBUG("[lwm2m:security]: Invalid secret key\n");
            return -EINVAL;
        }
        memcpy(instance->secret_key, args->secret_key, args->secret_key_len);
        instance->secret_key_len = args->secret_key_len;

        /* assign a credential tag */
        instance->cred_tag = ++_security_object.tag_count;

        if (_update_credential(instance) < 0) {
            DEBUG("[lwm2m:security]: could not register the credential\n");
            return -ENOMEM;
        }
    }
#endif /* MODULE_WAKAAMA_CLIENT_DTLS */

    return 0;
}

int lwm2m_object_security_instance_create(const lwm2m_obj_security_args_t *args,
                                          int32_t instance_id)
{
    assert(args);
    int result = -ENOMEM;
    uint16_t _instance_id;
    lwm2m_obj_security_inst_t *instance = NULL;

    /* lock object */
    mutex_lock(&_security_object.lock);

    /* some sanity checks */
    if (!args || !args->server_id || !args->server_uri) {
        DEBUG("[lwm2m:security]: invalid arguments\n");
        result = -EINVAL;
        goto free_out;
    }

    DEBUG("[lwm2m:security]: creating new instance\n");

    /* determine ID for new instance */
    if (instance_id < 0) {
        _instance_id = lwm2m_list_newId((lwm2m_list_t *)_USED_INSTANCES(_security_object));
    }
    else {
        /* sanity check */
        if (instance_id >= (UINT16_MAX - 1)) {
            DEBUG("[lwm2m:security]: instance ID %" PRIi32 " is too big\n", instance_id);
            result = -EINVAL;
            goto free_out;
        }

        _instance_id = (uint16_t)instance_id;

        /* check that the ID is free to use */
        if (LWM2M_LIST_FIND(_USED_INSTANCES(_security_object), _instance_id ) != NULL) {
            DEBUG("[lwm2m:security]: instance ID %" PRIi32 " already in use\n", instance_id);
            result = -EINVAL;
            goto free_out;
        }
    }

    /* try to allocate an instance, by popping a free node from the list */
    _FREE_INSTANCES(_security_object) = (lwm2m_obj_security_inst_t *) lwm2m_list_remove(
        (lwm2m_list_t *) _FREE_INSTANCES(_security_object),
        UINT16_MAX,
        (lwm2m_list_t **) &instance
    );

    if (!instance) {
        DEBUG("[lwm2m:security]: can't allocate new instance\n");
        goto free_out;
    }

    result = _initialize_new_instance(instance, _instance_id, args);
    if (result < 0) {
        DEBUG("[lwm2m:security]: could not initialize new instance\n");
        goto free_out;
    }

    DEBUG("[lwm2m:security]: added instance with URI: %s\n", instance->uri);

    /* add the new instance to the list */
    _USED_INSTANCES(_security_object) = LWM2M_LIST_ADD(_USED_INSTANCES(_security_object), instance);
    result = instance->list.id;

free_out:
    mutex_unlock(&_security_object.lock);
    return result;
}

credman_tag_t lwm2m_object_security_get_credential(uint16_t instance_id)
{
#if IS_USED(MODULE_WAKAAMA_CLIENT_DTLS)
    lwm2m_obj_security_inst_t *instance;

    /* try to get the requested instance from the object list */
    instance = (lwm2m_obj_security_inst_t *)lwm2m_list_find(_USED_INSTANCES(_security_object),
                                                            instance_id);
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
