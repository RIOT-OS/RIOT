/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     lwm2m_objects
 * @defgroup    lwm2m_objects_ipso_sensor_base IPSO Sensor base object
 * @brief       Base IPSO sensor functionality to implement similar IPSO objects
 *
 * @experimental
 *
 * This module is **not a complete object**, but provides common functionalities
 * shared among different IPSO sensor objects. It should not be used directly,
 * from the application, but rather be used as a dependency for other objects.
 *
 * To use this module add `USEMODULE += wakaama_objects_ipso_sensor_base` to the dependencies
 * of your own object module.
 *
 * ## Resources
 * Given that IPSO sensors share a common set of resources, these are implemented by this module.
 *
 |                 Name                |  ID  | Mandatory |  Type  | Range |   Units  | Implemented |
 |:-----------------------------------:|:----:|:---------:|:------:|:-----:|:--------:|:-----------:|
 |             Sensor Value            | 5700 |    Yes    |  Float |   -   | See 5701 |     Yes     |
 |         Min. Measured Value         | 5601 |     No    |  Float |   -   | See 5701 |     Yes     |
 |         Max. Measured Value         | 5602 |     No    |  Float |   -   | See 5701 |     Yes     |
 |           Min. Range Value          | 5603 |     No    |  Float |   -   | See 5701 |     Yes     |
 |           Max. Range Value          | 5604 |     No    |  Float |   -   | See 5701 |     Yes     |
 | Reset Min. and Max. Measured Values | 5605 |     No    |    -   |   -   |     -    |     Yes     |
 |             Sensor Units            | 5701 |     No    | String |   -   |     -    |     Yes     |
 *
 * @{
 *
 * @file
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef OBJECTS_IPSO_SENSOR_BASE_H
#define OBJECTS_IPSO_SENSOR_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mutex.h"
#include "liblwm2m.h"
#include "lwm2m_client.h"

/**
 * @defgroup lwm2m_objects_ipso_sensor_base_config LwM2M IPSO Sensor base object compile configurations
 * @ingroup lwm2m_client_config
 * @{
 */
/**
 * @brief Maximum size for the units resource string
 */
#ifndef CONFIG_LWM2M_IPSO_SENSOR_BASE_UNITS_MAX_SIZE
#define CONFIG_LWM2M_IPSO_SENSOR_BASE_UNITS_MAX_SIZE (4U)
#endif
/** @} */

/**
 * @name IPSO sensor base object resource's IDs (shared among many IPSO sensor objects)
 * @{
 */
/**
 * @brief Sensor value resource ID.
 */
#define LWM2M_IPSO_BASE_SENSOR_VALUE_ID 5700
/**
 * @brief Min. measured value resource ID.
 */
#define LWM2M_IPSO_BASE_SENSOR_MIN_MEASURED_VALUE_ID 5601
/**
 * @brief Max. measured value resource ID.
 */
#define LWM2M_IPSO_BASE_SENSOR_MAX_MEASURED_VALUE_ID 5602
/**
 * @brief Min. range value resource ID.
 */
#define LWM2M_IPSO_BASE_SENSOR_MIN_RANGE_VALUE_ID 5603
/**
 * @brief Max. range value resource ID.
 */
#define LWM2M_IPSO_BASE_SENSOR_MAX_RANGE_VALUE_ID 5604
/**
 * @brief Reset min. and max. measured values resource ID.
 */
#define LWM2M_IPSO_BASE_SENSOR_RESET_MIN_MAX_MEASURED_VALUES_ID 5605
/**
 * @brief Sensor units resource ID.
 */
#define LWM2M_IPSO_BASE_SENSOR_UNITS_ID 5701
/** @} */

/**
 * @brief Callback for reading the sensor value.
 *
 * @param[in]  read_cb_arg  Data passed for the read callback when the instance was created.
 * @param[out] value        Pointer to the variable where the value will be stored.
 *
 * @return 0 on success
 * @return <0 otherwise
 */
typedef int lwm2m_obj_ipso_base_sensor_read_cb_t(void *read_cb_arg, int16_t *value);

/**
 * @brief Arguments for the creation of an object based on the IPSO Sensor Base
 *        object instance.
 */
typedef struct lwm2m_obj_ipso_sensor_base_args {
    int32_t instance_id;                            /**< ID for the new instance. It must be between 0 and (UINT16_MAX - 1),
                                                         if -1 the next available ID will be used. */
    char *units;                                    /**< Null-terminated string of the "Sensor Units" resource. May be NULL. */
    size_t units_len;                               /**< Length of units */
    int16_t min_range_value;                        /**< Minimum value that can be measured by the sensor. */
    int16_t max_range_value;                        /**< Maximum value that can be measured by the sensor. */
    void *read_cb_arg;                              /**< Data to pass to the read callback. May be NULL. */
    lwm2m_obj_ipso_base_sensor_read_cb_t *read_cb;  /**< Callback to read the sensor value. May be NULL. */
} lwm2m_obj_ipso_base_sensor_args_t;

/**
 * @brief LwM2M IPSO sensor base instance
 */
typedef struct lwm2m_obj_ipso_sensor_base_inst {
    lwm2m_list_t list;                                          /**< list handle */
    int16_t sensor_value;                                       /**< sensor value */
    int16_t min_sensor_value;                                   /**< minimum sensor value */
    int16_t max_sensor_value;                                   /**< maximum sensor value */
    int16_t min_range_value;                                    /**< minimum range value */
    int16_t max_range_value;                                    /**< maximum range value */
    char units[CONFIG_LWM2M_IPSO_SENSOR_BASE_UNITS_MAX_SIZE];   /**< units */
    void *read_cb_arg;                                          /**< Data to pass to the read callback. May be NULL. */
    lwm2m_obj_ipso_base_sensor_read_cb_t *read_cb;              /**< Callback to read the sensor value. May be NULL. */
} lwm2m_obj_ipso_sensor_base_inst_t;

/**
 * @brief LwM2M IPSO sensor base object
 */
typedef struct lwm2m_obj_ipso_sensor_base {
    lwm2m_object_t object;                              /**< LwM2M object base */
    lwm2m_obj_ipso_sensor_base_inst_t *free_instances;  /**< List of instances */
    uint16_t object_id;                                 /**< Object ID */
    mutex_t mutex;                                      /**< Mutex for the object */
} lwm2m_obj_ipso_sensor_base_t;

/**
 * @brief   Create a new object instance based on the IPSO Sensor Base and add it to the
 *          @p object list.
 *
 * @param[in, out] object           Pointer to the LwM2M IPSO object.
 * @param[in]      args             Initialize structure with the parameter for the instance. May
 *                                  not be NULL.
 *
 * @retval instance ID (>0) on success
 * @retval <0 otherwise
 */
int32_t lwm2m_object_ipso_sensor_base_instance_create(lwm2m_obj_ipso_sensor_base_t *object,
                                                      const lwm2m_obj_ipso_base_sensor_args_t *args);

/**
 * @brief   Initialize the a LwM2M object derived from the IPSO Sensor Base object.
 *
 * @param[in, out] client_data      Pointer to the LwM2M client.
 * @param[in, out] object           Pointer to the LwM2M IPSO object.
 * @param[in]      object_id        Object ID.
 * @param[in]      instances        List of allocated instances.
 * @param[in]      instance_count   Number of allocated instances.
 *
 * @retval 0 on success
 * @retval <0 otherwise
 */
int lwm2m_object_ipso_sensor_base_init_derived(lwm2m_client_data_t *client_data,
                                               lwm2m_obj_ipso_sensor_base_t *object,
                                               uint16_t object_id,
                                               lwm2m_obj_ipso_sensor_base_inst_t *instances,
                                               size_t instance_count);

/**
 * @brief Update the value of the illuminance sensor and trigger a notification
 *        to the observing servers, if any.
 *
 * @param[in] client_data   Pointer to the LwM2M client.
 * @param[in] object        Pointer to the LwM2M IPSO object.
 * @param[in] instance_id   ID of the instance to update.
 * @param[in] value         New value for the sensor.
 */
void lwm2m_object_ipso_sensor_base_update_value(const lwm2m_client_data_t *client_data,
                                                const lwm2m_obj_ipso_sensor_base_t *object,
                                                uint16_t instance_id, int16_t value);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* OBJECTS_IPSO_SENSOR_BASE_H */
