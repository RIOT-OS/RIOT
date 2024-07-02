/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     lwm2m_objects
 * @defgroup    lwm2m_objects_barometer Barometer
 * @brief       Barometer object implementation for LwM2M client using Wakaama
 *
 * @experimental
 *
 * This implements the LwM2M Barometer object (ID 3315) as specified in the LwM2M registry.
 * This IPSO object should be used with an air pressure sensor to report a barometer measurement.
 * It also provides resources for minimum/maximum measured values and the minimum/maximum range that
 * can be measured by the barometer sensor. An example measurement unit is pascals.
 *
 * The sensor value can be updated by the application using the
 * @ref lwm2m_object_barometer_update_value function, or polled when required if a callback is
 * registered upon object instantiation via @ref lwm2m_obj_barometer_args_t::read_cb.
 *
 * To use this object add `USEMODULE += wakaama_objects_barometer` to the application Makefile.
 *
 * ## Resources
 *
 * For an XML description of the object see
 * https://raw.githubusercontent.com/OpenMobileAlliance/lwm2m-registry/prod/version_history/3315-1_0.xml
 *
 * This object is based on the @ref lwm2m_objects_ipso_sensor_base, therefore it shares the same
 * resources.
 *
 * ## Usage
 *
 * 1. Initialize the LwM2M object with an initialized client pointer.
 *
 * ~~~~~~~~~~~~~~~{.c}
 * lwm2m_object_t *obj = lwm2m_object_barometer_init(&client_data);
 * ~~~~~~~~~~~~~~~
 *
 * 2. Create a new instance of the object with a given configuration (@ref lwm2m_obj_barometer_args_t).
 *    Here, you can decide the way of updating the sensor values: polling or pushing. In this case,
 *    we register a callback function that is called whenever the sensor value is read.
 *
 * ~~~~~~~~~~~~~~~{.c}
 *
 * // callback function to read the sensor value
 * int _read_cb(void *arg, int16_t *value)
 * {
 *     (void)arg;
 *
 *     // generate new value
 *     *value = 100;
 *
 *     return 0;
 * }
 *
 * // initialize an instance of barometer sensor object
 * const lwm2m_obj_barometer_args_t barometer_args = {
 *     .min_range_value = 0.0,
 *     .max_range_value = 2000.0,
 *     .units = "hPa",
 *     .units_len = sizeof("hPa") - 1,
 *     .instance_id = 0,
 *     .read_cb = _read_cb,
 *     .read_cb_arg = NULL
 * };
 *
 * int32_t res = lwm2m_object_barometer_instance_create(&barometer_args);
 *
 * if (res < 0) {
 *     puts("Could not create barometer object instance");
 * }
 * ~~~~~~~~~~~~~~~
 *
 * 3. You can now update the sensor values using the @ref lwm2m_object_barometer_update_value function.
 *
 * ~~~~~~~~~~~~~~~{.c}
 * uint16_t instance_id = (uint16_t)res;
 * lwm2m_object_barometer_update_value(&client_data, instance_id, new_value);
 * ~~~~~~~~~~~~~~~
 * @{
 *
 * @file
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef OBJECTS_BAROMETER_H
#define OBJECTS_BAROMETER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "liblwm2m.h"
#include "lwm2m_client.h"
#include "objects/ipso_sensor_base.h"

/**
 * @defgroup lwm2m_objects_barometer_config LwM2M Barometer object compile configurations
 * @ingroup lwm2m_client_config
 * @{
 */
/**
 * @brief Maximum number of instances of the object
 */
#ifndef CONFIG_LWM2M_BAROMETER_INSTANCES_MAX
#define CONFIG_LWM2M_BAROMETER_INSTANCES_MAX (1U)
#endif
/** @} */

/**
 * @brief LwM2M Barometer object ID
 */
#define LWM2M_BAROMETER_OBJECT_ID 3315

/**
 * @brief Arguments for the creation of a Barometer object instance.
 */
typedef lwm2m_obj_ipso_base_sensor_args_t lwm2m_obj_barometer_args_t;

/**
 * @brief   Initialize the Barometer object handle
 *
 * @param[in] client_data   Pointer to the LwM2M client data.
 *
 * @return Pointer to the global handle of the Barometer object.
 */
lwm2m_object_t *lwm2m_object_barometer_init(lwm2m_client_data_t *client_data);

/**
 * @brief   Create a new Barometer instance.
 *
 * @param[in]      args             Initialize structure with the parameter for the instance. Must
 *                                  not be NULL.
 *
 * @return > 0 value representing the instance ID if the instance was created successfully.
 * @return <0 otherwise
 */
int32_t lwm2m_object_barometer_instance_create(const lwm2m_obj_barometer_args_t *args);

/**
 * @brief Update the value of the barometer sensor and trigger a notification
 *        to the observing servers, if any.
 *
 * @param[in] client_data   Pointer to the LwM2M client.
 * @param[in] instance_id   ID of the instance to update.
 * @param[in] value         New value for the sensor.
 */
void lwm2m_object_barometer_update_value(const lwm2m_client_data_t *client_data,
                                         uint16_t instance_id, int16_t value);

#ifdef __cplusplus
}
#endif

#endif /* OBJECTS_BAROMETER_H */
/** @} */
