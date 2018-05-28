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
#include <inttypes.h>
#include <stdbool.h>
#include "lwm2mclient.h"
#include "xtimer.h"

#ifdef MODULE_SAUL_GPIO
#include "saul/periph.h"
#endif /* MODULE_SAUL_GPIO */

/*
 * RIOT debugging
 */
#define ENABLE_DEBUG (0)
#include "debug.h"

/*
 * Bit mask entries for observer notifications
 */
#define NOTIFY_VALUE       (0x01)
#define NOTIFY_COUNT       (0x02)
#define NOTIFY_POLARITY    (0x04)
#define NOTIFY_DEBOUNCE    (0x08)
#define NOTIFY_EDGE_SELECT (0x10)

typedef struct _sensor_data_t {
    struct _sensor_data_t *next;    /* matches lwm2m_list_t::next */
    uint16_t instance_id;           /* matches lwm2m_list_t::id */
    saul_reg_t *dev;                /* The device for this sensor */
    uint8_t notify;                 /* Bitmask for observer notifications */
    bool value;                     /* Most recently measured value (RO) */
    uint16_t count;                 /* Cumulative value of active state detected (RO) */
    bool polarity;                  /* 0 = Normal, 1= Reversed (RW) */
    uint32_t debounce;              /* Debounce value in ms; (internally stored as us) (RW) */
    uint32_t debounce_timer;        /* Timestamp since last measurement for debouncing (internal) */
    uint8_t edge_select;            /* 1 == falling, 2 == rising, 3 == both; (RW) */
    const char *sensor_type;        /* Sensor type (RO) */
} sensor_data_t;

typedef struct notification_tuple {
    uint8_t mask;
    uint16_t id;
} notification_tuple_t;

static bool prv_sensor_read_bool(sensor_data_t *target)
{
    phydat_t ret;
    uint32_t now = xtimer_now_usec();

    /* Skip measuring until debounce time expired */
    if (now - target->debounce_timer < target->debounce) {
        return target->value;
    }

    target->debounce_timer = now;

    if (saul_reg_read(target->dev, &ret) < 0) {
        DEBUG("%s: saul_reg_read() error\n", __func__);
        return false;
    }
    return (ret.val[0]) ? !target->polarity : target->polarity;
}

static void prv_update_bool_sensor(sensor_data_t *target)
{
    bool newval = prv_sensor_read_bool(target);
    if (newval && !target->value) {
        target->count++;
        target->notify |= NOTIFY_COUNT;
    }
    if (newval != target->value) {
        target->notify |= NOTIFY_VALUE;
    }
    target->value = newval;
}

#ifdef MODULE_SAUL_GPIO
static void gpio_isr(void *arg)
{
    prv_update_bool_sensor((sensor_data_t*)arg);
}

static uint32_t prv_edge_select(uint8_t es) {
    switch (es) {
        case 1:
            return GPIO_FALLING;
        case 2:
            return GPIO_RISING;
        case 3:
        default:
            return GPIO_BOTH;
    }
}

static void prv_set_interrupt(sensor_data_t *target)
{
    /* Register an interrupt to update sensor value */
    const saul_gpio_params_t *sdev = (const saul_gpio_params_t *)target->dev->dev;
    if (gpio_init_int(sdev->pin, sdev->mode,
                      prv_edge_select(target->edge_select), gpio_isr,
                      (void*)target)) {
        DEBUG("%s: Error registering gpio interrupt.\n", __func__);
    }
}
#else /* MODULE_SAUL_GPIO */
static void prv_set_interrupt(sensor_data_t *target)
{
    (void)target;
}
#endif /* MODULE_SAUL_GPIO */

static void prv_init_instance(sensor_data_t *sensor_instance, saul_reg_t *dev)
{
    sensor_instance->dev = dev;
    sensor_instance->notify = 0;
    sensor_instance->count = 0;
    sensor_instance->polarity = false;
    sensor_instance->debounce = 0;
    sensor_instance->debounce_timer = 0;
    sensor_instance->edge_select = 3;
    sensor_instance->sensor_type = dev->name;
    prv_update_bool_sensor(sensor_instance);
    prv_set_interrupt(sensor_instance);
}


