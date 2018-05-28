/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "lwm2mclient.h"

/*
 * RIOT debugging
 */
#define ENABLE_DEBUG (0)
#include "debug.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

typedef struct _sensor_data_t {
    struct _sensor_data_t *next;    /* matches lwm2m_list_t::next */
    uint16_t instance_id;           /* matches lwm2m_list_t::id */
    saul_reg_t *dev;                /* The device for this sensor */
    const char *unit;               /* Unit the sensor returns (RO) */
    float current_measured;         /* Most recently measured value (RO) */
    float min_measured;             /* Smallest measured value (RO) */
    float max_measured;             /* Largest measured value (RO) */
    const char *sensor_type;        /* The sensor type (RO) */
    /*char *app_type;                * Application type (RW)*
       float min_range;              * Lowest measurable value (RO) *
       float max_range;              * Highest measurable value (RO) */
} sensor_data_t;

static float prv_sensor_read_float(saul_reg_t *dev)
{
    phydat_t ret;

    if (saul_reg_read(dev, &ret) < 0) {
        DEBUG("%s: saul_reg_read() error\n", __func__);
        return 0;
    }
    return (float)ret.val[0] * powf(10, ret.scale);
}

static const char *prv_sensor_get_unit(saul_reg_t *dev)
{
    phydat_t ret;

    if (saul_reg_read(dev, &ret) < 0) {
        DEBUG("%s: saul_reg_read() error\n", __func__);
        return NULL;
    }
    return phydat_unit_to_str(ret.unit);
}

static void prv_update_float_sensor(sensor_data_t *target)
{
    target->current_measured = prv_sensor_read_float(target->dev);
    target->max_measured = max(target->current_measured, target->max_measured);
    target->min_measured = min(target->current_measured, target->min_measured);
    DEBUG("%s: measured new sensor value %f\n", __func__, target->current_measured);
}

static void prv_init_instance(sensor_data_t *sensor_instance, saul_reg_t *dev)
{
    sensor_instance->dev = dev;
    sensor_instance->sensor_type = dev->name;
    sensor_instance->unit = prv_sensor_get_unit(dev);
    sensor_instance->current_measured = prv_sensor_read_float(sensor_instance->dev);
    sensor_instance->max_measured = sensor_instance->current_measured;
    sensor_instance->min_measured = sensor_instance->current_measured;
}

static uint8_t prv_get_value(lwm2m_data_t *data, sensor_data_t *target)
{
    switch (data->id) {
        case LWM2M_SENSOR_VALUE_ATTR:
        case LWM2M_SENSOR_ANALOG_INPUT_ATTR:
            lwm2m_data_encode_float(target->current_measured, data);
            return COAP_205_CONTENT;
        case LWM2M_SENSOR_UNITS_ATTR:
            lwm2m_data_encode_string(target->unit, data);
            return COAP_205_CONTENT;
        case LWM2M_MIN_MEASURED_ATTR:
            lwm2m_data_encode_float(target->min_measured, data);
            return COAP_205_CONTENT;
        case LWM2M_MAX_MEASURED_ATTR:
            lwm2m_data_encode_float(target->max_measured, data);
            return COAP_205_CONTENT;
        /*case LWM2M_MIN_RANGE_ATTR:
            lwm2m_data_encode_float(target->min_range, data);
            return COAP_205_CONTENT;
           case LWM2M_MAX_RANGE_ATTR:
            lwm2m_data_encode_float(target->max_range, data);
            return COAP_205_CONTENT;*/
        case LWM2M_RESET_MIN_MAX_MEASURED_ATTR:
            return COAP_405_METHOD_NOT_ALLOWED;
        case LWM2M_SENSOR_TYPE_ATTR:
            lwm2m_data_encode_string(target->sensor_type, data);
            return COAP_205_CONTENT;
    }
    return COAP_404_NOT_FOUND;
}

static uint8_t prv_float_read(uint16_t instance_id, int *num_data,
                              lwm2m_data_t **data_array,
                              lwm2m_object_t *object)
{
    int i;
    uint8_t result;
    sensor_data_t *target =
        (sensor_data_t *)lwm2m_list_find(object->instanceList, instance_id);

    if (NULL == target) {
        return COAP_404_NOT_FOUND;
    }

    /* Server is asking for everything. */
    if (*num_data == 0) {
        uint16_t res_list[] = {
            LWM2M_SENSOR_ANALOG_INPUT_ATTR,
            LWM2M_SENSOR_VALUE_ATTR, LWM2M_SENSOR_UNITS_ATTR, LWM2M_MIN_MEASURED_ATTR,
            LWM2M_MAX_MEASURED_ATTR, LWM2M_SENSOR_TYPE_ATTR,
            /*LWM2M_MIN_RANGE_ATTR, LWM2M_MAX_RANGE_ATTR,*/
        };
        int nb_res = sizeof(res_list) / sizeof(uint16_t);

        *data_array = lwm2m_data_new(nb_res);
        if (*data_array == NULL) {
            return COAP_500_INTERNAL_SERVER_ERROR;
        }
        *num_data = nb_res;
        for (i = 0; i < nb_res; i++) {
            (*data_array)[i].id = res_list[i];
        }
    }

    i = 0;
    do {
        result = prv_get_value((*data_array) + i, target);
        i++;
    } while (i < *num_data && result == COAP_205_CONTENT);

    return result;
}

