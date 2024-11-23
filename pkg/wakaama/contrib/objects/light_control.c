/*
 * Copyright (C) 2024 HAW Hamburg
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

#include "mutex.h"
#include "inttypes.h"
#include "liblwm2m.h"
#include "lwm2m_client.h"
#include "objects/light_control.h"
#include "ztimer.h"
#include "ztimer/stopwatch.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define _USED_INSTANCES(_obj) (_obj.wakaama_object.instanceList)
#define _FREE_INSTANCES(_obj) (_obj.free_instances)

/**
 * @brief LwM2M Light Control object instance
 */
typedef struct lwm2m_obj_light_control_inst {
    lwm2m_list_t list;                                              /**< list handle */
    bool status;                                                    /**< light status */
    uint8_t dimmer;                                                 /**< dimmer value */
    ztimer_stopwatch_t stopwatch;                                   /**< stopwatch for on_time */
    lwm2m_obj_light_control_cb_t cb;                                /**< application callback */
    void *cb_arg;                                                   /**< callback argument */
    char color[CONFIG_LWM2M_LIGHT_CONTROL_COLOR_MAX_SIZE];          /**< light color */
    char app_type[CONFIG_LWM2M_LIGHT_CONTROL_APP_TYPE_MAX_SIZE];    /**< application type */
} lwm2m_obj_light_control_inst_t;

/**
 * @brief 'Read' callback for the LwM2M Light Control object implementation.
 *
 * @param[in] context           LWM2M Context
 * @param[in] instance_id       ID of the instance to read resource from.
 * @param[in] num_data          Number of elements in @p data_array.
 * @param[in, out] data_array   IDs of resources to read. Array of data structures to place values.
 * @param[in] object            Light Control object handle
 *
 * @return COAP_205_CONTENT on success
 * @return COAP_404_NOT_FOUND if the instance was not found
 * @return COAP_500_INTERNAL_SERVER_ERROR otherwise
 */
static uint8_t _read_cb(lwm2m_context_t * context, uint16_t instance_id, int * num_data,
                        lwm2m_data_t ** data_array, lwm2m_object_t * object);

/**
 * @brief 'Write' callback for the LwM2M Light Control object implementation.
 *
 * @param[in] context           LWM2M Context
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
static uint8_t _write_cb(lwm2m_context_t * context, uint16_t instance_id, int num_data,
                         lwm2m_data_t * data_array, lwm2m_object_t * object, lwm2m_write_type_t write_type);

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
 * @brief Mark a resource as changed on the LwM2M engine.
 *
 * @param[in] instance_id    ID of the instance to mark the resource as changed.
 * @param[in] resource_id    ID of the resource to mark as changed.
 */
static void _mark_resource_changed(uint16_t instance_id, uint16_t resource_id);

struct lwm2m_light_control_object {
    lwm2m_object_t wakaama_object;                   /**< Wakaama internal object */
    mutex_t lock;                                    /**< mutex for the instances access */
    lwm2m_obj_light_control_inst_t *free_instances;  /**< list of free instances */
    lwm2m_obj_light_control_inst_t instances[CONFIG_LWM2M_LIGHT_INSTANCES_MAX]; /**< instances */
};

