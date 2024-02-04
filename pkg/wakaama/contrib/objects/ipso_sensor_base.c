/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @{
 * @ingroup     lwm2m_objects_ipso_sensor_base
 *
 * @file
 * @brief       Implementation of IPSO sensor base functionalities
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "liblwm2m.h"
#include "lwm2m_client.h"
#include "objects/ipso_sensor_base.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define _USED_INSTANCES(obj) (obj->object.instanceList)
#define _FREE_INSTANCES(obj) (obj->free_instances)

/**
 * @brief 'Read' callback for the LwM2M Illuminance Sensor object implementation.
 *
 * @param[in] instance_id       ID of the instance to read resource from.
 * @param[in] num_data          Number of elements in @p data_array.
 * @param[in, out] data_array   IDs of resources to read. Array of data structures to place values.
 * @param[in] object            Illuminance Sensor object handle
 *
 * @return COAP_205_CONTENT on success
 * @return COAP_404_NOT_FOUND if the instance was not found
 * @return COAP_500_INTERNAL_SERVER_ERROR otherwise
 */
static uint8_t _read_cb(uint16_t instance_id, int *num_data, lwm2m_data_t **data_array,
                        lwm2m_object_t *object);

/**
 * @brief 'Execute' callback for the LwM2M Illuminance Sensor object implementation.
 *
 * @param[in] instance_id       ID of the instance to execute resource from.
 * @param[in] resource_id       ID of the resource to execute.
 * @param[in] buffer            Pointer to the buffer containing the payload.
 * @param[in] length            Length of the payload.
 * @param[in] object            Illuminance Sensor object handle
 *
 * @return COAP_204_CHANGED on success
 * @return COAP_404_NOT_FOUND if the instance was not found
 * @return COAP_405_METHOD_NOT_ALLOWED if the resource is not executable
 */
static uint8_t _exec_cb(uint16_t instance_id, uint16_t resource_id, uint8_t *buffer, int length,
                        lwm2m_object_t *object);

/**
 * @brief Gets the current value of a given @p instance.
 *
 * @param[in, out] data     Initialized data structure.
 * @param[in] instance      Pointer to the instance to get the value from.
 *
 * @return COAP_205_CONTENT on success
 * @return COAP_404_NOT_FOUND if the value is not found
 */
static uint8_t _get_value(lwm2m_data_t *data, lwm2m_obj_ipso_sensor_base_inst_t *instance);

/**
 * @brief Mark a resource as changed.
 *
 * @param[in] object         Object to mark the resource from.
 * @param[in] instance_id    ID of the instance to mark the resource from.
 * @param[in] resource_id    ID of the resource to mark.
 */
static void _mark_resource_as_changed(const lwm2m_object_t *object, uint16_t instance_id,
                                      uint16_t resource_id);

/**
 * @brief Add an instance to the free instance list.
 *
 * @param[out] object   IPSO sensor base object
 * @param[in] instance  Instance to add to the free list
 */
static void _put_instance_in_free_list(lwm2m_obj_ipso_sensor_base_t *object,
                                       lwm2m_obj_ipso_sensor_base_inst_t *instance);

/**
 * @brief Get a free instance from the list.
 *
 * @param[in] object        IPSO sensor base object
 *
 * @return Instance if available, NULL otherwise
 */
static lwm2m_obj_ipso_sensor_base_inst_t *_get_instance_from_free_list(
    lwm2m_obj_ipso_sensor_base_t *object);

static void _put_instance_in_free_list(lwm2m_obj_ipso_sensor_base_t *object,
                                       lwm2m_obj_ipso_sensor_base_inst_t *instance)
{
    assert(object);
    assert(instance);

    instance->list.id = UINT16_MAX;
    instance->list.next = NULL;

    _FREE_INSTANCES(object) = (lwm2m_obj_ipso_sensor_base_inst_t *)LWM2M_LIST_ADD(
        _FREE_INSTANCES(object), instance
        );
}

static lwm2m_obj_ipso_sensor_base_inst_t *_get_instance_from_free_list(
    lwm2m_obj_ipso_sensor_base_t *object)
{
    assert(object);
    lwm2m_obj_ipso_sensor_base_inst_t *instance = NULL;

    /* try to allocate an instance, by popping a free node from the list */
    _FREE_INSTANCES(object) = (lwm2m_obj_ipso_sensor_base_inst_t *)lwm2m_list_remove(
        (lwm2m_list_t *)_FREE_INSTANCES(object), UINT16_MAX, (lwm2m_list_t **)&instance
        );

    return instance;
}

int lwm2m_object_ipso_sensor_base_init_derived(lwm2m_client_data_t *client_data,
                                               lwm2m_obj_ipso_sensor_base_t *object,
                                               uint16_t object_id,
                                               lwm2m_obj_ipso_sensor_base_inst_t *instances,
                                               size_t instance_count)
{
    assert(object);
    assert(instances);
    memset(object, 0, sizeof(lwm2m_obj_ipso_sensor_base_t));

    /* initialize the wakaama LwM2M object */
    object->object.objID = object_id;
    object->object.readFunc = _read_cb;
    object->object.executeFunc = _exec_cb;
    object->object.userData = client_data;

    /* initialize the instances and add them to the free instance list */
    for (unsigned i = 0; i < instance_count; i++) {
        _put_instance_in_free_list(object, &instances[i]);
    }

    return 0;
}

static uint8_t _get_value(lwm2m_data_t *data, lwm2m_obj_ipso_sensor_base_inst_t *instance)
{
    assert(data);
    assert(instance);
    int res;

    switch (data->id) {
    case LWM2M_IPSO_BASE_SENSOR_VALUE_ID:
        if (instance->read_cb) {
            res = instance->read_cb(instance->read_cb_arg, &instance->sensor_value);
            if (res) {
                DEBUG("[lwm2m IPSO sensor base:read]: read callback failed\n");
                return COAP_500_INTERNAL_SERVER_ERROR;
            }
        }
        lwm2m_data_encode_float(instance->sensor_value, data);
        break;

    case LWM2M_IPSO_BASE_SENSOR_MIN_MEASURED_VALUE_ID:
        lwm2m_data_encode_float(instance->min_sensor_value, data);
        break;

    case LWM2M_IPSO_BASE_SENSOR_MAX_MEASURED_VALUE_ID:
        lwm2m_data_encode_float(instance->max_sensor_value, data);
        break;

    case LWM2M_IPSO_BASE_SENSOR_MIN_RANGE_VALUE_ID:
        lwm2m_data_encode_float(instance->min_range_value, data);
        break;

    case LWM2M_IPSO_BASE_SENSOR_MAX_RANGE_VALUE_ID:
        lwm2m_data_encode_float(instance->max_range_value, data);
        break;

    case LWM2M_IPSO_BASE_SENSOR_UNITS_ID:
        lwm2m_data_encode_string(instance->units, data);
        break;

    default:
        return COAP_404_NOT_FOUND;
    }
    return COAP_205_CONTENT;
}

static uint8_t _read_cb(uint16_t instance_id, int *num_data, lwm2m_data_t **data_array,
                        lwm2m_object_t *object)
{
    lwm2m_obj_ipso_sensor_base_inst_t *instance;
    uint8_t result;
    int i = 0;

    /* try to get the requested instance from the object list */
    instance = (lwm2m_obj_ipso_sensor_base_inst_t *)lwm2m_list_find(object->instanceList,
                                                                    instance_id);
    if (!instance) {
        DEBUG("[lwm2m IPSO sensor base:read]: can't find instance %d\n", instance_id);
        result = COAP_404_NOT_FOUND;
        goto out;
    }

    /* if the number of resources is not specified, we need to read all resources */
    if (!*num_data) {
        DEBUG("[lwm2m IPSO sensor base:read]: reading all resources\n");

        uint16_t res_list[] = {
            LWM2M_IPSO_BASE_SENSOR_VALUE_ID,
            LWM2M_IPSO_BASE_SENSOR_MIN_MEASURED_VALUE_ID,
            LWM2M_IPSO_BASE_SENSOR_MAX_MEASURED_VALUE_ID,
            LWM2M_IPSO_BASE_SENSOR_MIN_RANGE_VALUE_ID,
            LWM2M_IPSO_BASE_SENSOR_MAX_RANGE_VALUE_ID,
            LWM2M_IPSO_BASE_SENSOR_UNITS_ID
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
        DEBUG("[lwm2m IPSO sensor base:read]: reading resource %d\n", (*data_array)[i].id);
        result = _get_value(&(*data_array)[i], instance);
        i++;
    } while (i < *num_data && COAP_205_CONTENT == result);

out:
    return result;
}

static uint8_t _exec_cb(uint16_t instance_id, uint16_t resource_id, uint8_t *buffer, int length,
                        lwm2m_object_t *object)
{
    (void)buffer;
    (void)length;

    lwm2m_obj_ipso_sensor_base_inst_t *instance;
    uint8_t result;

    /* try to get the requested instance from the object list */
    instance = (lwm2m_obj_ipso_sensor_base_inst_t *)lwm2m_list_find(object->instanceList,
                                                                    instance_id);
    if (!instance) {
        DEBUG("[lwm2m IPSO sensor base:exec]: can't find instance %d\n", instance_id);
        result = COAP_404_NOT_FOUND;
        goto out;
    }

    switch (resource_id) {
    case LWM2M_IPSO_BASE_SENSOR_RESET_MIN_MAX_MEASURED_VALUES_ID:
        instance->min_sensor_value = instance->sensor_value;
        instance->max_sensor_value = instance->sensor_value;
        result = COAP_204_CHANGED;
        break;

    default:
        result = COAP_405_METHOD_NOT_ALLOWED;
        break;
    }

out:
    return result;
}

int32_t lwm2m_object_ipso_sensor_base_instance_create(lwm2m_obj_ipso_sensor_base_t *object,
                                                      const lwm2m_obj_ipso_base_sensor_args_t *args)
{
    assert(object);
    assert(args);

    int32_t result = -ENOMEM;
    lwm2m_obj_ipso_sensor_base_inst_t *instance = NULL;
    uint16_t _instance_id;

    if (object->free_instances == NULL) {
        DEBUG("[lwm2m:IPSO sensor base]: object not initialized\n");
        result = -EINVAL;
        goto out;
    }

    mutex_lock(&object->mutex);

    /* determine ID for new instance */
    if (args->instance_id < 0) {
        _instance_id = lwm2m_list_newId((lwm2m_list_t *)_USED_INSTANCES(object));
    }
    else {
        /* sanity check */
        if (args->instance_id >= (UINT16_MAX - 1)) {
            DEBUG("[lwm2m:IPSO sensor base]: instance ID %" PRIi32 " is too big\n",
                  args->instance_id);
            result = -EINVAL;
            goto free_out;
        }

        _instance_id = (uint16_t)args->instance_id;

        /* check that the ID is free to use */
        if (LWM2M_LIST_FIND(_USED_INSTANCES(object), _instance_id ) != NULL) {
            DEBUG("[lwm2m:IPSO sensor base]: instance ID %" PRIi32 " already in use\n",
                  args->instance_id);
            goto free_out;
        }
    }

    /* get a free instance */
    instance = _get_instance_from_free_list(object);
    if (!instance) {
        DEBUG("[lwm2m:IPSO sensor base]: can't allocate new instance\n");
        goto free_out;
    }

    memset(instance, 0, sizeof(lwm2m_obj_ipso_sensor_base_inst_t));

    instance->list.id = _instance_id;
    instance->sensor_value = 0;
    instance->min_sensor_value = 0;
    instance->max_sensor_value = 0;
    instance->min_range_value = args->min_range_value;
    instance->max_range_value = args->max_range_value;
    instance->read_cb = args->read_cb;
    instance->read_cb_arg = args->read_cb_arg;

    /* if units are specified, copy locally */
    if (args->units) {
        if (args->units_len > CONFIG_LWM2M_IPSO_SENSOR_BASE_UNITS_MAX_SIZE) {
            DEBUG("[lwm2m:IPSO sensor base]: not enough space for units string\n");
            /* restore instance */
            _put_instance_in_free_list(object, instance);
            return -ENOBUFS;
        }
        memcpy(instance->units, args->units, args->units_len);
    }
    else {
        memset(instance->units, 0, CONFIG_LWM2M_IPSO_SENSOR_BASE_UNITS_MAX_SIZE);
    }

    DEBUG("[lwm2m:IPSO sensor base]: new instance with ID %d\n", _instance_id);

    /* add the new instance to the list */
    object->object.instanceList = LWM2M_LIST_ADD(object->object.instanceList, instance);
    result = instance->list.id;

free_out:
    mutex_unlock(&object->mutex);
out:
    return result;
}

static void _mark_resource_as_changed(const lwm2m_object_t *object, uint16_t instance_id,
                                      uint16_t resource_id)
{
    lwm2m_uri_t uri;

    uri.objectId = object->objID;
    uri.instanceId = instance_id;
    uri.resourceId = resource_id;

    uri.flag = LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID | LWM2M_URI_FLAG_RESOURCE_ID;

    lwm2m_resource_value_changed(lwm2m_client_get_ctx(object->userData), &uri);
}

void lwm2m_object_ipso_sensor_base_update_value(const lwm2m_client_data_t *client_data,
                                                const lwm2m_obj_ipso_sensor_base_t *object,
                                                uint16_t instance_id, int16_t value)
{
    (void)client_data;
    lwm2m_obj_ipso_sensor_base_inst_t *instance;

    /* try to get the requested instance from the object list */
    instance = (lwm2m_obj_ipso_sensor_base_inst_t *)lwm2m_list_find(object->object.instanceList,
                                                                    instance_id);
    if (!instance) {
        DEBUG("[lwm2m:IPSO sensor base:update]: can't find instance %" PRIiSIZE "\n", instance_id);
        return;
    }

    instance->sensor_value = value;
    _mark_resource_as_changed(&object->object, instance_id, LWM2M_IPSO_BASE_SENSOR_VALUE_ID);

    if (value < instance->min_sensor_value) {
        instance->min_sensor_value = value;
        _mark_resource_as_changed(&object->object, instance_id,
                                  LWM2M_IPSO_BASE_SENSOR_MIN_MEASURED_VALUE_ID);
    }

    if (value > instance->max_sensor_value) {
        instance->max_sensor_value = value;
        _mark_resource_as_changed(&object->object, instance_id,
                                  LWM2M_IPSO_BASE_SENSOR_MAX_MEASURED_VALUE_ID);
    }
}