static uint8_t prv_float_write(uint16_t instance_id, int num_data,
                               lwm2m_data_t *data_array,
                               lwm2m_object_t *object)
{
    (void)instance_id;
    (void)object;
    int i;

    for (i = 0; i < num_data; i++) {
        switch (data_array[i].id) {
            case LWM2M_SENSOR_VALUE_ATTR:
            case LWM2M_SENSOR_ANALOG_INPUT_ATTR:
            case LWM2M_SENSOR_UNITS_ATTR:
            case LWM2M_MIN_MEASURED_ATTR:
            case LWM2M_MAX_MEASURED_ATTR:
            /* case LWM2M_MIN_RANGE_ATTR:
               case LWM2M_MAX_RANGE_ATTR: */
            case LWM2M_RESET_MIN_MAX_MEASURED_ATTR:
            case LWM2M_SENSOR_TYPE_ATTR:
                return COAP_405_METHOD_NOT_ALLOWED;
        }
    }
    return COAP_404_NOT_FOUND;
}

static uint8_t prv_float_execute(uint16_t instance_id, uint16_t resource_id,
                                 uint8_t *buffer, int length,
                                 lwm2m_object_t *object)
{
    (void)buffer;
    (void)length;
    float value;
    sensor_data_t *target =
        (sensor_data_t *)lwm2m_list_find(object->instanceList, instance_id);
    if (NULL == target) {
        return COAP_404_NOT_FOUND;
    }

    switch (resource_id) {
        case LWM2M_SENSOR_VALUE_ATTR:
        case LWM2M_SENSOR_ANALOG_INPUT_ATTR:
        case LWM2M_SENSOR_UNITS_ATTR:
        case LWM2M_MIN_MEASURED_ATTR:
        case LWM2M_MAX_MEASURED_ATTR:
        case LWM2M_SENSOR_TYPE_ATTR:
            /* case LWM2M_MIN_RANGE_ATTR:
               case LWM2M_MAX_RANGE_ATTR: */
            return COAP_405_METHOD_NOT_ALLOWED;
        case LWM2M_RESET_MIN_MAX_MEASURED_ATTR:
            value = prv_sensor_read_float(target->dev);
            target->max_measured = value;
            target->min_measured = value;
            return COAP_204_CHANGED;
    }
    return COAP_404_NOT_FOUND;
}

