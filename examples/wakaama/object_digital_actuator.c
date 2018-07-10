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

/*
 * RIOT debugging
 */
#define ENABLE_DEBUG (0)
#include "debug.h"

#define NOTIFY_VALUE    (0x01)
#define NOTIFY_POLARITY (0x02)
#define NOTIFY_APP_TYPE (0x03)

typedef struct _actuator_data_t {
    struct _actuator_data_t *next;  /* matches lwm2m_list_t::next */
    uint16_t instance_id;           /* matches lwm2m_list_t::id */
    saul_reg_t *dev;                /* The device for this sensor */
    uint8_t notify;                 /* Bitmask for observer notifications */
    bool value;                     /* Digitial output state (RW) */
    bool polarity;                  /* 0 = Normal, 1= Reversed (RW) */
    char *app_type;                 /* Application type (RW) */
} actuator_data_t;

typedef struct notification_tuple {
    uint8_t mask;
    uint16_t id;
} notification_tuple_t;

static int prv_actuator_write_bool(actuator_data_t *target)
{
    phydat_t data = {
        .val[0] = (int16_t)target->value,
        .val[1] = 0,
        .val[2] = 0,
        .scale = 0,
        .unit = UNIT_NONE,
    };

    int ret = saul_reg_write(target->dev, &data);
    if (ret != 1) {
        DEBUG("%s: saul_reg_write() error\n", __func__);
    }
    return ret;
}

static bool prv_actuator_read_bool(actuator_data_t *target)
{
    phydat_t ret;

    if (saul_reg_read(target->dev, &ret) < 0) {
        DEBUG("%s: saul_reg_read() error\n", __func__);
        return false;
    }
    return (ret.val[0]) ? !target->polarity : target->polarity;
}

static void priv_update_actuator_value(actuator_data_t *target)
{
    bool old_value = target->value;
    target->value = prv_actuator_read_bool(target);
    if (target->value != old_value) {
        target->notify |= NOTIFY_VALUE;
    }
}

static void prv_init_instance(actuator_data_t *actuator_instance, saul_reg_t *dev)
{
    actuator_instance->dev = dev;
    actuator_instance->polarity = false;
    actuator_instance->value = prv_actuator_read_bool(actuator_instance);
    actuator_instance->app_type = lwm2m_strdup(dev->name);
}


static uint8_t prv_get_value(lwm2m_data_t *data, actuator_data_t *target)
{
    switch (data->id) {
        case LWM2M_DOUTPUT_ATTR:
            lwm2m_data_encode_bool(target->value, data);
            return COAP_205_CONTENT;
        case LWM2M_DOUTPUT_POLARITY_ATTR:
            lwm2m_data_encode_bool(target->polarity, data);
            return COAP_205_CONTENT;
        case LWM2M_APP_TYPE_ATTR:
            lwm2m_data_encode_string(target->app_type, data);
            return COAP_205_CONTENT;
    }
    return COAP_404_NOT_FOUND;
}