struct lwm2m_light_control_object _light_control_object = {
    .lock = MUTEX_INIT,
    .wakaama_object = {
        .objID = LWM2M_LIGHT_CONTROL_OBJECT_ID,
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
            int64_t time = (int64_t) ztimer_stopwatch_measure(&instance->stopwatch);
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

static uint8_t _read_cb(lwm2m_context_t * context, uint16_t instance_id, int * num_data,
                        lwm2m_data_t ** data_array, lwm2m_object_t * object)
{
    (void)context;

    lwm2m_obj_light_control_inst_t *instance;
    uint8_t result = COAP_404_NOT_FOUND;
    int i = 0;

    mutex_lock(&_light_control_object.lock);

    /* try to get the requested instance from the object list */
    instance = (lwm2m_obj_light_control_inst_t *)lwm2m_list_find(object->instanceList, instance_id);
    if (!instance) {
        DEBUG("[lwm2m:light_control:read]: can't find instance %" PRId16 "\n", instance_id);
        result = COAP_404_NOT_FOUND;
        goto free_out;
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
            goto free_out;
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
        DEBUG("[lwm2m:light_control:read]: reading resource %" PRId16 "\n", (*data_array)[i].id);
        result = _get_value(&(*data_array)[i], instance);
        i++;
    } while (i < *num_data && COAP_205_CONTENT == result);

free_out:
    mutex_unlock(&_light_control_object.lock);
    return result;
}

static uint8_t _write_cb(lwm2m_context_t * context, uint16_t instance_id, int num_data,
                         lwm2m_data_t * data_array, lwm2m_object_t * object, lwm2m_write_type_t write_type)
{
    (void)context;
    (void)write_type;

    lwm2m_obj_light_control_inst_t *instance;
    uint8_t result = COAP_204_CHANGED;
    bool call_cb = false;

    mutex_lock(&_light_control_object.lock);

    /* try to get the requested instance from the object list */
    instance = (lwm2m_obj_light_control_inst_t *)lwm2m_list_find(object->instanceList, instance_id);
    if (!instance) {
        DEBUG("[lwm2m:light_control:write]: can't find instance %" PRId16 "\n", instance_id);
        result = COAP_404_NOT_FOUND;
        goto free_out;
    }

    for (int i = 0; i < num_data && result == COAP_204_CHANGED; i++) {
        switch (data_array[i].id) {
        case LWM2M_LIGHT_CONTROL_ON_OFF_ID: {
            bool prev_status = instance->status;

            lwm2m_data_decode_bool(&data_array[i], &instance->status);

            /* reset timer on OFF -> ON transitions */
            if (instance->status && !prev_status) {
                ztimer_stopwatch_reset(&instance->stopwatch);
            }

            call_cb = true;
            _mark_resource_changed(instance_id, LWM2M_LIGHT_CONTROL_ON_OFF_ID);
            break;
        }

        case LWM2M_LIGHT_CONTROL_ON_TIME_ID:
        {
            int64_t val;
            lwm2m_data_decode_int(&data_array[i], &val);
            if (val != 0) {
                DEBUG("[lwm2m:light_control:write]: invalid on_time value, only can write 0\n");
                result = COAP_400_BAD_REQUEST;
            } else {
                ztimer_stopwatch_reset(&instance->stopwatch);
                _mark_resource_changed(instance_id, LWM2M_LIGHT_CONTROL_ON_TIME_ID);
            }
            break;
        }

        case LWM2M_LIGHT_CONTROL_DIMMER_ID:
        {
            int64_t val;
            lwm2m_data_decode_int(&data_array[i], &val);
            if (val < 0 || val > 100) {
                DEBUG("[lwm2m:light_control:write]: invalid dimmer value\n");
                result = COAP_400_BAD_REQUEST;
            } else {
                instance->dimmer = (uint8_t)val;
                call_cb = true;
                _mark_resource_changed(instance_id, LWM2M_LIGHT_CONTROL_DIMMER_ID);
            }
            break;
        }

        case LWM2M_LIGHT_CONTROL_COLOUR_ID:
            if (data_array[i].type != LWM2M_TYPE_STRING && data_array[i].type != LWM2M_TYPE_OPAQUE) {
                DEBUG("[lwm2m:light_control:write]: invalid type for color"
                      "(%" PRId8 ")\n", (uint8_t)(data_array[i].type));
                result = COAP_400_BAD_REQUEST;
                break;
            }

            if (data_array[i].value.asBuffer.length >
                CONFIG_LWM2M_LIGHT_CONTROL_COLOR_MAX_SIZE - 1) {
                DEBUG("[lwm2m:light_control:write]: value too big for color\n");
                result = COAP_500_INTERNAL_SERVER_ERROR;
                break;
            }

            memcpy(instance->color, data_array[i].value.asBuffer.buffer,
                   data_array[i].value.asBuffer.length);
            instance->color[data_array[i].value.asBuffer.length] = '\0';
            call_cb = true;
            _mark_resource_changed(instance_id, LWM2M_LIGHT_CONTROL_COLOUR_ID);
            break;

        case LWM2M_LIGHT_CONTROL_APP_TYPE_ID:
            if (data_array[i].type != LWM2M_TYPE_STRING && data_array[i].type != LWM2M_TYPE_OPAQUE) {
                DEBUG("[lwm2m:light_control:write]: invalid type for app_type"
                      "(%" PRId8 ")\n", (uint8_t)(data_array[i].type));
                result = COAP_400_BAD_REQUEST;
                break;
            }

            if (data_array[i].value.asBuffer.length >
                CONFIG_LWM2M_LIGHT_CONTROL_APP_TYPE_MAX_SIZE - 1) {
                DEBUG("[lwm2m:light_control:write]: value too big for app_type\n");
                result = COAP_500_INTERNAL_SERVER_ERROR;
                break;
            }

            memcpy(instance->app_type, data_array[i].value.asBuffer.buffer,
                   data_array[i].value.asBuffer.length);
            instance->app_type[data_array[i].value.asBuffer.length] = '\0';
            call_cb = true;
            _mark_resource_changed(instance_id, LWM2M_LIGHT_CONTROL_APP_TYPE_ID);
            break;
        }
    }

    if (call_cb && instance->cb) {
        instance->cb(object, instance_id, instance->status, instance->dimmer, instance->color,
                     instance->app_type, instance->cb_arg);
    }

free_out:
    mutex_unlock(&_light_control_object.lock);
    return result;
}

static void _mark_resource_changed(uint16_t instance_id, uint16_t resource_id)
{
    lwm2m_uri_t uri;
    LWM2M_URI_RESET(&uri);
    uri.objectId = LWM2M_LIGHT_CONTROL_OBJECT_ID;
    uri.instanceId = instance_id;
    uri.resourceId = resource_id;

    lwm2m_client_data_t *client_data;
    client_data = (lwm2m_client_data_t *)_light_control_object.wakaama_object.userData;
    lwm2m_resource_value_changed(client_data->lwm2m_ctx, &uri);
}


lwm2m_object_t *lwm2m_object_light_control_init(lwm2m_client_data_t *client_data)
{
    /* initialize the instances */
    for (unsigned i = 0; i < CONFIG_LWM2M_LIGHT_INSTANCES_MAX; i++) {
        _light_control_object.instances[i].list.next = NULL;
        _light_control_object.instances[i].list.id = UINT16_MAX;

        _FREE_INSTANCES(_light_control_object) = (lwm2m_obj_light_control_inst_t *) LWM2M_LIST_ADD(
            _FREE_INSTANCES(_light_control_object),
            &(_light_control_object.instances[i])
        );
    }

    _light_control_object.wakaama_object.userData = client_data;

    return &(_light_control_object.wakaama_object);
}

int lwm2m_object_light_control_instance_create(const lwm2m_obj_light_control_args_t *args,
                                               int32_t instance_id)
{
    assert(args);
    int result = -ENOMEM;
    lwm2m_obj_light_control_inst_t *instance = NULL;
    uint16_t _instance_id;

    /* lock object */
    mutex_lock(&_light_control_object.lock);

    /* determine ID for new instance */
    if (instance_id < 0) {
        _instance_id = lwm2m_list_newId((lwm2m_list_t *)_USED_INSTANCES(_light_control_object));
    }
    else {
        /* sanity check */
        if (instance_id >= (UINT16_MAX - 1)) {
            DEBUG("[lwm2m:light_control]: instance ID %" PRIi32 " is too big\n", instance_id);
            result = -EINVAL;
            goto free_out;
        }

        _instance_id = (uint16_t)instance_id;

        /* check that the ID is free to use */
        if (LWM2M_LIST_FIND(_USED_INSTANCES(_light_control_object), _instance_id ) != NULL)
        {
            DEBUG("[lwm2m:light_control]: instance ID %" PRIi32 " already in use\n", instance_id);
            goto free_out;
        }
    }

    /* try to allocate an instance, by popping a free node from the list */
    _FREE_INSTANCES(_light_control_object) = (lwm2m_obj_light_control_inst_t *) lwm2m_list_remove(
        (lwm2m_list_t *) _FREE_INSTANCES(_light_control_object),
        UINT16_MAX,
        (lwm2m_list_t **) &instance
    );

    if (!instance) {
        DEBUG("[lwm2m:light_control]: can't allocate new instance\n");
        goto free_out;
    }

    memset(instance, 0, sizeof(lwm2m_obj_light_control_inst_t));

    instance->list.id = _instance_id;
    instance->status = false;

    ztimer_stopwatch_init(ZTIMER_SEC, &instance->stopwatch);
    ztimer_stopwatch_start(&instance->stopwatch);

    /* if callback specified set it */
    if (args->cb) {
        instance->cb = args->cb;
    }

    /* if callback argument specified set it */
    if (args->cb_arg) {
        instance->cb_arg = args->cb_arg;
    }

    /* if color is specified, copy locally */
    if (args->color) {
        if (args->color_len > CONFIG_LWM2M_LIGHT_CONTROL_COLOR_MAX_SIZE) {
            DEBUG("[lwm2m:light_control]: not enough space for color string\n");
            goto free_out;
        }
        memcpy(instance->color, args->color, args->color_len);
    }

    /* if app is specified, copy locally */
    if (args->app_type) {
        if (args->app_type_len > CONFIG_LWM2M_LIGHT_CONTROL_APP_TYPE_MAX_SIZE) {
            DEBUG("[lwm2m:light_control]: not enough space for app_type string\n");
            goto free_out;
        }
        memcpy(instance->app_type, args->app_type, args->app_type_len);
    }

    DEBUG("[lwm2m:light_control]: new instance with ID %" PRIu16 "\n", _instance_id);

    /* add the new instance to the list */
    _USED_INSTANCES(_light_control_object) = LWM2M_LIST_ADD(
        _USED_INSTANCES(_light_control_object),
        instance
    );

    result = 0;

free_out:
    mutex_unlock(&_light_control_object.lock);
    return result;
}

int lwm2m_object_light_control_update_dimmer(uint16_t instance_id, uint8_t dimmer, bool call_cb)
{
    int result = -EINVAL;
    lwm2m_obj_light_control_inst_t *instance;

    mutex_lock(&_light_control_object.lock);

    if (dimmer > 100) {
        DEBUG("[lwm2m:light_control]: invalid dimmer value\n");
        goto free_out;
    }

    instance = (lwm2m_obj_light_control_inst_t *)LWM2M_LIST_FIND(
        _USED_INSTANCES(_light_control_object),
        instance_id
    );

    if (!instance) {
        DEBUG("[lwm2m:light_control]: can't find instance %" PRId16 "\n", instance_id);
        goto free_out;
    }

    instance->dimmer = dimmer;

    if (call_cb && instance->cb) {
        instance->cb(&_light_control_object.wakaama_object, instance_id, instance->status,
                     instance->dimmer, instance->color, instance->app_type, instance->cb_arg);
    }

    _mark_resource_changed(instance_id, LWM2M_LIGHT_CONTROL_DIMMER_ID);

    result = 0;

free_out:
    mutex_unlock(&_light_control_object.lock);
    return result;
}

int lwm2m_object_light_control_update_status(uint16_t instance_id, bool status, bool call_cb)
{
    int result = -EINVAL;
    lwm2m_obj_light_control_inst_t *instance;

    mutex_lock(&_light_control_object.lock);

    instance = (lwm2m_obj_light_control_inst_t *)LWM2M_LIST_FIND(
        _USED_INSTANCES(_light_control_object),
        instance_id
    );

    if (!instance) {
        DEBUG("[lwm2m:light_control]: can't find instance %" PRId16 "\n", instance_id);
        goto free_out;
    }

    if (status && !instance->status) {
        ztimer_stopwatch_start(&instance->stopwatch);
    }

    instance->status = status;

    if (call_cb && instance->cb) {
        instance->cb(&_light_control_object.wakaama_object, instance_id, instance->status,
                     instance->dimmer, instance->color, instance->app_type, instance->cb_arg);
    }

    _mark_resource_changed(instance_id, LWM2M_LIGHT_CONTROL_ON_OFF_ID);

    result = 0;

free_out:
    mutex_unlock(&_light_control_object.lock);
    return result;
}

int lwm2m_object_light_control_update_color(uint16_t instance_id, const char *color, size_t len,
                                            bool call_cb)
{
    int result = -EINVAL;
    lwm2m_obj_light_control_inst_t *instance;

    mutex_lock(&_light_control_object.lock);

    if (len > CONFIG_LWM2M_LIGHT_CONTROL_COLOR_MAX_SIZE - 1) {
        DEBUG("[lwm2m:light_control]: color string too long\n");
        result = -ENOBUFS;
        goto free_out;
    }

    instance = (lwm2m_obj_light_control_inst_t *)LWM2M_LIST_FIND(
        _USED_INSTANCES(_light_control_object),
        instance_id
    );

    if (!instance) {
        DEBUG("[lwm2m:light_control]: can't find instance %" PRId16 "\n", instance_id);
        goto free_out;
    }

    memcpy(instance->color, color, len);
    instance->color[len] = '\0';

    if (call_cb && instance->cb) {
        instance->cb(&_light_control_object.wakaama_object, instance_id, instance->status,
                     instance->dimmer, instance->color, instance->app_type, instance->cb_arg);
    }

    _mark_resource_changed(instance_id, LWM2M_LIGHT_CONTROL_COLOUR_ID);

    result = 0;

free_out:
    mutex_unlock(&_light_control_object.lock);
    return result;
}

int lwm2m_object_light_control_update_app_type(uint16_t instance_id, const char *app_type,
                                               size_t len, bool call_cb)
{
    int result = -EINVAL;
    lwm2m_obj_light_control_inst_t *instance;

    mutex_lock(&_light_control_object.lock);

    if (len > CONFIG_LWM2M_LIGHT_CONTROL_APP_TYPE_MAX_SIZE - 1) {
        DEBUG("[lwm2m:light_control]: app_type string too long\n");
        result = -ENOBUFS;
        goto free_out;
    }

    instance = (lwm2m_obj_light_control_inst_t *)LWM2M_LIST_FIND(
        _USED_INSTANCES(_light_control_object),
        instance_id
    );

    if (!instance) {
        DEBUG("[lwm2m:light_control]: can't find instance %" PRId16 "\n", instance_id);
        goto free_out;
    }

    memcpy(instance->app_type, app_type, len);
    instance->app_type[len] = '\0';

    if (call_cb && instance->cb) {
        instance->cb(&_light_control_object.wakaama_object, instance_id, instance->status,
                     instance->dimmer, instance->color, instance->app_type, instance->cb_arg);
    }

    _mark_resource_changed(instance_id, LWM2M_LIGHT_CONTROL_APP_TYPE_ID);

    result = 0;

free_out:
    mutex_unlock(&_light_control_object.lock);
    return result;
}
