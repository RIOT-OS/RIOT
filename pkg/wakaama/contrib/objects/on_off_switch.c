/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @{
 * @ingroup     lwm2m_objects_on_off_switch
 *
 * @file
 * @brief       On/off switch object implementation for LwM2M client using Wakaama
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "mutex.h"
#include "inttypes.h"
#include "liblwm2m.h"
#include "lwm2m_client.h"
#include "objects/on_off_switch.h"
#include "ztimer.h"
#include "ztimer/stopwatch.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define _USED_INSTANCES(_obj) (_obj.wakaama_object.instanceList)
#define _FREE_INSTANCES(_obj) (_obj.free_instances)

/**
 * @brief LwM2M On/off switch object instance
 */
typedef struct lwm2m_obj_on_off_switch_inst {
    lwm2m_list_t list;                                              /**< list handle */
    bool status;                                                    /**< whether digital input is ON or OFF */
    uint32_t counter;                                               /**< counter for ON-transitions of the digital input*/
    ztimer_stopwatch_t stopwatch;                                   /**< stopwatch to count ON/OFF time periods */
    char app_type[CONFIG_LWM2M_ON_OFF_SWITCH_APP_TYPE_MAX_SIZE];    /**< application type */
} lwm2m_obj_on_off_switch_inst_t;

/**
 * @brief 'Read' callback for the LwM2M On/off switch object implementation.
 *
 * @param[in] context           LWM2M Context
 * @param[in] instance_id       ID of the instance to read resource from.
 * @param[in] num_data          Number of elements in @p data_array.
 * @param[in, out] data_array   IDs of resources to read. Array of data structures to place values.
 * @param[in] object            On/off switch object handle
 *
 * @return COAP_205_CONTENT on success
 * @return COAP_404_NOT_FOUND if the instance was not found
 * @return COAP_500_INTERNAL_SERVER_ERROR otherwise
 */
static uint8_t _read_cb(lwm2m_context_t *context, uint16_t instance_id, int *num_data,
                        lwm2m_data_t **data_array, lwm2m_object_t *object);

/**
 * @brief 'Write' callback for the LwM2M On/off switch object implementation.
 *
 * @param[in] context           LWM2M Context
 * @param[in] instance_id       ID of the instance to write resource to.
 * @param[in] num_data          Number of elements in @p data_array.
 * @param[in] data_array        IDs of resources to write and values.
 * @param[in] object            On/off switch object handle
 *
 * @return COAP_204_CHANGED on success
 * @return COAP_404_NOT_FOUND if the instance was not found
 * @return COAP_400_BAD_REQUEST if a value is not encoded correctly
 * @return COAP_500_INTERNAL_SERVER_ERROR otherwise
 */
static uint8_t _write_cb(lwm2m_context_t *context, uint16_t instance_id, int num_data,
                         lwm2m_data_t *data_array, lwm2m_object_t *object,
                         lwm2m_write_type_t write_type);

/**
 * @brief Gets the current value of a resource from a given On/off switch object @p instance.
 *
 * @param[in, out] data     Initialized resource data structure.
 * @param[in] instance      Pointer to the instance to get the value from.
 *
 * @return COAP_205_CONTENT on success
 * @return COAP_404_NOT_FOUND if the value is not found
 */
static uint8_t _get_value(lwm2m_data_t *data, lwm2m_obj_on_off_switch_inst_t *instance);

/**
 * @brief Sets the value of a resource in a given On/off switch object @p instance.
 *
 * @param[in] data          Data structure containing the new value.
 * @param[in] instance      Pointer to the instance to set the value.
 *
 * @return COAP_204_CHANGED on success
 * @return COAP_400_BAD_REQUEST if the value is not encoded correctly
 * @return COAP_404_NOT_FOUND if the value is not found
 */
static uint8_t _set_value(lwm2m_data_t *data, lwm2m_obj_on_off_switch_inst_t *instance);

/**
 * @brief Mark a resource as changed on the LwM2M engine.
 *
 * @param[in] instance_id    ID of the instance to mark the resource as changed.
 * @param[in] resource_id    ID of the resource to mark as changed.
 */
static void _mark_resource_changed(uint16_t instance_id, uint16_t resource_id);

struct lwm2m_on_off_switch_object {
    lwm2m_object_t wakaama_object;                                                      /**< Wakaama internal object */
    mutex_t lock;                                                                       /**< mutex for the instances access */
    lwm2m_obj_on_off_switch_inst_t *free_instances;                                     /**< list of free instances */
    lwm2m_obj_on_off_switch_inst_t instances[CONFIG_LWM2M_ON_OFF_SWITCH_INSTANCES_MAX]; /**< instances */
};

struct lwm2m_on_off_switch_object _on_off_switch_object = {
    .lock = MUTEX_INIT,
    .wakaama_object = {
        .objID = LWM2M_ON_OFF_SWITCH_OBJECT_ID,
        .instanceList = NULL,
        .readFunc = _read_cb,
        .writeFunc = _write_cb,
        .executeFunc = NULL,
        .createFunc = NULL,
        .deleteFunc = NULL,
        .discoverFunc = NULL,
        .userData = NULL
    }
};

static uint8_t _get_value(lwm2m_data_t *data, lwm2m_obj_on_off_switch_inst_t *instance)
{
    assert(data);
    assert(instance);

    switch (data->id) {
    case LWM2M_ON_OFF_SWITCH_DIGITAL_INPUT_STATE_ID:
        lwm2m_data_encode_bool(instance->status, data);
        break;

    case LWM2M_ON_OFF_SWITCH_DIGITAL_INPUT_COUNTER_ID:
        lwm2m_data_encode_int(instance->counter, data);
        break;

    case LWM2M_ON_OFF_SWITCH_ON_TIME_ID:
        if (instance->status) {
            int64_t time = (int64_t)ztimer_stopwatch_measure(&instance->stopwatch);
            lwm2m_data_encode_int(time, data);
        }
        else {
            lwm2m_data_encode_int(0, data);
        }
        break;

    case LWM2M_ON_OFF_SWITCH_OFF_TIME_ID:
        if (!instance->status) {
            int64_t time = (int64_t)ztimer_stopwatch_measure(&instance->stopwatch);
            lwm2m_data_encode_int(time, data);
        }
        else {
            lwm2m_data_encode_int(0, data);
        }
        break;

    case LWM2M_ON_OFF_SWITCH_APP_TYPE_ID:
        lwm2m_data_encode_string(instance->app_type, data);
        break;

    default:
        return COAP_404_NOT_FOUND;
    }
    return COAP_205_CONTENT;
}

static uint8_t _read_cb(lwm2m_context_t *context, uint16_t instance_id, int *num_data,
                        lwm2m_data_t **p_data_array, lwm2m_object_t *object)
{
    (void)context;

    lwm2m_obj_on_off_switch_inst_t *instance;
    lwm2m_data_t *data_array = *p_data_array;
    uint8_t result = COAP_404_NOT_FOUND;

    mutex_lock(&_on_off_switch_object.lock);

    /* try to get the requested instance from the object list */
    instance = (lwm2m_obj_on_off_switch_inst_t *)lwm2m_list_find(object->instanceList, instance_id);
    if (!instance) {
        DEBUG("[lwm2m:on_off_switch:read]: can't find instance %" PRId16 "\n", instance_id);
        result = COAP_404_NOT_FOUND;
        goto free_out;
    }

    /* if the number of resources is not specified, we need to read all resources */
    if (!*num_data) {
        DEBUG("[lwm2m:on_off_switch:read]: reading all resources\n");

        uint16_t res_list[] = {
            LWM2M_ON_OFF_SWITCH_DIGITAL_INPUT_STATE_ID,
            LWM2M_ON_OFF_SWITCH_DIGITAL_INPUT_COUNTER_ID,
            LWM2M_ON_OFF_SWITCH_ON_TIME_ID,
            LWM2M_ON_OFF_SWITCH_OFF_TIME_ID,
            LWM2M_ON_OFF_SWITCH_APP_TYPE_ID
        };

        /* allocate structures to return resources */
        int res_num = ARRAY_SIZE(res_list);
        data_array = lwm2m_data_new(res_num);

        /* make sure to update the returned pointer */
        *p_data_array = data_array;

        if (NULL == data_array) {
            result = COAP_500_INTERNAL_SERVER_ERROR;
            goto free_out;
        }

        /* return the number of resources being read */
        *num_data = res_num;

        /* set the IDs of the resources in the data structures */
        for (int i = 0; i < res_num; i++) {
            data_array[i].id = res_list[i];
        }
    }

    /* now get the values */
    for (int i = 0; i < *num_data; i++) {
        result = _get_value(&data_array[i], instance);

        if (COAP_205_CONTENT != result) {
            break;
        }
    }

free_out:
    mutex_unlock(&_on_off_switch_object.lock);
    return result;
}

static uint8_t _set_value(lwm2m_data_t *data, lwm2m_obj_on_off_switch_inst_t *instance)
{
    assert(data);
    assert(instance);

    switch (data->id) {
    case LWM2M_ON_OFF_SWITCH_DIGITAL_INPUT_STATE_ID: {
        bool previous_status = instance->status;

        int decode_result = lwm2m_data_decode_bool(data, &instance->status);
        if (!decode_result) {
            DEBUG("[lwm2m:on_off_switch:write]: invalid value for digital_input_state\n");
            return COAP_400_BAD_REQUEST;
        }

        /* reset timer on transitions */
        if (instance->status != previous_status) {
            ztimer_stopwatch_reset(&instance->stopwatch);

            if (instance->status) {
                instance->counter++;
            }
        }

        _mark_resource_changed(instance->list.id, LWM2M_ON_OFF_SWITCH_DIGITAL_INPUT_STATE_ID);
        return COAP_204_CHANGED;
    }

    case LWM2M_ON_OFF_SWITCH_ON_TIME_ID:
    {
        int64_t value;

        int decode_result = lwm2m_data_decode_int(data, &value);
        if (!decode_result) {
            DEBUG("[lwm2m:on_off_switch:write]: invalid on_time value\n");
            return COAP_400_BAD_REQUEST;
        }

        if (value != 0) {
            DEBUG("[lwm2m:on_off_switch:write]: invalid on_time value, only can write 0\n");
            return COAP_400_BAD_REQUEST;
        }

        ztimer_stopwatch_reset(&instance->stopwatch);
        _mark_resource_changed(instance->list.id, LWM2M_ON_OFF_SWITCH_ON_TIME_ID);
        return COAP_204_CHANGED;
    }

    case LWM2M_ON_OFF_SWITCH_OFF_TIME_ID:
    {
        int64_t value;
        int decode_result = lwm2m_data_decode_int(data, &value);
        if (!decode_result) {
            DEBUG("[lwm2m:on_off_switch:write]: invalid off_time value\n");
            return COAP_400_BAD_REQUEST;
        }

        if (value != 0) {
            DEBUG("[lwm2m:on_off_switch:write]: invalid off_time value, only can write 0\n");
            return COAP_400_BAD_REQUEST;
        }

        ztimer_stopwatch_reset(&instance->stopwatch);
        _mark_resource_changed(instance->list.id, LWM2M_ON_OFF_SWITCH_OFF_TIME_ID);
        return COAP_204_CHANGED;
    }

    case LWM2M_ON_OFF_SWITCH_APP_TYPE_ID:
        if (data->type != LWM2M_TYPE_STRING && data->type != LWM2M_TYPE_OPAQUE) {
            DEBUG("[lwm2m:on_off_switch:write]: invalid type for app_type"
                  "(%" PRId8 ")\n", (uint8_t)(data->type));
            return COAP_400_BAD_REQUEST;
        }

        if (data->value.asBuffer.length >
            CONFIG_LWM2M_ON_OFF_SWITCH_APP_TYPE_MAX_SIZE - 1) {
            DEBUG("[lwm2m:on_off_switch:write]: value too big for app_type\n");
            return COAP_500_INTERNAL_SERVER_ERROR;
        }

        memcpy(instance->app_type, data->value.asBuffer.buffer,
               data->value.asBuffer.length);
        instance->app_type[data->value.asBuffer.length] = '\0';
        _mark_resource_changed(instance->list.id, LWM2M_ON_OFF_SWITCH_APP_TYPE_ID);
        return COAP_204_CHANGED;

    default:
        return COAP_404_NOT_FOUND;
    }

}

static uint8_t _write_cb(lwm2m_context_t *context, uint16_t instance_id, int num_data,
                         lwm2m_data_t *data_array, lwm2m_object_t *object,
                         lwm2m_write_type_t write_type)
{
    (void)context;
    (void)write_type;

    lwm2m_obj_on_off_switch_inst_t *instance;
    uint8_t result = COAP_404_NOT_FOUND;

    mutex_lock(&_on_off_switch_object.lock);

    /* try to get the requested instance from the object list */
    instance = (lwm2m_obj_on_off_switch_inst_t *)lwm2m_list_find(object->instanceList, instance_id);
    if (!instance) {
        DEBUG("[lwm2m:on_off_switch:write]: can't find instance %" PRId16 "\n", instance_id);
        result = COAP_404_NOT_FOUND;
        goto free_out;
    }

    for (int i = 0; i < num_data; i++) {
        result = _set_value(&data_array[i], instance);

        if (result != COAP_204_CHANGED) {
            DEBUG("[lwm2m:on_off_switch:write]: error writing resource %" PRId16 "\n",
                  data_array[i].id);
            goto free_out;
        }
    }

free_out:
    mutex_unlock(&_on_off_switch_object.lock);
    return result;
}

static void _mark_resource_changed(uint16_t instance_id, uint16_t resource_id)
{
    lwm2m_uri_t uri;

    LWM2M_URI_RESET(&uri);
    uri.objectId = LWM2M_ON_OFF_SWITCH_OBJECT_ID;
    uri.instanceId = instance_id;
    uri.resourceId = resource_id;

    lwm2m_client_data_t *client_data;
    client_data = (lwm2m_client_data_t *)_on_off_switch_object.wakaama_object.userData;
    assert(client_data);
    lwm2m_resource_value_changed(client_data->lwm2m_ctx, &uri);
}

lwm2m_object_t *lwm2m_object_on_off_switch_init(lwm2m_client_data_t *client_data)
{
    assert(client_data);

    /* initialize the instances */
    for (unsigned i = 0; i < CONFIG_LWM2M_ON_OFF_SWITCH_INSTANCES_MAX; i++) {
        _on_off_switch_object.instances[i].list.next = NULL;
        _on_off_switch_object.instances[i].list.id = UINT16_MAX;

        _FREE_INSTANCES(_on_off_switch_object) = (lwm2m_obj_on_off_switch_inst_t *)LWM2M_LIST_ADD(
            _FREE_INSTANCES(_on_off_switch_object),
            &(_on_off_switch_object.instances[i])
            );
    }

    _on_off_switch_object.wakaama_object.userData = client_data;

    return &(_on_off_switch_object.wakaama_object);
}

int lwm2m_object_on_off_switch_instance_create(const lwm2m_obj_on_off_switch_args_t *args,
                                               int32_t instance_id)
{
    assert(args);
    int result = -ENOMEM;
    lwm2m_obj_on_off_switch_inst_t *instance = NULL;
    uint16_t _instance_id;

    /* lock object */
    mutex_lock(&_on_off_switch_object.lock);

    /* determine ID for new instance */
    if (instance_id < 0) {
        _instance_id = lwm2m_list_newId((lwm2m_list_t *)_USED_INSTANCES(_on_off_switch_object));
    }
    else {
        /* sanity check */
        if (instance_id >= (UINT16_MAX - 1)) {
            DEBUG("[lwm2m:on_off_switch]: instance ID %" PRIi32 " is too big\n", instance_id);
            result = -EINVAL;
            goto free_out;
        }

        _instance_id = (uint16_t)instance_id;

        /* check that the ID is free to use */
        if (LWM2M_LIST_FIND(_USED_INSTANCES(_on_off_switch_object), _instance_id ) != NULL) {
            DEBUG("[lwm2m:on_off_switch]: instance ID %" PRIi32 " already in use\n", instance_id);
            goto free_out;
        }
    }

    /* try to allocate an instance, by popping a free node from the list */
    _FREE_INSTANCES(_on_off_switch_object) = (lwm2m_obj_on_off_switch_inst_t *)lwm2m_list_remove(
        (lwm2m_list_t *)_FREE_INSTANCES(_on_off_switch_object),
        UINT16_MAX,
        (lwm2m_list_t **)&instance
        );

    if (!instance) {
        DEBUG("[lwm2m:on_off_switch]: can't allocate new instance\n");
        goto free_out;
    }

    memset(instance, 0, sizeof(lwm2m_obj_on_off_switch_inst_t));

    instance->list.id = _instance_id;
    instance->status = false;

    ztimer_stopwatch_init(ZTIMER_SEC, &instance->stopwatch);
    ztimer_stopwatch_start(&instance->stopwatch);

    /* if app is specified, copy locally */
    if (args->app_type) {
        if (args->app_type_len > CONFIG_LWM2M_ON_OFF_SWITCH_APP_TYPE_MAX_SIZE) {
            DEBUG("[lwm2m:on_off_switch]: not enough space for app_type string\n");
            goto free_out;
        }
        memcpy(instance->app_type, args->app_type, args->app_type_len);
    }

    DEBUG("[lwm2m:on_off_switch]: new instance with ID %" PRIu16 "\n", _instance_id);

    /* add the new instance to the list */
    _USED_INSTANCES(_on_off_switch_object) = LWM2M_LIST_ADD(
        _USED_INSTANCES(_on_off_switch_object),
        instance
        );

    result = 0;

free_out:
    mutex_unlock(&_on_off_switch_object.lock);
    return result;
}

int lwm2m_object_on_off_switch_update_status(uint16_t instance_id, bool status)
{
    int result = -EINVAL;
    lwm2m_obj_on_off_switch_inst_t *instance;

    mutex_lock(&_on_off_switch_object.lock);

    instance = (lwm2m_obj_on_off_switch_inst_t *)LWM2M_LIST_FIND(
        _USED_INSTANCES(_on_off_switch_object),
        instance_id
        );

    if (!instance) {
        DEBUG("[lwm2m:on_off_switch]: can't find instance %" PRId16 "\n", instance_id);
        goto free_out;
    }

    lwm2m_data_t data = {
        .type = LWM2M_TYPE_BOOLEAN,
        .id = LWM2M_ON_OFF_SWITCH_DIGITAL_INPUT_STATE_ID,
        .value.asBoolean = status
    };

    int set_result = _set_value(&data, instance);
    if (set_result != COAP_204_CHANGED) {
        DEBUG("[lwm2m:on_off_switch]: error updating status\n");
        goto free_out;
    }

    result = 0;

free_out:
    mutex_unlock(&_on_off_switch_object.lock);
    return result;
}

int lwm2m_object_on_off_switch_update_app_type(uint16_t instance_id, const char *app_type,
                                               size_t len)
{
    assert(app_type);

    int result = -EINVAL;
    lwm2m_obj_on_off_switch_inst_t *instance;

    mutex_lock(&_on_off_switch_object.lock);

    instance = (lwm2m_obj_on_off_switch_inst_t *)LWM2M_LIST_FIND(
        _USED_INSTANCES(_on_off_switch_object),
        instance_id
        );

    if (!instance) {
        DEBUG("[lwm2m:on_off_switch]: can't find instance %" PRId16 "\n", instance_id);
        goto free_out;
    }

    lwm2m_data_t data = {
        .type = LWM2M_TYPE_STRING,
        .id = LWM2M_ON_OFF_SWITCH_APP_TYPE_ID,
        .value.asBuffer.buffer = (uint8_t *)app_type,
        .value.asBuffer.length = len
    };

    int set_result = _set_value(&data, instance);
    if (set_result != COAP_204_CHANGED) {
        DEBUG("[lwm2m:on_off_switch]: error updating app_type\n");
        goto free_out;
    }

    result = 0;

free_out:
    mutex_unlock(&_on_off_switch_object.lock);
    return result;
}