static uint8_t prv_get_value(lwm2m_data_t *data, sensor_data_t *target)
{
    switch (data->id) {
        case LWM2M_DINPUT_ATTR:
            lwm2m_data_encode_bool(target->value, data);
            return COAP_205_CONTENT;
        case LWM2M_DINPUT_COUNTER_ATTR:
            lwm2m_data_encode_int(target->count, data);
            return COAP_205_CONTENT;
        case LWM2M_DINPUT_POLARITY_ATTR:
            lwm2m_data_encode_bool(target->polarity, data);
            return COAP_205_CONTENT;
        case LWM2M_DINPUT_DEBOUNCE_ATTR:
            lwm2m_data_encode_int(target->debounce / US_PER_MS, data);
            return COAP_205_CONTENT;
        case LWM2M_DINPUT_EDGESELECT_ATTR:
            lwm2m_data_encode_int(target->edge_select, data);
            return COAP_205_CONTENT;
        case LWM2M_SENSOR_TYPE_ATTR:
            lwm2m_data_encode_string(target->sensor_type, data);
            return COAP_205_CONTENT;
        case LWM2M_DINPUT_COUNTER_RESET_ATTR:
            return COAP_405_METHOD_NOT_ALLOWED;
    }
    return COAP_404_NOT_FOUND;
}