static uint8_t prv_bool_read(uint16_t instance_id, int *num_data,
                             lwm2m_data_t **data_array,
                             lwm2m_object_t *object)
{
    int i;
    uint8_t result;
    actuator_data_t *target =
        (actuator_data_t *)lwm2m_list_find(object->instanceList, instance_id);

    if (NULL == target) {
        return COAP_404_NOT_FOUND;
    }

    /* Server is asking for everything. */
    if (*num_data == 0) {
        uint16_t res_list[] = {
            LWM2M_DOUTPUT_ATTR, LWM2M_DOUTPUT_POLARITY_ATTR,
            LWM2M_APP_TYPE_ATTR,
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
    bool value;
    actuator_data_t *target =
        (actuator_data_t *)lwm2m_list_find(object->instanceList, instance_id);

    if (NULL == target) {
        return COAP_404_NOT_FOUND;
    }

    do {
        switch (data_array[i].id) {
            case LWM2M_DOUTPUT_POLARITY_ATTR:
                if (lwm2m_data_decode_bool(&data_array[i], &value) != 1) {
                    result = COAP_400_BAD_REQUEST;
                } else {
                    target->notify |= NOTIFY_VALUE;
                    target->polarity = value;
                    result = COAP_204_CHANGED;
                }
                break;
            case LWM2M_DOUTPUT_ATTR:
                if (lwm2m_data_decode_bool(&data_array[i], &value) != 1) {
                    result = COAP_400_BAD_REQUEST;
                } else {
                    target->notify |= NOTIFY_POLARITY;
                    target->value = (value) ? !target->polarity : target->polarity;
                    prv_actuator_write_bool(target);
                    result = COAP_204_CHANGED;
                }
                break;
            case LWM2M_APP_TYPE_ATTR:
                if (data_array[i].value.asBuffer.length == 0) {
                    if (target->app_type != NULL) {
                        lwm2m_free(target->app_type);
                    }
                    target->app_type = NULL;
                    target->notify |= NOTIFY_APP_TYPE;
                    result = COAP_204_CHANGED;
                } else {
                    char *new = lwm2m_malloc(data_array[i].value.asBuffer.length+1);
                    if (new == NULL) {
                        result = COAP_500_INTERNAL_SERVER_ERROR;
                    } else {
                        memset(new, '\0', data_array[i].value.asBuffer.length+1);
                        memcpy(new, (char*)data_array[i].value.asBuffer.buffer, data_array[i].value.asBuffer.length);
                        if (target->app_type != NULL) {
                            lwm2m_free(target->app_type);
                        }
                        target->app_type = new;
                        target->notify |= NOTIFY_APP_TYPE;
                        result = COAP_204_CHANGED;
                    }
                }
                break;
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
    (void)object;
    (void)instance_id;

    switch (resource_id) {
        case LWM2M_DOUTPUT_POLARITY_ATTR:
        case LWM2M_DOUTPUT_ATTR:
        case LWM2M_APP_TYPE_ATTR:
            return COAP_405_METHOD_NOT_ALLOWED;
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
            LWM2M_DOUTPUT_POLARITY_ATTR, LWM2M_DOUTPUT_ATTR,
            LWM2M_APP_TYPE_ATTR,
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
                case LWM2M_DOUTPUT_POLARITY_ATTR:
                case LWM2M_DOUTPUT_ATTR:
                case LWM2M_APP_TYPE_ATTR:
                    break;
                default:
                    result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

int lwm2m_get_bool_actuator(lwm2m_object_t **object, saul_reg_t *dev)
{
    actuator_data_t *actuator_instance;
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

        (*object)->objID = LWM2M_DIGITAL_ACTUATOR_ID;

        (*object)->readFunc = prv_bool_read;
        (*object)->writeFunc = prv_bool_write;
        (*object)->executeFunc = prv_bool_execute;
        (*object)->discoverFunc = prv_bool_discover;
    }
    else {
        /* Check for mismatching sensor IDs */
        if ((*object)->objID != LWM2M_DIGITAL_ACTUATOR_ID) {
            DEBUG("%s: mismatching sensor ids\n", __func__);
            return -EINVAL;
        }
        /* If we got an existing object, check whether an instance for this dev already exists */
        lwm2m_list_t *target;
        for (target = (*object)->instanceList; target != NULL; target = target->next) {
            if (((actuator_data_t *)target)->dev == dev) {
                DEBUG("%s: sensor instance for dev already exists\n", __func__);
                return -EEXIST;
            }
        }
    }

    actuator_instance = (actuator_data_t *)lwm2m_malloc(sizeof(actuator_data_t));
    if (actuator_instance == NULL) {
        DEBUG("%s: couldn't allocate memoru for actuator_data_t\n", __func__);
        /* Only free object if we allocated it ourselves
         * TODO/FIXME: Maybe never free ever and let the caller worry about that?
         */
        if (self_alloc) {
            DEBUG("%s: freeing self-allocated lwm2m_object_t\n", __func__);
            lwm2m_free((*object));
        }
        return -ENOMEM;
    }
    memset(actuator_instance, 0, sizeof(actuator_data_t));

    actuator_instance->instance_id = lwm2m_list_newId((*object)->instanceList);
    prv_init_instance(actuator_instance, dev);
    (*object)->instanceList = LWM2M_LIST_ADD((*object)->instanceList, actuator_instance);

    return 0;
}

void lwm2m_free_bool_actuator(lwm2m_object_t *obj)
{
    if (obj == NULL) {
        return;
    }
    if (obj->userData) {
        lwm2m_free(obj->userData);
    }
    while (obj->instanceList != NULL) {
        actuator_data_t *instance = (actuator_data_t *)obj->instanceList;
        obj->instanceList = obj->instanceList->next;
        lwm2m_free(instance);
    }
    lwm2m_free(obj);
}

int lwm2m_auto_add_bool_actuators(lwm2m_context_t *context)
{
    int added = 0;
    int id = LWM2M_DIGITAL_ACTUATOR_ID;
    int added_tmp = 0;
    saul_reg_t *reg;
    lwm2m_object_t *object = NULL;

    /* Collect all devices for the current object ID */
    for (reg = saul_reg; reg != NULL; reg = reg->next) {
        if (lwm2m_saul_type_to_obj_id(reg->driver->type) == id) {
            DEBUG("%s: adding instance of %s\n", __func__,
                  lwm2m_object_id_to_str(id));
            if (lwm2m_get_bool_actuator(&object, reg) == 0) {
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

void lwm2m_poll_bool_actuators(lwm2m_context_t *lwm2mH)
{
    lwm2m_object_t *object = (lwm2m_object_t *)lwm2mH->objectList;

    DEBUG("%s: updating sensor values", __func__);

    for (; object != NULL; object = object->next) {
        if (object->objID == LWM2M_DIGITAL_ACTUATOR_ID) {
            lwm2m_uri_t uri;
            uri.flag = LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID | LWM2M_URI_FLAG_RESOURCE_ID;
            uri.objectId = object->objID;
            actuator_data_t *instance = (actuator_data_t *)object->instanceList;

            for (; instance != NULL; instance = instance->next) {
                uint8_t i;
                notification_tuple_t notify_array[] = {
                    {NOTIFY_VALUE, LWM2M_DOUTPUT_ATTR},
                    {NOTIFY_POLARITY, LWM2M_DOUTPUT_POLARITY_ATTR},
                    {NOTIFY_APP_TYPE, LWM2M_DOUTPUT_ATTR},
                };
                priv_update_actuator_value(instance);

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

