/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @{
 * @ingroup     lwm2m_objects_light_control
 *
 * @file
 * @brief       Light Control object implementation for LwM2M client using Wakaama
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "liblwm2m.h"
#include "lwm2m_client.h"
#include "xtimer.h"
#include "objects/light_control.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief LwM2M Light Control object instance
 */
typedef struct lwm2m_obj_light_control_inst {
    lwm2m_list_t list;                                              /**< list handle */
    bool status;                                                    /**< light status */
    uint8_t dimmer;                                                 /**< dimmer value */
    char color[CONFIG_LWM2M_LIGHT_CONTROL_COLOR_MAX_SIZE];          /**< light color */
    char app_type[CONFIG_LWM2M_LIGHT_CONTROL_APP_TYPE_MAX_SIZE];    /**< application type */
    xtimer_ticks32_t on_time;                                       /**< timestamp of last ON */
    lwm2m_obj_light_control_cb_t cb;                                /**< application callback */
    void *cb_arg;                                                   /**< callback argument */
} lwm2m_obj_light_control_inst_t;

/**
 * @brief 'Read' callback for the LwM2M Light Control object implementation.
 *
 * @param[in] instance_id       ID of the instance to read resource from.
 * @param[in] num_data          Number of elements in @p data_array.
 * @param[in, out] data_array   IDs of resources to read. Array of data structures to place values.
 * @param[in] object            Light Control object handle
 *
 * @return COAP_205_CONTENT on success
 * @return COAP_404_NOT_FOUND if the instance was not found
 * @return COAP_500_INTERNAL_SERVER_ERROR otherwise
 */
static uint8_t _read_cb(uint16_t instance_id, int *num_data, lwm2m_data_t **data_array,
                        lwm2m_object_t *object);

/**
 * @brief 'Write' callback for the LwM2M Light Control object implementation.
 *
 * @param[in] instance_id       ID of the instance to write resource to.
 * @param[in] num_data          Number of elements in @p data_array.
 * @param[in] data_array        IDs of resources to write and values.
 * @param[in] object            Light Control object handle
 *
 * @return COAP_204_CHANGED on success
 * @return COAP_404_NOT_FOUND if the instance was not found
 * @return COAP_400_BAD_REQUEST if a value is not encoded correctly
 * @return COAP_500_INTERNAL_SERVER_ERROR otherwise
 */
static uint8_t _write_cb(uint16_t instance_id, int num_data, lwm2m_data_t * data_array,
                         lwm2m_object_t * object);

/**
 * @brief Gets the current value of a given @p instance.
 *
 * @param[in, out] data     Initialized data structure.
 * @param[in] instance      Pointer to the instance to get the value from.
 *
 * @return COAP_205_CONTENT on success
 * @return COAP_404_NOT_FOUND if the value is not found
 */
static uint8_t _get_value(lwm2m_data_t *data, lwm2m_obj_light_control_inst_t *instance);

/**
 * @brief Light Control object implementation descriptor.
 */
static lwm2m_object_t _light_control_object = {
    .next           = NULL,
    .objID          = LWM2M_LIGHT_CONTROL_OBJECT_ID,
    .instanceList   = NULL,
    .readFunc       = _read_cb,
    .writeFunc      = _write_cb,
    .executeFunc    = NULL,
    .createFunc     = NULL,
    .deleteFunc     = NULL,
    .discoverFunc   = NULL,
    .userData       = NULL
};

/**
 * @brief Pool of Light Control object instances.
 */
static lwm2m_obj_light_control_inst_t _instances[CONFIG_LWM2M_LIGHT_INSTANCES_MAX];

static uint8_t _get_value(lwm2m_data_t *data, lwm2m_obj_light_control_inst_t *instance)
{
    assert(data);
    assert(instance);

    switch (data->id) {
    case LWM2M_LIGHT_CONTROL_ON_OFF_ID:
        lwm2m_data_encode_bool(instance->status, data);
        break;

    case LWM2M_LIGHT_CONTROL_DIMMER_ID:
        lwm2m_data_encode_int(instance->dimmer, data);
        break;

    case LWM2M_LIGHT_CONTROL_ON_TIME_ID:
        if (instance->status) {
            int64_t time = (xtimer_now_usec() - xtimer_usec_from_ticks(instance->on_time)) /
                           US_PER_SEC;
            lwm2m_data_encode_int(time, data);
        }
        else {
            lwm2m_data_encode_int(0, data);
        }
        break;

    case LWM2M_LIGHT_CONTROL_COLOUR_ID:
        lwm2m_data_encode_string(instance->color, data);
        break;

    case LWM2M_LIGHT_CONTROL_APP_TYPE_ID:
        lwm2m_data_encode_string(instance->app_type, data);
        break;

    default:
        return COAP_404_NOT_FOUND;
    }
    return COAP_205_CONTENT;
}

static uint8_t _read_cb(uint16_t instance_id, int *num_data, lwm2m_data_t **data_array,
                        lwm2m_object_t *object)
{
    lwm2m_obj_light_control_inst_t *instance;
    uint8_t result;
    int i = 0;

    /* try to get the requested instance from the object list */
    instance = (lwm2m_obj_light_control_inst_t *)lwm2m_list_find(object->instanceList, instance_id);
    if (!instance) {
        DEBUG("[lwm2m:light_control:read]: can't find instance %d\n", instance_id);
        result = COAP_404_NOT_FOUND;
        goto out;
    }

    /* if the number of resources is not specified, we need to read all resources */
    if (!*num_data) {
        DEBUG("[lwm2m:light_control:read]: reading all resources\n");

        uint16_t res_list[] = {
            LWM2M_LIGHT_CONTROL_ON_OFF_ID,
            LWM2M_LIGHT_CONTROL_DIMMER_ID,
            LWM2M_LIGHT_CONTROL_ON_TIME_ID,
            LWM2M_LIGHT_CONTROL_COLOUR_ID,
            LWM2M_LIGHT_CONTROL_APP_TYPE_ID
        };

        /* allocate structures to return resources */
        int res_num = ARRAY_SIZE(res_list);
        *data_array = lwm2m_data_new(res_num);

        if (NULL == *data_array) {
            result = COAP_500_INTERNAL_SERVER_ERROR;
            goto out;
        }

        /* return the number of resources being read */
        *num_data = res_num;

        /* set the IDs of the resources in the data structures */
        for (i = 0; i < res_num; i++) {
            (*data_array)[i].id = res_list[i];
        }
    }

    /* now get the values */
    i = 0;
    do {
        DEBUG("[lwm2m:light_control:read]: reading resource %d\n", (*data_array)[i].id);
        result = _get_value(&(*data_array)[i], instance);
        i++;
    } while (i < *num_data && COAP_205_CONTENT == result);

out:
    return result;
}

static uint8_t _write_cb(uint16_t instance_id, int num_data, lwm2m_data_t * data_array,
                         lwm2m_object_t * object)
{
    lwm2m_obj_light_control_inst_t *instance;
    uint8_t result = COAP_204_CHANGED;

    /* try to get the requested instance from the object list */
    instance = (lwm2m_obj_light_control_inst_t *)lwm2m_list_find(object->instanceList, instance_id);
    if (!instance) {
        DEBUG("[lwm2m:light_control:write]: can't find instance %d\n", instance_id);
        return COAP_404_NOT_FOUND;
    }

    for (int i = 0; i < num_data && result == COAP_204_CHANGED; i++) {
        switch (data_array[i].id) {
        case LWM2M_LIGHT_CONTROL_ON_OFF_ID:
            lwm2m_data_decode_bool(&data_array[i], &instance->status);

            if (instance->status) {
                instance->on_time = xtimer_now();
            }

            if (instance->cb) {
                instance->cb(object, instance_id, instance->status, instance->dimmer,
                             instance->cb_arg);
            }
            break;

        case LWM2M_LIGHT_CONTROL_DIMMER_ID:
        {
            int64_t val;
            lwm2m_data_decode_int(&data_array[i], &val);
            if (val < 0 || val > 100) {
                DEBUG("[lwm2m:light_control:write]: invalid dimmer value\n");
                result = COAP_400_BAD_REQUEST;
            } else {
                instance->dimmer = (uint8_t)val;

                if (instance->cb) {
                    instance->cb(object, instance_id, instance->status, instance->dimmer,
                                 instance->cb_arg);
                }
            }
            break;
        }

        case LWM2M_LIGHT_CONTROL_COLOUR_ID:
            if (data_array[i].type != LWM2M_TYPE_STRING) {
                result = COAP_400_BAD_REQUEST;
                break;
            }

            if (data_array[i].value.asBuffer.length >
                CONFIG_LWM2M_LIGHT_CONTROL_COLOR_MAX_SIZE - 1) {
                result = COAP_500_INTERNAL_SERVER_ERROR;
                break;
            }

            memcpy(instance->color, data_array[i].value.asBuffer.buffer,
                   data_array[i].value.asBuffer.length);
            instance->color[data_array[i].value.asBuffer.length] = '\0';
            break;

        case LWM2M_LIGHT_CONTROL_APP_TYPE_ID:
            if (data_array[i].type != LWM2M_TYPE_STRING) {
                result = COAP_400_BAD_REQUEST;
                break;
            }

            if (data_array[i].value.asBuffer.length >
                CONFIG_LWM2M_LIGHT_CONTROL_APP_TYPE_MAX_SIZE - 1) {
                result = COAP_500_INTERNAL_SERVER_ERROR;
                break;
            }

            memcpy(instance->app_type, data_array[i].value.asBuffer.buffer,
                   data_array[i].value.asBuffer.length);
            instance->app_type[data_array[i].value.asBuffer.length] = '\0';
            break;
        }
    }

    return result;
}

lwm2m_object_t *lwm2m_object_light_control_get(void)
{
    return &_light_control_object;
}

int lwm2m_object_light_control_instance_create(lwm2m_object_t *object, uint16_t instance_id,
                                               const lwm2m_obj_light_control_args_t *args)
{
    assert(object);
    assert(args);

    lwm2m_obj_light_control_inst_t *instance = NULL;

    if (object->objID != LWM2M_LIGHT_CONTROL_OBJECT_ID) {
        return -1;
    }

    /* try to allocate an instance */
    for (unsigned i = 0; i < CONFIG_LWM2M_LIGHT_INSTANCES_MAX; i++) {
        if (_instances[i].list.id != LWM2M_LIGHT_CONTROL_OBJECT_ID) {
            instance = &_instances[i];
        }
    }

    if (!instance) {
        DEBUG("[lwm2m:light_control]: can't allocate new instance\n");
        goto out;
    }

    memset(instance, 0, sizeof(lwm2m_obj_light_control_inst_t));

    instance->list.id = instance_id;
    instance->status = false;

    /* if callback specified set it and the optional argument */
    if (args->cb) {
        instance->cb = args->cb;
        if (args->cb_arg) {
            instance->cb_arg = args->cb_arg;
        }
    }

    /* if color is specified, copy locally */
    if (args->color) {
        if (args->color_len > CONFIG_LWM2M_LIGHT_CONTROL_COLOR_MAX_SIZE) {
            DEBUG("[lwm2m:light_control]: not enough space for color string\n");
            goto free_out;
        }
        memcpy(instance->color, args->color, args->color_len);
    }

    /* if color is specified, copy locally */
    if (args->app_type) {
        if (args->app_type_len > CONFIG_LWM2M_LIGHT_CONTROL_APP_TYPE_MAX_SIZE) {
            DEBUG("[lwm2m:light_control]: not enough space for app_type string\n");
            goto free_out;
        }
        memcpy(instance->app_type, args->app_type, args->app_type_len);
    }

    DEBUG("[lwm2m:light_control]: new instance with ID %d\n", instance_id);

    /* add the new instance to the list */
    object->instanceList = LWM2M_LIST_ADD(object->instanceList, instance);
    goto out;

free_out:
    instance->list.id = 0;
    instance = NULL;
out:
    return instance == NULL ? -1 : 0;
}
