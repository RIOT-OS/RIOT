/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @{
 * @ingroup     lwm2m_objects_humidity
 *
 * @file
 * @brief       Humidity Sensor object implementation for LwM2M client using Wakaama
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "mutex.h"
#include "liblwm2m.h"
#include "lwm2m_client.h"
#include "objects/humidity.h"
#include "objects/ipso_sensor_base.h"

#define ENABLE_DEBUG 1
#include "debug.h"

/**
 * @brief Humidity Sensor object implementation descriptor.
 */
static lwm2m_obj_ipso_sensor_base_t _humidity_object;

/**
 * @brief Pool of object instances.
 */
static lwm2m_obj_ipso_sensor_base_inst_t _instances[CONFIG_LWM2M_HUMIDITY_INSTANCES_MAX];

lwm2m_object_t *lwm2m_object_humidity_init(lwm2m_client_data_t *client_data)
{
    assert(client_data);
    int res = lwm2m_object_ipso_sensor_base_init_derived(client_data, &_humidity_object,
                                                         LWM2M_HUMIDITY_OBJECT_ID,
                                                         _instances,
                                                         CONFIG_LWM2M_HUMIDITY_INSTANCES_MAX);

    if (res) {
        DEBUG("[lwm2m:humidity]: failed to create object\n");
        return NULL;
    }

    return &_humidity_object.object;
}

int32_t lwm2m_object_humidity_instance_create(const lwm2m_obj_humidity_args_t *args)
{
    int32_t result = lwm2m_object_ipso_sensor_base_instance_create(&_humidity_object, args);

    if (result) {
        DEBUG("[lwm2m:humidity]: failed to create instance\n");
    }

    return result;
}

void lwm2m_object_humidity_update_value(const lwm2m_client_data_t *client_data,
                                        uint16_t instance_id, int16_t value)
{
    lwm2m_object_ipso_sensor_base_update_value(client_data, &_humidity_object, instance_id, value);
}