static uint8_t prv_float_discover(uint16_t instance_id, int *num_data,
                                  lwm2m_data_t **data_array,
                                  lwm2m_object_t *object)
{
    uint8_t result;
    int i;

    (void)instance_id;
    (void)object;

    result = COAP_205_CONTENT;

    /* Server is asking for everything. */
    if (*num_data == 0) {
        uint16_t res_list[] = {
            LWM2M_SENSOR_ANALOG_INPUT_ATTR,
            LWM2M_SENSOR_VALUE_ATTR, LWM2M_SENSOR_UNITS_ATTR, LWM2M_MIN_MEASURED_ATTR,
            LWM2M_MAX_MEASURED_ATTR, /*LWM2M_MIN_RANGE_ATTR, LWM2M_MAX_RANGE_ATTR,*/
        };
        int nb_res = sizeof(res_list) / sizeof(uint16_t);

        *data_array = lwm2m_data_new(nb_res);
        if (*data_array == NULL) {
            return COAP_500_INTERNAL_SERVER_ERROR;
        }
        *num_data = nb_res;
        for (i = 0; i < nb_res; i++) {
            (*data_array)[i].id = res_list[i];
        }
    }
    else {
        for (i = 0; i < *num_data && result == COAP_205_CONTENT; i++) {
            switch ((*data_array)[i].id) {
                case LWM2M_SENSOR_ANALOG_INPUT_ATTR:
                case LWM2M_SENSOR_VALUE_ATTR:
                case LWM2M_SENSOR_UNITS_ATTR:
                case LWM2M_MIN_MEASURED_ATTR:
                case LWM2M_MAX_MEASURED_ATTR:
                /* case LWM2M_MIN_RANGE_ATTR:
                   case LWM2M_MAX_RANGE_ATTR: */
                case LWM2M_RESET_MIN_MAX_MEASURED_ATTR:
                case LWM2M_SENSOR_TYPE_ATTR:
                    break;
                default:
                    result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

static bool prv_sensor_supported(int sensor_id)
{
    switch (sensor_id) {
        case LWM2M_GENERIC_SENSOR_ID:
        case LWM2M_ILLUMINANCE_SENSOR_ID:
        case LWM2M_ANALOG_SENSOR_ID:
        case LWM2M_TEMPERATURE_SENSOR_ID:
        case LWM2M_HUMIDITY_SENSOR_ID:
        case LWM2M_BAROMETER_SENSOR_ID:
        case LWM2M_VOLTAGE_SENSOR_ID:
        case LWM2M_CURRENT_SENSOR_ID:
        case LWM2M_FREQUENCY_SENSOR_ID:
        case LWM2M_DEPTH_SENSOR_ID:
        case LWM2M_PERCENTAGE_SENSOR_ID:
        case LWM2M_ALTITUDE_SENSOR_ID:
        case LWM2M_LOAD_SENSOR_ID:
        case LWM2M_PRESSURE_SENSOR_ID:
        case LWM2M_LOUDNESS_SENSOR_ID:
        case LWM2M_CONCENTRATION_SENSOR_ID:
        case LWM2M_ACIDITY_SENSOR_ID:
        case LWM2M_CONDUCTIVITY_SENSOR_ID:
        case LWM2M_POWER_SENSOR_ID:
        case LWM2M_POWER_FACTOR_SENSOR_ID:
        case LWM2M_DISTANCE_SENSOR_ID:
        case LWM2M_RATE_SENSOR_ID:
            return true;
        default:
            return false;
    }
}

int lwm2m_get_float_sensor(lwm2m_object_t **object, int sensor_id, saul_reg_t *dev)
{
    sensor_data_t *sensor_instance;
    bool self_alloc = false;

    if (object == NULL) {
        DEBUG("%s: EINVAL object == NULL\n", __func__);
        return -EINVAL;
    }

    if (!prv_sensor_supported(sensor_id)) {
        DEBUG("%s: EINVAL invalid sensor_id\n", __func__);
        return -EINVAL;
    }

    if (dev == NULL) {
        DEBUG("%s: EINVAL dev == NULL\n", __func__);
        return -EINVAL;
    }

    /* Allocate new object if we didn't get an existing one to add an instance to. */
    if ((*object) == NULL) {
        (*object) = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
        if ((*object) == NULL) {
            DEBUG("%s: couldn't allocate memory for lwm2m_object_t\n", __func__);
            return -ENOMEM;
        }
        self_alloc = true;

        (*object)->objID = sensor_id;

        (*object)->readFunc = prv_float_read;
        (*object)->writeFunc = prv_float_write;
        (*object)->executeFunc = prv_float_execute;
        (*object)->discoverFunc = prv_float_discover;
    }
    else {
        /* Check for mismatching sensor IDs */
        if ((*object)->objID != sensor_id) {
            DEBUG("%s: mismatching sensor ids\n", __func__);
            return -EINVAL;
        }

        /* If we got an existing object, check whether an instance for this dev already exists */
        lwm2m_list_t *target;
        for (target = (*object)->instanceList; target != NULL; target = target->next) {
            if (((sensor_data_t *)target)->dev == dev) {
                DEBUG("%s: sensor instance for dev already exists\n", __func__);
                return -EEXIST;
            }
        }
    }

    sensor_instance = (sensor_data_t *)lwm2m_malloc(sizeof(sensor_data_t));
    if (sensor_instance == NULL) {
        DEBUG("%s: couldn't allocate memoru for sensor_data_t\n", __func__);
        /* Only free object if we allocated it ourselves
         * TODO/FIXME: Maybe never free ever and let the caller worry about that?
         */
        if (self_alloc) {
            DEBUG("%s: freeing self-allocated lwm2m_object_t\n", __func__);
            lwm2m_free((*object));
        }
        return -ENOMEM;
    }
    memset(sensor_instance, 0, sizeof(sensor_data_t));

    sensor_instance->instance_id = lwm2m_list_newId((*object)->instanceList);
    prv_init_instance(sensor_instance, dev);
    (*object)->instanceList = LWM2M_LIST_ADD((*object)->instanceList, sensor_instance);

    return 0;
}

void lwm2m_free_float_sensor(lwm2m_object_t *obj)
{
    if (obj == NULL) {
        return;
    }
    if (obj->userData) {
        lwm2m_free(obj->userData);
    }
    while (obj->instanceList != NULL) {
        sensor_data_t *instance = (sensor_data_t *)obj->instanceList;
        obj->instanceList = obj->instanceList->next;
        lwm2m_free(instance);
    }
    lwm2m_free(obj);
}

int lwm2m_auto_add_float_sensors(lwm2m_context_t *context)
{
    int added = 0;
    int i;

    uint16_t ids[] = {
        LWM2M_GENERIC_SENSOR_ID,    LWM2M_ILLUMINANCE_SENSOR_ID,
        LWM2M_ANALOG_SENSOR_ID,     LWM2M_TEMPERATURE_SENSOR_ID,
        LWM2M_HUMIDITY_SENSOR_ID,   LWM2M_BAROMETER_SENSOR_ID,
        LWM2M_VOLTAGE_SENSOR_ID,    LWM2M_CURRENT_SENSOR_ID,
        LWM2M_FREQUENCY_SENSOR_ID,  LWM2M_DEPTH_SENSOR_ID,
        LWM2M_PERCENTAGE_SENSOR_ID, LWM2M_ALTITUDE_SENSOR_ID,
        LWM2M_LOAD_SENSOR_ID,       LWM2M_PRESSURE_SENSOR_ID,
        LWM2M_LOUDNESS_SENSOR_ID,   LWM2M_CONCENTRATION_SENSOR_ID,
        LWM2M_ACIDITY_SENSOR_ID,    LWM2M_CONDUCTIVITY_SENSOR_ID,
        LWM2M_POWER_SENSOR_ID,      LWM2M_POWER_FACTOR_SENSOR_ID,
        LWM2M_DISTANCE_SENSOR_ID,   LWM2M_RATE_SENSOR_ID,
    };

    /* Iterate over all supported object IDs */
    for (i = 0; i < (int)(sizeof(ids) / sizeof(uint16_t)); i++) {
        int added_tmp = 0;
        saul_reg_t *reg;
        lwm2m_object_t *object = NULL;
        /* Collect all devices for the current object ID */
        for (reg = saul_reg; reg != NULL; reg = reg->next) {
            if ((int)ids[i] == lwm2m_saul_type_to_obj_id(reg->driver->type)) {
                DEBUG("%s: adding instance of %s\n", __func__,
                      lwm2m_object_id_to_str(ids[i]));
                if (lwm2m_get_float_sensor(&object, ids[i], reg) == 0) {
                    added_tmp++;
                }
            }
        }
        if (object != NULL) {
            if (lwm2m_add_object(context, object) == COAP_NO_ERROR) {
                added += added_tmp;
            } else {
                DEBUG("%s: lwm2m_add_object() failed for %s\n", __func__,
                      lwm2m_object_id_to_str(ids[i]));
            }
        }
    }

    return added;
}

void lwm2m_poll_float_sensors(lwm2m_context_t *lwm2mH)
{
    lwm2m_object_t *object = (lwm2m_object_t *)lwm2mH->objectList;

    DEBUG("%s: updating sensor values", __func__);

    for (; object != NULL; object = object->next) {
        if (prv_sensor_supported(object->objID)) {
            lwm2m_uri_t uri;
            uri.flag = LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID | LWM2M_URI_FLAG_RESOURCE_ID;
            uri.objectId = object->objID;
            sensor_data_t *instance = (sensor_data_t *)object->instanceList;

            for (; instance != NULL; instance = instance->next) {
                float current = instance->current_measured;
                float max = instance->max_measured;
                float min = instance->min_measured;

                uri.instanceId = instance->instance_id;

                prv_update_float_sensor(instance);

                if (current != instance->current_measured) {
                    uri.resourceId = LWM2M_SENSOR_VALUE_ATTR;
                    lwm2m_resource_value_changed(lwm2mH, &uri);
                    DEBUG("%s: sensor value changed for %"PRIu16"/%"PRIu16"/%"PRIu16"\n",
                          __func__, uri.objectId, uri.instanceId,
                          uri.resourceId);
                    uri.resourceId = LWM2M_SENSOR_ANALOG_INPUT_ATTR;
                    lwm2m_resource_value_changed(lwm2mH, &uri);
                    DEBUG("%s: sensor value changed for %"PRIu16"/%"PRIu16"/%"PRIu16"\n",
                          __func__, uri.objectId, uri.instanceId,
                          uri.resourceId);
                }
                if (max != instance->max_measured) {
                    uri.resourceId = LWM2M_MAX_MEASURED_ATTR;
                    lwm2m_resource_value_changed(lwm2mH, &uri);
                    DEBUG("%s: max measured changed for %"PRIu16"/%"PRIu16"/%"PRIu16"\n",
                          __func__, uri.objectId, uri.instanceId,
                          uri.resourceId);
                }
                if (min != instance->min_measured) {
                    uri.resourceId = LWM2M_MIN_MEASURED_ATTR;
                    lwm2m_resource_value_changed(lwm2mH, &uri);
                    DEBUG("%s: min measured changed for %"PRIu16"/%"PRIu16"/%"PRIu16"\n",
                          __func__, uri.objectId, uri.instanceId,
                          uri.resourceId);
                }
            }
        }
    }
}