static uint8_t prv_bool_read(uint16_t instance_id, int *num_data,
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
            LWM2M_DINPUT_ATTR, LWM2M_DINPUT_COUNTER_ATTR,
            LWM2M_DINPUT_POLARITY_ATTR, LWM2M_DINPUT_DEBOUNCE_ATTR,
            LWM2M_DINPUT_EDGESELECT_ATTR, LWM2M_DINPUT_COUNTER_RESET_ATTR,
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

static uint8_t prv_bool_write(uint16_t instance_id, int num_data,
                               lwm2m_data_t *data_array,
                               lwm2m_object_t *object)
{
    int i = 0;
    uint8_t result;
    union {
        bool b;
        int64_t i;
    } value;
    sensor_data_t *target =
        (sensor_data_t *)lwm2m_list_find(object->instanceList, instance_id);

    if (NULL == target) {
        return COAP_404_NOT_FOUND;
    }

    do {
        switch (data_array[i].id) {
            case LWM2M_DINPUT_ATTR:
            case LWM2M_DINPUT_COUNTER_ATTR:
            case LWM2M_DINPUT_COUNTER_RESET_ATTR:
                return COAP_405_METHOD_NOT_ALLOWED;
            case LWM2M_DINPUT_POLARITY_ATTR:
                if (lwm2m_data_decode_bool(&data_array[i], &value.b) != 1) {
                    result = COAP_400_BAD_REQUEST;
                } else {
                    target->notify |= NOTIFY_POLARITY;
                    target->polarity = value.b;
                    result = COAP_204_CHANGED;
                }
            case LWM2M_DINPUT_DEBOUNCE_ATTR:
                if (lwm2m_data_decode_int(&data_array[i], &value.i) != 1) {
                    result = COAP_400_BAD_REQUEST;
                } else {
                    target->notify |= NOTIFY_DEBOUNCE;
                    target->debounce = (uint32_t)value.i * US_PER_MS;
                    result = COAP_204_CHANGED;
                }
            case LWM2M_DINPUT_EDGESELECT_ATTR:
                if (lwm2m_data_decode_int(&data_array[i], &value.i) != 1) {
                    result = COAP_400_BAD_REQUEST;
                } else if (value.i < 1 || value.i > 3) {
                    result = COAP_400_BAD_REQUEST;
                } else {
                    target->notify |= NOTIFY_EDGE_SELECT;
                    target->edge_select = (uint32_t)value.i;
                    prv_set_interrupt(target);
                    result = COAP_204_CHANGED;
                }
            default:
                result = COAP_404_NOT_FOUND;
                break;
        }
        i++;
    } while (i < num_data && result == COAP_204_CHANGED);

    return result;
}

static uint8_t prv_bool_execute(uint16_t instance_id, uint16_t resource_id,
        uint8_t *buffer, int length,
        lwm2m_object_t *object)
{
    (void)buffer;
    (void)length;
    sensor_data_t *target =
        (sensor_data_t *)lwm2m_list_find(object->instanceList, instance_id);
    if (NULL == target) {
        return COAP_404_NOT_FOUND;
    }

    switch (resource_id) {
        case LWM2M_DINPUT_ATTR:
        case LWM2M_DINPUT_COUNTER_ATTR:
        case LWM2M_DINPUT_POLARITY_ATTR:
        case LWM2M_DINPUT_DEBOUNCE_ATTR:
        case LWM2M_DINPUT_EDGESELECT_ATTR:
        case LWM2M_SENSOR_TYPE_ATTR:
            return COAP_405_METHOD_NOT_ALLOWED;
        case LWM2M_DINPUT_COUNTER_RESET_ATTR:
            target->count = 0;
            return COAP_204_CHANGED;
    }
    return COAP_404_NOT_FOUND;
}

static uint8_t prv_bool_discover(uint16_t instance_id, int *num_data,
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
            LWM2M_DINPUT_ATTR, LWM2M_DINPUT_COUNTER_ATTR,
            LWM2M_DINPUT_POLARITY_ATTR, LWM2M_DINPUT_DEBOUNCE_ATTR,
            LWM2M_DINPUT_EDGESELECT_ATTR, LWM2M_DINPUT_COUNTER_RESET_ATTR,
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
                case LWM2M_DINPUT_ATTR:
                case LWM2M_DINPUT_COUNTER_ATTR:
                case LWM2M_DINPUT_POLARITY_ATTR:
                case LWM2M_DINPUT_DEBOUNCE_ATTR:
                case LWM2M_DINPUT_EDGESELECT_ATTR:
                case LWM2M_SENSOR_TYPE_ATTR:
                case LWM2M_DINPUT_COUNTER_RESET_ATTR:
                    break;
                default:
                    result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

int lwm2m_get_bool_sensor(lwm2m_object_t **object, saul_reg_t *dev)
{
    sensor_data_t *sensor_instance;
    bool self_alloc = false;

    if (object == NULL) {
        DEBUG("%s: EINVAL object == NULL\n", __func__);
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

        (*object)->objID = LWM2M_DIGITAL_SENSOR_ID;

        (*object)->readFunc = prv_bool_read;
        (*object)->writeFunc = prv_bool_write;
        (*object)->executeFunc = prv_bool_execute;
        (*object)->discoverFunc = prv_bool_discover;
    }
    else {
        /* Check for mismatching sensor IDs */
        if ((*object)->objID != LWM2M_DIGITAL_SENSOR_ID) {
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

void lwm2m_free_bool_sensor(lwm2m_object_t *obj)
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

int lwm2m_auto_add_bool_sensors(lwm2m_context_t *context)
{
    int added = 0;
    int id = LWM2M_DIGITAL_SENSOR_ID;
    int added_tmp = 0;
    saul_reg_t *reg;
    lwm2m_object_t *object = NULL;

    /* Collect all devices for the current object ID */
    for (reg = saul_reg; reg != NULL; reg = reg->next) {
        if (lwm2m_saul_type_to_obj_id(reg->driver->type) == id) {
            DEBUG("%s: adding instance of %s\n", __func__,
                  lwm2m_object_id_to_str(id));
            if (lwm2m_get_bool_sensor(&object, reg) == 0) {
                added_tmp++;
            }
        }
    }
    if (object != NULL) {
        if (lwm2m_add_object(context, object) == COAP_NO_ERROR) {
            added += added_tmp;
        } else {
            DEBUG("%s: lwm2m_add_object() failed for %s\n", __func__,
                  lwm2m_object_id_to_str(id));
        }
    }

    return added;
}

void lwm2m_poll_bool_sensors(lwm2m_context_t *lwm2mH)
{
    lwm2m_object_t *object = (lwm2m_object_t *)lwm2mH->objectList;

    DEBUG("%s: updating sensor values", __func__);

    for (; object != NULL; object = object->next) {
        if (object->objID == LWM2M_DIGITAL_SENSOR_ID) {
            lwm2m_uri_t uri;
            uri.flag = LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID | LWM2M_URI_FLAG_RESOURCE_ID;
            uri.objectId = object->objID;
            sensor_data_t *instance = (sensor_data_t *)object->instanceList;

            for (; instance != NULL; instance = instance->next) {
                uint8_t i;
                notification_tuple_t notify_array[] = {
                    {NOTIFY_VALUE, LWM2M_DINPUT_ATTR},
                    {NOTIFY_COUNT, LWM2M_DINPUT_COUNTER_ATTR},
                    {NOTIFY_POLARITY, LWM2M_DINPUT_COUNTER_ATTR},
                    {NOTIFY_DEBOUNCE, LWM2M_DINPUT_DEBOUNCE_ATTR},
                    {NOTIFY_EDGE_SELECT, LWM2M_DINPUT_EDGESELECT_ATTR},
                };

                prv_update_bool_sensor(instance);

                for (i = 0; i < sizeof(notify_array) / sizeof(notification_tuple_t); i++) {
                    if (instance->notify & notify_array[i].mask) {
                        instance->notify &= ~notify_array[i].mask;
                        uri.resourceId = notify_array[i].id;
                        lwm2m_resource_value_changed(lwm2mH, &uri);
                        DEBUG("%s: sensor value changed for %"PRIu16"/%"PRIu16"/%"PRIu16"\n",
                                __func__, uri.objectId, uri.instanceId,
                                uri.resourceId);
                    }
                }
            }
        }
    }
}
