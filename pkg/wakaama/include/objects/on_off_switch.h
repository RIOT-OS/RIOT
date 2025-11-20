/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     lwm2m_objects
 * @defgroup    lwm2m_objects_on_off-switch On/Off Switch
 * @brief       On/Off switch object implementation for LwM2M client using Wakaama
 *
 * @experimental This API is considered experimental and may change in future releases without
 *               deprecation process.
 *
 * This implements the LwM2M IPSO on/off switch object (ID 3342) as specified in the LwM2M registry.
 *
 * This IPSO object should be used with an On/Off switch to report the state of the switch.
 *
 * To use this object add `USEMODULE += wakaama_objects_on_off_switch` to the application Makefile.
 *
 * ## Resources
 *
 * For an XML description of the object see
 * https://raw.githubusercontent.com/OpenMobileAlliance/lwm2m-registry/prod/3342.xml
 *
 * | Name                  | ID   | Mandatory | Type    | Range | Units | Implemented |
 * |-----------------------|------|-----------|---------|-------|-------|-------------|
 * | Digital Input State   | 5500 | Yes       | Boolean | -     | -     | Yes         |
 * | Digital Input Counter | 5501 | No        | Integer | -     | -     | Yes         |
 * | On time               | 5852 | No        | Integer | -     | s     | Yes         |
 * | Off Time              | 5854 | No        | Integer | -     | s     | Yes         |
 * | Application Type      | 5750 | No        | String  | -     | -     | Yes         |
 *
 * ## Usage
 *
 * 1. Initialize the LwM2M client with @ref lwm2m_object_on_off_switch_init, by passing a pointer
 *    to the LwM2M client data.
 *
 * 2. Now you can create instances of the on/off switch object with
 *    @ref lwm2m_object_on_off_switch_instance_create.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #define APP_TYPE "BTN 0"
 *
 * // ... ///
 *
 * lwm2m_object_t *on_off_switch;
 * lwm2m_client_data_t client_data;
 *
 * lwm2m_client_init(&client_data);
 * on_off_switch = lwm2m_object_on_off_switch_init(&client_data);
 *
 * lwm2m_obj_on_off_switch_args_t args = {
 *    .app_type = APP_TYPE,
 *    .app_type_len = sizeof(APP_TYPE) - 1
 * };
 *
 * int result = lwm2m_object_on_off_switch_instance_create(&args, 0);
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * 3. You can update the status and app_type of the on/off switch instance with
 *    @ref lwm2m_object_on_off_switch_update_status and
 *    @ref lwm2m_object_on_off_switch_update_app_type respectively. As this will make sure to send
 *    notifications to servers that may be observing these resources, avoid calling them from
 *    interrupt contexts.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * lwm2m_object_on_off_switch_update_app_type(0, true);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "liblwm2m.h"

/**
 * @defgroup lwm2m_objects_on_off_switch_config LwM2M On/Off switch object compile configurations
 * @ingroup lwm2m_client_config
 * @{
 */
/**
 * @brief Maximum number of instances of the object
 */
#ifndef CONFIG_LWM2M_ON_OFF_SWITCH_INSTANCES_MAX
#define CONFIG_LWM2M_ON_OFF_SWITCH_INSTANCES_MAX                (3U)
#endif

/**
 * @brief Maximum size for the application type string
 */
#ifndef CONFIG_LWM2M_ON_OFF_SWITCH_APP_TYPE_MAX_SIZE
#define CONFIG_LWM2M_ON_OFF_SWITCH_APP_TYPE_MAX_SIZE    (16U)
#endif
/** @} */

/**
 * @brief On/Off switch object ID.
 */
#define LWM2M_ON_OFF_SWITCH_OBJECT_ID         3342

/**
 * @name On/Off switch object resource's IDs.
 * @{
 */
/**
 * @brief Digital input state esource ID.
 */
#define LWM2M_ON_OFF_SWITCH_DIGITAL_INPUT_STATE_ID   5500
/**
 * @brief Digital Input Counter resource ID.
 */
#define LWM2M_ON_OFF_SWITCH_DIGITAL_INPUT_COUNTER_ID 5501
/**
 * @brief On Time resource ID.
 */
#define LWM2M_ON_OFF_SWITCH_ON_TIME_ID               5852
/**
 * @brief Off Time resource ID.
 */
#define LWM2M_ON_OFF_SWITCH_OFF_TIME_ID              5854
/**
 * @brief Application type resource ID.
 */
#define LWM2M_ON_OFF_SWITCH_APP_TYPE_ID              5750
/** @} */

/**
 * @brief Arguments for the creation of an on/off switch object instance.
 */
typedef struct lwm2m_obj_on_off_switch_args {
    const char *app_type;               /**< Array of chars with the app type. May be NULL. */
    size_t app_type_len;                /**< Length of app_type */
} lwm2m_obj_on_off_switch_args_t;

/**
 * @brief Initialize the on/off switch object.
 *
 * @param[in] client_data  LwM2M client data.
 *
 * @return Pointer to the On/Off switch object on success
 */
lwm2m_object_t *lwm2m_object_on_off_switch_init(lwm2m_client_data_t *client_data);

/**
 * @brief   Create a new on/off switch instance and add it to the @p object list.
 *
 * @param[in]      args             Initialize structure with the parameter for the instance. Must
 *                                  not be NULL.
 * @param[in]      instance_id      ID for the new instance. It must be between 0 and
 *                                  (UINT16_MAX - 1), if -1 the next available ID will be used.
 *
 * @return 0 on success
 * @return -EINVAL if an invalid @p instance_id is given
 * @return -ENOMEM if no memory is available to create a new instance
 */
int lwm2m_object_on_off_switch_instance_create(const lwm2m_obj_on_off_switch_args_t *args,
                                               int32_t instance_id);

/**
 * @brief Update the status of a on/off switch instance.
 *
 * @param[in] instance_id  ID of the instance to update.
 * @param[in] status       New status of the switch.
 *
 * @return 0 on success
 * @return -EINVAL if the instance does not exist
 */
int lwm2m_object_on_off_switch_update_status(uint16_t instance_id, bool status);

/**
 * @brief Update the application type of a on/off switch instance.
 *
 * @param[in] instance_id  ID of the instance to update.
 * @param[in] app_type     String representing new application type. Must be NULL terminated, must
 *                         not be NULL.
 * @param[in] len          Length of the @p app_type string.
 *
 * @return 0 on success
 * @return -EINVAL if the instance does not exist
 * @return -ENOBUFS if the app_type string is too long (size > @ref CONFIG_LWM2M_ON_OFF_SWITCH_APP_TYPE_MAX_SIZE)
 */
int lwm2m_object_on_off_switch_update_app_type(uint16_t instance_id, const char *app_type,
                                               size_t len);

#ifdef __cplusplus
}
#endif

/** @} */
